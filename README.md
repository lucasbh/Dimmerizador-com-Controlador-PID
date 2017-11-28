# Dimmerizador com Controlador PID
 
FUNDAÇÃO EDSON QUEIROZ
UNIVERSIDADE DE FORTALEZA – UNIFOR
Centro de Ciências Tecnológicas – CCT

Controle Digital
Dimmerizador PID
Prof. Afonso Henrique

## AUTORES
Lucas Bezerra Holanda

Marcos Vinicius Cândido Leitão

Pedro Benevides Cavalcante

2017
Fortaleza – CE


## O Projeto

   A base do projeto será a programação implementada numa plataforma Arduino, na qual efetuará todas as funções de PID do sistema, recebendo as leituras dos LDRs e convertendo em sinais controlados para que os Leds possam ser dimerizados e assim efetuar a iluminação necessária. 


### Material Utilizado

|        Componentes         |                                 Descrição                                  |
|----------------------------|----------------------------------------------------------------------------|
| - Arduino Uno              | Plataforma de prototipagem eletrônica de hardware livre.                   |
| - 4 Led's de Alto Brilho   | Diodo usado para emissão de luz.                                           |
| - 3 LDR's                  | Resistor com resistência variando com a luz incidida sobre ele.            |
| - Módulo HC-06             | Módulo Bluetooth responsável por enviar e receber dados através da serial. |
| - Placa de Fibra de Vidro  | Placa dupla face utilizada pra desenvolver a PCi do projeto.               |


### SOFTWARES UTILIZADOS

ROBOREMO

   Roboremo é um aplicativo para android gratuito disponível na playstore que é utilizado para desenvolver simples interfaces e se comunicar de diferentes formas. O aplicativo se comunicará com o arduino via bluetooth, e este envia os dados inseridos na tela de acordo com o que foi programado pelo usuário. 

PROTEUS 

   O Proteus Design Suite é um Electronic Design Automation ferramenta (EDA), incluindo captura esquemática, simulação e módulos de layout PCi. 
	
### DESENVOLVIMENTO DE PCi


	

### A IMPLEMENTAÇÃO

   O programa começa com a inicialização das variáveis do programa e as variáveis de cada controlador PID (um para cada ponto de luz, totalizando quatro). O setPoint, que é variável de entrada do sistema, é inicializado com o valor 85. No setup é configurado cada pino e iniciado o software serial para comunicação bluetooth e o serial padrão para visualizar os dados do projeto no monitor serial. Na função loop() o programa sempre inicia verificando se há dado serial e, caso verdadeiro, os setPoints do projeto são atualizados de acordo com o que foi recebido. Em seguida é chamado a função getPID para cada ponto de luminosidade, que realiza todos os cálculos de controle e retorna um inteiro, que é um valor de 0 a 255. Logo após, é atualizado o PWM de cada LED com os valores obtidos em seu respectivo cálculo de controle e o programa retorna entra em loop.

