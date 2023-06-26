/*  ======================================  */
/*                 SUMÁRIO:                 */
/*                                          */
/*    1 CALLBACK DA GPIO ISR                */
/*    2 MEDICAO DA CAPACITANCIA             */
/*    3 CALIBRACAO                          */
/*      3.1 Calibrar                        */
/*      3.2 Coef A                          */
/*      3.3 Coef B                          */
/*    4 REALIZAR LEITURA                    */
/*    5 LOOP DA SERIAL                      */
/*                                          */
/*  ======================================  */



/*  ======================================  */
/*    1 CALLBACK DA GPIO ISR                */

void OUT_GPIO_ISR(){
  TCCR1B = 0;  // pausa o Timer 1
  flagLeitura = false;  // indica que a leitura acabou
}



/*  ======================================  */
/*    2 MEDICAO DA CAPACITANCIA             */

void medirCapacitancia(){
  flagLeitura = true;  // indica que a leitura comecou
  
  TCCR1B = (1 << TIMER1_PRESCALER);  // configura o prescaler e inicia o Timer 1
  digitalWrite(TRIG_555, LOW);       // inicio do pulso negativo em TRIG_555
  TCNT1 = 0;                         // reseta o Timer 1 para 0 (reinicia a contagem)
  delayMicroseconds(12);             // garante a largura minima do pulso (10us)
  digitalWrite(TRIG_555, HIGH);      // fim do pulso negativo em TRIG_555
  
  while(flagLeitura);  // aguarda a interrupcao na GPIO indicando o fim do pulso em OUT_555

  duracao = (float) TCNT1 / (float) (TIMER1_CLK_FREQ);
  capacitancia = (float) duracao / (1.1 * RA);
  Serial.println(TCNT1);
  Serial.println(duracao);
  Serial.println(capacitancia);
}



/*  ======================================  */
/*    3 CALIBRACAO                          */

// 3.1 Calibrar
void calibrar(){
  Serial.println("----------------------------------------");
  Serial.println("Executando nova calibracao:");
  Serial.println("1) Esvazie o recipiente e escreva \'b\'");

  calibrarCoefB();

  Serial.println("2) Encha o recipiente ate 20cm e escreva \'a\'");

  calibrarCoefA();
  
  Serial.println("Fim da calibracao");
  Serial.println("----------------------------------------");

  #ifndef MODO_LEITURA_CONTINUA
    Serial.println("Para realizar uma leitura, digite \'l\' ou para realizar uma calibracao, digite \'c\'");
  #endif  // MODO_LEITURA_CONTINUA
}

// 3.2 Coef B
void calibrarCoefB(){
  char c = 0;
  bool first_try = true;
  bool read_ok = false;

  while (!read_ok){
    delay(100);
    if (Serial.available()){
      c = Serial.read();
      if (c != 'b' && c != 'B'){
        if (first_try) first_try = false;
        else{
          Serial.println("Resposta invalida! Tente novamente!");
          read_ok = false;
        }
      }
      else{
        read_ok = true;
        #ifdef MODO_LEITURA_MEDIA
          int i = 0;
          unsigned long millisAtual = millis();
          while (i < NUM_MEDIA){
            if (millis() - millisAtual >= 50){
              millisAtual = millis();
              i++;
              medirCapacitancia();
              coefB += (float) capacitancia / NUM_MEDIA;
            }
          }
        #else
          medirCapacitancia();
          coefB = (float) capacitancia;
        #endif  // MODO_LEITURA_MEDIA
      }
    }
  }

  Serial.print("coefB = ");
  Serial.println(coefB);
}

// 3.2 Coef digitalRead()
void calibrarCoefA(){
  char c = 0;
  bool first_try = true;
  bool read_ok = false;

  while (!read_ok){
    delay(100);
    if (Serial.available()){
      c = Serial.read();
      if (c != 'a' && c != 'A'){
        if (first_try) first_try = false;
        else{
          Serial.println("Resposta invalida! Tente novamente!");
          read_ok = false;
        }
      }
      else{
        read_ok = true;
        #ifdef MODO_LEITURA_MEDIA
          int i = 0;
          unsigned long millisAtual = millis();
          while (i < NUM_MEDIA){
            if (millis() - millisAtual >= 50){
              millisAtual = millis();
              i++;
              medirCapacitancia();
              coefA += (float) (capacitancia - coefB) / (20.0 * NUM_MEDIA);
            }
          }
          #else
            medirCapacitancia();
            coefA = (float) (capacitancia - coefB) / 20.0;
          #endif  // MODO_LEITURA_MEDIA
      }
    }
  }

  Serial.print("coefA = ");
  Serial.println(coefA);
}


/*  ======================================  */
/*    4 REALIZAR LEITURA                    */

void leitura(){
  #ifdef MODO_LEITURA_MEDIA
    int i = 0;
    unsigned long millisAtual = millis();
    while (i < NUM_MEDIA){
      if (millis() - millisAtual >= 50){
        millisAtual = millis();
        i++;
        medirCapacitancia();
        altura += (float) ((capacitancia - coefB) / coefA) / NUM_MEDIA;
      }
    }
  #else
    medirCapacitancia();
    altura = (float) (capacitancia - coefB) / coefA;
  #endif  // NUM_MEDIA

  Serial.print("Medida da altura [cm] = ");
  Serial.println(altura, 2);
  
  #ifndef MODO_LEITURA_CONTINUA
    Serial.println();
    Serial.println("Para realizar uma nova leitura, digite \'l\'");
  #endif  // MODO_LEITURA_CONTINUA
}



/*  ======================================  */
/*    5 LOOP DA SERIAL                      */

void loopSerial(){
  if (Serial.available()){
    char c = Serial.read();

    flagLeitura = false;
    flagCalib = false;
    flagCalibA = false;
    flagCalibB = false;

    if (c == 'l' || c == 'L')
      flagLeitura = true;
    else if (c == 'c' || c == 'C')
      flagCalib = true;
    else if (c == 'a' || c == 'A')
      flagCalibA = true;
    else if (c == 'b' || c == 'B')
      flagCalibA = true;
  }
}
