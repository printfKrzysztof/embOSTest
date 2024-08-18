/**
 * @file test3.c
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "threads_inc.h"
#include "main.h"

void queueTransmitterThread(void const *argument)
{

    const int max = ((uint8_t *)argument)[0]; // Number of measurements per task
    while (!start_flag)
    {
        osThreadYield(); // Forcing task switch so lower priority has a chance to take context
    }
    int i = 0;
    while (1)
    {
        values[0][i] = __HAL_TIM_GetCounter(&htim2);
        while (osMessagePut(queueHandle, values[0][i], osWaitForever) != osOK)
        {
            values[2][0]++;
        }
        i++;
        osThreadYield();
        if (i >= max)
            break;
    }

    while (1)
    {
        osDelay(10000); // Forcing delay so that main_thread has a chance to take context
    }
}

void queueRecieverThread(void const *argument)
{
    const int max = ((uint8_t *)argument)[0]; // Number of measurements per task
    while (!start_flag)
    {
        osThreadYield(); // Forcing task switch so lower priority has a chance to take context
    }
    osEvent evt;
    int i = 0;
    while (1)
    {

        // Wait for a message from the queue
        evt = osMessageGet(queueHandle, osWaitForever);
        if ((evt.status == osEventMessage) && (evt.value.v == values[0][i]))
        {
            values[1][i++] = __HAL_TIM_GetCounter(&htim2);
            osThreadYield();
        }
        else
        {
            values[3][0]++;
        }

        if (i >= max)
            break;
    }

    while (1)
    {
        osDelay(10000); // Forcing delay so that main_thread has a chance to take context
    }
}