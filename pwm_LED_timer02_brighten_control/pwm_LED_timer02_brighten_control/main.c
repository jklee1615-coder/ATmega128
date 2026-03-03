//0번 타이머 PC PWM, 반전모드 / 2번 타이머 PC PWM, 비반전모드
#define F_CPU 16000000L
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>


int main(void)
{
	unsigned char Light =0;
	
    DDRB=0x90; //1001  0000
	TCCR0|=(1<<WGM00)				//PC PWM
			|(1<<COM00)|(1<<COM01)	//반전 모드
			|(1<<CS00);				//분주비 1
	
	TCCR2|=(1<<WGM20)		//PC PWM
			|(1<<COM21)		//비반전 모드
			|(1<<CS20);		//분주비 1
	TCNT0 =0;
	TCNT2 =0;
    while (1) 
    {
		//0,2 타이머 동시에 LED 서서히 밝아짐
		for(Light=0;Light<255;Light++)
		{
			OCR0 = Light;
			OCR2 = 255-Light;//실제로 0,1이 바뀌는 타이밍은 다르지만, 매우 빠른시간이기 때문에 동시에 켜지고 꺼지는것처럼 보임
			_delay_ms(10);
		}
		//0,2 타이머 동시에 LED 서서히 어두워짐
		for (Light=255;0<Light;Light--)
		{
			OCR0 = Light;
			OCR2 = 255-Light;
			_delay_ms(10);
		}
    }
	return 0;
}

