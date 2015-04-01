//
//
// main.c
//
// 2015. 3. 25.
//
// Microprocessor Applicate
//
// write by Wantaek Choe & Bumjoon Kim
//
// with ARTMega 128 KT-M128 Training Kit (No.8)
//
//

#include <avr/io.h>

#define LEDA	0x40    // mcu_pd 6 in mcu_pd[0..7]
#define LEDB	0X80    // mcu_pd 7 in mcu_pd[0..7]
#define EX_LED	(*(volatile unsigned int *)0x8008)  // 1000000000000001

#define	ON		1
#define OFF		0

void hw_init (void);
void led_control (unsigned char led, unsigned sw);
void eled_control(unsigned char led, unsigned sw);
void delay(unsigned short td);

unsigned char eledValue;    // EX_LED는 write only, read가 안되기 때문에 전역변수를 통해 제어해야 한다.

int main(void) {
	
	unsigned char ledIndex;
	unsigned char ledDir;
	
	hw_init();  // 포트, LED 등을 초기화시킨다.
	
	ledIndex	= 0;    // LED제어용 Index변수
    ledDir		= 0;    // LED제어용 Direction변수, ledIndex에 음수, 양수값을 더해서 방향을 결정한다.
	
	while(1) {
        // ledIndex값이 0이 되면 ledDir값이 1이 되어 루프가 실행될 때마다 ledIndex값이 증가한다.
        if(ledIndex == 0) {
            ledDir	= 1;
        }
        // ledIndex값이 7이 되면 ledDir값이 -1이 되어 루프가 실행될 때마다 ledIndex값이 감소한다.
        if(ledIndex == 7) {
            ledDir	= -1;
        }
        
        eled_control(ledIndex, ON); // Index 번호에 해당하는 LED를 켠다.
        // eledValue    |= 0x01<<ledIndex | 0x01<<(7-ledIndex) ;
        delay(100);
		eled_control(ledIndex, OFF); // Index 번호에 해당하는 LED를 끈다.
        
        ledIndex	+= ledDir;  // Dir값에 따라 Index가 증가하거나 감소한다. (1 ~ 7 -> 7 ~ 1 반복)

	}
	return 0;
}

// 보드 초기화 함수
void hw_init (void) {
	eledValue   = 0;    // EX_LED를 제어하기 위한 전역변수 eledValue를 0으로 초기화한다.
	DDRD		|= (LEDA|LEDB); // DDRD에서 LEDA와 LEDB에 해당하는 비트를 1로 만든다.
    PORTD		&= ~(LEDA|LEDB);    // PORTD에서 LEDA와 LEDB에 해당하는 비트를 0으로 만든다.
	MCUCR		= 0x80; // MCU 제어 레지스터 MCUCR의 Bit7(SRE: External SRAM Enable)을 1(Enable)로 만든다.
}

// LEDA나 LEDB를 켜거나 끈다.
void led_control (unsigned char led, unsigned sw) {
    
    // 첫번째 인자가 LEDA나 LEDB가 아닌경우 아무것도 하지않고 함수를 종료시킨다.
    if ( led != LEDA && led != LEDB ) {
        return;
    }
    
	if(sw) {
        // sw값이 0이 아니면(1이면) mcu_pd[0..7]중 led에 해당되는 Bit를 1로 만든다
		PORTD |= led;
	}
	else {
        // sw값이 0이면 mcu_pd[0..7]중 led에 해당되는 Bit를 0으로 만든다
		PORTD &= ~(led);
    }
}

// EX_LED를 켜거나 끈다.
void eled_control(unsigned char ledN, unsigned sw) {
	
    unsigned char led;
	
    led = 0;    // led를 0으로 초기화
	
    // led번호 값이 7보다 크거나 0보다 작으면 함수를 종료시킨다.
    if(ledN > 7 || ledN < 0)
		return;
	
    led = (1<<ledN);    // led에 led번호에 해당되는 Bit를 1로 만든다.
    
	if(sw) {
        // sw값이 0이 아니면(1이면) 전역변수 eledValue에 led번호에 해당하는 Bit를 led변수를 사용해 1로 만든다.
        eledValue	|= led;
	}
	else {
        // sw값이 0이면 전역변수 eledValue에 led번호에 해당하는 Bit를 led변수를 사용해 0으로 만든다.
        eledValue	&= ~(led);
	}

    // EX_LED값에 전역변수 eledValue를 대입해 led번호에 해당하는 led를 켜거나 끈다.
    // EX_LED는 WriteOnly이기 때문에 값을 읽어올 수 없으므로 전역변수를 통해 제어한다.
    EX_LED	= eledValue;
}

// 타이머를 이용하지 않는 delay함수, 정확한 지연시간을 알 수 없다.
void delay(unsigned short td) {
	volatile unsigned short i, j;
	for(i = 0; i < td; i++)
		for(j = 0; j < 1000; j++);
}