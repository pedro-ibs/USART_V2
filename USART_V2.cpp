// -------------------------------------------------------------------
//   Copyright (C) Pedro Igor B. S 2019
// -------------------------------------------------------------------
//   Arquivo: USART.h
//   Descrição: Esse progrma é uma bibliotéca para facilitar o uso do
//   periferio USART do microcontrolador AVR ATMEGA328P
//   Autores: Pedro Igor Borçatti da Silva
//
//   ****************************************************************
//   Nesta versão algumas funções foram removidas e outras retraba-
//   lhadas a princípio a diferença estão nos nomes das funções que
//   indicam o periférico: "usart_blablabla" e principalmente em
//   como o buffer e montado é capturado, nessa versão o buffer é
//   montado na interrupção e capturamos ele através de um ponteiro
//   isso para otimizar memória e desempenho. Uma peculiaridade dessa
//   versão é que o buffer não é apagado na sua leitura isso deve
//   ser feito por outra função
//   ****************************************************************
//
//   Licença: GNU GPL 2
// -------------------------------------------------------------------
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; version 2 of the
//   License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
// -------------------------------------------------------------------

#include <avr/io.h>									// lib para AVR
#include <avr/interrupt.h>								// lib para inte
#include "USART_V2.h"									// my lib

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const uint16_t	_lenBFF		= SIZE_BUFFER;						// tamanho do buffer
uint16_t	_contBFF	= 0;							// contador de byte
uint8_t	_buffer [ _lenBFF ];								// buffer
uint8_t trash;

/* Private Functions ---------------------------------------------------------*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=Modo de operação===================================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usart_Asynchronous(void){								// USART em mondo assincrono
	ClrBit( UCSR0C, UMSEL01 );							// setar bit de UCSR0C para modo assincrono
	ClrBit( UCSR0C, UMSEL00 );							// setar bit de UCSR0C para modo assincrono
	ClkPolarity_0;									// deve ser zerado para mondo assincrono
}

void usart_Synchronous(void){								// USART em mondo sincrono
	ClrBit( UCSR0C, UMSEL01 );							// setar bit de UCSR0C para modo sincrono
	SetBit( UCSR0C, UMSEL00 );							// setar bit de UCSR0C para modo sincrono
	ClkPolarity_1;									// ajusta clk XCK para amostragem e saída de dados
}

void usart_SpiMaster(void){								// USART em modo SPI mestre
	SetBit( UCSR0C, UMSEL01 );							// setar bit de UCSR0C para modo SPI mestre
	SetBit(	UCSR0C, UMSEL00 );							// setar bit de UCSR0C para modo SPI mestre
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=BIT DE PARIDADE ===================================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usart_ParityOdd(void){								// bit de paridade impar
	SetBit( UCSR0C, UPM01 );							// setar bit de UCSR0C para paridade impar
	SetBit( UCSR0C, UPM00 );							// setar bit de UCSR0C para paridade impar
}

void usart_ParityEven(void){								// bit de paridade par
	SetBit( UCSR0C, UPM01 );							// setar bit de UCSR0C paridade par
	ClrBit( UCSR0C, UPM00 );							// setar bit de UCSR0C paridade par
}

void usart_ParityNone(void){								// sem bit de paridade
	ClrBit( UCSR0C, UPM01 );							// setar bit de UCSR0C sem paridade
	ClrBit( UCSR0C, UPM00 );							// setar bit de UCSR0C sem paridade
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=BIT tamanho do frame ==============================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usart_FrameBit5(void){								// frame com 5 bits
	ClrBit( UCSR0B, UCSZ02 );							// configurar frame para 5 bits
	ClrBit( UCSR0C, UCSZ01 );							// configurar frame para 5 bits
	ClrBit( UCSR0C, UCSZ00 );							// configurar frame para 5 bits
}

void usart_FrameBit6(void){								// frame com 6 bits
	ClrBit( UCSR0B, UCSZ02 );							// configurar frame para 6 bits
	ClrBit( UCSR0C, UCSZ01 );							// configurar frame para 6 bits
	SetBit( UCSR0C, UCSZ00 );							// configurar frame para 6 bits
}

void usart_FrameBit7(void){								// frame com 7 bits
	ClrBit( UCSR0B, UCSZ02 );							// configurar frame para 7 bits
	SetBit( UCSR0C, UCSZ01 );							// configurar frame para 7 bits
	ClrBit( UCSR0C, UCSZ00 );							// configurar frame para 7 bits
}

void usart_FrameBit8(void){								// frame com 8 bits
	ClrBit( UCSR0B, UCSZ02 );							// configurar frame para 8 bits
	SetBit( UCSR0C, UCSZ01 );							// configurar frame para 8 bits
	SetBit( UCSR0C, UCSZ00 );							// configurar frame para 8 bits
}

void usart_FrameBit9(void){								// frame com 9 bits
	SetBit( UCSR0B, UCSZ02 );							// configurar frame para 9 bits
	SetBit( UCSR0C, UCSZ01 );							// configurar frame para 9 bits
	SetBit( UCSR0C, UCSZ00 );							// configurar frame para 9 bits
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=INICIANDO USART ===================================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  usart_BaudRate( unsigned long int BaudRate, unsigned long int freg){		// iniciar porra serial
	uint8_t conf	= (uint8_t) ( freg / 16 / BaudRate - 1	);			// converter dados de entrada
	UBRR0H			= ( conf  >> 8			);			// configurar baud rate
	UBRR0L 			= ( conf 			);												// configurar baud rate
	OpenTXD;									// abrir canal TXD
	OpenRXD;									// abrir canal RXD
	Received_ON;									// habilitar interrupção para byte recebido
	sei();										// habilitar interrupção geral
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=ENVIAR DADOS ======================================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usart_PrintChr( uint8_t chr ){							// enviar um carecter
	while( ! ( UCSR0A & ( 1 << UDRE0 ) ) );						// Espera a limpeza do registr. de transmissão
	UDR0 = chr;									// Coloca o dado no registrador e o envia
}

void usart_PrintStr( const char *str ){							// enviar uma String
	while(*str) usart_PrintChr(*str++);
}

void usart_PrintStrLn( const char *str){						// enviar istring e pular linha
	usart_PrintStr(str);
	usart_PrintChr(0x0A);
}

void usart_PrintStrChr( const char *str, uint8_t chr){					// enviar istring e uma char
	usart_PrintStr(str);
	usart_PrintChr(chr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//=Lel Buffer ========================================================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t *usart_ReadBuffer(void){							// ler informação do buffe
	return (uint8_t*)_buffer;
}

uint8_t usart_SizeBuffer(void){								// tamanho do contador do buffer (tamanho do buufer)
	return _contBFF;
}

void usart_CleanBuffer( char clr ){							// limpa buffer
	for(uint16_t i=0; i<SIZE_BUFFER; i++) _buffer[i] = clr;
	_contBFF = 0x00;
}

/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/*########################################################################################################################################################*/
/* Private Functions ---------------------------------------------------------*/
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