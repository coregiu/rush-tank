/**
  ******************************************************************************
  *
  * main function
  * author: coregiu
  * 
  * 
  ******************************************************************************
**/

#include <controller.h>
/* 71ms per period */
#define READ_PS2_INTVAL_TIME_MS_H   0X00
#define READ_PS2_INTVAL_TIME_MS_L   0X00

const struct car_config g_car_config = {0X00, 0X00, 0X00, 0X00};

void delay_time_ms(uint mil_sec)
{
	uint i, j;
	for (i = mil_sec; i > 0; i--)
		for (j = 70; j > 0; j--)
			;
}

/**
 * init timer 0.
 */
void init_timer_0()
{
	// init timer0
	TMOD |= 0X01;
    TH0 = g_car_config.left_motor_period_h;
    TL0 = g_car_config.left_motor_period_l; 
	ET0 = 1;
    TR0 = 1;
	EA  = 1; //开启总中断
}

/**
 * init timer 0.
 */
void init_timer_1()
{
	// init timer1
	TMOD |= 0x10;			     
	TH1 = g_car_config.right_motor_period_h;	 /* Init value */
	TL1 = g_car_config.right_motor_period_l; 
	ET1 = 1;                     /* enable timer1 interrupt */
	TR1 = 1;  
	EA  = 1;                      /* interupt enable */
}

/**
 * init uart.
 */
void init_uart()
{
	// init uart
	TMOD |= 0x20; //用定时器设置串口波特率	   9600
	TH1  = 0xfd;
	TL1  = 0xfd;
	TR1  = 1;
	ES   = 0;
	SCON = 0X50; 
	EA   = 1; //开启总中断
}

void main()
{
	delay_time_ms(500);

	init_timer_0();
	init_timer_1();
	init_uart();
	init_modules();

	uart_log_start_info();

	while (1)
	{
		// read ps2 command and set is_has_command/non_motor_cmd_times value.
		uint commands[COMMANDS_LENGTH][2] = {{0}};
		uint is_has_command = 0;
		read_ps2(&is_has_command, commands);
		// executet the commands.
		execute_commands(is_has_command, commands); 
		delay_time_ms(150);
	}
}

/**
 * timer 0 interrupt function.
 * read ps2 command and execute it by 50ms interval.
 */
void time_0_isr(void) __interrupt 1
{
	// reset number of beginning.
	TH0 = g_car_config.left_motor_period_h;
	TL0 = g_car_config.left_motor_period_l;
}

/**
 * timer 1 interrupt function.
 * read ps2 command and execute it by 50ms interval.
 */
void time_1_isr(void) __interrupt 3
{
	// reset number of beginning.
	TH1 = g_car_config.right_motor_period_h;
	TL1 = g_car_config.right_motor_period_l;
}