/**
  ******************************************************************************
  *
  * The manager of pwm
  * author: coregiu
  *
  *
  ******************************************************************************
**/
#ifndef _PWM_MANAGER_H
#define _PWM_MANAGER_H

#include <command_receiver.h>

extern const struct module_command_receiver pwm_manager;

void log_pwm_info(struct command_key *command_key);

#endif