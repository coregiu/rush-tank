/**
  ******************************************************************************
  *
  * The driver of motor
  * author: coregiu
  * 
  * | 按键     | 功能                                     | 监控指示灯   |
  * | -------- | --------------------------------------- | ---------- |
  * | 左侧上键 | 坦克前进，长按有效，放开停止                  | P0_0       |
  * | 左侧下键 | 坦克后退，长按有效，放开停止                  | P0_1       |
  * | 左侧左键 | 坦克左转，长按有效，放开停止                  | P0_2       |
  * | 左侧右键 | 坦克右转，长按有效，放开停止                  | P0_3       |
  * | 右侧上健 | 坦克加速，每按一次提速一格，放开保持当时速度     | P0_4       |
  * | 右侧下键 | 坦克减速，每按一次减速一格，放开保持当时速度     | P0_5       |
  * | 右侧左键 | 坦克左前行驶，每按一次左偏一格，放开保持偏转角度  | P0_6       |
  * | 右侧右键 | 坦克右前行驶，每按一次右偏一格，放开保持偏转角度  | P0_7       |
  * | 左侧1键  | 坦克以最大速度行驶                           | P0_0       |
  * | 左侧2键  | 坦克停止                                   | P0_1       |
  * | 右侧1键  | 坦克左向飘移                               | P0_2        |
  * | 右侧2键  | 坦克右向飘移                               | P0_3        |
  * 
  * MOVE command:
  *     key: LEFT_TOP
  *     monitor led: P0_0
  *     gpio:
  *         LEFT_EN = 1
  *         LEFT_MV = 1
  *         LEFT_BK = 0
  *         RIGHT_EN = 1
  *         RIGHT_MV = 1
  *         RIGHT_BK = 0
  * 
  * BACK command:
  *     key: LEFT_DOWN
  *     monitor led: P0_1
  *     gpio:
  *         LEFT_EN = 1
  *         LEFT_MV = 0
  *         LEFT_BK = 1
  *         RIGHT_EN = 1
  *         RIGHT_MV = 0
  *         RIGHT_BK = 1
  * 
  * TURN LEFT command:
  *     key: LEFT_LEFT
  *     monitor led: P0_2
  *     gpio:
  *         LEFT_EN = 0
  *         LEFT_MV = 0
  *         LEFT_BK = 0
  *         RIGHT_EN = 1
  *         RIGHT_MV = 1
  *         RIGHT_BK = 0
  * 
  * TURN RIGHT command:
  *     key: LEFT_RIGHT
  *     monitor led: P0_3
  *     gpio:
  *         LEFT_EN = 1
  *         LEFT_MV = 1
  *         LEFT_BK = 0
  *         RIGHT_EN = 0
  *         RIGHT_MV = 0
  *         RIGHT_BK = 0
  * 
  * P2_0 : RIGHT_EN  L298N_EnA
  * P2_1 : RIGHT_MV  L298N_IN2
  * P2_2 : RIGHT_BK  L298N_IN1 
  * P2_3 : LEFT_EN   L298N_EnB
  * P2_4 : LEFT_MV   L298N_IN4
  * P2_5 : LEFT_BK   L298N_IN3
  * 
  ******************************************************************************
**/
#include <motor_driver.h>
#include <mcs51/8051.h>

// the position of gpio in CAR_STATE_LIST array.
enum gpio_position
{
    LEFT_EN_POSITION  = 0, 
    LEFT_MV_POSITION  = 1, 
    LEFT_BK_POSITION  = 2, 
    RIGHT_EN_POSITION = 3, 
    RIGHT_MV_POSITION = 4, 
    RIGHT_BK_POSITION = 5
};

// car sate to set to gpio: {LEFT_EN, LEFT_MV, LEFT_BK, RIGHT_EN, RIGHT_MV, RIGHT_BK}
const uchar CAR_STATE_LIST[7][6] = {{0,  0, 0, 0,  0, 0},  // init
                                    {20, 1, 0, 20, 1, 0},  // move
                                    {20, 0, 1, 20, 0, 1},  // back
                                    {0,  0, 0, 20, 1, 0},  // left
                                    {20, 1, 0, 0,  0, 0},  // right
                                    {100, 1, 0, 100, 0, 1},  // left_back
                                    {100, 0, 1, 100, 1, 0}}; // right_back

