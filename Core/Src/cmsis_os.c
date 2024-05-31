#include "cmsis_os.h"
#include <stdlib.h>

#define CMSIS2EMBOS(x) ((x) + 3)
#define EMBOS2CMSIS(x) ((osPriority)((x) - 3))

CMSIS_OS_GLOBAL os_global;

//
// System Timer available
//
// Get the RTOS kernel system timer counter.
// MUST REMAIN UNCHANGED: osKernelSysTick shall be consistent in every CMSIS-RTOS.
// return : RTOS kernel system timer as 32-bit value
uint32_t osKernelSysTick0(void)
{
    // returns the system time in timer clock cycles
    return OS_GetTime_Cycles();
}

// ============= Kernel Control Functions ======================================
//
// Initialize the RTOS Kernel for creating objects.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osKernelInitialize shall be consistent in every CMSIS-RTOS.
//
osStatus osKernelInitialize(void)
{
    OS_IncDI();
    OS_InitKern();
    OS_InitHW();
    return osOK;
}

// Start the RTOS Kernel.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osKernelStart shall be consistent in every CMSIS-RTOS.
//
osStatus osKernelStart(void)
{
    /* This function starts the embOS scheduler and schould be the last function
     * called from main().
     *
     * It will activate and start the task with the highest priority.
     * automatically enables interrupts, and never return
     */
    OS_Running = 1u;
    OS_Start();
    return osOK;
}

osStatus osKernelStartThread(osThreadDef_t *thread_def, void *argument)
{
    osThreadCreate(thread_def, argument);
    return osKernelStart();
}

// Check if the RTOS kernel is already started.
// MUST REMAIN UNCHANGED: osKernelRunning shall be consistent in every CMSIS-RTOS.
// return : 0 RTOS is not started, 1 RTOS is started.
//
int32_t osKernelRunning(void)
{
    return OS_IsRunning(); // OS_Running;
}

// ============= Thread Management =============================================
//
// Create a thread and add it to Active Threads and set it to state READY.
// param[in]   thread_def  thread definition referenced with osThread.
// param[in]   argument    pointer that is passed to the thread function as start argument.
// return : thread ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osThreadCreate shall be consistent in every CMSIS-RTOS.
//
osThreadId osThreadCreate(osThreadDef_t *thread_def, void *argument)
{
    OS_CreateTaskEx(thread_def->threadId, (char *)thread_def->name,
                    CMSIS2EMBOS(thread_def->tpriority),
                    (void (*)(void *))(thread_def->pthread), thread_def->stack,
                    thread_def->stacksize, 5, argument);
    return thread_def->threadId;
}

// Return the thread ID of the current running thread.
// return : thread ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osThreadGetId shall be consistent in every CMSIS-RTOS.
//
osThreadId osThreadGetId(void)
{
    return OS_GetTaskID();
}

// Terminate execution of a thread and remove it from Active Threads.
// param[in]   thread_id   thread ID obtained by osThreadCreate or osThreadGetId.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osThreadTerminate shall be consistent in every CMSIS-RTOS.
//
osStatus osThreadTerminate(osThreadId thread_id)
{
    // If pTaskis the NULLpointer, the current task terminates.
    // The specified task will terminate immediately.
    // The memory used for stack and task control block can be reassigned.
    OS_TerminateTask(thread_id);
    return osOK;
}

// Pass control to next thread that is in state READY.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osThreadYield shall be consistent in every CMSIS-RTOS.
//
osStatus osThreadYield(void)
{
    // Calls the scheduler to force a task switch.
    OS_TASK_Yield();
    return osOK;
}

// Change priority of an active thread.
// param[in]   thread_id   thread ID obtained by osThreadCreate or osThreadGetId.
// param[in]   priority    new priority value for the thread function.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osThreadSetPriority shall be consistent in every CMSIS-RTOS.
//
osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority)
{
    OS_SetPriority(thread_id, CMSIS2EMBOS(priority));
    return osOK;
}

