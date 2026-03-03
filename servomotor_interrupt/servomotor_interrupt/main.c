//타이머/카운터로 서보모터 제어하고, 0,1 인터럽트로 PWM
#define F_CPU 16000000L //16M 클럭수
#include <util/delay.h> // 딜레이 함수
#include <avr/io.h> //avr i/o헤더파일
#include <avr/interrupt.h> //인터럽트 헤더파일

ISR(INT0_vect)
{ //0번 인터럽트
	cli();
	OCR1A = 999; //-90도로 이동(999동안 켬)
	sei();
}

ISR(INT1_vect)
{	//1번 인터럽트
	cli();
	OCR1A = 4999;//90도로 이동(4999동안 켬)
	sei();
}

int main(void)
{
	DDRB |= (1<< PB5); //OC1A핀을 출력으로 설정
	DDRD = 0xFC; //(butten이 D포트에 연결) 0, 1스위치를 입력으로 설정
	
	EICRA= 0x0F; //0, 1스위치 상승 에지
	EICRB= 0x00;
	EIMSK= 0x03; //인터럽트 허용
	EIFR = 0x03; //플래그 허용
	
	TCCR1A= (1<< COM1A1)|(0<< COM1A0) //비반전 모드
			|(1<< WGM11)|(0<<WGM10); //고속 PWM모드 (Mode 14)
	TCCR1B= (1<<WGM13)|(1<<WGM12) //고속 PWM모드 (Mode 14)
			|(0<<CS12)|(1<<CS11)|(0<<CS10); // 분주비 8
	
	ICR1=39999; //39999까지 세고 리셋(주파수 만듦) 50HZ
	
	OCR1A=2999; //ICR1이 39999까지 세는 동안 2999동안 켜지며 반복되어 듀티비 형성
				//초기 위치 0도
	sei(); //인터럽트 허용
    while (1) 
    {//초기 상태로 돌리려면 리셋
    }
}