```
 /*
 * Pinos
 * LDR PEQUENO: min:983     max:25 
 * LDR GRANDE:  min:1012    max:33 
 * LDR0: max = 7   min = 1023
 * LDR1: max = 5   min = 1020
 * LDR2: max = 10  min = 1023
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10,11);                                     //Inicia o software serial nos pinos 10 e 11
//Pinos
const int led0 = 3,    led1 = 5,    led2 = 6,    led3 = 9;
const int ldr0 = A0,   ldr1 = A1,   ldr2 = A2,   ldr3 = A3;

//Variaveis do PID
float kp = 0.3,  ki = 0.009,  kd = 0.006,  i0, i1, i2, i3;
int setPoint = 85;                                                  //O setPoint de luminosidade sempre começa em 85% ???
int erro0, erro1, erro2, erro3;
int vAtual0, vAtual1, vAtual2, vAtual3;
unsigned long tAtual0 = 0, tAtual1 = 0, tAtual2 = 0, tAtual3 = 0;
unsigned long lastTime = 0;
float pid0, pid1, pid2, pid3;

//Variaveis extras
int ldrRead0, ldrRead1, ldrRead2;

void setup() {
  // put your setup code here, to run once:
  pinMode(led0, OUTPUT);
  pinMode(ldr0, INPUT);
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop() {
  
  getSerial();        //Lê o bluetooth
  
  //Calculo do PID e escrita do pid nos pinos
  ldrRead0 = map(analogRead(ldr0), 1023, 7, 0, 100);
  ldrRead1 = map(analogRead(ldr1), 1023, 7, 0, 100);
  ldrRead2 = map(analogRead(ldr2), 1023, 7, 0, 100);
  
  pid0 = getPID(ldrRead0, led0);
  analogWrite(led0, pid0);
  pid1 = getPID((ldrRead0/2 + ldrRead1/2), led1); 
  analogWrite(led1, pid1);
  pid2 = getPID(ldrRead1/2 + ldrRead2/2, led2);
  analogWrite(led2, pid2);
  pid3 = getPID(ldrRead2, led3);
  analogWrite(led3, pid3);

  sendSerial();
}


/* 
 *  A função getPID retorna o controlador PID, que está limitado a um valor entre 0 e 255.
 *  O switchcase analisa qual LDR e setPoint foram passados na função e executa o PID com base nisso
 */
float getPID(int ledRead, const int &LED)
{
  int *valorAtual, *erro;
  float *i;
  unsigned long *tempoAtual;

  switch (LED)
  {
    case led0:
    {
      valorAtual = &vAtual0;
      erro = &erro0;
      tempoAtual = &tAtual0;
      i = &i0;
    }break;
    case led1:
    {
      valorAtual = &vAtual1;
      erro = &erro1;
      tempoAtual = &tAtual1;
      i = &i1;
    }break;
    case led2:
    {
      valorAtual = &vAtual2;
      erro = &erro2;
      tempoAtual = &tAtual2;
      i = &i2;
    }break;
    case led3:
    {
      valorAtual = &vAtual3;
      erro = &erro3;
      tempoAtual = &tAtual3;
      i = &i3;
    }break;
  }

  //Encontrando E(t)
  int valorAnterior = *valorAtual;
  *valorAtual = ledRead;
  *erro = setPoint - *valorAtual;

  //Encontrando dt
  unsigned long tempoAnterior = *tempoAtual;
  *tempoAtual = millis();
  double dt = *tempoAtual - tempoAnterior;

  float p = *erro * kp;                  // P = kp*E(t)
  *i += *erro * ki * dt;                 // i = ki*SE(t)*dt
  if(*i > 200) { *i = 200; }                // i está limitado a -50 e 200
  if(*i < -50)   { *i = -50; }
  float d = (*valorAtual - valorAnterior) * dt * kd;     // d = kd*(dE(t)/d(t))
  float PID = 50+ p + *i + d;
  if(PID > 255) { PID = 255; }
  if(PID < 0) { PID = 0; }

  return PID;
}

/* 
 *  Leitura dos dados via bluetooth
 *  O app envia o ID do objeto mais o valor. EX: setar o slider da sala0 para 68% envia a string "L060"
 */
void getSerial()
{
  //  Leitura dos dados via bluetooth
  //  O app envia o ID do objeto mais o valor. EX: setar o slider da sala0 para 68% envia a string "L060"
  String serialRead = "";
  while(mySerial.available())
  {
    char c = mySerial.read();
    serialRead += c;
    delay(2);
  }
  if(serialRead.length() > 0)
  {

    if(serialRead.substring(0,2) == "L0")
    {
      setPoint = serialRead.substring(2).toInt();
    }
  }

  String strCmp = serialRead.substring(0,2);
  if(strCmp == "L0") { setPoint = serialRead.substring(2).toInt(); }
  if(strCmp == "p+") { kp = kp + 0.1; }
  if(strCmp == "p-") { kp -= 0.1; }
  if(strCmp == "i+") { ki += 0.001; }
  if(strCmp == "i-") { ki -= 0.001; }
  if(strCmp == "d+") { kd += 0.001; }
  if(strCmp == "d-") { kd -= 0.001; }
}
  
void sendSerial()         //Dados para o monitor serial
{
  if(millis() - lastTime > 1000){
    lastTime = millis();
    String msgPID = String((float)pid0) + " " + String((float)pid1) + " " + String((float)pid2) + " " + String((float)pid3);
    String msgLDR = "LDR 0: " + String((int)ldrRead0) + " LDR 1: " + String((int)ldrRead1) + " LDR 2: " + String((int)ldrRead2);
    Serial.println(msgPID + "\t" + msgLDR);
    
    mySerial.println("kp " + String((float)kp));
    mySerial.println("ki " + String((float)ki));
    mySerial.println("kd " + String((float)kd));
  }
}
```
 
