# Capacitor de Placas Paralelas

Projeto Final da disciplina de PBLE03, que consiste na construção de um capacitor de placas paralelas caseiro para medição do nível de água de um recipiente.

Um circuito com um CI555 em configuração de multivibrador monoestável é utilizado de modo que o tempo de carga do capacitor seja traduzido em um pulso cuja duração possa ser lida pelo microcontrolador. A partir dessa duração é possível determinar o nível de água no recipiente, de acordo com a fórmula:

![image](https://github.com/lucasnoce/CapacitorPlacasParalelas/assets/62445590/72ebbad2-1869-4385-91a2-85445627da90)

O sistema desenvolvido é controlado a partir do terminal serial e pode ser calibrado a qualquer momento. As leituras podem ser realizadas a partir do envio de um comando ou o sistema pode operar em modo de leitura contínua, com taxa de amostragem de 200 ms.
