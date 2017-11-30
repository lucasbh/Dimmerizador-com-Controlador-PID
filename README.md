# Dimmerizador com Controlador PID
 
**FUNDAÇÃO EDSON QUEIROZ**

**UNIVERSIDADE DE FORTALEZA – UNIFOR**

**Centro de Ciências Tecnológicas – CCT**

Controle Digital

Dimmerizador PID

**Prof. Afonso Henrique**

## AUTORES
**Lucas Bezerra Holanda**

**Marcos Vinicius Cândido Leitão**

**Pedro Benevides Cavalcante**

2017
Fortaleza – CE


## O Projeto

   A base do projeto será a programação implementada numa plataforma Arduino, na qual efetuará todas as funções de PID do sistema, recebendo as leituras dos LDRs e convertendo em sinais controlados para que os Leds possam ser dimerizados e assim efetuar a iluminação necessária. 


**Tabela 1 - Material Utilizado**

|        Componentes         |                                 Descrição                                  |
|----------------------------|----------------------------------------------------------------------------|
| - Arduino Uno              | Plataforma de prototipagem eletrônica de hardware livre.                   |
| - 4 Led's de Alto Brilho   | Diodo usado para emissão de luz.                                           |
| - 3 LDR's                  | Resistor com resistência variando com a luz incidida sobre ele.            |
| - Módulo HC-06             | Módulo Bluetooth responsável por enviar e receber dados através da serial. |
| - Placa de Fibra de Vidro  | Placa dupla face utilizada pra desenvolver a PCi do projeto.               |
| - Resistores               | Valores dos Resistores mencionados na tabela 2.                            |


### SOFTWARES UTILIZADOS

- ROBOREMO

   Roboremo é um aplicativo para android gratuito disponível na playstore que é utilizado para desenvolver simples interfaces e se comunicar de diferentes formas. O aplicativo se comunicará com o arduino via bluetooth, e este envia os dados inseridos na tela de acordo com o que foi programado pelo usuário. 

- PROTEUS 

   O Proteus Design Suite é um Electronic Design Automation ferramenta (EDA), incluindo captura esquemática, simulação e módulos de layout PCi. 
	
### DESENVOLVIMENTO DE PCi

   Para otimizar o espaço como a organização da parte física, resolvemos desenvolver uma pci para o projeto. Utilizando a ferramenta do Proteus de desenvolvimento de PCB, e partindo do esquemático desenvolvido no próprio Proteus. A placa foi desenvolvida para ser usada como um shield no Arduino Uno utilizado no projeto. As imagens necessárias para realizar a impressão estão listadas abaixo:
   
**Parte Superior da PCI**|**Parte Inferior da PCI**|
 :-----------------------------:|:-------------------------------:|
<img src="https://user-images.githubusercontent.com/31712391/33335012-8aef9f06-d44a-11e7-9d7b-d009ee8ab9fc.png" width="250">|<img src="https://user-images.githubusercontent.com/31712391/33335118-e61d5990-d44a-11e7-9e0d-dc828838e3e3.png" width="250">

- Após realizar a impressão e  soldar os componetes a placa fica semelhante ao que é mostrado no modelo 3D abaixo:

|**PCI Superior em 3D**|**PCI Inferior em 3D**|
|:-----------------------------:|:-------------------------------:|
<img src="https://user-images.githubusercontent.com/31712391/33335241-41cbefea-d44b-11e7-912b-8225e9aac878.png" width="250">|<img src="https://user-images.githubusercontent.com/31712391/33335263-527d603a-d44b-11e7-8f97-2afcf3bdd9e2.png" width="250">

**Tabela 2 - Lista de Resistores Utilizados:**

|    **Resistor**   |    **Valor**      |
|-------------------|-------------------|
|         R8        |        10kΩ       |
|         R9        |         1kΩ       |
|         R10       |         1kΩ       |
|         R12       |       2,2kΩ       |




### A IMPLEMENTAÇÃO

   O programa começa com a inicialização das variáveis do programa e as variáveis de cada controlador PID (um para cada ponto de luz, totalizando quatro). O setPoint, que é a variável de entrada do sistema, é inicializado com o valor 85. No setup é configurado cada pino e iniciado o software serial para comunicação bluetooth e o serial padrão para visualizar os dados do projeto no monitor serial. Na função loop() o programa sempre inicia verificando se há dado serial e, caso verdadeiro, os setPoints do projeto são atualizados de acordo com o que foi recebido. Em seguida é chamado a função getPID para cada ponto de luminosidade, que realiza todos os cálculos de controle e retorna um inteiro, que é um valor de 0 a 255. Logo após, é atualizado o PWM de cada LED com os valores obtidos em seu respectivo cálculo de controle e o programa retorna entra em loop.

 
#### - MELHORIAS NO PID

- SAMPLE TIME

		loat kp = 0.3,  ki = 0.09,  kd = 0.06,  i0, i1, i2, i3;
		int lastValor0, lastValor1, lastValor2, lastValor3;
		unsigned long lastT0 = 0, lastT1 = 0, lastT2 = 0, lastT3 = 0;
		unsigned long lastTime = 0;
		int tAmostragem = 100;     // Tempo de amostragem = 0.2s
		float pid0, pid1, pid2, pid3;
		int pwmMin = 0, pwmMax = 255; 
				// Com a melhoria sample time, sabemos que o intervalo
				// do tempo de execução é constante e dt não é mais necessário.
	    if(*i > pwmMax) { *i = pwmMax; }           // a integração está limitada a 0 e 255
	    if(*i < pwmMin) { *i = pwmMin; }

				// Com a melhoria sample time, sabemos que o intervalo
				// do tempo de execução é constante e dt não é mais necessário.