### MELHORIAS NO PID


 

## METODOLOGIA

   O dimmerizador controla o nível de luminosidade do ambiente automaticamente e se adapta a mudanças de claridade, aumentando ou diminuindo o brilho dos LEDs de acordo a luminosidade captada pelo LDR. No projeto desenvolvido, foi projetado três pontos de leitura de luminosidade e quatro prontos e iluminação, com LDRs e LEDs respectivamente. 

   No aplicativo Roboremo foi desenvolvido uma interface com três sliders, em que cada um representa o nível de luminosidade desejado em seu respectivo ponto de leitura. A aplicação envia uma string por bluetooth para o arduino, que processará a informação de acordo com o que foi programado e mudará o setPoint do ponto de luminosidade selecionado. A string enviada possui o seguinte formato: “ID do objeto” + “setPoint”, sendo setPoint um valor inteiro de 0 a 100.
   	
   Ao aplicar o controle em malha aberta o dimmerizador não se adapta às condições de luminosidade do ambiente, fazendo com que o brilho dos LEDs seja alterado somente pelo aplicativo. Para fazer o dimmerizador se adaptar a luminosidade do ambiente, foi necessário realizar um controle de malha fechada. Dessa forma, o setPoint se torna a entrada do sistema, a luminosidade captada pelo LDR será a variável de controle e a resposta do sistema será o valor aplicado ao PWM que controla o brilho do LED. 
   	
   Com a realimentação, o brilho led começa a variar um pouco com a luminosidade, porém, a variação é quase imperceptível. Foi então adicionado um ganho proporcional Kp =  4, o que possibilitou a luminosidade a atingir o setPoint. Utilizando somente o ganho proporcional, o LED ficou pequenos ruídos, pois a luminosidade nunca atingia o setPoint ideal. 
   	
   Para tentar melhorar o sistema, foi adicionado um compensador derivativo de ganho Kd, que não mostrou resultado satisfatório. Com o ganho alto o compensador derivativo causa instabilidade no sistema aumentando o ruído. Já com o ganho baixo, o compensador teve influência muito baixa sobre a resposta do sistema. O motivo é que a resposta do LDR é quase instantânea, e como a velocidade do clock do arduino é limitada, o derivativo não efetua os cálculos de forma precisa. 
   	
   Foi então adicionado um compensador integrativo de ganho Ki. Com o ganho Ki = 0.5, o compensador mostrou-se bem efetivo, mantendo o brilho sempre em torno do setPoint desejado. Para causar um efeito agradável, o Ki foi fixado em 0.009, fazendo com que as alterações de luminosidade fizessem com o que o LED mudasse seu brilho gradativamente. Com Ki = 0.009, o ganho ideal encontrado para Kp foi 0.3 e para Kd foi 0.006. 
   	
   Com o controle PID ideal encontrado para a aplicação, iniciou-se a montagem do projeto. O circuito ficou montado em uma protoboard e ficou dentro do fundo falso de uma caixa de papelão. Os LDR e LEDs ficaram fixados em cima do fundo falso, e a tampa da caixa de papelão foi usada para controlar a iluminação do sistema.


 

REFERENCIA BIBLIOGRAFICA

[1] - http://www.ni.com/white-paper/3782/pt/
[2] - https://www.arduino.cc/en/Guide/Introduction
[3] - https://produto.mercadolivre.com.br/MLB-830700015-modulo-trasceiver-bluetooth-rs232ttl-arduino-pic-avr-hc06-_JM
[4] - http://www.bulbs.com/learning/dimmers.aspx