// Get current priority of an active thread.
// param[in]   thread_id   thread ID obtained by osThreadCreate or osThreadGetId.
// return : current priority value of the thread function.
// MUST REMAIN UNCHANGED: osThreadGetPriority shall be consistent in every CMSIS-RTOS.
//
osPriority osThreadGetPriority(osThreadId thread_id)
{
    return EMBOS2CMSIS(OS_GetPriority(thread_id));
}

// ============= Generic Wait Functions ========================================
//
// Wait for Timeout (Time Delay).
// param[in]   millisec    time delay value
// return : status code that indicates the execution status of the function.
//
osStatus osDelay(uint32_t millisec)
{
    OS_Delay(osKernelTicksByMilliSec(millisec));
    return osOK;
}

// Generic Wait available
//
// Wait for Signal, Message, Mail, or Timeout.
// param[in] millisec      timeout value or 0 in case of no time-out
// return : event that contains signal, message, or mail information or error code.
// MUST REMAIN UNCHANGED: osWait shall be consistent in every CMSIS-RTOS.
//
osEvent osWait(uint32_t millisec)
{
    osEvent event;
    event.status = osOK;
    return event;
}

// ============= Timer Management Functions ====================================
//
// Create a timer.
// param[in]   timer_def   timer object referenced with osTimer.
// param[in]   type      osTimerOnce for one-shot or osTimerPeriodic for periodic behavior.
// param[in]   argument    argument to the timer call back function.
// return : timer ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osTimerCreate shall be consistent in every CMSIS-RTOS.
//
osTimerId osTimerCreate(osTimerDef_t *timer_def, os_timer_type type,
                        void *argument)
{
    OS_CreateTimerEx(timer_def->timerId,
                     (OS_TIMER_EX_ROUTINE *)(timer_def->ptimer), -1, argument);
    return timer_def->timerId;
}

// Start or restart a timer.
// param[in]   timer_id    timer ID obtained by osTimerCreate.
// param[in]   millisec    time delay value of the timer.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osTimerStart shall be consistent in every CMSIS-RTOS.
//
osStatus osTimerStart(osTimerId timer_id, uint32_t millisec)
{
    OS_SetTimerPeriodEx(timer_id, osKernelTicksByMilliSec(millisec));
    OS_RetriggerTimerEx(timer_id);
    return osOK;
}

// Stop the timer.
// param[in]   timer_id    timer ID obtained by osTimerCreate.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osTimerStop shall be consistent in every CMSIS-RTOS.
//
osStatus osTimerStop(osTimerId timer_id)
{
    OS_StopTimerEx(timer_id);
    return osOK;
}

// Delete a timer that was created by osTimerCreate.
// param[in]   timer_id    timer ID obtained by osTimerCreate.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osTimerDelete shall be consistent in every CMSIS-RTOS.
//
osStatus osTimerDelete(osTimerId timer_id)
{
    OS_DeleteTimerEx(timer_id);
    return osOK;
}

// ============= Signal Management =============================================
//
// Set the specified Signal Flags of an active thread.
// param[in]   thread_id   thread ID obtained by osThreadCreate or osThreadGetId.
// param[in]   signals     specifies the signal flags of the thread that should be set.
// return : previous signal flags of the specified thread
//          or 0x80000000 in case of incorrect parameters.
// MUST REMAIN UNCHANGED: osSignalSet shall be consistent in every CMSIS-RTOS.
//
int32_t osSignalSet(osThreadId thread_id, int32_t signals)
{
    // Returns a list of events that have occurred for a specified task.
    // The event mask of the events that have actually occurred
    // the actual events remain signaled
    //
    int32_t result = OS_GetEventsOccurred(thread_id);
    OS_SignalEvent(signals, thread_id);
    return result;
}

int32_t OS_ClearEvent(osThreadId thread_id, int32_t signals)
{
    if (thread_id == 0)
        thread_id = osThreadGetId();
    int32_t result = thread_id->Events;
    // uint32_t val = osDisableInterrupt( );
    OS_DisableInt(); // MOV.W   R1, #0x80   MSR.W   BASEPRI, R1
    thread_id->Events &= ~signals;
    if (OS_Global.Counters.Cnt.DI == 0)
        OS_EnableInt(); // MOV.W   R1, #0x00   MSR.W   BASEPRI, R1
    // osRestoreInterrupt( val );
    return result;
}

