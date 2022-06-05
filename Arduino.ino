uint32_t pwmPin9 = 9; // canal 1   define o pino do PWM
uint32_t pwmPin8 = 8; // canal 2
uint32_t pwmPin7 = 7; // canal 3
uint32_t pwmPin6 = 6; // canal 4

uint32_t channel_1 = g_APinDescription[pwmPin9].ulPWMChannel; // atribui a porta ao PWM
uint32_t channel_2 = g_APinDescription[pwmPin8].ulPWMChannel; 
uint32_t channel_3 = g_APinDescription[pwmPin7].ulPWMChannel; 
uint32_t channel_4 = g_APinDescription[pwmPin6].ulPWMChannel; 


double saida=0.0, saida1=0.0, saida2=0.0, Vo[] = {0.0, 0.0, 0.0}, Vi[] = {0.0, 0.0, 0.0};

void setup() 
{
	pinMode(13,OUTPUT);
	pinMode(0,INPUT);

	analogWriteResolution(12);
  analogReadResolution(12);

	SetPin(pwmPin6); // Habilita o canal 
	SetPin(pwmPin7); // Habilita o canal 
	SetPin(pwmPin8); // Habilita o canal 
	SetPin(pwmPin9); // Habilita o canal 
	  
	pmc_enable_periph_clk(PWM_INTERFACE_ID); // habilita a utilização do Clock.

	// PWMC_ConfigureChannelExt(PWM_INTERFACE,channel_X,PWM_CMR_CPRE_MCK_DIV_2,PWM_CMR_CALG, PWM_CMR_CPOL,0,PWM_CMR_DTE,0,0);  
	PWMC_ConfigureChannelExt(PWM_INTERFACE,channel_1,PWM_CMR_CPRE_MCK_DIV_2,PWM_CMR_CALG, 0,0,PWM_CMR_DTE,0,0); // Configurando do canal 1.  
	PWMC_ConfigureChannelExt(PWM_INTERFACE,channel_2,PWM_CMR_CPRE_MCK_DIV_2,PWM_CMR_CALG, 0,0,PWM_CMR_DTE,0,0); // Configurando do canal 2.  

	PWMC_ConfigureChannelExt(PWM_INTERFACE,channel_3,PWM_CMR_CPRE_MCK_DIV_2,PWM_CMR_CALG, 0,0,PWM_CMR_DTE,0,0); // Configurando do canal 3.
	PWMC_ConfigureChannelExt(PWM_INTERFACE,channel_4,PWM_CMR_CPRE_MCK_DIV_2,PWM_CMR_CALG, 0,0,PWM_CMR_DTE,0,0); // Configurando do canal 4.

	/*===================================================================================================================================  
	           Para definir a frequência de Amostragem/chaveamento, basta definir o CPRD
	PWMC_SetPeriod(PWM_INTERFACE, channel_1, CPRD);     CPRD define a frequência de chaveamento de acordo com CPRD = 84MHz/(2*X*freq) 
	para 10kHz com preSCALER 2 TEM-SE  ->>>>      CPRD = 84MHz/(2*X*freq) = 84MHz/(2*2*10000) = 2100
	X podendo ser atribuído pelos valores  1, 2, 4, 8, 16, 32, 64, 128, 256, 512, ou 1024)
	=====================================================================================================================================*/  
	 
	PWMC_SetPeriod(PWM_INTERFACE, channel_1, 2100);  // para 10kHz
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_1, 1050); // Setando um duty cycle inicial 50%
	PWMC_EnableChannel(PWM_INTERFACE, channel_1); //Ativar o canal para saída PWM
	   
	PWMC_SetPeriod(PWM_INTERFACE, channel_2, 2100); 
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_2, 1050); 
	PWMC_EnableChannel(PWM_INTERFACE, channel_2); //Ativar o canal para saída PWM

	PWMC_SetPeriod(PWM_INTERFACE, channel_3, 2100); 
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_3, 1050); 
	PWMC_EnableChannel(PWM_INTERFACE, channel_3); //Ativar o canal para saída PWM

	PWMC_SetPeriod(PWM_INTERFACE, channel_4, 525); 
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_4, 105); 
	PWMC_EnableChannel(PWM_INTERFACE, channel_4); //Ativar o canal para saída PWM 
   
	PWM_INTERFACE->PWM_IER1 = 0x10; //enable interrupt on channel 4 - 00010000
	PWM_INTERFACE->PWM_IER1 = 0x20; //enable interrupt on channel 5 - 00100000
	       
	NVIC_DisableIRQ(PWM_IRQn); // set up interrupt
	NVIC_ClearPendingIRQ(PWM_IRQn);
	NVIC_SetPriority(PWM_IRQn, 0);
	NVIC_EnableIRQ((IRQn_Type)36); 
	NVIC_EnableIRQ(PWM_IRQn);
	PWMC_EnableChannel(PWM_INTERFACE, channel_1);
	PWMC_EnableChannel(PWM_INTERFACE, channel_2);
	PWMC_EnableChannel(PWM_INTERFACE, channel_3);
	PWMC_EnableChannel(PWM_INTERFACE, channel_4);
}

