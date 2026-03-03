#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/io.h>
#include "LCD.h"//LCD에 영문, 숫자 출력하기 위한 헤더파일
#include "hangeul_lcd.h"// LCD에 한글 출력하기 위한 헤더파일

unsigned char getch(){
	unsigned char data;//data 변수 정의
	while((UCSR0A & 0x80)==0);//RXC0=1이면 빠져나감(수신 문자 있음),0이면 못나감(수신 문자 없음)
	data = UDR0;//PC로부터 받은 데이터를 data에 저장
	UCSR0A |= 0x80;// 1로 초기화
	
	return data;// 데이터 출력
}
int main(void)
{
	unsigned char count =0; //count 변수 선언
	LCD_init();//LCD.h에 있는 사용자 정의 함수, LCD 설정 초기화
	
	DDRE= 0xFE;//PE0= 수신부, PE1= 송신부
	UCSR0A= 0x00;//플래그, 오류 초기화
	UCSR0B= 0x18;//RXEN0=1(수신부 동작), TXEN0=1(송신부 동작)
	UCSR0C= 0x06;//UCSZ0 0,1 =1, 8bit의 데이터
	UBRR0H= 0x00;
	UBRR0L= 103;// 9600bps로 전달
	
	LCD_goto_XY(0,0);//LCD.h에 있는 사용자 정의 함수, LCD 0,0으로 이동
	
	while (1)
	{
		LCD_write_data(getch());//lcd_write_data= 헤더파일에 있는 글자 출력 함수
		count++;// 문자 하나 찍을 때마다 카운트 증가

		if(count ==16)//16일 때 2행 1열로 이동.
		{
			LCD_goto_XY(1,0);
		}
		else if(count==33)//32일 때 LCD초기화, 0,0으로 돌아감
		{
			LCD_clear();
			LCD_goto_XY(0,0); count =0;
		}
	}
	return 0;
}

