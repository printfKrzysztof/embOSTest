/**
 * @file frame_controller.h
 * @author Krzysztof Bortnowski (krzysztof.bortnowski@sesto.pl)
 * @brief
 * @version 0.1
 * @date 2024-04-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "stdlib.h"
#include "inttypes.h"

#define MAX_ARGS_COMMAND_FRAME 4
#define MAX_ARGS_SCORE_FRAME 400
#define COMMAND_FRAME_SIZE (5 + MAX_ARGS_COMMAND_FRAME)
#define SCORE_FRAME_SIZE (6 + MAX_ARGS_SCORE_FRAME)

typedef enum
{
    CMD_TASK_FORCE_SWITCH = 0,
    CMD_TASK_FORCE_SWITCH_PRIORITY,
    CMD_TASK_SWITCH,
    CMD_TASK_SWITCH_PRIORITY,
    CMD_SEMAPHORE,
    CMD_QUEUE,
    CMD_RESEND_LAST_FRAME,
    CMD_COMMAND_NO
} commands_em;

#define ERR_FRM_NO_START -1
#define ERR_FRM_CRC -2
#define ERR_FRM_WRONG_COMMAND -3
#define ERR_FRM_TOO_MANY_ARGS -4

/**
 * @brief Decodes with CRC Check
 *
 * @param frame
 * @param command
 * @param arg_count
 * @param args
 * @return int
 */
int DecodeCommandFrame(uint8_t *frame, uint8_t *command, uint8_t *arg_count, uint8_t *args);

/**
 * @brief Codes frame with CRC
 *
 * @param frame
 * @param command
 * @param arg_count
 * @param args
 * @return ERR_FRM
 */
int CodeScoreFrame(uint8_t *frame, uint8_t command, uint16_t arg_count, uint8_t *args);