// Clear the specified Signal Flags of an active thread.
// param[in]   thread_id   thread ID obtained by osThreadCreate or osThreadGetId.
// param[in]   signals     specifies the signal flags of the thread that shall be cleared.
// return : previous signal flags of the specified thread
//          or 0x80000000 in case of incorrect parameters.
// MUST REMAIN UNCHANGED: osSignalClear shall be consistent in every CMSIS-RTOS.
//
int32_t osSignalClear(osThreadId thread_id, int32_t signals)
{
    // Returns the actual state of events
    // and then clears the events of a specified task.
    // Returns the actual state of events and then
    // clears ** the ALL events ** of a specified task.
    //
    // return OS_ClearEvents( thread_id );
    //
    return OS_ClearEvent(thread_id, NULL);
}

// Wait for one or more Signal Flags to become signaled for the current RUNNING thread.
// param[in]   signals     wait until all specified signal flags set or 0 for any single signal flag.
// param[in]   millisec    timeout value or 0 in case of no time-out.
// return : event flag information or error code.
// MUST REMAIN UNCHANGED: osSignalWait shall be consistent in every CMSIS-RTOS.
//
osEvent osSignalWait(int32_t signals, uint32_t millisec)
{
    osEvent event;
    event.status = osEventSignal;

    // Not allowed in ISR ?
    // event.status = osErrorISR
    //
    // The task is not suspended even if no events are signaled.
    if (millisec == 0)
    {
        // Returns a list of events that have occurred for a specified task.
        // The event mask of the events that have actually occurred.
        event.value.signals = OS_GetEventsOccurred(0);
    }

    else if (millisec == osWaitForever)
    {
        if (signals == 0) // Wait forever until any single signal flag
        {
            // Waits for one of the events specified in the bitmask and
            // clears the event memory after an event occurs
            event.value.signals = OS_WaitEvent(0xFFFFFFFF);
        }

        else // Wait forever until all specified signal flags set
        {
            // Waits for one or more of the events specified by the Eventmask
            // and clears only those events that were specified in the eventmask.
            event.value.signals = OS_WaitSingleEvent(signals);
        }
    }

    else
    {
        if (signals == 0) // Wait millisec until any single signal flag
        {
            // Waits for the specified events for a given time, and clears
            // ** the event memory ** after one of the requsted events occurs,
            // or after the timeout expired.
            event.value.signals = OS_WaitEventTimed(0xFFFFFFFF,
                                                    osKernelTicksByMilliSec(millisec));
        }

        else // Wait millisec until all specified signal flags set
        {
            // Waits for the specified events for a given time; after an event occurs,
            // only ** the requested events ** are cleared.
            event.value.signals = OS_WaitSingleEventTimed(signals,
                                                          osKernelTicksByMilliSec(millisec));
        }
    }

    if (event.value.signals == 0)
    {
        event.status = (millisec > 0) ? osEventTimeout : osOK;
    }

    return event;
}

// ============= Mutex Management ==============================================
//
/* Resource semaphores are used for managingresources by avoiding conflicts
 * caused by simultaneous use of a resource. The resource managed can be of
 * any kind: a part of the program that is not reentrant, a piece of hardware
 * like the display, a flash prom that can only be written to by a single task
 * at a time, a motor in a CNC control that can only be controlled by one task
 * at a time, and a lot more.
 *
 * The basic procedure is as follows:
 * Any task that uses a resource first claims it calling the OS_Use() or
 * OS_Request() routines of embOS. If the resource is available, the program
 * execution of the task continues, but the resource is blocked for other tasks.
 *
 * If a second task now tries to use the same resource while it is in use
 * by the first task, this second task is suspended until the first task releases
 * the resource. However, if the first task that uses the resource calls
 * OS_Use() again for that resource, it is not suspended because the resource
 * is blocked only for other tasks.
 *
 * A resource semaphore contains a counter that keeps track of how many times
 * the resource has been claimed by calling OS_Request() or OS_Use()
 * by a particular task. It is released when that counter reaches 0,
 * which means the OS_Unuse() routine has to be called exactly the same number
 * of times as OS_Use() or OS_Request(). If it is not, the resource remains
 * blocked for other tasks.
 *
 * On the other hand, a task cannot release a resource that it does not own
 * by calling OS_Unuse().
 *
 * counter = 0 after OS_CreateRSema()
 * counter++ : OS_Use() or OS_Request()
 * counter-- : OS_Unuse()
 *
 * A programmer can prefer mutex rather than creating a semaphore with count 1.
 */
