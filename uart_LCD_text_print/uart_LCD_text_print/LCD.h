/*LCD 전용 헤더파일입니다!
필요할 때 바로 꺼내쓸 수 있도록 메모장에 저장해 두세요. */
#ifndef LCD_H_
#define LCD_H_

// --- 핀 연결 설정 ---
#define PORT_DATA       PORTA   // 데이터 핀(D0~D7) 포트
#define DDR_DATA        DDRA
#define PORT_CONTROL    PORTC   // 제어 핀(RS, RW, E) 포트
#define DDR_CONTROL     DDRC

#define RS_PIN          0       // 0: 명령어, 1: 데이터
#define RW_PIN          1       // 0: 쓰기 모드 (항상 0으로 사용)
#define E_PIN           2       // Enable 신호 (High->Low 일 때 전송)

// [내부용] Enable 신호를 줬다 뺏는 함수 (LCD에게 "가져가!" 신호 주는 역할)
void LCD_pulse_enable(void)
{
	PORT_CONTROL |= (1<<E_PIN);   // Enable ON
	_delay_ms(1);                 // LCD가 인식할 시간
	PORT_CONTROL &= ~(1<<E_PIN);  // Enable OFF (이때 전송됨)
	_delay_ms(1);
}

// [내부용] 명령어 전송 함수 (RS = 0)
void LCD_write_command(uint8_t command)
{
	PORT_CONTROL &= ~(1<<RS_PIN); // RS핀 0 (명령어 모드)
	PORT_DATA = command;          // 명령어 값 데이터 포트에 올림
	LCD_pulse_enable();           // 전송 펄스!
	_delay_ms(2);
}

// -----------------------------------------------------------------
// ★ 여기서부터 실제로 메인에서 사용하는 중요 함수들입니다 ★
// -----------------------------------------------------------------

/* 1. 글자 한 개 출력 함수 (RS = 1) */
void LCD_write_data(uint8_t data)
{
	PORT_CONTROL |= (1<<RS_PIN);  // RS핀 1 (데이터 모드)
	PORT_DATA = data;             // 글자 데이터(ASCII) 포트에 올림
	LCD_pulse_enable();           // 전송 펄스!
	_delay_ms(2);
}

/* 2. 화면 전체 지우기 함수 */
void LCD_clear(void)
{
	LCD_write_command(0x01);      // 0x01: Clear Display 명령어
	_delay_ms(2);                 // 지우는 건 시간이 좀 걸림
}

/* 3. LCD 초기화 함수 (프로그램 시작할 때 딱 한 번 호출) */
void LCD_init(void)
{
	_delay_ms(50);                // 전원 켜지고 LCD 안정화 대기
	
	// 포트 방향 설정 (모두 출력)
	DDR_DATA = 0xff;
	DDR_CONTROL |= (1<<RS_PIN) | (1<<RW_PIN) | (1<<E_PIN);
	PORT_CONTROL &= ~(1<<RW_PIN); // RW핀 0 (쓰기 모드 고정)
	
	// 필수 설정 명령어 전송
	LCD_write_command(0x38);      // 8비트 모드, 2줄 사용
	LCD_write_command(0x0C);      // 화면 켜기(ON), 커서 끄기, 깜빡임 끄기
	LCD_write_command(0x06);      // 글자 쓰면 커서 자동 오른쪽 이동
	
	LCD_clear();                  // 화면 한번 깨끗하게 지우고 시작
}

/* 4. 문자열 출력 함수 ("Hello" 처럼 문장 출력) */
void LCD_write_string(char *string)
{
	uint8_t i;
	// 문자열이 끝날 때('\0')까지 반복해서 한 글자씩 찍음
	for(i=0; string[i] != '\0'; i++)
	{
		LCD_write_data(string[i]);
	}
}
/* 5. 커서 위치 이동 함수 (row: 0~1줄, col: 0~15칸) */
void LCD_goto_XY(uint8_t row, uint8_t col)
{
	col %= 16; // 16칸 넘어가면 0으로 (안전장치)
	row %= 2;  // 2줄 넘어가면 0으로
	
	// LCD 메모리 주소 계산: 첫째 줄은 0x00부터, 둘째 줄은 0x01부터 시작
	uint8_t address = (0x40 * row) + col;
	
	// 커서 이동 명령어(0x80) + 주소
	LCD_write_command(0x80 + address);
}

#endif
