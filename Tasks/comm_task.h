#ifndef __COMM_TASK_H
#define __COMM_TASK_H

#include "task_manager.h"

void Comm_Task(void *pvParameters);
static void Process_SerialCommand(char *cmd);
static void Show_HelpInfo(void);

#endif