// Create and Initialize a Mutex object.
// param[in]   mutex_def   mutex definition referenced with osMutex.
// return : mutex ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osMutexCreate shall be consistent in every CMSIS-RTOS.
//
osMutexId osMutexCreate(osMutexDef_t *mutex_def)
{
    OS_CreateRSema(mutex_def->mutexId);
    return mutex_def->mutexId;
}

// Wait until a Mutex becomes available.
// param[in]   mutex_id    mutex ID obtained by osMutexCreate.
// param[in]   millisec    timeout value or 0 in case of no time-out.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMutexWait shall be consistent in every CMSIS-RTOS.
//
osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec)
{
    osStatus status = osOK;

    if (millisec == 0)
    {
        if (OS_Request(mutex_id) == 0)
        {
            status = osErrorResource;
        }
    }

    else if (millisec == osWaitForever)
    {
        OS_Use(mutex_id);
    }

    else if (0 == OS_UseTimed(mutex_id, osKernelTicksByMilliSec(millisec)))
    {
        status = osErrorTimeoutResource;
    }

    return status;
}

// Release a Mutex that was obtained by osMutexWait.
// param[in]   mutex_id    mutex ID obtained by osMutexCreate.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMutexRelease shall be consistent in every CMSIS-RTOS.
//
osStatus osMutexRelease(osMutexId mutex_id)
{
    OS_Unuse(mutex_id);
    return osOK;
}

// Delete a Mutex that was created by osMutexCreate.
// param[in]   mutex_id    mutex ID obtained by osMutexCreate.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMutexDelete shall be consistent in every CMSIS-RTOS.
//
osStatus osMutexDelete(osMutexId mutex_id)
{
    OS_DeleteRSema(mutex_id);
    return osOK;
}

// ============= Semaphore Management Functions ================================
/* Counting semaphores are counters that are managed by embOS.
 * They are not as widely used as resource semaphores, events or mailboxes,
 * but they can be very useful sometimes.
 *
 * They are used in situations where a task needs to wait for something
 * that can be signaled one or more times.
 *
 * The semaphores can be accessed from any point, any task,
 * or any interrupt in any way.
 *
 * OS_CreateCSema()
 * Creates a counting semaphore with a specified initial count value
 *
 * OS_SignalCSema()
 * Increments the counter of a semaphore.
 * If one or more tasks are waiting for an event to be signaled to this
 * semaphore, the task that has the highest priority will become the running task
 *
 * OS_WaitCSema()
 * Decrements the counter of a semaphore.
 * If the counter of the semaphore is not 0, the counter is decremented
 * and program execution continues.
 * If the counter is 0, WaitCSema()waits until the counter is incremented by
 * another task, a timer or an interrupt handler via a call to OS_SignalCSema().
 * The counter is then decremented and program execution continues.
 *
 * OS_WaitCSemaTimed()
 * Decrements a semaphore counter if the semaphore is available
 * within a specified time.
 * If the semaphore was not signaled within the specified time, the program
 * execution continues but returns a value of 0.
 *
 * OS_CSemaRequest()
 * Decrements the counter of a semaphore, if it is signaled.
 * If the counter is 0, OS_CSemaRequest() does not wait and does not modify
 * the semaphore counter. The function returns with error state.
 *
 */

// Create and Initialize a Semaphore object used for managing resources.
// param[in]   semaphore_def semaphore definition referenced with osSemaphore.
// param[in]   count     number of available resources.
// return : semaphore ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osSemaphoreCreate shall be consistent in every CMSIS-RTOS.
//
osSemaphoreId osSemaphoreCreate(osSemaphoreDef_t *semaphore_def,
                                int32_t count)
{
    // Creates a counting semaphore with a specified initial count value.
    OS_CreateCSema(semaphore_def->semaphoreId, count);
    return semaphore_def->semaphoreId;
}

