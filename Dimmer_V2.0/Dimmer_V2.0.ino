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
int flagManAuto = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  pinMode(ldr0, INPUT);
  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(ldr3, INPUT);
  
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop() {
  
  //Leitura do bluetooth
  getSerial();        

  if(!flagManAuto)      // Se a flag estiver em automatico (false)
  {
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
  }else                 // Se a flag estiver em manual (true)
  {
    int ledManual = setPoint*2.55;
    analogWrite(led0, ledManual);
    analogWrite(led1, ledManual);
    analogWrite(led2, ledManual);
    analogWrite(led3, ledManual);
  }
}


float getPID(int ledRead, const int &LED)
{
//A função getPID retorna o controlador PID, que está limitado a um valor entre 0 e 255.
//O switchcase analisa qual LDR e setPoint foram passados na função e executa o PID com base nisso

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

  //Encontrando E(t) (erro)
  int valorAnterior = *valorAtual;
  *valorAtual = ledRead;
  int lastErro = *erro;
  *erro = setPoint - *valorAtual;

  //Encontrando dt
  unsigned long tempoAnterior = *tempoAtual;
  *tempoAtual = millis();
  double dt = *tempoAtual - tempoAnterior;

  float p = *erro * kp;                                   // P = kp*E(t)
  *i += *erro * ki * dt;                                  // i = ki*SE(t)*dt
  if(*i > 200) { *i = 200; }                              // i está limitado a -50 e 200
  if(*i < -50)   { *i = -50; }
  float d = kd * ((*erro - lastErro) / dt);               // d = kd*(dE(t)/d(t))
  
  float PID = 50 + p + *i + d;
  if(PID > 255) { PID = 255; }
  if(PID < 0) { PID = 0; }

  return PID;
}

void getSerial()
{
//Leitura dos dados via bluetooth
//O app envia o ID do objeto mais o valor. EX: setar o slider da sala0 para 68% envia a string "L060"

  String serialRead = "";
  while(mySerial.available())       //Leitura do dado serial
  {
    char c = mySerial.read();
    serialRead += c;
    delay(2);
  }
  if(serialRead.length() > 0)
  {
    String strCmp = serialRead.substring(0,2);                           //Comparação do dado recebido p/ executar uma ação

    // atualiza o setPoint
    if(strCmp == "L0") { setPoint = serialRead.substring(2).toInt(); }

    // setTunings: Altera os ganhos do controlador
    if(strCmp == "p+") { kp = kp + 0.1; }        
    if(strCmp == "p-") { kp -= 0.1; }
    if(strCmp == "i+") { ki += 0.001; }
    if(strCmp == "i-") { ki -= 0.001; }
    if(strCmp == "d+") { kd += 0.001; }
    if(strCmp == "d-") { kd -= 0.001; }

    // on/off(Auto/Man): altera de manual para automatico.
    if(strCmp == "FA") { flagManAuto = 0; }
    if(strCmp == "FM") { flagManAuto = 1; }
  }
}

void sendSerial()         
{
// Envia dados para o monitor serial e para o aplicativo.

  if(millis() - lastTime > 1000){
    if(!flagManAuto){
      lastTime = millis();
      String msgPID = String((float)pid0) + " " + String((float)pid1) + " " + String((float)pid2) + " " + String((float)pid3);
      String msgLDR = "LDR 0: " + String((int)ldrRead0) + " LDR 1: " + String((int)ldrRead1) + " LDR 2: " + String((int)ldrRead2);
      Serial.println(msgPID + "\t" + msgLDR);
      
      mySerial.println("kp " + String((float)kp));
      mySerial.println("ki " + String((float)ki));
      mySerial.println("kd " + String((float)kd));
      mySerial.println("M0");
      mySerial.println("A1");
    }
    else{
      mySerial.println("M1");
      mySerial.println("A0");
    }
  }
}