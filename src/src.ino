/*  ======================================  */
/*               DEFINICOES:                */

#define TRIG_555 2  // output uno / input 555
#define OUT_555  3  // input uno  / output 555

#define RA  150  // Resistor [ohms]

#define TIMER1_PRESCALER_1    CS10         // prescaler = 1 (mesma frequencia do uC)
#define TIMER1_PRESCALER_8    CS11         // prescaler = 8 (freq / 8)
#define TIMER1_PRESCALER_64   (CS11+CS10)  // prescaler = 64
#define TIMER1_PRESCALER_256  CS12         // prescaler = 256
#define TIMER1_PRESCALER_1024 CS12+CS10    // prescaler = 1024

#define TIMER1_PRESCALER TIMER1_PRESCALER_64

#define TIMER1_MAX_COUNT 65535
#define TIMER1_CLK_FREQ  2     // clock do uC [MHz]



/*  ======================================  */
/*            MODOS DE OPERACAO:            */

//#define MODO_LEITURA_CONTINUA
//#define MODO_LEITURA_MEDIA

#if defined(MODO_LEITURA_CONTINUA) && defined(MODO_LEITURA_MEDIA)
#error "Modos de leitura continua e media nao podem ser habilitadas simultaneamente!"
#endif // MODO_LEITURA_CONTINUA

#ifdef MODO_LEITURA_MEDIA
#define NUM_MEDIA 5  // numero de medicoes por leitura
#endif



/*  ======================================  */
/*            VARIAVEIS GLOBAIS:            */

#ifdef MODO_LEITURA_CONTINUA
  unsigned long tempoLeituraContinua = 0;
#endif  // MODO_LEITURA_CONTINUA

unsigned long duracao = 0;  // duracao do pulso de OUT_555 [ms]
float capacitancia = 0.0;   // valor calculado da capacitancia ( C = tw / (1.1*RA) )
float altura = 0.0;         // valor calculado do nivel da agua ( C(h) = coefA*h + coefB )
float coefA = 0.0;
float coefB = 0.0;

volatile bool flagLeitura = false;  // flag para execucao de uma leitura (le se true)
volatile bool flagCalib = true;     // flag para execucao da calibracao (calibra se true)
volatile bool flagCalibA = false;   // flag para execucao da calibracao apenas do coef A
volatile bool flagCalibB = false;   // flag para execucao da calibracao apenas do coef B



/*  ======================================  */
/*                  SETUP:                  */

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // Pinout:
  pinMode(TRIG_555, OUTPUT);
  digitalWrite(TRIG_555, HIGH);

  pinMode(OUT_555, INPUT);

  // Serial:
  Serial.begin(9600);

  // Interrupcao de GPIO:
  attachInterrupt(digitalPinToInterrupt(OUT_555), OUT_GPIO_ISR, FALLING);

  // Configuracao do Timer:
  TCCR1A = 0;  // configura o Timer 1 para o modo normal

  #ifdef MODO_LEITURA_CONTINUA
    calibrar();
    tempoLeituraContinua = millis();
  #endif  // MODO_LEITURA_CONTINUA
}



/*  ======================================  */
/*                  LOOP:                   */
void loop(){
  #ifndef MODO_LEITURA_CONTINUA
    loopSerial();

    if (flagCalib){
      flagCalib = false;
      calibrar();
    }
    else if (flagCalibA){
      flagCalibA = false;
      calibrarCoefA();
    }
    else if (flagCalibB){
      flagCalibB = false;
      calibrarCoefB();
    }
    else if (flagLeitura){
      leitura();
    }
  #else
    if (millis() - tempoLeituraContinua >= 200){
      tempoLeituraContinua = millis();
      leitura();
    }
  #endif  // !MODO_LEITURA_CONTINUA
}
