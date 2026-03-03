//16bit C/T로 서보모터, 8bit C/T로 서보모터 위치 값FND출력하는 코드
#define F_CPU 16000000L //CPU 클럭 16MHz
#include <avr/interrupt.h> //인터럽트 헤더파일
#include <util/delay.h> //딜레이 함수
#include <stdio.h> //표준 i/o헤더파일
#include <avr/io.h> //avr i/o헤더파일

enum{C0,C1,C2,C3}; //열거형 변수로 자리수에 숫자 넣어줌
	
unsigned char FND_DATA_TAL[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E};
//FND 표시 숫자 16진수로 나타냄
volatile unsigned char lowValue=0; //전역 변수 선언
volatile unsigned char highValue =0;
volatile unsigned int adcValue=0;
volatile unsigned char timer0Cnt=0;

void print_FND(char selCx, unsigned char data) //
{ //이름: print_FND, 매개변수 selCx, data
	switch(selCx){//switch 함수 이용하여 해당하는 자릿수에 숫자 출력
		case C3: PORTG &= 0xF0; PORTG|= 0x01;
		break;
		
		case C2: PORTG &= 0xF0; PORTG|= 0x02;
		break;
		
		case C1: PORTG &= 0xF0; PORTG|= 0x04;
		break;
		
		case C0: PORTG &= 0xF0; PORTG|= 0x08;
		break;
		
		default: PORTG &= 0xF0; PORTG |= 0xF0;
		break;
	} PORTE = FND_DATA_TAL[data];//데이터 E포트로
}
ISR(TIMER0_OVF_vect){ //변환값 정리하는 함수
	cli();
	TCNT0= 99;
	timer0Cnt++;//타이머/카운터 주기마다 증가
	
	if(timer0Cnt>=100){// 1초가 지나면 0으로 초기화
		timer0Cnt =0;
		while((ADCSRA& 0x10)== 0x00);//플래그 0이면 대기
		
		lowValue = ADCL; //Low값 대입
		highValue = ADCH; //High값 대입
		adcValue = (highValue <<8)| lowValue; //adc값에 16bit(내용물 10bit)의 값 대입
	} sei();
}

void ADC_init(unsigned char channel)//ADC채널 선택, 변환 시작하는 코드
{ //이름: ADC_init, 매개변수 channel인 함수 설정
	ADMUX |= (1<<REFS0); //기준 전압 AVCC선택
	ADCSRA |= 0x07; //분주비 128
	ADCSRA |= (1<<ADEN); //ADC 모듈 허용
	ADCSRA |= (1<<ADFR); //Free running모드
	ADMUX=((ADMUX & 0xE0)|channel); //채널 선택
	ADCSRA |= (1<<ADSC); // 변환 시작
}
int main(void)
{
    DDRB=0x20; //OC1A핀 출력으로 설정
	PORTB= 0x20; //ON
	DDRE= 0xFF; //LED 출력
	PORTE = 0x00; //OFF
	DDRG = 0x0F; //4자리 모두 출력
	PORTG= 0x0F; //ON
	EICRA= 0x0F; //0,1 상승에지
	EICRB= 0x00; 
	EIMSK= 0x03; //인터럽트 허용
	EIFR = 0x03; //플래그 허용
	
	TCCR1A= (1<< COM1A1)|(0<< COM1A0) // 비반전 모드
			|(1<< WGM11)|(0<<WGM10); // 고속 PWM모드
	TCCR1B= (1<<WGM13)|(1<<WGM12) // 고속 PWM모드(Mode 14)
			|(0<<CS12)|(1<<CS11)|(0<<CS10); //분주비 8
	
	ICR1=39999; //0~39999까지 세고 리셋(데이터 시트에 명시된 50Hz로 설정)
	
	OCR1A=2999; //0~2999까지만 ON (듀티비 정함), 초기 0도(펄스폭 1.5ms)
	
	TCCR0 =(1<< CS02)|(0<<CS01)|(1<< CS00); //분주비 128
				//나머지(WGM 등) 정의 안한것은 기본값인 0값으로 들어감
	TCNT0 = 99; //카운터 시작값(99~255까지 셈)
	TIMSK |= 1<< TOIE0; //오버플로우 인터럽트 허용
	TIFR |= 1<< TOV0; //오버플로우 플래그 저장
	
	ADC_init(0); //0채널, ADC변환 시작
	sei(); //인터럽트 허용
	
    while (1) //
    {
		unsigned int position = 999+ ((unsigned long)adcValue *4000/1023);
			//position 변수 선언 (0~1023을 서보모터의 OCR1A값인 999~4999로 변환)
		OCR1A= position;
			//계산된 position값을 OCR1A보내서 PWM으로 각도 조절
		print_FND(C3, OCR1A/1000);//FND로 데이터 출력
		_delay_ms(3);
		print_FND(C2, (OCR1A/100)%10);
		_delay_ms(3);
		print_FND(C1, (OCR1A/10)%10);
		_delay_ms(3);
		print_FND(C0, OCR1A%10);
		_delay_ms(3);
    }return 0;
}

