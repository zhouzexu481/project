#ifndef __CONTROL_TASK_H
#define __CONTROL_TASK_H

#include "task_manager.h"

void Control_Task(void *pvParameters);
static void Process_ControlCommand(ControlCommand_t *cmd);

#endif
