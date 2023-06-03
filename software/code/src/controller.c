/**
  ******************************************************************************
  *
  * main controller
  * author: coregiu
  * 
  * 
  ******************************************************************************
**/

#include <controller.h>

/**
 * init uart and all receive modules
 * 
 */
void init_modules()
{
    motor_driver.init();
}

/**
 * receive ps2 data, convert to commands,
 * and notify modules to execute commands;
 * 
 */
void execute_commands(uchar key_module, uchar **commands)
{
    if (key_module)
    {
        for (uchar i = 0; i < COMMANDS_LENGTH; i++)
        {
            notify_all(commands[i][1], commands[i][0], key_module);
        }
    }
    else
    {
        notify_all(MODULE_MOTOR, COMMAND_LEFT_2, key_module);
    }    
}

/**
 * notify modules to execute the commands.
 * car_module: the module of car, such as motor, music, led
 * car_cmd: the command
 * 
 */
void notify_all(enum module car_module, uchar car_cmd, uchar key_module)
{
    switch (car_module)
    {
    case MODULE_MOTOR:
    {
        uchar exe_cmds[2] = {key_module, car_cmd};
        motor_driver.update_state(exe_cmds);
        break;
    }        
    default:
        break;
    }
}