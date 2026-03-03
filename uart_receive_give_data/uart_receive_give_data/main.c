#define F_CPU 16000000L
#include <avr/io.h>

void putch(unsigned char data)
// 사용자 정의 함수putch , 매개변수 data 정의
//data출력 함수
{
	while((UCSR0A & 0x20)==0);
	//UDRE0=0일 때 대기 , 1일 때 빠져나옴
	UDR0 = data;// data(text) 출력
	UCSR0A |= 0x20;//1로 초기화
}

unsigned char getch(){//매개변수 없는 getch 사용자 정의 함수
	unsigned char data;// data 변수 정의
	while((UCSR0A&0x80)==0);// RXC0=0 대기, 1이면 빠져나감
	data = UDR0; //PC에서 들여온 문자를 data에 저장
	UCSR0A |= 0x80;//1로 초기화
	
	return data;// data 반환.
}

int main(void)
{
	unsigned char text[] = "\r \n Welcome! newTC AB-M12Pro \r \n USART 0 Test Program.\r \n";
	//text문자열 정의
	unsigned char echo[]= "ECHO >>";
	//echo문자열 정의
	unsigned char i=0;
	//문자열 가르킬 포인터 i
	DDRE = 0xFE;//PB0 입력(수신), PB1 출력(송신)
	
	UCSR0A= 0x00;//상태, 오류 플래그를 0으로 만듦
	UCSR0B= 0x18;//RXEN0=1(수신부 동작), TXEN0=1(송신부 동작)
	UCSR0C= 0x06;//UCSZ0 1,0=1 8bit의 데이터
	UBRR0H= 0x00;
	UBRR0L= 103;//9600bps로 전달
	while (text[i] !='\0') putch(text[i++]);//i++는 i값을 출력한 후에 i를 +1시키겠다는 의미
	//text출력 함수
	i=0;//포인터 초기화
	while (echo[i] !='\0') putch(echo[i++]);
	//echo출력 함수
	while (1) putch(getch());//데이터 받아와서 출력하는 함수
	//getch 는 수신의 역할함수 이기 때문에 받아온 문자를 리턴으로 반환하여 바로 putch에 넣어서 송신하는 원리의 함수이다.
	return 0;
}

