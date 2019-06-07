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

#ifndef USART_V2_H_
#define USART_V2_H_

#include <avr/io.h>                                                                     // lib para AVR
#include <avr/interrupt.h>                                                              // lib para inte

#define SetBit( RES, BIT ) ( RES |=   ( 1 << BIT ) )                                    // Por BIT em nível alto
#define ClrBit( RES, BIT ) ( RES &= ~ ( 1 << BIT ) )					// Por BIT em nível baixo
#define TstBit( RES, BIT ) ( RES &    ( 1 << BIT ) )					// testar BIT, retorna 0 ou 1
#define CplBit( RES, BIT ) ( RES ^=   ( 1 << BIT ) )					// Inverter estado do BIT

#define SIZE_BUFFER     128                                                             // tamanho do buffer de leirura

#define OpenTXD 	SetBit( UCSR0B, TXEN0 )						// habilitar transmissão da USATR ( PIN TXD )
#define OpenRXD 	SetBit( UCSR0B, RXEN0 )						// habilitar recepção da USATR ( PIN TRD )
#define CloseTXD 	ClrBit( UCSR0B, TXEN0 )						// desabilitar transmissão da USATR ( PIN TXD )
#define CloseRXD 	ClrBit( UCSR0B, RXEN0 )						// desabilitar recepção da USATR ( PIN TRD )
#define Received_ON 	SetBit( UCSR0B, RXCIE0 )					// habilitar interrupção para byte recebido
#define Send_ON		SetBit( UCSR0B, TXCIE0 )					// habilitar interrupção para byte enviado
#define DataEmpty_ON	SetBit( UCSR0B, UDRIE0 )					// habilitar interrupção para buffer UDR0 cheio
#define Received_OFF 	ClrBit( UCSR0B, RXCIE0 )					// desabilitar interrupção para byte recebido
#define Send_OFF	ClrBit( UCSR0B, TXCIE0 )					// desabilitar interrupção para byte enviado
#define DataEmpty_OFF	ClrBit( UCSR0B, UDRIE0 )					// desabilitar interrupção para buffer UDR0 cheio
#define Received	USART_RX_vect							// vetor de interupição para byte recebido
#define Send		USART_TX_vect							// vetor de interupição para byte enviado
#define DataEmpty	USART_UDRE_vect							// vetor de interupição buffer UDR0 cheio
#define	Interrupt	ISR								// função de interrupção
#define StopBit_1	ClrBit( UCSR0C, USBS0 )						// um bit de parada
#define StopBit_2	SetBit( UCSR0C, USBS0 )						// dois bits de parada
#define ClkPolarity_0	ClrBit( UCSR0C, UCPOL0 )					// deve ser zerado para mondo assincrono
#define ClkPolarity_1	SetBit( UCSR0C, UCPOL0 )					// ajusta clk XCK para amostragem e saída de dados

void usart_Asynchronous(void);								// USART em mondo assincrono
void usart_Synchronous(void);								// USART em mondo sincrono
void usart_SpiMaster(void);								// USART em modo SPI mestre
void usart_ParityOdd(void);								// bit de paridade impar
void usart_ParityEven(void);								// bit de paridade par
void usart_ParityNone(void);								// sem bit de paridade
void usart_FrameBit5(void);								// frame com 5 bits
void usart_FrameBit6(void);								// frame com 6 bits
void usart_FrameBit7(void);								// frame com 7 bits
void usart_FrameBit8(void);								// frame com 8 bits
void usart_FrameBit9(void);								// frame com 9 bits
void usart_BaudRate( unsigned long int BaudRate, unsigned long int freg);		// iniciar porra serial
void usart_PrintChr( uint8_t chr );							// enviar um carecter
void usart_PrintStr( const char *str );							// enviar uma String
void usart_PrintStrLn( const char *str);						// enviar istring e pular linha
void usart_PrintStrChr( const char *str, uint8_t chr);					// enviar istring e uma char
uint8_t *usart_ReadBuffer(void);							// ler informação do buffe
uint8_t usart_SizeBuffer(void);								// tamanho do contador do buffer (tamanho do buufer)
void usart_CleanBuffer( char clr );							// limpa buffer							);	// enviar istring e pular linha

#endif /* USART_V2_H_ */
