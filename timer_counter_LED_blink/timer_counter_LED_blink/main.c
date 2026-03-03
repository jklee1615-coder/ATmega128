//타이머 카운터 0번만(노말모드)을 이용해서 LED 1초마다 왼쪽으로 이동시키기
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char LED = 0x01;
volatile unsigned char timercnt=0;// 0.01s가 몇 번 반복됐는지 알기 위한 변수

SIGNAL(TIMER0_OVF_vect){//0오버 플로우 됐을 때 0.01s 를 100번 반복할때 마다
	//즉, 1초 마다 LED값 ++
	cli();
	TCNT0=99;// 초기값 99
	timercnt++;
	if(timercnt==100)
	 {timercnt=0;
		if(LED== 0x80) LED= 0x01;
		else LED= LED<<1;
		 
	}
	sei();
}


int main(void)
{
    DDRB = 0xFF; //LED B포트 출력
	TCCR0 = 0x07;// 1024분주
	TCNT0 =99;// (0.01 *(0.0625u /1024))*157 =0.01s를 얻기 위한 카운트 값
	TIMSK = 0x01;//0번 타이머 오버플로우
	TIFR |=1 <<TOV0;//TOV0이 1로 이동. 이때 or 연산이기 때문에 1됨.(문법적)
	//TOV0을 0으로(Flag는 다른 포트와 다르게 1 넣으면 0으로 초기화 해줌)만드는 동작임
	sei();
    while (1) 
    {
		PORTB = LED;// LED ON
    }
	return 0;
}

