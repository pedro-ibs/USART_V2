# USART_V2

Copyright (C) Pedro Igor B. S 2019
Arquivo: USART_V2.h USART_V2.cpp

Descrição: Esse é uma bibliotéca para facilitar o uso do

periferio USART do microcontrolador AVR ATMEGA328P

Autores: Pedro Igor Borçatti da Silva

Licença: GNU GPL 2

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; version 2 of the
License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***
***

## Resumo

Essa biblioteca para abstrair os recursos do periférico de comunicação serial USART sem que diminua o controle sobre o mesmo, ela está separada entre funções e macros que permitem configurar e usar os recursos da USART. Sua principal característica é que a mesma utiliza a interna para tratar o recebimento dos dados permitindo maior dinâmica no código desenvolvido com ela. essa biblioteca foi desenvolvida baseada na informação do datasheet do [ATMEGA38P-PU/AU](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf), aconselho sempre conferir as infomações fonecidas pela microchip des dos datasheet até as application notes.

---

## Como a  USART_V2 Recebe os dados
Para receber os dados a biblioteca utiliza o vetor de interrupção  **USART_RX_vect**, que é ativa assim que um **Frame** é recebido, assim que um Frame é recebido pelo RXD é verificado se o buffer está cheio se nesse caso o dado é descartado para não comprometer os dados já recebidos. Se o buffer estiver vazio o dado é recebido e o buffer montado.

Esse é o único ponto em que a biblioteca não é flexível, mas utilizando a interrupção interna evitamos a travar o código para receber dados,  além de otimizar nossos processos e ter uma  garantia de que o dado vão ser. 

```
ISR(USART_RX_vect){
	/* trava de buffer cheio */
	if( _contBFF < _lenBFF ){
		/* Espera a limpeza do registr. de transmissão */
		while( ! ( UCSR0A & ( 1 << RXC0 ) ) );
		/*ler o dado recebido e retorna e armazenar no buffer*/
		_buffer[ _contBFF++ ] = UDR0;
	} else {
		/*jogar fora*/
		trash  = UDR0;
	}
}
```

Para configurar os o tamanho do buffer mude o valor da macro **SIZE_BUFFER** em **USART_V2.h**, por padrão o tamanho é de **128 caracteres**:

```
#define SIZE_BUFFER     128
```

## Configurações Gerais
### transmissão e Recepção de Dados

Com as macros definidas em **USART_V2.h** podemos configurar alguns pontos no fluxo de transmissão e ou recepção de dados. 


### **Transmissão**
```
/* habilitar transmissão da USATR ( PIN TXD )  */
OpenTXD;
/* desabilitar transmissão da USATR ( PIN TXD )  */
CloseTXD;
```

### **Recepção**
```
/* habilitar recepção da USATR ( PIN TRD )  */
OpenRXD;
/* desabilitar recepção da USATR ( PIN TRD )  */    
CloseRXD;
```

### **Interrupções para Recepção e ou Transmissão de dados**
```
/* habilitar interrupção para byte recebido */
Received_ON;     
/* desabilitar interrupção para byte recebido */ 
Received_OFF     

/* habilitar interrupção para byte enviado */
Send_ON;
/* desabilitar interrupção para byte enviado */
Send_OFF;

/* habilitar interrupção para buffer UDR0 cheio */
DataEmpty_OFF; 
/* desabilitar interrupção para buffer UDR0 cheio */     
DataEmpty_ON;   
```
Para utilizar os vetores das Interrupções  é preciso ter a função com o vetor respectivo, Observe que para utilizar esse recurso possivelmente alguns pontos dessa biblioteca deverá ser alterado do contrário sua alteração poderá não funcionar corretamente.

```
/* Para dados recebidos */
ISR(Received){
	/* seu códigoo */
}

/* Para dados enviados  */
ISR(Send){
	/* seu códigoo */
}

/* Para buffer UDR0 cheio  */
ISR(DataEmpty){
	/* seu códigoo */
}

```

***

## Modo de Operação

A usart pode operar em 3 modos, o modo Síncrono, Assíncrono e SPI Master, cada modo poderá lidar com os pinos do microcontrolador de forma diferrente veja  as docummentações do [ATMEGA38P-PU/AU](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf) para poder adequar seu projeto corretamente para cada um desses modos.

```
/* USART em modo assíncrono */
usart_Asynchronous();

/* USART em modo síncrono */
usart_Synchronous();

/* USART em modo SPI mestre */
usart_SpiMaster();
```

***

## Configuração do Frame
Essas Configurações  determinam o tamanho do frame e seus bits de controle.

```
/* quantidade de  Stop bit */
StopBit_1; // um bit de parada
StopBit_2; // dois bits de parada

/* tamanho do Frame */
usart_FrameBit5();
usart_FrameBit6();
usart_FrameBit7();
usart_FrameBit8();	// tamanho de um byte
usart_FrameBit9();

/* bits de paridade  */
usart_ParityOdd();	// bit de paridade ímpar
usart_ParityEven();	// bit de paridade par
usart_ParityNone();	// sem bit de paridade
```


# Usando Em modo Assíncrono
Usar a USART em modo Assíncrono é bastante comum pois requer apenas o uso de dois pinos, o TXD e o rXD, Para usar essa biblioteca que em modo assíncrono é necessário carregar as configurações adequadas sendo essa a primeira etapa da configuração.

A biblioteca irá montar um buffer do tipo char conforme os dados são recebidos o acesso desse buffer se dá através de um ponteiro, nesse caso é necessário criar um ponteiro do tipo char (**bff**)   para poder receber o endereço do buffer montado pela biblioteca. Posteriormente começamos a configurar a Usart:


Configuração	| Valor
----------------| --------:
Frame		| 8 bits
bit de Paridade	| nenhum
Bit de Parada	| 1 bit
BaudRate	| 9600 (96kbps)


```
#include "USART_V2.h"
#include <stdio.h>

int main(void){	
	/* ponteiro  */
	char *bff = NULL;
	/* usaet em modo assincrono  */
	usart_Asynchronous();
	/* sem bit de paridade */
	usart_ParityNone();
	/* um bit de parada */
	StopBit_1;
	/* tamanho do frame de 8 bits */
	usart_FrameBit8();
	
	/* velocidade de comunicação 96kbps */
	usart_BaudRate(9600, F_CPU);				
```

Após isso capturamos o endereço do buffer da biblioteca e armazenamos em um variável que aponta para ele (**bff**), vamos utilizar essa variável para ler as informações recebidas 

	
```
	/* capturar addr do vetor do buffer */
	bff = usart_ReadBuffer();
```


Agora podemos manipular os dados, no caso desse programa ele  fará algo assim que estiver com algo no buffer, um **delay** é utilizado para garantir que todos os dados sejam recebidos, após isso é enviado os mesmo dados que o MCU recebeu com a função **usart_PrintStrLn(bff)**, como ela recebe **bff** que aponta para o buffer da biblioteca os dados serão os mesmo que os recebidos, por fim o buffer é limpo com  **usart_CleanBuffer(0x00);** isso sempre deve ser feito pois senão o contador do buffer não será zerado, o parâmetro que essa função recebe é o valor que sobrescreve o buffer todo  no processo de limpeza  


```
	while(true){
		/* se o buffer tiver mais de um byte  */
		if(usart_SizeBuffer() > 1){
			/* aguadar receber todos os dados  */
			_delay_ms(100);
			/* imprimir dados até 0x00  */
			usart_PrintStrLn(bff);
			/* limpar buffer */
			usart_CleanBuffer(0x00);
		}
	}
	return 0;
}
```
