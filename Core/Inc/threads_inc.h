/**
 * @file threads.h
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef THREADS_INC_H_
#define THREADS_INC_H_
#include "cmsis_os.h"

void mainThread(void const *argument);
void forceSwitchThread(void const *argument);
void forceSwitchPriorityThread(void const *argument);
void switchThread(void const *argument);
void switchPriorityThread(void const *argument);
void semaphoreThread(void const *argument);
void queueTransmitterThread(void const *argument);
void queueRecieverThread(void const *argument);

extern osMutexId mutexHandle;
extern osMessageQId queueHandle;
#endif // THREADS_INC_H_