enum car_run_state current_car_status = STOP;
struct motor_run_state g_left_motor_run_state  = {0, 0, 0};
struct motor_run_state g_right_motor_run_state = {0, 0, 0};

uchar current_right_command_status = SWITCH_ON;

void stop()
{
    g_left_motor_run_state.pwm_rate  = 0;
    g_right_motor_run_state.pwm_rate = 0;
    current_car_status = STOP;
    current_right_command_status = SWITCH_ON;

}

void exec_car_state_update(enum car_run_state run_state)
{
    if (current_car_status != run_state)
    {
        LEFT_EN  = 0;
        LEFT_MV  = CAR_STATE_LIST[run_state][LEFT_MV_POSITION];
        LEFT_BK  = CAR_STATE_LIST[run_state][LEFT_BK_POSITION];
        g_left_motor_run_state.pwm_rate = CAR_STATE_LIST[run_state][LEFT_EN_POSITION];

        RIGHT_EN = 0;
        RIGHT_MV = CAR_STATE_LIST[run_state][RIGHT_MV_POSITION];
        RIGHT_BK = CAR_STATE_LIST[run_state][RIGHT_BK_POSITION];
        g_right_motor_run_state.pwm_rate = CAR_STATE_LIST[run_state][RIGHT_EN_POSITION];

        current_car_status == run_state;
    }
}

void exec_car_pwm_update(enum car_run_state run_state)
{
    if (current_right_command_status == SWITCH_OFF)
    {
        return;
    }
    switch (run_state)
        {
        // 加速，占空比逐步提升
        case FAST:
            g_left_motor_run_state.pwm_rate += g_motor_config.pwm_change_step;
            g_left_motor_run_state.pwm_rate = g_left_motor_run_state.pwm_rate > g_motor_config.pwm_period_times ? g_motor_config.pwm_period_times : g_left_motor_run_state.pwm_rate;
            
            g_right_motor_run_state.pwm_rate += g_motor_config.pwm_change_step;
            g_right_motor_run_state.pwm_rate = g_right_motor_run_state.pwm_rate > g_motor_config.pwm_period_times ? g_motor_config.pwm_period_times : g_right_motor_run_state.pwm_rate;
            break;
        // 减速，占空比逐步下降
        case SLOW:
            g_left_motor_run_state.pwm_rate = 
            (g_left_motor_run_state.pwm_rate <= g_motor_config.pwm_change_step) ? 0 : (g_left_motor_run_state.pwm_rate - g_motor_config.pwm_change_step);
            
            g_right_motor_run_state.pwm_rate = 
            (g_right_motor_run_state.pwm_rate <= g_motor_config.pwm_change_step) ? 0 : (g_right_motor_run_state.pwm_rate - g_motor_config.pwm_change_step);
            break;
        // 全速前进，占空比拉满
        case FATEST:
            g_left_motor_run_state.pwm_rate = g_motor_config.pwm_period_times;
            g_right_motor_run_state.pwm_rate = g_motor_config.pwm_period_times;
            break;
        default:
            break;
        }
    current_right_command_status = SWITCH_OFF;
}

void init_motor_driver()
{
    exec_car_state_update(STOP);
}

