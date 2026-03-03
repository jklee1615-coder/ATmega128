//ADC 통한 디지털값을 USART이용하여 테라텀에 출력하기
#define F_CPU 16000000L //cpu 클록 주기
#include <util/delay.h> //딜레이 함수
#include <stdio.h> //표준 i/o헤더파일
#include <string.h> //strcat 함수 쓰기 위한 파일
#include <avr/io.h> //avr i/o헤더파일

void ADC_init(unsigned char channel)//채널 선택 후 변환까지의 명령 내림
{//이름ADC_init, 매개변수 channel의 함수 선언
	ADMUX |= (1<<REFS0); //AVCC 기준 전압 선택
	ADCSRA |= 0x07; //128분주비 설정
	ADCSRA |= (1<<ADEN); //ADC 모듈 허용
	ADCSRA |= (1<<ADFR); //Free running 모드 설정
	ADMUX=((ADMUX & 0xE0)|channel); //채널 선택
	ADCSRA |= (1<<ADSC); //변환 시작
}

int read_ADC() //디지털값 주워담는 함수
{
	int ADval; //변수 선언
	while((ADCSRA&0x10)==0x00); //0인 경우 대기
	int a=ADCL; //low값 읽기
	int b=ADCH<<8; //high값 읽고 8bit 밀기( 밀어서 10bit로 붙혀줌)
	ADval= a|b; //상위, 하위를 합쳐 10bit 결과 생성(그릇은 16bit)
	return ADval; //값 출력
}

void putch(unsigned char data) //데이터 출력함수
{ //이름 putch, 매개변수 data인 함수
	while((UCSR0A & 0x20)==0); //0인 경우(레지스터가 차 있을때) 대기
	UDR0 = data; //데이터를 보냄
	UCSR0A |= 0x20;//1로 초기화
}

int main(void)
{
    ADC_init(0); //0채널, 컨버터
	DDRE= 0xFE; //USART 송, 수신 설정
	UCSR0A = 0x00; //상태 초기화하고 시작하겠다는 의미
	UCSR0B = 0x18; //송, 수신부 동작
	UCSR0C = 0x06; //전송 비트수 8로 설정
	UBRR0H = 0x00; UBRR0L= 103; //전송 속도 9600bps로 설정
	
	unsigned char i=0; 
	char text[10]; //text라는 10byte 문자열 선언
	int d= read_ADC(); //d에 디지털값 저장
	sprintf(text, "%d", d); //정수 d를 10진수 문자열로 변환하여 text에 저장
	char c[2]= "\r\n"; //캐리지 리턴, 줄바꿈
	strcat(text,c); //text와 c 문자열 이어 붙혀주는 함수
    while (text[i] != '\0')
    { //배열 null값이 아니면 데이터 하나씩 출력
		putch(text[i++]);
    }
	return 0;
}

