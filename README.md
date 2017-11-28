# Dimmerizador com Controlador PID
 
FUNDAÇÃO EDSON QUEIROZ

UNIVERSIDADE DE FORTALEZA – UNIFOR

Centro de Ciências Tecnológicas – CCT

Controle Digital
Dimmerizador PID
Prof. Afonso Henrique

## AUTORES
**Lucas Bezerra Holanda**

**Marcos Vinicius Cândido Leitão**

**Pedro Benevides Cavalcante**

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

- Lista de Resistores Utilizados:

|    **Resistor**   |    **Valor**      |
|-------------------|-------------------|
|         R8        |        10kΩ       |
|         R9        |         1kΩ       |
|         R10       |         1kΩ       |
|         R12       |       2,2kΩ       |




### A IMPLEMENTAÇÃO

   O programa começa com a inicialização das variáveis do programa e as variáveis de cada controlador PID (um para cada ponto de luz, totalizando quatro). O setPoint, que é variável de entrada do sistema, é inicializado com o valor 85. No setup é configurado cada pino e iniciado o software serial para comunicação bluetooth e o serial padrão para visualizar os dados do projeto no monitor serial. Na função loop() o programa sempre inicia verificando se há dado serial e, caso verdadeiro, os setPoints do projeto são atualizados de acordo com o que foi recebido. Em seguida é chamado a função getPID para cada ponto de luminosidade, que realiza todos os cálculos de controle e retorna um inteiro, que é um valor de 0 a 255. Logo após, é atualizado o PWM de cada LED com os valores obtidos em seu respectivo cálculo de controle e o programa retorna entra em loop.

 
### MELHORIAS NO PID

#### Sample Time
O PID inicial é projetado para ser chamado de forma irregular. Isso causa dois problemas: 
--> O PID não obtem um comportamento consistente, uma vez que ele é chamado com uma frequência totalmente irregular. 
--> É preciso fazer computação matematica extra derivada e integral, uma vez que ambos dependem da mudança de tempo. 

Para isso, a solução foi certificar que o PID seja chamado em um intervalo regular. A maneira decidida para fazer isso é especificar que a função de cálculo seja chamada de cada ciclo com base em um Tempo de Amostra pré-determinado, o PID decide se ele deve calcular ou retornar imediatamente. Uma vez que sabemos que o PID está sendo avaliado em um intervalo constante, os cálculos derivado e integral também podem ser simplificados.

#### Derivative Kick
Uma vez que o erro = Setpoint - entrada, qualquer alteração no Setpoint causa uma alteração instantânea no erro. A derivada desta mudança é infinita (na prática, uma vez que o dt não é 0, apenas acaba sendo um número muito grande). Esse número é alimentado na equação de pid, o que resulta em um pico indesejável na saída.

<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/DonMExplain.png?raw=true" width="350">

A derivada do erro é igual à derivada negativa da entrada, exceto quando o Setpoint está mudando. Isso acaba sendo uma solução perfeita. Em vez de adicionar (Kd * derivative of Error), subtrai-se (Kd * derivative of Input). Isso é conhecido como usando "Derivação na Medição"
A Solução - Passo 1

Há várias maneiras pelas quais o encerramento pode ser mitigado, mas a forma que melhor se adaptou ao projeto foi a seguinte:
Informar ao PID os limites de saída. No código abaixo, será mostrado uma função SetOuputLimits. Uma vez atingido o limite, o PID deixa de somar (integrando.) Ele sabe que não há nada a ser feito; Uma vez que a saída não encerra, recebemos uma resposta imediata quando o ponto de ajuste cai para um alcance em que podemos fazer algo.
<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/No-Windup.png" width="350">

A Solução - Passo 2
Observe no gráfico acima, porém, que, enquanto nos livramos desse atraso de liquidação, não estamos por aí. Ainda há uma diferença entre o que o pid pensa que está enviando e o que está sendo enviado. Por quê? o Prazo Proporcional e (em menor medida) o Prazo Derivativo.

Mesmo que o Termo Integral tenha sido seguramente apertado, P e D ainda estão adicionando seus dois centavos, produzindo um resultado maior que o limite de saída. Se o usuário chama uma função chamada "SetOutputLimits", eles devem assumir que isso significa que "a saída permanecerá dentro desses valores". Assim, para a Etapa 2, fazemos isso uma suposição válida. Além de apertar o I-Term, apertamos o valor de saída para que ele permaneça onde esperamos.

O resultado
<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/No-Winup-Clamped.png" width="350">
Como podemos ver, o enrolamento é eliminado. Além disso, o resultado permanece onde queremos. Isso significa que não há necessidade de aperto externo da saída. Se você quiser que ele varie entre 0 e 256, você pode configurá-los como limites de saída. 

#### Reset Windup
O encerramento do restabelecimento é uma armadilha que provavelmente reivindica mais iniciantes do que qualquer outro. Ocorre quando o PID acha que pode fazer algo que não pode. Por exemplo, a saída PWM em um Arduino aceita valores de 0-255. Por padrão, o PID não conhece isso. Se pensa que 300-400-500 funcionará, ele tentará esses valores esperando obter o que precisa. Como na realidade o valor é apertado em 255, ele continuará tentando números cada vez maiores sem chegar a lugar algum.
O problema revela-se sob a forma de atrasos estranhos. Acima, podemos ver que a saída fica "acabada", acima do limite externo. Quando o ponto de ajuste é descartado, a saída deve diminuir antes de chegar abaixo da linha de 255.
<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/Windup.png" width="350">


#### On-The-Fly Tuning Changes
A capacidade de alterar parâmetros de sintonia enquanto o sistema está sendo executado é uma obrigação para qualquer algoritmo PID respeitável.
Se você alterar os parâmetros enquanto o sistema vai perceber que essas alterações farão o PID se comportar de maneira totalmente inesperada. Isso tudo acontece por causa do controle Integral.
<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/BadIntegral.png" width="350">
A solução para esse problema então é redimensionar errSum. Se o  Ki duplicar, basta cortar o errSum na metade. Isso evita que o termo I fique batendo. 
<img src="https://github.com/lucasbh/Dimmerizador-com-Controlador-PID/blob/master/Imagens/GoodIntegralEqn.png" width="350">


Em vez de ter o Ki vivo fora da integral, trazemos para dentro. Parece que não fizemos nada, mas veremos que na prática isso faz uma grande diferença.

Agora, tomamos o erro e multiplicamo-lo por qualquer que seja o Ki naquele momento. Em seguida, armazenamos a soma dessa. Quando o Ki muda, não há colisão porque todos os antigos Ki já estão "no banco", por assim dizer. Recebemos uma transferência suave sem operações matemáticas adicionais. Isso pode me fazer um geek, mas acho que isso é muito sexy.

## FUNCIONAMENTO

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
