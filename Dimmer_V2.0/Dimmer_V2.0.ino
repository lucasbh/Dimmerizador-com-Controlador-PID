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
float kp = 0.3,  ki = 0.09,  kd = 0.06,  i0, i1, i2, i3;
int setPoint = 85;                                                  //O setPoint de luminosidade sempre começa em 85% ???
int erro0, erro1, erro2, erro3;
int lastValor0, lastValor1, lastValor2, lastValor3;
unsigned long lastT0 = 0, lastT1 = 0, lastT2 = 0, lastT3 = 0;
unsigned long lastTime = 0;
int tAmostragem = 100;     // Tempo de amostragem = 0.2s
float pid0, pid1, pid2, pid3;
int pwmMin = 0, pwmMax = 255; 

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
    if(pid0 != -1){analogWrite(led0, pid0);}
    
    pid1 = getPID((ldrRead0/2 + ldrRead1/2), led1); 
    if(pid1 != -1){analogWrite(led1, pid1);}
    
    pid2 = getPID(ldrRead1/2 + ldrRead2/2, led2);
    if(pid2 != -1){analogWrite(led2, pid2);}
    
    pid3 = getPID(ldrRead2, led3);
    if(pid3 != -1){analogWrite(led3, pid3);}
  
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


float getPID(int ldrRead, const int &LED)
{
//A função getPID retorna o controlador PID, que está limitado a um valor entre 0 e 255.
//O switchcase analisa qual LDR e setPoint foram passados na função e executa o PID com base nisso

  int *lastValor, *erro;
  float *i;							//Soma dos erros
  unsigned long *lastTempo;

  switch (LED)
	  {
	    case led0:
	    {
	      lastValor = &lastValor0;
	      erro = &erro0;
	      lastTempo = &lastT0;
	      i = &i0;
	    }break;
	    case led1:
	    {
	      lastValor = &lastValor1;
	      erro = &erro1;
	      lastTempo = &lastT1;
	      i = &i1;
	    }break;
	    case led2:
	    {
	      lastValor = &lastValor2;
	      erro = &erro2;
	      lastTempo = &lastT2;
	      i = &i2;
	    }break;
	    case led3:
	    {
	      lastValor = &lastValor3;
	      erro = &erro3;
	      lastTempo = &lastT3;
	      i = &i3;
	    }break;
  }

  //Encontrando dt
  //unsigned long tempoAnterior = *tempoAtual;
  unsigned long Agora = millis();
  double difTempo = Agora - *lastTempo;
  if(difTempo>=tAmostragem)						     	 //Se a variação de tempo(dt) for maior que o tempo de amostragem, executa o codigo
  {
    //Encontrando E(t) (erro)
    *erro = setPoint - ldrRead;							 //Erro

    double dInput = ldrRead - *lastValor;				 // Melhoria Derivative Kick: Qualquer mudança no setPoint nao causara oscilação
    													//dInput = input - lastInput

    float p = *erro * kp;                                // P = kp*E(t)

    *i += *erro * ki;                              		 // i = ki*S(E(t)*dt)  --->  i = S(ki*E(t)*dt) Melhoria tuning changes
    											  	     // Com a melhoria sample time, sabemos que o intervalo
    													 // do tempo de execução é constante e dt não é mais necessário.
    if(*i > pwmMax) { *i = pwmMax; }                  	 // a integração está limitada a 0 e 255
    if(*i < pwmMin) { *i = pwmMin; }
		 
    											  	     // Com a melhoria sample time, sabemos que o intervalo
    													 // do tempo de execução é constante e dt não é mais necessário.

    float d = kd * dInput;               				 // d = kd*(dE(t)/d(t))			--> Com a melhoria

    float PID = p + *i - d;							 // Com a melhoria derivative kick, o compensador se torna negativo. 
    if(PID > pwmMax) { PID = pwmMax; }
    if(PID < pwmMin) { PID = pwmMin; }

    *lastValor = ldrRead;
    *lastTempo = Agora;
    return PID;
  }else return -1;
}

void setAmostragem(int tNovo)
{
//Seta o tempo de amostragem
  if(tNovo > 0)
  {
    double ratio = ((double)tNovo / (double)tAmostragem);
    ki *= ratio;
    kd /= ratio;
    tAmostragem = (unsigned long)tNovo;
  }
}

void setTunings(String strCmp, int ganho)
{
//Melhoria setTunings. 
	if(strCmp == "kp") 
	{
		kp = (float)ganho / 1000; 
	}
	if(strCmp == "ki") 
	{
		int KI = (float)ganho / 1000; 
		ki = KI * tAmostragem;
	}
	if(strCmp == "kp") 
	{
		int KD = (float)ganho / 1000;
		kd = KD / tAmostragem;
	}
}


/*void setOutLimit(double Min, double Max)
{
//Melhoria reset windup.
   if(Min > Max) return;
   pwmMin = Min;
   pwmMax = Max;
    
   if(PID > pwmMax) PID = pwmMax;
   else if(PID < pwmMin) PID = pwmMin;
 
   if(i> outMax) i= outMax;
   else if(i< outMin) i= outMin;
}*/

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
    
    // on/off(Auto/Man): altera de manual para automatico.
    if(strCmp == "FA") { flagManAuto = 0; }
    if(strCmp == "FM") { flagManAuto = 1; }
    
    // setTunings: Altera os ganhos do controlador
    if(strCmp == "kp" || "ki" || "kd")
    {
		int ganho = serialRead.substring(2).toInt();
		setTunings(strCmp, ganho);
	}

	//setSampleTime
	if(strCmp == "ST") 
	{
		int newAmostragem = serialRead.substring(2).toInt();
		setAmostragem(newAmostragem); 
	}
  }
}

void sendSerial()         
{
// Envia dados para o monitor serial e para o aplicativo.

  if(millis() - lastTime > 1000){
    if(!flagManAuto){
      lastTime = millis();
      String msgPID = String((float)pid0) + " | " + String((float)pid1) + " | " + String((float)pid2) + " | " + String((float)pid3);
      String msgLDR = "LDR 0: " + String((int)ldrRead0) + "| LDR 1: " + String((int)ldrRead1) + "| LDR 2: " + String((int)ldrRead2);
      Serial.println(msgPID + "\t" + msgLDR);
      
      mySerial.println("kp " + String((float)kp));
      mySerial.println("ki " + String((float)ki));
      mySerial.println("kd " + String((float)kd));
    }
  }
}