/**
  ******************************************************************************
  *
  * main controller
  * author: coregiu
  * 
  * 
  ******************************************************************************
**/

#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

#include <mcs51/8051.h>
#include <ps2_receiver.h>
#include <motor_driver.h>
#include <command_receiver.h>

/**
 * init uart and all receive modules
 * 
 */
void init_modules();

/**
 * execute commands;
 * 
 */
void execute_commands(uchar key_module, uchar **commands);

/**
 * notify modules to execute the commands.
 * car_module: the module of car, such as motor, music, led
 * car_cmd: the command
 * 
 */
void notify_all(uchar car_module, uchar car_cmd, uchar key_module);

#endif