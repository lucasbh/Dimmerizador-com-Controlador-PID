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
  int ledRead0 = map(analogRead(ldr0), 1023, 7, 0, 100);
  float pid0 = getPID(ledRead0, led0);
  analogWrite(led0, pid0);
  int ledRead1 = map(analogRead(ldr0), 1023, 7, 0, 100)/2 + map(analogRead(ldr1), 1020, 5, 0, 100)/2;
  float pid1 = getPID(ledRead1, led1); 
  analogWrite(led1, pid1);
  int ledRead2 = map(analogRead(ldr1), 1020, 5, 0, 100)/2 + map(analogRead(ldr2), 1023, 10, 0, 100)/2;
  float pid2 = getPID(ledRead2, led2);
  analogWrite(led2, pid2);
  int ledRead3 = map(analogRead(ldr2), 1023, 10, 0, 100);
  float pid3 = getPID(ledRead3, led3);
  analogWrite(led3, pid3);

//Dados para o monitor serial
  if(millis() - lastTime > 1000){
    lastTime = millis();
    int ldrr0 = map(analogRead(ldr0), 1023, 7, 0, 100);
    int ldrr1 = map(analogRead(ldr1), 1020, 5, 0, 100);
    int ldrr2 = map(analogRead(ldr2), 1023, 10, 0, 100);
    /*int ldrr0 = analogRead(ldr0);
    int ldrr1 = analogRead(ldr1);
    int ldrr2 = analogRead(ldr2);*/
    String msgPID = String((float)pid0) + " " + String((float)pid1) + " " + String((float)pid2) + " " + String((float)pid3);
    String msgLDR = String((int)ldrr0) + " " + String((int)ldrr1) + " " + String((int)ldrr2);
    Serial.println(msgPID + "\t" + msgLDR);
  }
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
        if(serialRead.substring(0,2) == "p+")
    {
      kp = serialRead.substring(2).toFloat();
    }
  }
}