void update_motor_state(uchar car_cmd[])
{
    uart_log_data('|');
    uart_log_hex_data(car_cmd[0]);
    uart_log_hex_data(car_cmd[1]);
    if (car_cmd[0] <= LEFT_KEY)
    {   
        current_right_command_status = SWITCH_ON;
    }

    switch (car_cmd[1])
        {
        case COMMAND_LEFT_TOP:
            // uart_log_string_data("e:1"); // send 1
            LED_LEFT_TOP = !LED_LEFT_TOP;
            exec_car_state_update(MOVE);
            break;
        case COMMAND_LEFT_DOWN:
            // uart_log_string_data("e:2"); // send 2
            LED_LEFT_DOWN = !LED_LEFT_DOWN;
            exec_car_state_update(BACK);
            break;
        case COMMAND_LEFT_LEFT:
            // uart_log_string_data("e:3"); // send 3
            LED_LEFT_LEFT = !LED_LEFT_LEFT;
            exec_car_state_update(LEFT);
            break;
        case COMMAND_LEFT_RIGHT:
            // uart_log_string_data("e:4"); // send 4
            LED_LEFT_RIGHT = !LED_LEFT_RIGHT;
            exec_car_state_update(RIGHT);
            break;
        case COMMAND_RIGHT_TOP:
            // uart_log_string_data("e:5"); // send 5
            LED_RIGHT_TOP = !LED_RIGHT_TOP;
            exec_car_pwm_update(FAST);
            break;
        case COMMAND_RIGHT_DOWN:
            // uart_log_string_data("e:6"); // send 6
            LED_RIGHT_DOWN = !LED_RIGHT_DOWN;
            exec_car_pwm_update(SLOW);
            break;
        case COMMAND_RIGHT_LEFT:
            // uart_log_string_data("e:7"); // send 7
            LED_RIGHT_LEFT = !LED_RIGHT_LEFT;
            exec_car_pwm_update(LEFT_TUNE);
            break;
        case COMMAND_RIGHT_RIGHT:
            // uart_log_string_data("e:8"); // send 8
            LED_RIGHT_RIGHT = !LED_RIGHT_RIGHT;
            exec_car_pwm_update(RIGHT_TUNE);
            break;
        case COMMAND_LEFT_1:
            // uart_log_string_data("e:9"); // send 9
            LED_LEFT_TOP = !LED_LEFT_TOP;
            exec_car_state_update(MOVE);
            exec_car_pwm_update(FATEST);
            break;
        case COMMAND_LEFT_2:
            // uart_log_string_data("e:A"); // send A
            // LED_LEFT_DOWN = !LED_LEFT_DOWN;
            stop();
            break;
        case COMMAND_RIGHT_1:
            // uart_log_string_data("e:B"); // send B
            LED_LEFT_LEFT = !LED_LEFT_LEFT;
            exec_car_state_update(LEFT_BACK);
            break;
        case COMMAND_RIGHT_2:
            // uart_log_string_data("e:C"); // send C
            LED_LEFT_RIGHT = !LED_LEFT_RIGHT;
            exec_car_state_update(RIGHT_BACK);
            break;
        default:
            break;
        }
}

const struct module_command_receiver motor_driver = {init_motor_driver, update_motor_state};

void left_motor_pwm_control()
{
    if (g_left_motor_run_state.pwm_rate <= 0)
    {
        g_left_motor_run_state.pwm_period_times = 0;
        g_left_motor_run_state.pwm_hight_times  = 0;
        LEFT_EN = 0;
        return;
    }

    if (g_left_motor_run_state.pwm_hight_times < g_left_motor_run_state.pwm_rate)
    {
        LEFT_EN = 1;
        g_left_motor_run_state.pwm_hight_times++;
    }
    else
    {
        LEFT_EN = 0;
    }

    g_left_motor_run_state.pwm_period_times++;
    if (g_left_motor_run_state.pwm_period_times >= g_motor_config.pwm_period_times)
    {
        g_left_motor_run_state.pwm_period_times = 0;
        g_left_motor_run_state.pwm_hight_times  = 0;
    }
}

const struct motor_pwm_control left_motor_pwm_controller = {left_motor_pwm_control};

void control_right_motor_pwm()
{
    if (g_right_motor_run_state.pwm_rate <= 0)
    {
        g_right_motor_run_state.pwm_period_times = 0;
        g_right_motor_run_state.pwm_hight_times  = 0;
        RIGHT_EN = 0;
        return;
    }

    if (g_right_motor_run_state.pwm_hight_times < g_right_motor_run_state.pwm_rate)
    {
        RIGHT_EN = 1;
        g_right_motor_run_state.pwm_hight_times++;
    }
    else
    {
        RIGHT_EN = 0;
    }

    g_right_motor_run_state.pwm_period_times++;
    if (g_right_motor_run_state.pwm_period_times >= g_motor_config.pwm_period_times)
    {
        g_right_motor_run_state.pwm_period_times = 0;
        g_right_motor_run_state.pwm_hight_times  = 0;
    }
}

const struct motor_pwm_control right_motor_pwm_controller = {control_right_motor_pwm};