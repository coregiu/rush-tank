/**
  ******************************************************************************
  *
  * The manager of pwm
  * author: coregiu
  *
  *
  ******************************************************************************
**/

#include <pwm_manager.h>

// 当前系统默认占空比次数
const uchar DEFAULT_TUNE_PWM_STEP = 2;

// 当前系统的占空比，以此占空比来控制电机速度
uchar tune_pwm_step;

// 前一条指令。当前后两条指令相同时不做操作
uchar pre_cmd;

void init_tune_pwm_step()
{
    tune_pwm_step = DEFAULT_TUNE_PWM_STEP;
    pre_cmd = COMMAND_NULL;
}

void update_tune_pwm_step(struct command_key *command_key)
{
    if (command_key->right_key == pre_cmd)
    {
        return;
    }
    pre_cmd = command_key->right_key;
    switch (command_key->right_key)
    {
    case COMMAND_RIGHT_1:
        // uart_log_string_data("e:B"); // send B
        LED_LEFT_LEFT = !LED_LEFT_LEFT;
        tune_pwm_step += g_motor_config.pwm_change_step;
        tune_pwm_step = tune_pwm_step >= g_motor_config.pwm_period_times ? g_motor_config.pwm_period_times : tune_pwm_step;
        break;
    case COMMAND_RIGHT_2:
        // uart_log_string_data("e:C"); // send C
        LED_LEFT_RIGHT = !LED_LEFT_RIGHT;
        tune_pwm_step -= g_motor_config.pwm_change_step;
        tune_pwm_step = tune_pwm_step <= g_motor_config.pwm_change_step ? g_motor_config.pwm_change_step : tune_pwm_step;
        break;
    default:
        pre_cmd = COMMAND_NULL;
        break;
    }

    // log_pwm_info(command_key);
}


const struct module_command_receiver pwm_manager = {init_tune_pwm_step, update_tune_pwm_step};

/**
 * 信息打印，格式： 命令-微调幅度
 */
void log_pwm_info(struct command_key *command_key)
{
    if (command_key->right_key == COMMAND_RIGHT_1)
    {
        uart_log_data('1');
        uart_log_data('1');
        uart_log_data('-');
    }
    else if(command_key->right_key == COMMAND_RIGHT_2)
    {
        uart_log_data('1');
        uart_log_data('2');
        uart_log_data('-');
    }
    uart_log_data(tune_pwm_step + 0x30);
    uart_log_enter_char();
}