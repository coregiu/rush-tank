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
  *         LEFT_EN = 20%
  *         LEFT_MV = 1
  *         LEFT_BK = 0
  *         RIGHT_EN = 20%
  *         RIGHT_MV = 1
  *         RIGHT_BK = 0
  *
  * BACK command:
  *     key: LEFT_DOWN
  *     monitor led: P0_1
  *     gpio:
  *         LEFT_EN = 20%
  *         LEFT_MV = 0
  *         LEFT_BK = 1
  *         RIGHT_EN = 20%
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
  *         RIGHT_EN = 20%
  *         RIGHT_MV = 1
  *         RIGHT_BK = 0
  *
  * TURN RIGHT command:
  *     key: LEFT_RIGHT
  *     monitor led: P0_3
  *     gpio:
  *         LEFT_EN = 20%
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
#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <command_receiver.h>

#define RIGHT_EN P2_0
#define RIGHT_MV P2_1
#define RIGHT_BK P2_2
#define LEFT_MV P2_3
#define LEFT_BK P2_4
#define LEFT_EN P2_5

// CAR run state
enum car_run_state
{
    STOP = 0,
    MOVE = 1,
    BACK = 2,
    LEFT = 3,
    RIGHT = 4,
    LEFT_BACK = 5,
    RIGHT_BACK = 6,
    FAST = 7,
    SLOW = 8,
    LEFT_TUNE = 9,
    RIGHT_TUNE = 10,
    FATEST = 11
};

struct motor_run_state
{
    uchar pwm_rate; // 当前命令下的占空比（周期是5次）
    uchar pwm_period_times; // 当前周期内总共执行次数
    uchar pwm_hight_times;  // 当前周期内高电平执行次数
};

extern struct motor_run_state g_left_motor_run_state;
extern struct motor_run_state g_right_motor_run_state;

extern const struct module_command_receiver motor_driver;

struct motor_pwm_control
{
    void (*update_pwm_status)();
};
extern const struct motor_pwm_control left_motor_pwm_controller;
extern const struct motor_pwm_control right_motor_pwm_controller;

void log_command(struct command_key *command_key);
void log_motor_state(struct motor_run_state *motor_run_state);

#endif