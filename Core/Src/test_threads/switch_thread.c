/**
 * @file switch_thread.c
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "threads_inc.h"
#include "main.h"

void switchThread(void const *argument)
{

    const uint8_t data = ((uint8_t *)argument)[0]; // Task number
    const int max = ((uint8_t *)argument)[1];      // Number of measurements per task
    while (!start_flag)
    {
        osDelay(1); // Forcing task switch so lower priority has a chance to take context
    }

    int i = 0;
    uint32_t temp;
    values[data][i++] = __HAL_TIM_GetCounter(&htim2);
    values[data][i] = __HAL_TIM_GetCounter(&htim2);
    while (1)
    {
        temp = __HAL_TIM_GetCounter(&htim2);
        if ((temp - values[data][i]) > 1000)
        {
            values[data][++i] = temp;
            values[data][++i] = temp;
        }
        else
        {
            values[data][i] = temp;
        }

        if (i >= max)
            break;
    }

    while (1)
    {
        osDelay(10); // Forcing delay so that main_thread has a chance to take context
    }
}