// Wait until a Semaphore token becomes available.
// param[in]  semaphore_id  semaphore object referenced with osSemaphoreCreate.
// param[in]  millisec      timeout value or 0 in case of no time-out.
// return :   number of available tokens : (tokens after wait) + 1
//            or -1 in case of incorrect parameters.
// MUST REMAIN UNCHANGED: osSemaphoreWait shall be consistent in every CMSIS-RTOS.
//
int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec)
{
    int32_t result = -1; // OS_WaitCSemaTimed() timeout

    if (millisec == 0)
    {
        // Decrements the counter of a semaphore, if it is signaled
        if (OS_CSemaRequest(semaphore_id))
        {
            // Returns the counter value of a specified semaphore
            result = OS_GetCSemaValue(semaphore_id);
        }
    }

    else if (millisec == osWaitForever)
    {
        // Decrements the counter of a semaphore
        // If the counter of the semaphore is not 0, the counter is decremented
        // and program execution continues.
        // If the counter is 0, WaitCSema() waits until the counter is incremented
        // by another task, a timer or an interrupt handler
        // via a call to OS_SignalCSema().
        OS_WaitCSema(semaphore_id);

        // Returns the counter value of a specified semaphore
        result = OS_GetCSemaValue(semaphore_id);
    }
    // Decrements a semaphore counter if the semaphore is available
    // within a specified time.
    else if (OS_WaitCSemaTimed(semaphore_id,
                               osKernelTicksByMilliSec(millisec)))
    {
        result = OS_GetCSemaValue(semaphore_id);
    }

    return result + 1;
}

/**
 * @brief Release a Semaphore token
 * @param  semaphore_id  semaphore object referenced with osSemaphore.
 * @retval  status code that indicates the execution status of the function.
 * @note   MUST REMAIN UNCHANGED: osSemaphoreRelease shall be consistent in every CMSIS-RTOS.
 */
osStatus osSemaphoreRelease(osSemaphoreId semaphore_id)
{
    // Increments the counter of a semaphore
    // If one or more tasks are waiting for an event to be signaled to
    // this semaphore, the task that has the highest priority will
    // become the running task.
    OS_SignalCSema(semaphore_id);
    return osOK;
}

// Release a Semaphore token.
// param[in]     semaphore_id  semaphore object referenced with osSemaphoreCreate.
// return :  status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osSemaphoreRelease shall be consistent in every CMSIS-RTOS.
//
osStatus osSemaphoreDelete(osSemaphoreId semaphore_id)
{
    // Deletes a specified semaphore.
    // Before deleting a semaphore, make sure that no task is waiting for it
    // and that notask will signal that semaphore at a later point.
    OS_DeleteCSema(semaphore_id);
    return osOK;
}

// ============= Memory Pool Management Functions ==============================

// Create and Initialize a memory pool.
// param[in]   pool_def    memory pool definition referenced with osPool.
// return : memory pool ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osPoolCreate shall be consistent in every CMSIS-RTOS.
//
osPoolId osPoolCreate(osPoolDef_t *pool_def)
{
    // void OS_MEMF_Create (OS_MEMF* pMEMF, void* pPool, OS_UINT NumBlocks, OS_UINT BlockSize);
    // BlockSize is aligment at 4 Bytes !!!
    //
    OS_MEMF_Create(pool_def->poolId, pool_def->pool, pool_def->pool_sz,
                   pool_def->item_sz);
    return pool_def->poolId;
}

// Allocate a memory block from a memory pool.
// param[in]   pool_id     memory pool ID obtain referenced with osPoolCreate.
// return : address of the allocated memory block or NULL in case of no memory available.
// MUST REMAIN UNCHANGED: osPoolAlloc shall be consistent in every CMSIS-RTOS.
//
// Requests allocation of a memory block.
// Waits until a block of memory is available
// If there is no free memory block in the pool, the calling task is suspended
// until a memory block becomes available.
//
void *osPoolAlloc(osPoolId pool_id)
{
    return OS_MEMF_Alloc(pool_id, 0);
}