- DERIVATE KICK

	  //unsigned long tempoAnterior = *tempoAtual;
	  unsigned long Agora = millis();
	  double difTempo = Agora - *lastTempo;
	  if(difTempo>=tAmostragem)		//Se a variação de tempo(dt) for maior que o tempo de amostragem, executa o codigo
	  {
	    //Encontrando E(t) (erro)
	    *erro = setPoint - ldrRead;		 //Erro

	    double dInput = ldrRead - *lastValor;	// Melhoria Derivative Kick: Qualquer mudança no setPoint nao causara oscilação
					//dInput = input - lastInput

	    float p = *erro * kp;   // P = kp*E(t)
	    *i += *erro * ki;    // i = ki*S(E(t)*dt)  --->  i = S(ki*E(t)*dt) Melhoria tuning changes
				// Com a melhoria sample time, sabemos que o intervalo
				// do tempo de execução é constante e dt não é mais necessário.
	    if(*i > pwmMax) { *i = pwmMax; }           // a integração está limitada a 0 e 255
	    if(*i < pwmMin) { *i = pwmMin; }

				// Com a melhoria sample time, sabemos que o intervalo
				// do tempo de execução é constante e dt não é mais necessário.

	    float d = kd * dInput;       // d = kd*(dE(t)/d(t))			--> Com a melhoria

	    float PID = p + *i - d;		 // Com a melhoria derivative kick, o compensador se torna negativo. 
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
 
- ON/OFF (AUTOMATE/MANUAL)

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

- Initialization 




## FUNCIONAMENTO

   O dimmerizador controla o nível de luminosidade do ambiente automaticamente e se adapta a mudanças de claridade, aumentando ou diminuindo o brilho dos LEDs de acordo a luminosidade captada pelo LDR. No projeto desenvolvido, foi projetado três pontos de leitura de luminosidade e quatro prontos e iluminação, com LDRs e LEDs respectivamente. 

   No aplicativo Roboremo foi desenvolvido uma interface com três sliders, em que cada um representa o nível de luminosidade desejado em seu respectivo ponto de leitura. A aplicação envia uma string por bluetooth para o arduino, que processará a informação de acordo com o que foi programado e mudará o setPoint do ponto de luminosidade selecionado. A string enviada possui o seguinte formato: “ID do objeto” + “setPoint”, sendo setPoint um valor inteiro de 0 a 100.
   
   **Interface do aplicativo de controle criado no Roboremo**
   
   <img src="https://user-images.githubusercontent.com/31712391/33371475-0277b01c-d4e2-11e7-8d11-d407ddcf4671.jpeg" width="250">

   	
   Ao aplicar o controle em malha aberta o dimmerizador não se adapta às condições de luminosidade do ambiente, fazendo com que o brilho dos LEDs seja alterado somente pelo aplicativo. Para fazer o dimmerizador se adaptar a luminosidade do ambiente, foi necessário realizar um controle de malha fechada. Dessa forma, o setPoint se torna a entrada do sistema, a luminosidade captada pelo LDR será a variável de controle e a resposta do sistema será o valor aplicado ao PWM que controla o brilho do LED. 
   	
   Com a realimentação, o brilho led começa a variar um pouco com a luminosidade, porém, a variação é quase imperceptível. Foi então adicionado um ganho proporcional Kp =  4, o que possibilitou a luminosidade a atingir o setPoint. Utilizando somente o ganho proporcional, o LED ficou pequenos ruídos, pois a luminosidade nunca atingia o setPoint ideal. 
   	
   Para tentar melhorar o sistema, foi adicionado um compensador derivativo de ganho Kd, que não mostrou resultado satisfatório. Com o ganho alto o compensador derivativo causa instabilidade no sistema aumentando o ruído. Já com o ganho baixo, o compensador teve influência muito baixa sobre a resposta do sistema. O motivo é que a resposta do LDR é quase instantânea, e como a velocidade do clock do arduino é limitada, o derivativo não efetua os cálculos de forma precisa. 
   	
   Foi então adicionado um compensador integrativo de ganho Ki. Com o ganho Ki = 0.5, o compensador mostrou-se bem efetivo, mantendo o brilho sempre em torno do setPoint desejado. Para causar um efeito agradável, o Ki foi fixado em 0.009, fazendo com que as alterações de luminosidade fizessem com o que o LED mudasse seu brilho gradativamente. Com Ki = 0.009, o ganho ideal encontrado para Kp foi 0.3 e para Kd foi 0.006. 
   	
   Com o controle PID ideal encontrado para a aplicação, iniciou-se a montagem do projeto. O circuito ficou montado em uma protoboard e ficou dentro do fundo falso de uma caixa de papelão. Os LDR e LEDs ficaram fixados em cima do fundo falso, e a tampa da caixa de papelão foi usada para controlar a iluminação do sistema.
   
   |**Arduino com Shield conectado**|**Suporte do projeto com Led's e Ldr's**|
|:-----------------------------:|:-------------------------------:|
<img src="https://user-images.githubusercontent.com/31712391/33371571-5988c1ca-d4e2-11e7-836f-f11c57cf7ee9.jpeg" width="250">|<img src="https://user-images.githubusercontent.com/31712391/33371574-5b2f9d50-d4e2-11e7-8559-545ddc5f4e78.jpeg" width="250">



 

REFERENCIA BIBLIOGRAFICA

[1] - http://www.ni.com/white-paper/3782/pt/
[2] - https://www.arduino.cc/en/Guide/Introduction
[3] - https://produto.mercadolivre.com.br/MLB-830700015-modulo-trasceiver-bluetooth-rs232ttl-arduino-pic-avr-hc06-_JM
[4] - http://www.bulbs.com/learning/dimmers.aspx
