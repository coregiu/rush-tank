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
void execute_commands(struct command_key *command_key)
{
    notify_all(command_key);
}

/**
 * notify modules to execute the commands.
 * car_module: the module of car, such as motor, music, led
 * car_cmd: the command
 * 
 */
void notify_all(struct command_key *command_key)
{
    switch (command_key->exe_module)
    {
    case MODULE_MOTOR:
    {
        motor_driver.update_state(command_key);
        break;
    }        
    default:
        break;
    }
}