void loop()      
{
	/*=============================================
	 programa geralmente não faz nada no loop. Apenas espera a interrupção ocorrer. 
	 Aqui qualquer função não prioritária pode ser incluida. 
	 Não utilizar nenhuma função que desabilite as interrupções 
	 =======================================================*/  
}

void PWM_Handler() // Rotina de processamento da interrupção. A cada ciclo de chaveamento, está função é executada uma única vez.
{
	volatile long dummy = PWM_INTERFACE->PWM_ISR1; // clear interrupt flag
	dummy = PWM_INTERFACE->PWM_ISR2; // clear interrupt flag

	digitalWrite(13,HIGH);
	/*=======================================================================================
	Usa-se a porta 13 do arduino como saída para verificar se a interrupção ocorreu. 
	A saída 13 marca o tempo que o código da interrupção demora a ser executado. 
	Equanto a saída 13 está em nível alto, a função da interrupção ainda está sendo executada.
	==========================================================================================
	  
	==========================================================================================
	Insira o código aqui !!!!!!!!  A cada ciclo de chaveamento o arduino roda uma vez este loop.
	Faça aqui a amostragem das entradas, cálculo dos controladores e resultado das saídas dos PWMs.
	================================================================================================ */
	/*
	// Filtro Notch
	Vi[2] = Vi[1];
  Vi[1] = Vi[0];
	Vi[0] = ((analogRead(A0)*3.3)/4095.0) - (1.65/2.0);

  Vo[2] = Vo[1]; 
	Vo[1] = Vo[0];
	Vo[0] = 0.9698010481*(Vi[0]+Vi[2])+1.9054478861*(Vo[1]-Vi[1])-0.9396020961*Vo[2];
	
	analogWrite(DAC1, ((Vo[0]+(1.65/2.0))*3071.0)/1.65);
  */

  // Filtro Passa-Faixa
  Vi[2] = Vi[1];
  Vi[1] = Vi[0];
  Vi[0] = ((analogRead(A0)*3.3)/4095.0) - (1.65/2.0);

  Vo[2] = Vo[1]; 
  Vo[1] = Vo[0];
  Vo[0] = 0.0301989519*(Vi[0]-Vi[2])+1.9054478861*Vo[1]-0.9396020961*Vo[2];
  
  analogWrite(DAC1, ((Vo[0]+(1.65/2.0))*3071.0)/1.65);
  
	/*=================================================
	Fim do código
	Basta agora atualizar os PWMs, EX:
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_1, PWM);   
	onde PWM = Duty * CRPD   = Duty * 2100  para o caso F=10kHz. Onde   0< Duty <1.  
	================================================*/

	// Atualização dos PWMs, 1050 representa um Duty de 50% para freq de 10kHz
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_1, 42000*saida);     
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_2, 42000*saida1); 
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_3, 42000*saida2); 
	PWMC_SetDutyCycle(PWM_INTERFACE, channel_4, 20); 
	digitalWrite(13,LOW);  
	/* Fim da marcação de tempo da interrupção 
	Quando a saída 13 vai a nível baixo, significa que a interrupção acabou.
	Note que o tempo de interrupção DEVE ser menor que o período do PWM. 
	Ou seja, para um PWM de 1kHz, o tempo de interrupção deve ser menor que 1ms.   */
	 
	PWM_INTERFACE->PWM_IDR1 = 0x10; //enable interrupt on channel 4 - 00010000
	  
}// fim da interrupção 

void SetPin(uint8_t pwmPin)
{
	PIO_Configure(g_APinDescription[pwmPin].pPort, 
	g_APinDescription[pwmPin].ulPinType, 
	g_APinDescription[pwmPin].ulPin, 
	g_APinDescription[pwmPin].ulPinConfiguration);
}
