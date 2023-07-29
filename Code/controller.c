#define F_CPU 1000000
/********************************************************************
	File: controller.c
	Author: Omar Sheleby
********************************************************************/


#include <avr/io.h> /* To use ICU/Timer1 Registers */
#include <avr/interrupt.h>
#include "lcd.h"
#include "gpio.h"

#define GREEN 0
#define RED 1
#define YELLOW 2

uint16 seconds;
uint16 state;

void led_init(){
	GPIO_setupPinDirection(PORTC_ID, PIN0_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORTC_ID, PIN1_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(PORTC_ID, PIN2_ID, PIN_OUTPUT);
	GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_HIGH);
	GPIO_writePin(PORTC_ID, PIN1_ID, LOGIC_LOW);
	GPIO_writePin(PORTC_ID, PIN2_ID, LOGIC_LOW);
}


void Timer1_CTC_Init(unsigned char initial)
{
	TCNT1 = initial;		/* Set timer1 initial count to zero */

	OCR1A = 977;  		  /* each 977 tick (1sec) */

	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=0
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A);

	/* Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. Prescaler = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}


ISR(TIMER1_COMPA_vect){
	seconds--;
	if(seconds == 9) LCD_clearScreen();
}

int main(void){
	LCD_init(); /* initialize LCD driver */
	led_init();
	SREG |= (1<<7); /* Enable global interrupts in MC */
	seconds = 30;
	state = RED;
	LCD_moveCursor(0,1);
	LCD_intgerToString(seconds);

	Timer1_CTC_Init(0);

	while(1)
	{
		LCD_moveCursor(0,1);
		LCD_intgerToString(seconds);
		if(seconds == 0 && state == GREEN)
		{
			seconds = 5;
			state = YELLOW;
			GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_LOW);
			GPIO_writePin(PORTC_ID, PIN1_ID, LOGIC_HIGH);
			GPIO_writePin(PORTC_ID, PIN2_ID, LOGIC_LOW);
			LCD_moveCursor(0,1);
			LCD_intgerToString(seconds);

		}
		else if(seconds == 0 && state == YELLOW)
		{
			seconds = 30;
			state = RED;
			GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_HIGH);
			GPIO_writePin(PORTC_ID, PIN1_ID, LOGIC_LOW);
			GPIO_writePin(PORTC_ID, PIN2_ID, LOGIC_LOW);
			LCD_moveCursor(0,1);
			LCD_intgerToString(seconds);
		}
		else if(seconds == 0 && state == RED)
		{
			seconds = 20;
			state = GREEN;
			GPIO_writePin(PORTC_ID, PIN0_ID, LOGIC_LOW);
			GPIO_writePin(PORTC_ID, PIN1_ID, LOGIC_LOW);
			GPIO_writePin(PORTC_ID, PIN2_ID, LOGIC_HIGH);
			LCD_moveCursor(0,1);
			LCD_intgerToString(seconds);
		}
	}

}
