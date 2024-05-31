/**
 * @file main_thread.c
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "stdlib.h"
#include "inttypes.h"
#include "threads_inc.h"
#include "main.h"
#include "frame_controller.h"
#include "cmsis_os.h"
// Threads definitions

osThreadId tasks[MAX_THREADS];
#define defalut_stack_size 156

uint32_t os_thread_stack[80][(defalut_stack_size + 3) / 4];
osThreadDef_t os_thread_def[80];
OS_TASK os_thread_id[80];

osThreadDef(ForceSwitchHigh, forceSwitchPriorityThread, osPriorityAboveNormal, 0, defalut_stack_size);
osThreadDef(ForceSwitchLow, forceSwitchPriorityThread, osPriorityBelowNormal, 0, defalut_stack_size);

osThreadDef(SwitchNormal, switchThread, osPriorityNormal, 0, defalut_stack_size);
osThreadDef(SwitchHigh, switchPriorityThread, osPriorityAboveNormal, 0, defalut_stack_size);
osThreadDef(SwitchLow, switchPriorityThread, osPriorityBelowNormal, 0, defalut_stack_size);

osThreadDef(SemaphoreThread, semaphoreThread, osPriorityNormal, 0, defalut_stack_size);
osSemaphoreDef(Semaphore);

osThreadDef(QueueTransmitter, queueTransmitterThread, osPriorityNormal, 0, 128);
osThreadDef(QueueReciever, queueRecieverThread, osPriorityNormal, 0, 128);
osMessageQDef(Queue, 16, uint32_t);

void resetValues(uint8_t *buffer_tx, uint8_t *buffer_rx)
{
    for (size_t i = 0; i < MAX_THREADS; i++)
    {
        for (size_t j = 0; j < MAX_TEST_PER_THREAD; j++)
        {
            values[i][j] = 0;
            values[i][j] = 0;
        }
    }
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    start_flag = 0;

    for (size_t i = 0; i < COMMAND_FRAME_SIZE; i++)
    {
        buffer_rx[i] = 0;
    }
    for (size_t i = 0; i < SCORE_FRAME_SIZE; i++)
    {
        buffer_tx[i] = 0;
    }
}

void mainThread(void const *argument)
{

    (void)(argument);
    uint8_t buffer_rx[COMMAND_FRAME_SIZE];
    uint8_t buffer_tx[SCORE_FRAME_SIZE];
    while (1)
    {

        if (HAL_UART_Receive(&huart2, buffer_rx, COMMAND_FRAME_SIZE, 100) == HAL_OK)
        {

            commands_em command;
            uint8_t arg_count, args[4];
            if (DecodeCommandFrame(buffer_rx, &command, &arg_count, args) == 0)
            {
                switch (command)
                {
                case CMD_TASK_FORCE_SWITCH:

                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of threads
                    // Argument 2 - Number of measurements per task

                    uint8_t task_args[args[0]][2];
                    memset(task_args, 0, sizeof(task_args));
                    for (size_t i = 0; i < args[0]; i++)
                    {
                        task_args[i][0] = i;
                        task_args[i][1] = args[1];
                        os_thread_def[i].threadId = &os_thread_id[i];
                        os_thread_def[i].pthread = (os_pthread)forceSwitchThread;
                        os_thread_def[i].tpriority = osPriorityNormal;
                        os_thread_def[i].stacksize = (defalut_stack_size + 3) / 4 * 4; // Align stack size
                        os_thread_def[i].name = "ForceSwitchNormal";
                        os_thread_def[i].stack = os_thread_stack[i];
                        // Create the task
                        tasks[i] = osThreadCreate(&os_thread_def[i], task_args[i]);
                        if (tasks[i] == NULL)
                        {
                            // Handle error: Failed to create task
                        }
                    }

                    osDelay(1);
                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(10); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_TASK_FORCE_SWITCH, (uint16_t)(args[1] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_TASK_FORCE_SWITCH_PRIORITY:

                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of threads Low
                    // Argument 2 - Number of threads High
                    // Argument 3 - Number of measurements per task
                    uint8_t task_count_sum = args[0] + args[1];
                    uint8_t task_args[task_count_sum][2];
                    for (size_t i = 0; i < task_count_sum; i++)
                    {
                        task_args[i][0] = i;
                        task_args[i][1] = args[2];
                        if (i < args[0])
                            tasks[i] = osThreadCreate(osThread(ForceSwitchLow), task_args[i]);
                        else
                            tasks[i] = osThreadCreate(osThread(ForceSwitchHigh), task_args[i]);
                    }

                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(10); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < task_count_sum; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    for (size_t i = 0; i < task_count_sum; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_TASK_FORCE_SWITCH_PRIORITY, (uint16_t)(args[2] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_TASK_SWITCH:
                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of threads
                    // Argument 2 - Number of measurements per task

                    uint8_t task_args[args[0]][2];
                    for (size_t i = 0; i < args[0]; i++)
                    {
                        task_args[i][0] = i;
                        task_args[i][1] = args[1];
                        tasks[i] = osThreadCreate(osThread(SwitchNormal), task_args[i]);
                    }

                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(1000); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_TASK_SWITCH, (uint16_t)(args[1] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_TASK_SWITCH_PRIORITY:
                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of threads Low
                    // Argument 2 - Number of threads High
                    // Argument 3 - Number of measurements per task
                    uint8_t task_count_sum = args[0] + args[1];
                    uint8_t task_args[task_count_sum][2];
                    for (size_t i = 0; i < task_count_sum; i++)
                    {
                        task_args[i][0] = i;
                        task_args[i][1] = args[2];
                        if (i < args[0])
                            tasks[i] = osThreadCreate(osThread(SwitchLow), task_args[i]);
                        else
                            tasks[i] = osThreadCreate(osThread(SwitchHigh), task_args[i]);
                    }

                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(1000); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < task_count_sum; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    for (size_t i = 0; i < task_count_sum; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_TASK_SWITCH_PRIORITY, (uint16_t)(args[2] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_SEMAPHORE:
                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of threads
                    // Argument 2 - Number of measurements per task

                    uint8_t task_args[args[0]][2];
                    semaphoreHandle = osSemaphoreCreate(osSemaphore(Semaphore), 1); // Creating bionary semaphore (mutex)
                    for (size_t i = 0; i < args[0]; i++)
                    {
                        task_args[i][0] = i;
                        task_args[i][1] = args[1];
                        tasks[i] = osThreadCreate(osThread(SemaphoreThread), task_args[i]);
                    }

                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(10); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    osSemaphoreDelete(semaphoreHandle);

                    for (size_t i = 0; i < args[0]; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_SEMAPHORE, (uint16_t)(args[1] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_QUEUE:
                {
                    resetValues(buffer_tx, buffer_rx); // Reseting all values

                    // Argument 1 - Number of measurements per task

                    queueHandle = osMessageCreate(osMessageQ(Queue), NULL); // Creating bionary semaphore (mutex)

                    tasks[0] = osThreadCreate(osThread(QueueTransmitter), args);
                    tasks[1] = osThreadCreate(osThread(QueueReciever), args);

                    HAL_TIM_Base_Start(&htim2);
                    start_flag = 1;
                    osDelay(10); // 10 milisecond block for main task

                    HAL_TIM_Base_Stop(&htim2);

                    for (size_t i = 0; i < 2; i++) // For each task
                    {
                        osThreadTerminate(tasks[i]);
                    }

                    osMessageDelete(queueHandle);

                    for (size_t i = 0; i < 2; i++) // For each task
                    {
                        if (CodeScoreFrame(buffer_tx, CMD_QUEUE, (uint16_t)(args[0] * 4), (uint8_t *)(values[i])) == 0)
                        {
                            HAL_UART_Transmit(&huart2, buffer_tx, SCORE_FRAME_SIZE, 1000);
                        }
                        // osDelay(10);
                    }
                }
                break;

                case CMD_COMMAND_NO:
                {
                    if (CodeScoreFrame(buffer_tx, CMD_COMMAND_NO, 0, args) == 0)
                        HAL_UART_Transmit(&huart2, buffer_tx, 10, 100);
                }
                break;
                default:
                    break;
                }
            }
        }
    }
}