// Requests allocation of a memory block. Waits until a block of memory
// is available or the timeout has expired.
//
void *osPoolAllocTimed(osPoolId pool_id, uint32_t millisec)
{
    return OS_MEMF_AllocTimed(pool_id, osKernelTicksByMilliSec(millisec), 0);
}

// Requests allocation of a memory block. Continues execution in any case.
// The calling task is never suspended by calling OS_MEMF_Request()
//
void *osPoolRequest(osPoolId pool_id)
{
    return OS_MEMF_Request(pool_id, 0);
}

// Allocate a memory block from a memory pool and set memory block to zero.
// param[in]   pool_id     memory pool ID obtain referenced with osPoolCreate.
// return : address of the allocated memory block or NULL in case of no memory available.
// MUST REMAIN UNCHANGED: osPoolCAlloc shall be consistent in every CMSIS-RTOS.
void *osPoolCAlloc(osPoolId pool_id)
{
    void *p = osPoolAlloc(pool_id);

    if (p)
    {
        memset(p, 0, pool_id->BlockSize);
    }

    return p;
}

// Return an allocated memory block back to a specific memory pool.
// param[in]   pool_id     memory pool ID obtain referenced with osPoolCreate.
// param[in]   block     address of the allocated memory block that is returned to the memory pool.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osPoolFree shall be consistent in every CMSIS-RTOS.
osStatus osPoolFree(osPoolId pool_id, void *block)
{
    OS_MEMF_Release(pool_id, block);
    return osOK;
}

// ============= Message Queue Management Functions =============================

// Create and Initialize a Message Queue.
// param[in]   queue_def   queue definition referenced with osMessageQ.
// param[in]   thread_id   thread ID (obtained by osThreadCreate or osThreadGetId) or NULL.
// return : message queue ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osMessageCreate shall be consistent in every CMSIS-RTOS.
//
osMessageQId osMessageCreate(osMessageQDef_t *queue_def,
                             osThreadId thread_id)
{
    OS_CreateMB(queue_def->messageQId, 4, queue_def->queue_sz, queue_def->pool);
    return queue_def->messageQId;
}

// Put a Message to a Queue.
// param[in]   queue_id    message queue ID obtained with osMessageCreate.
// param[in]   info      message information.
// param[in]   millisec    timeout value or 0 in case of no time-out.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMessagePut shall be consistent in every CMSIS-RTOS.
//
osStatus osMessagePut(osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
    osStatus status = osOK;

    if (millisec == 0)
    {
        if (OS_PutMailCond(queue_id, (const void *)&info) > 0)
        {
            status = osErrorResource;
        }
    }
    else if (millisec == osWaitForever)
    {
        OS_PutMail(queue_id, (const void *)&info);
    }
    else
    {
        OS_TIME osKernelTickCountPrev = osKernelTickCount();

        while (1)
        {
            if (OS_PutMailCond(queue_id, (const void *)&info) == 0)
                return status;

            osDelay(1);

            if ((osKernelTickCount() - osKernelTickCountPrev) > osKernelTicksByMilliSec(millisec))
                return osErrorTimeoutResource;
        }
    }

    return status;
}

// Get a Message or Wait for a Message from a Queue.
// param[in]   queue_id    message queue ID obtained with osMessageCreate.
// param[in]   millisec    timeout value or 0 in case of no time-out.
// return : event information that includes status code.
// MUST REMAIN UNCHANGED: osMessageGet shall be consistent in every CMSIS-RTOS.
//
osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec)
{
    osEvent event;
    event.status = osEventMessage;

    // The task is not suspended even if no events are signaled.
    if (millisec == 0)
    {
        if (OS_GetMailCond(queue_id, &event.value.v) > 0)
        {
            event.status = osOK;
        }
    }

    else if (millisec == osWaitForever)
    {
        OS_GetMail(queue_id, &event.value.v);
    }

    else if (OS_GetMailTimed(queue_id, &event.value.v,
                             osKernelTicksByMilliSec(millisec)) > 0)
    {
        event.status = osEventTimeout;
    }

    return event;
}

