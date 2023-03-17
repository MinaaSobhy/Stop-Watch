/*
 * main.c
 *
 *  Created on: SEP 15, 2022
 *      Author: Mina Sobhy
 */


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//define global variables
unsigned char sec1=0;
unsigned char sec2=0;
unsigned char min1=0;
unsigned char min2=0;
unsigned char hour1=0;
unsigned char hour2=0;


/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	// Set timer1 initial count to zero
	TCNT1 = 0;

	//reseting all the values
	sec1=0;
	sec2=0;
	min1=0;
	min2=0;
	hour1=0;
	hour2=0;
}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	// Configure INT0/PD2 as input pin
	DDRD  &= (~(1<<PD2));
	// Trigger INT0 with the falling edge
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	// Enable external interrupt pin INT0
	GICR  |= (1<<INT0);
}

/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	/* Configure timer control register TCCR1B
	 * 1. Non PWM mode FOC =0
	 * 2. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 3. Stop timer1 CS10=0 CS11=0 CS12=0
	 */
	TCCR1B &= ~(1<<CS11) & ~(1<<CS10);
}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	// Configure INT1/PD3 as input pin
	DDRD  &= (~(1<<PD3));
	// Trigger INT1 with the raising edge
	MCUCR |= (1<<ISC11) | (1<<ISC10);
	// Enable external interrupt pin INT1
	GICR  |= (1<<INT1);

}

/* External INT2 Interrupt Service Routine */
ISR(INT2_vect)
{
	/* Configure timer control register TCCR1B
	 * 1. Non PWM mode FOC =0
	 * 2. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 3. resuming timer1 with Pre-scaler= F_CPU/64
	 * 	  CS10=1 CS11=1 CS12=0
	 */
	TCCR1B |= (1<<CS11) | (1<<CS10);
}

/* External INT2 enable and configuration function */
void INT2_Init(void)
{
	// Configure INT2/PB2 as input pin
	DDRB   &= ~(1<<PB2);
	// Trigger INT2 with the falling edge
	MCUCSR &= ~(1<<ISC2);
	// Enable external interrupt pin INT2
	GICR   |= (1<<INT2);

}

/* Timer1 Interrupt Service Routine */
ISR(TIMER1_COMPA_vect)
{
	//increment the value of the variable second each time the timer fire the ISR
	sec1++;

}
/* Timer1 enable and configuration function */
void TIMER_INIT(void)
{
	TCNT1 = 0;		/* Set timer1 initial count to zero */

	OCR1A = 15625;    /* Set the Compare value to 15625 ((1MHz/64 = 15625 ) represent the number of ticks in one second */

	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=0 (Non-PWM)
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A);

	/* Configure timer control register TCCR1B
	 * 1. Non PWM mode FOC =0
	 * 2. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 3. Pre-scaler = F_CPU/64 CS10=1 CS11=1 CS12=0
	 */
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
}
int main()
{
	// Timer1 initialization
	TIMER_INIT();

	// Enable interrupts by setting I-bit
	SREG   |= (1<<7);

	//interrupt initialization
	INT0_Init();
	INT1_Init();
	INT2_Init();

	//Initializing ports for lCD
	DDRA |= 0x3F;
	DDRC |= 0x0F;


	while(1)
	{
		// check if overflow occurs at the first slot as its maximum count is 9
		if( sec1 == 10)
		{
			sec1 = 0;
			sec2++;
		}

		// Display the required number on the 7-segment at the first slot
		PORTA = 0x01;
		PORTC = (PORTC & 0xF0) | (sec1 & 0x0F);
		_delay_ms(1);

		// check if overflow occurs at the second slot as its maximum count is 9
		if( sec2 == 6)
		{
			sec2 = 0;
			min1++;
		}
		// Display the required number on the 7-segment at the second slot
		PORTA = 0x02;
		PORTC = (PORTC & 0xF0) | (sec2 & 0x0F);
		_delay_ms(1);

		// check if overflow occurs at the third slot as its maximum count is 9
		if( min1 == 10)
		{
			min1 = 0;
			min2++;
		}
		// Display the required number on the 7-segment at the third slot
		PORTA = 0x04;
		PORTC = (PORTC & 0xF0) | (min1 & 0x0F);
		_delay_ms(1);

		// check if overflow occurs at the forth slot as its maximum count is 9
		if( min2 == 6 )
		{
			min2 = 0;
			hour1++;
		}
		// Display the required number on the 7-segment at the forth slot
		PORTA = 0x08;
		PORTC = (PORTC & 0xF0) | (min2 & 0x0F);
		_delay_ms(1);

		// check if overflow occurs at the fifth slot as its maximum count is 9
		if( hour1 == 10)
		{
			hour1 = 0;
			hour2++;
		}
		// check if overflow occurs at the sixth slot as its maximum count is 9
		else if( hour2 ==10)
		{
			hour1 = 0;
			hour2 = 0;
		}
		// Display the required number on the 7-segment at the fifth slot
		PORTA = 0x10;
		PORTC = (PORTC & 0xF0) | (hour1 & 0x0F);
		_delay_ms(1);

		// Display the required number on the 7-segment at the sixth slot
		PORTA = 0x20;
		PORTC = (PORTC & 0xF0) | (hour2 & 0x0F);
		_delay_ms(1);

	}



	return 0;
}
