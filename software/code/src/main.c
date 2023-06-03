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
#define READ_PS2_INTVAL_TIME_MS_H   0XFC
#define READ_PS2_INTVAL_TIME_MS_L   0X66

struct motor_config g_motor_config = {0XFC, 0X66, 100, 20};

void delay_time_ms(unsigned int mil_sec)
{
	unsigned int i, j;
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
    TH0 = g_motor_config.motor_period_h;
    TL0 = g_motor_config.motor_period_l; 
	ET0 = 1;
    TR0 = 1;
	EA  = 1; //开启总中断
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
	init_uart();
	init_modules();
	uart_log_start_info();


	while (1)
	{
		// read ps2 command and set is_has_command/non_motor_cmd_times value.
		uchar commands[COMMANDS_LENGTH][2] = {{0}};
		uchar key_module = NON_KEY;
		read_ps2(&key_module, commands);
        // uart_log_hex_data(key_module);
		// executet the commands.
		execute_commands(key_module, commands); 
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
	TH0 = g_motor_config.motor_period_h;
	TL0 = g_motor_config.motor_period_l;

    left_motor_pwm_controller.update_pwm_status();
    right_motor_pwm_controller.update_pwm_status();
}