osStatus osMessageDelete(osMessageQId queue_id)
{
    OS_DeleteMB(queue_id);
    return osOK;
}

// ============= Mail Queue Management Functions ===============================

// Mail Queues available
// Create and Initialize mail queue.
// param[in]   queue_def   reference to the mail queue definition obtain with osMailQ
// param[in]   thread_id   thread ID (obtained by osThreadCreate or osThreadGetId) or NULL.
// return : mail queue ID for reference by other functions or NULL in case of error.
// MUST REMAIN UNCHANGED: osMailCreate shall be consistent in every CMSIS-RTOS.
//
osMailQId osMailCreate(osMailQDef_t *queue_def, osThreadId thread_id)
{
    queue_def->mailId->messageId = osMessageCreate(queue_def->messageQDef,
                                                   thread_id);
    queue_def->mailId->poolId = osPoolCreate(queue_def->poolDef);
    return queue_def->mailId;
}

// Allocate a memory block from a mail.
// param[in]   queue_id    mail queue ID obtained with osMailCreate.
// param[in]   millisec    timeout value or 0 in case of no time-out
// return : pointer to memory block that can be filled with mail or NULL in case of error.
// MUST REMAIN UNCHANGED: osMailAlloc shall be consistent in every CMSIS-RTOS.
//
void *osMailAlloc(osMailQId queue_id, uint32_t millisec)
{
    void *p;

    if (millisec == 0)
    {
        p = osPoolRequest(queue_id->poolId);
    }

    else if (millisec == osWaitForever)
    {
        p = osPoolAlloc(queue_id->poolId);
    }

    else
    {
        p = osPoolAllocTimed(queue_id->poolId, millisec);
    }

    return p;
}

// Allocate a memory block from a mail and set memory block to zero.
// param[in]   queue_id    mail queue ID obtained with osMailCreate.
// param[in]   millisec    timeout value or 0 in case of no time-out
// return : pointer to memory block that can be filled with mail or NULL in case of error.
// MUST REMAIN UNCHANGED: osMailCAlloc shall be consistent in every CMSIS-RTOS.
//
void *osMailCAlloc(osMailQId queue_id, uint32_t millisec)
{
    void *p = osMailAlloc(queue_id, millisec);

    if (p)
    {
        memset(p, 0, queue_id->poolId->BlockSize);
    }

    return p;
}

// Put a mail to a queue.
// param[in]   queue_id    mail queue ID obtained with osMailCreate.
// param[in]   mail      memory block previously allocated with osMailAlloc or osMailCAlloc.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMailPut shall be consistent in every CMSIS-RTOS.
//
osStatus osMailPut(osMailQId queue_id, void *mail)
{
    return osMessagePut(queue_id->messageId, (uint32_t)mail, osWaitForever);
}

// Get a mail from a queue.
// param[in]   queue_id    mail queue ID obtained with osMailCreate.
// param[in]   millisec    timeout value or 0 in case of no time-out
// return : event that contains mail information or error code.
// MUST REMAIN UNCHANGED: osMailGet shall be consistent in every CMSIS-RTOS.
//
osEvent osMailGet(osMailQId queue_id, uint32_t millisec)
{
    osEvent event = osMessageGet(queue_id->messageId, millisec);

    if (event.status == osEventMessage)
    {
        event.status = osEventMail;
    }

    return event;
}

// Free a memory block from a mail.
// param[in]   queue_id    mail queue ID obtained with osMailCreate.
// param[in]   mail      pointer to the memory block that was obtained with osMailGet.
// return : status code that indicates the execution status of the function.
// MUST REMAIN UNCHANGED: osMailFree shall be consistent in every CMSIS-RTOS.
//
osStatus osMailFree(osMailQId queue_id, void *mail)
{
    return osPoolFree(queue_id->poolId, mail);
}

// ============= Memory Management Functions ===================================

static osMutexDef(cmsis_memory);
static osMutexId cmsis_memory;

osStatus osMemoryLock(uint32_t timeout)
{
    if (cmsis_memory == 0)
        cmsis_memory = osMutexCreate(osMutex(cmsis_memory));

    return osMutexWait(cmsis_memory, timeout);
}

