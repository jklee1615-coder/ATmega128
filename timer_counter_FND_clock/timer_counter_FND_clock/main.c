//타이머 카운터 이용하여 시계 만들기(분주비:1024, 오버플로우, 타이머 크기 10ms)
#define F_CPU 16000000L
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

enum{C0,C1,C2,C3}; //열거형 변수(숫자에 이름을 붙혀주는 기능)
	//c프로그래밍에서 int x=0;을 해줘서 x=0을 기억하는 것과 같은 원리임
unsigned char FND[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};
//FND 각 숫자를 16진수로 표현
volatile unsigned char time_s=0; //초,분, 주기 반복을 세어줄 매개변수 정의
volatile unsigned char time_m=0, timercnt=0;
void print_FND(unsigned char selCx, unsigned char data);
SIGNAL(TIMER0_OVF_vect);
int main(void)
{
    DDRE= 0xFF;  //FND LED 포트 모두 출력
	PORTE= 0x00; // FND 모두 OFF
	DDRG= 0x0F;  // FND 4자리 모두 출력
	PORTG= 0x0F; // 4자리 모두 ON
	TCCR0= 0x07; // 1024 분주비 사용
	TCNT0=99;	 // 0번 타이머 초기값 99
	TIMSK= 0x01; //TOIE0이 1이 되며 오버플로우
	TIFR= 0x01;  //오버플로우 알림
	sei(); //인터럽트 허용
    while (1) 
    {//1ms마다 FND를 하나씩켬
		print_FND(C3,time_s%10);
		_delay_ms(1);
		print_FND(C2,time_s/10);
		_delay_ms(1);
		print_FND(C1,time_m%10);
		_delay_ms(1);
		print_FND(C0,time_m/10);
		_delay_ms(1);
    }
	return 0;
}

SIGNAL(TIMER0_OVF_vect)
{//1s 마다 인터럽트됨
	cli();
	TCNT0=99;
	timercnt++;
	if(timercnt ==100){
		if(time_s>=59){
			time_m++;
			time_s =0;
		}
		else
		time_s++;
		if(time_m>59){
			time_m =0;
		}
		timercnt =0;
	}
	sei();
}

void print_FND(unsigned char selCx, unsigned char data)
{//return이 없는 사용자 정의 함수 정의 (이름이 print_FND이고 매개변수가 selCx 와 data임)
	switch(selCx){//스위치함수 괼호의 변수와 값 같은 케이스 가서 실행함, 같은 값이 없다면 default문가서 실행
		case C3:
		PORTG &=0xF0;//다른 포트4~7은 건들이지 않고 0~3포트만 끄는 코드
		PORTG |= 0x08;
		break;
		
		case C2:
		PORTG &=0xF0;
		PORTG |= 0x04;
		break;
		
		case C1:
		PORTG &=0xF0;
		PORTG |= 0x02;
		break;
		
		case C0:
		PORTG &=0xF0;
		PORTG |= 0x01;
		break;
		
		default:
		PORTG&= 0xF0;
		PORTG|=0x00;
		break;
	}
	if(selCx == C1 &&time_s%2 ==0)//짝수초 일때 점에 불들어오게 바꿈
	PORTE= FND[data]&0x7F;
	else //홀수초 일때는 FND만 켜지게함
	PORTE = FND[data];
}