void osMemoryUnlock(void)
{
    osMutexRelease(cmsis_memory);
}

/* Allocates a block of size bytes of memory, returning a pointer
 * to the beginning of the block. The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * On success, a pointer to the memory block allocated by the function.
 * The type of this pointer is always void*, which can be cast to the desired
 * type of data pointer in order to be dereferenceable.
 *
 * If the function failed to allocate the requested block of memory,
 * a null pointer is returned.
 *
 */
void *osMalloc(size_t size, uint32_t timeout)
{
    void *p = 0;
    osStatus status = osMemoryLock(timeout);
    if (status == osOK)
    {
        p = malloc(size);
        osMemoryUnlock();
    }
    return p;
}

/* Allocates a block of memory for an array of num elements,
 * each of them size bytes long, and initializes all its bits to zero.
 *
 * The effective result is the allocation of a zero-initialized memory block
 * of (num*size) bytes.
 *
 * On success, a pointer to the memory block allocated by the function.
 * The type of this pointer is always void*, which can be cast to the desired
 * type of data pointer in order to be dereferenceable.
 *
 * If the function failed to allocate the requested block of memory,
 * a null pointer is returned.
 *
 */
void *osCalloc(size_t nelem, size_t elsize, uint32_t timeout)
{
    void *p = osMalloc(nelem * elsize, timeout);

    if (p)
    {
        memset(p, 0, nelem * elsize);
    }

    return p;
}

/* Changes the size of the memory block pointed to by ptr.
 * The function may move the memory block to a new location (whose address is
 * returned by the function).
 *
 * The content of the memory block is preserved up to the lesser of the new
 * and old sizes, even if the block is moved to a new location.
 *
 * If the new size is larger, the value of the newly allocated portion
 * is indeterminate.
 *
 * In case that ptr is a null pointer, the function behaves like malloc,
 * assigning a new block of size bytes and returning a pointer to its beginning.
 *
 * If the function fails to allocate the requested block of memory,
 * a null pointer is returned, and the memory block pointed to by argument ptr
 * is not deallocated (it is still valid, and with its contents unchanged).
 *
 * A pointer to the reallocated memory block, which may be either the same
 * as ptr or a new location. The type of this pointer is void*, which can be
 * cast to the desired type of data pointer in order to be dereferenceable.
 */
void *osRealloc(void *ptr, size_t size, uint32_t timeout)
{
    void *p = 0;
    osStatus status = osMemoryLock(timeout);
    if (status == osOK)
    {
        p = realloc(ptr, size);
        osMemoryUnlock();
    }
    return p;
}

/* A block of memory previously allocated by a call to malloc, calloc or realloc
 * is deallocated, making it available again for further allocations.
 *
 * If ptr does not point to a block of memory allocated with the above functions
 * it causes undefined behavior.
 * If ptr is a null pointer, the function does nothing.
 *
 * Notice that this function does not change the value of ptr itself,
 * hence it still points to the same (now invalid) location.
 *
 */
void osFree(void *ptr)
{
    osMemoryLock(osWaitForever);
    free(ptr);
    osMemoryUnlock();
}

/*
 * Informs RTOS that interrupt code is executing.
 *
 * If osEnterInterrupt() is used, it should be the first function to be called
 * in the interrupt handler. It must be used with osLeaveInterrupt() as the last
 * function called. The use of this function has the following effects, it:
 *
 * disables task switches
 * keeps interrupts in internal routines disabled.
 */
void osEnterInterrupt(void)
{
    OS_EnterInterrupt();
}

/*
 * Informs RTOS that the end of the interrupt routine has been reached;
 * executes task switching within ISR.
 *
 * If osLeaveInterrupt()is used, it should be the last function to be called
 * in the interrupt handler. If the interrupt has caused a task switch, it will
 * be executed (unless the program which was interrupted was in a critical region).
 *
 */
void osLeaveInterrupt(void)
{
    OS_LeaveInterrupt();
}

uint32_t osDisableInterrupt(void)
{

    __disable_interrupt();
    return (uint32_t)0;
}

void osRestoreInterrupt(uint32_t val)
{
    __enable_interrupt();
}