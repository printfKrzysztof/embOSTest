#ifndef __CMSIS_OS_H__
#define __CMSIS_OS_H__

#include <stdint.h>
#include <stddef.h>

#include "RTOS.h"

#ifndef OS_TIMER_FREQ
#define OS_TIMER_FREQ (SystemCoreClock)
#endif
#ifndef OS_TICK_FREQ
#define OS_TICK_FREQ (1000u)
#endif
#ifndef OS_INT_FREQ
#define OS_INT_FREQ (OS_TICK_FREQ)
#endif

#define OS_STKSIZE 256
// API version (main [31:16] .sub [15:0])
#define osCMSIS 0x10002
// RTOS identification and version (main [31:16] .sub [15:0])
#define osCMSIS_RTX ((4 << 16) | 00)
// RTOS identification string
#define osKernelSystemId "EMBOS V4.00"
//
// main thread      1=main can be thread, 0=not available
#define osFeature_MainThread 1
//
// Memory Pools:    1=available, 0=not available
#define osFeature_Pool 1
//
// Mail Queues:     1=available, 0=not available
#define osFeature_MailQ 1
//
// Message Queues:  1=available, 0=not available
#define osFeature_MessageQ 1
//
// maximum number of Signal Flags available per thread
// bit31 = 0x80000000 : incorrect parameters
#define osFeature_Signals 31
//
// maximum count for osSemaphoreCreate function
#define osFeature_Semaphore 0xFFFFFFFF
//
// osWait function: 1=available, 0=not available
#define osFeature_Wait 0
//
// osKernelSysTick functions: 1=available, 0=not available
#define osFeature_SysTick 1
//
//
//
#ifdef __cplusplus
extern "C"
{
#endif

    // ==== Enumeration, structures, defines =======================================
    //
    // Priority used for thread control.
    // MUST REMAIN UNCHANGED: osPriority shall be consistent in every CMSIS-RTOS.
    //
    typedef enum
    {
        osPriorityIdle = -3,        // priority: idle (lowest)
        osPriorityLow = -2,         // priority: low
        osPriorityBelowNormal = -1, // priority: below normal
        osPriorityNormal = 0,       // priority: normal (default)
        osPriorityAboveNormal = +1, // priority: above normal
        osPriorityHigh = +2,        // priority: high
        osPriorityRealtime = +3,    // priority: realtime (highest)
        osPriorityError = 0x84      // system cannot determine priority
    } osPriority;                   // or thread has illegal priority

// Timeout value.
// MUST REMAIN UNCHANGED: osWaitForever shall be consistent in every CMSIS-RTOS.
//
#define osWaitForever 0xFFFFFFFF // wait forever timeout value
    // Status code values returned by CMSIS-RTOS functions.
    // MUST REMAIN UNCHANGED: osStatus shall be consistent in every CMSIS-RTOS.
    //
    typedef enum
    {
        // function completed; no error or event occurred.
        osOK = 0,
        //
        // function completed; signal event occurred.
        osEventSignal = 0x08,
        // function completed; message event occurred.
        osEventMessage = 0x10,
        // function completed; mail event occurred.
        osEventMail = 0x20,
        //
        // function completed; timeout occurred.
        osEventTimeout = 0x40,
        //
        // parameter error: a mandatory parameter was missing or specified an incorrect object.
        osErrorParameter = 0x80,
        //
        // resource not available: a specified resource was not available.
        osErrorResource = 0x81,
        // resource not available within given time: a specified resource was not available within the timeout period.
        osErrorTimeoutResource = 0xC1,
        // not allowed in ISR context: the function cannot be called from interrupt service routines.
        osErrorISR = 0x82,
        // function called multiple times from ISR with same object.
        osErrorISRRecursive = 0x83,
        // system cannot determine priority or thread has illegal priority.
        osErrorPriority = 0x84,
        // system is out of memory: it was impossible to allocate or reserve memory for the operation.
        osErrorNoMemory = 0x85,
        // value of a parameter is out of range.
        osErrorValue = 0x86,
        // unspecified RTOS error: run-time error but no other error message fits.
        osErrorOS = 0xFF,
        //
        os_status_reserved = 0x7FFFFFFF
        // prevent from enum down-size compiler optimization.
        // 32 bits for osStatus
    } osStatus;

    // Timer type value for the timer definition.
    // MUST REMAIN UNCHANGED: os_timer_type shall be consistent in every CMSIS-RTOS.
    //
    typedef enum
    {
        osTimerOnce = 0,    // one-shot timer
        osTimerPeriodic = 1 // repeating timer --- EMBOS can not support !
    } os_timer_type;

    typedef struct _CMSIS_OS_GLOBAL
    {
        uint32_t dummy;
    } CMSIS_OS_GLOBAL;

    extern CMSIS_OS_GLOBAL os_global;

    // Entry point of a thread.
    // MUST REMAIN UNCHANGED: os_pthread shall be consistent in every CMSIS-RTOS.
    //
    typedef void (*os_pthread)(void *argument);

    // Entry point of a timer call back function.
    // MUST REMAIN UNCHANGED: os_ptimer shall be consistent in every CMSIS-RTOS.
    //
    typedef void (*os_ptimer)(void *argument);

    // >>> the following data type definitions may shall adapted towards a specific RTOS

    // Thread ID identifies the thread (pointer to a thread control block).
    // CAN BE CHANGED: os_thread_cb is implementation specific in every CMSIS-RTOS.
    //
    typedef OS_TASK osThreadType;
    typedef osThreadType *osThreadId;

    // Timer ID identifies the timer (pointer to a timer control block).
    // CAN BE CHANGED: os_timer_cb is implementation specific in every CMSIS-RTOS.
    //
    typedef OS_TIMER_EX osTimerType;
    typedef osTimerType *osTimerId;

    // Mutex ID identifies the mutex (pointer to a mutex control block).
    // CAN BE CHANGED: os_mutex_cb is implementation specific in every CMSIS-RTOS.
    //
    typedef OS_RSEMA osMutexType;
    typedef osMutexType *osMutexId;

    // Semaphore ID identifies the semaphore (pointer to a semaphore control block).
    // CAN BE CHANGED: os_semaphore_cb is implementation specific in every CMSIS-RTOS.
    //
    typedef OS_CSEMA osSemaphoreType;
    typedef osSemaphoreType *osSemaphoreId;

    // Pool ID identifies the memory pool (pointer to a memory pool control block).
    // CAN BE CHANGED: os_pool_cb is implementation specific in every CMSIS-RTOS.
    //
    typedef OS_MEMF osPoolType;
    typedef osPoolType *osPoolId;

    // Message ID identifies the message queue (pointer to a message queue control block).
    // CAN BE CHANGED: os_messageQ_cb is implementation specific in every CMSIS-RTOS.
    //
    // OS_MAILBOX : Messages of fixed size
    // CMSIS_OS   : Messages of fixed size : 4 Bytes for Value or Pointer
    //
    typedef OS_MAILBOX osMessageQType;
    typedef osMessageQType *osMessageQId;

    // Mail ID identifies the mail queue (pointer to a mail queue control block).
    // CAN BE CHANGED: os_mailQ_cb is implementation specific in every CMSIS-RTOS.
    //
    // OS_MAILBOX : Messages of fixed size
    // CMSIS_OS   : Messages of fixed size : 1..32767 Bytes for Buffer
    //
    typedef struct _osMailQ_cb
    {
        osMessageQId messageId;
        osPoolId poolId;
    } osMailQType;

    typedef osMailQType *osMailQId;

    // Thread Definition structure contains startup information of a thread.
    // CAN BE CHANGED: os_thread_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_thread_def
    {
        osThreadId threadId;
        uint8_t *name;
        os_pthread pthread;   // start address of thread function
        osPriority tpriority; // initial thread priority
        uint32_t stacksize;   // stack size requirements in bytes;
        uint32_t *stack;      //
    } osThreadDef_t;

    // Timer Definition structure contains timer parameters.
    // CAN BE CHANGED: os_timer_def is implementation specific in every CMSIS-RTOS.
    //
    typedef const struct os_timer_def
    {
        osTimerId timerId;
        os_ptimer ptimer;
    } osTimerDef_t;

    // Mutex Definition structure contains setup information for a mutex.
    // CAN BE CHANGED: os_mutex_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_mutex_def
    {
        osMutexId mutexId;
    } osMutexDef_t;

    // Semaphore Definition structure contains setup information for a semaphore.
    // CAN BE CHANGED: os_semaphore_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_semaphore_def
    {
        osSemaphoreId semaphoreId;
    } osSemaphoreDef_t;

    // Definition structure for memory block allocation.
    // CAN BE CHANGED: os_pool_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_pool_def
    {
        osPoolId poolId;
        uint32_t pool_sz; // number of items (elements) in the pool
        uint32_t item_sz; // size of an item
        void *pool;       // pointer to memory for pool
    } osPoolDef_t;

    // Definition structure for message queue.
    // CAN BE CHANGED: os_messageQ_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_messageQ_def
    {
        osMessageQId messageQId;
        uint32_t queue_sz; // number of elements in the queue
        void *pool;        // memory array for messages
    } osMessageQDef_t;

    // Definition structure for mail queue.
    // CAN BE CHANGED: os_mailQ_def is implementation specific in every CMSIS-RTOS.
    //
    typedef struct os_mailQ_def
    {
        osMailQId mailId;
        osMessageQDef_t *messageQDef;
        osPoolDef_t *poolDef;
        uint32_t queue_sz; // number of elements in the queue
        uint32_t item_sz;  // size of an item
    } osMailQDef_t;

    // Event structure contains detailed information about an event.
    // MUST REMAIN UNCHANGED: os_event shall be consistent in every CMSIS-RTOS.
    // However the struct may be extended at the end.
    //
    typedef struct
    {
        osStatus status; // status code: event or error information

        union
        {
            uint32_t v;      // message as 32-bit value
            void *p;         // message or mail as void pointer
            int32_t signals; // signal flags
        } value;             // event value

        union
        {
            osMailQId mail_id;       // mail id obtained by osMailCreate
            osMessageQId message_id; // message id obtained by osMessageCreate
        } def;                       // event definition

    } osEvent;

// ======= Kernel Control Functions ============================================

// The RTOS kernel system timer frequency in Hz.
// Reflects the system timer setting and is typically defined in a configuration file.
#define osKernelSysTickFrequency (OS_FSYS)
// The RTOS kernel frequency in Hz.
// Reflects the system timer setting and is typically defined in a configuration file.
#define osKernelTickFrequency (OS_TICK_FREQ)
//
#define osKernelTickPeriod (1 / osKernelTickFrequency)
#define osKernelTicksPerSecond (osKernelTickFrequency)

#if (osKernelTickFrequency == 1000)
#define osKernelTicksPerMilliSec (1)
#else
#define osKernelTicksPerMilliSec (osKernelTickFrequency / 1000)
#endif

//
// Convert timeout in millisec to system ticks
#if (osKernelTickFrequency == 1000)
#define osKernelTicksByMilliSec(millisec) (millisec)
#else
#define osKernelTicksByMilliSec(millisec) ((millisec) * osKernelTicksPerMilliSec)
#endif

// Convert timeout in second to system ticks
#define osKernelTicksBySecond(second) ((second) * osKernelTicksPerSecond)

// Convert kernel ticks to millisec
#if (osKernelTickFrequency == 1000)
#define osKernelTicks2MilliSec(ticks) (ticks)
#else
#define osKernelTicks2MilliSec(ticks) ((ticks) / osKernelTicksPerMilliSec)
#endif

// Convert kernel ticks to second
#define osKernelTicks2Second(ticks) ((ticks) / osKernelTicksPerSecond)
//
//
#define osKernelSysTicksPerSecond (osKernelSysTickFrequency)
#define osKernelSysTicksPerMilliSec (osKernelSysTickFrequency / 1000)
//
// Convert timeout in millisec to system ticks
#define osKernelSysTicksByMilliSec(millisec) ((millisec) * osKernelSysTicksPerMilliSec)
// Convert timeout in second to system ticks
#define osKernelSysTicksBySecond(second) ((second) * osKernelSysTicksPerSecond)
// Convert system ticks to millisec
#define osKernelSysTicks2MilliSec(ticks) ((ticks) / osKernelSysTicksPerMilliSec)
// Convert system ticks to second
#define osKernelSysTicks2Second(ticks) ((ticks) / osKernelSysTicksPerSecond)
    //

#define osKernelSysTickMicroSec_i \
    (osKernelSysTickFrequency / 1000000)
//
#define osKernelSysTickMicroSec_f \
    ((((uint64_t)(osKernelSysTickFrequency - 1000000 * (osKernelSysTickFrequency / 1000000))) << 16) / 1000000)
//
// Convert a microseconds value to a RTOS kernel system timer value.
#define osKernelSysTickMicroSec(microsec) \
    ((microsec * osKernelSysTickMicroSec_i) + ((microsec * osKernelSysTickMicroSec_f) >> 16))
//
#define osKernelSysTickMilliSec(millisec) \
    osKernelSysTicksByMilliSec(millisec)

// return RTOS kernel time as 32-bit value in milli second
//
// #include "rt_Time.h"
// #define osKernelTickTime                ( os_time / osKernelTicksPerMilliSec )
//
#define osKernelTickTime (OS_Time / osKernelTicksPerMilliSec)
#define osKernelTickCount() OS_GetTime32()
#define osKernelSysTick() OS_GetTime_Cycles()

    osStatus osKernelInitialize(void);
    osStatus osKernelStart(void);
    int32_t osKernelRunning(void);

// ======= Thread Management ===================================================
//
// Create a Thread Definition with function, priority, and stack requirements.
// param         name         name of the thread function.
// param         priority     initial priority of the thread function.
// param         instances    number of possible thread instances.
// param         stacksz      stack size (in bytes) requirements for the thread function.
// CAN BE CHANGED: The parameters to osThreadDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
//
#if defined(osObjectsExternal)
#define osThreadDef(name, thread, priority, instances, stacksz) \
    extern osThreadDef_t os_thread_def_##name
#else
#define osThreadDef(name, thread, priority, instances, stacksz)                  \
    OS_TASK os_thread_id_##name;                                                 \
    uint32_t os_thread_stack_##name[((stacksz ? stacksz : OS_STKSIZE) + 3) / 4]; \
    osThreadDef_t os_thread_def_##name =                                         \
        {&os_thread_id_##name, #name, (os_pthread)(thread), (priority),          \
         ((((stacksz ? stacksz : OS_STKSIZE) + 3) / 4) << 2),                    \
         os_thread_stack_##name}
#endif

#define osThread(name) \
    &os_thread_def_##name

    osThreadId osThreadCreate(osThreadDef_t *thread_def, void *argument);
    osThreadId osThreadGetId(void);
    osStatus osThreadTerminate(osThreadId thread_id);
    osStatus osThreadYield(void);
    osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority);
    osPriority osThreadGetPriority(osThreadId thread_id);

    // ======= Generic Wait Functions ==============================================
    //
    // Wait for Timeout (Time Delay).
    // param[in]     millisec      time delay value
    // return status code that indicates the execution status of the function.
    //
    osStatus osDelay(uint32_t millisec);

#if (defined(osFeature_Wait) && (osFeature_Wait != 0))
    osEvent osWait(uint32_t millisec);
#endif

// ======= Timer Management Functions ==========================================
//
// Define a Timer object.
// param         name          name of the timer object.
// param         function      name of the timer call back function.
// CAN BE CHANGED: The parameter to osTimerDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
#if defined(osObjectsExternal)
#define osTimerDef(name, function) \
    extern osTimerDef_t os_timer_def_##name
#else
#define osTimerDef(name, function)     \
    OS_TIMER_EX os_timer_id_##name;    \
    osTimerDef_t os_timer_def_##name = \
        {&os_timer_id_##name, (function)}
#endif

#define osTimer(name) \
    &os_timer_def_##name

    osTimerId osTimerCreate(osTimerDef_t *timer_def, os_timer_type type,
                            void *argument);
    osStatus osTimerStart(osTimerId timer_id, uint32_t millisec);
    osStatus osTimerStop(osTimerId timer_id);
    osStatus osTimerDelete(osTimerId timer_id);

    // ======= Signal Management ===================================================
    //
    int32_t osSignalSet(osThreadId thread_id, int32_t signals);
    int32_t osSignalClear(osThreadId thread_id, int32_t signals);
    osEvent osSignalWait(int32_t signals, uint32_t millisec);

// ======= Mutex Management ====================================================
#if defined(osObjectsExternal)
#define osMutexDef(name) \
    extern osMutexDef_t os_mutex_def_##name
#else
#define osMutexDef(name)         \
    OS_RSEMA os_mutex_id_##name; \
    osMutexDef_t os_mutex_def_##name = {&os_mutex_id_##name}
#endif

#define osMutex(name) \
    &os_mutex_def_##name

    osMutexId osMutexCreate(osMutexDef_t *mutex_def);
    osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec);
    osStatus osMutexRelease(osMutexId mutex_id);
    osStatus osMutexDelete(osMutexId mutex_id);

    // ======= Semaphore Management Functions ======================================

#if (defined(osFeature_Semaphore) && (osFeature_Semaphore != 0))
//
// Define a Semaphore object.
// param         name          name of the semaphore object.
// CAN BE CHANGED: The parameter to osSemaphoreDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
//
#if defined(osObjectsExternal)
#define osSemaphoreDef(name) \
    extern osSemaphoreDef_t os_semaphore_def_##name
#else
#define osSemaphoreDef(name)         \
    OS_CSEMA os_semaphore_id_##name; \
    osSemaphoreDef_t os_semaphore_def_##name = {&os_semaphore_id_##name}
#endif

#define osSemaphore(name) \
    &os_semaphore_def_##name

    osSemaphoreId osSemaphoreCreate(osSemaphoreDef_t *semaphore_def,
                                    int32_t count);
    int32_t osSemaphoreWait(osSemaphoreId semaphore_id, uint32_t millisec);
    osStatus osSemaphoreRelease(osSemaphoreId semaphore_id);
    osStatus osSemaphoreDelete(osSemaphoreId semaphore_id);

#endif

// ============= Memory Pool Management Functions ==============================
//
#if (defined(osFeature_Pool) && (osFeature_Pool != 0))
//
// \brief Define a Memory Pool.
// param         name          name of the memory pool.
// param         no            maximum number of blocks (objects) in the memory pool.
// param         type          data type of a single block (object).
// CAN BE CHANGED: The parameter to osPoolDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
#if defined(osObjectsExternal)
#define osPoolDef(name, no, type) \
    extern osPoolDef_t os_pool_def_##name
#else
#define osPoolDef(name, no, type)                                                        \
    OS_MEMF os_pool_id_##name;                                                           \
    uint32_t                                                                             \
        os_pool_m_##name[((OS_MEMF_SIZEOF_BLOCKCONTROL + sizeof(type) + 3) / 4) * (no)]; \
    osPoolDef_t                                                                          \
        os_pool_def_##name =                                                             \
            {&os_pool_id_##name, (no),                                                   \
             (((OS_MEMF_SIZEOF_BLOCKCONTROL + sizeof(type) + 3) / 4) << 2),              \
             (os_pool_m_##name)}
#endif

#define osPool(name) \
    &os_pool_def_##name

    osPoolId osPoolCreate(osPoolDef_t *pool_def);
    void *osPoolAlloc(osPoolId pool_id);
    void *osPoolCAlloc(osPoolId pool_id);
    osStatus osPoolFree(osPoolId pool_id, void *block);

#endif

    // ======= Message Queue Management Functions ==================================

#if (defined(osFeature_MessageQ) && (osFeature_MessageQ != 0))
//
// \brief Create a Message Queue Definition.
// param         name          name of the queue.
// param         queue_sz      maximum number of messages in the queue.
// param         type          data type of a single message element (for debugger).
// CAN BE CHANGED: The parameter to osMessageQDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
//
#if defined(osObjectsExternal)
#define osMessageQDef(name, queue_sz, type) \
    extern osMessageQDef_t os_messageQ_def_##name
#else
#define osMessageQDef(name, queue_sz, type)          \
    OS_MAILBOX os_messageQ_id_##name;                \
    uint32_t os_messageQ_q_##name[(queue_sz)] = {0}; \
    osMessageQDef_t os_messageQ_def_##name =         \
        {&os_messageQ_id_##name, (queue_sz << 2), (os_messageQ_q_##name)}
#endif

#define osMessageQ(name) \
    &os_messageQ_def_##name

    osMessageQId osMessageCreate(osMessageQDef_t *queue_def,
                                 osThreadId thread_id);
    osStatus osMessagePut(osMessageQId queue_id, uint32_t info, uint32_t millisec);
    osEvent osMessageGet(osMessageQId queue_id, uint32_t millisec);
    osStatus osMessageDelete(osMessageQId queue_id);

#endif
// ======= Mail Queue Management Functions =====================================
#if (defined(osFeature_MailQ) && (osFeature_MailQ != 0))
//
// \brief Create a Mail Queue Definition.
// param         name          name of the queue
// param         queue_sz      maximum number of messages in queue
// param         type          data type of a single message element
// CAN BE CHANGED: The parameter to osMailQDef shall be consistent but the
//       macro body is implementation specific in every CMSIS-RTOS.
//
#if defined(osObjectsExternal)
#define osMailQDef(name, queue_sz, type) \
    extern osMailQDef_t os_mailQ_def_##name
#else
#define osMailQDef(name, queue_sz, type)                                    \
    osMailQ_cb os_MailQ_id_##name;                                          \
    osPoolDef(mail_##name, queue_sz, type);                                 \
    osMessageQDef(mail_##name, queue_sz);                                   \
    osMailQDef_t os_mailQ_def_##name =                                      \
        {&os_MailQ_id_##name, osMessageQ(mail_##name), osPool(mail_##name), \
         queue_sz, (((sizeof(type) + 3) >> 2) << 2)}
#endif

#define osMailQ(name) \
    &os_mailQ_def_##name
    osMailQId osMailCreate(osMailQDef_t *queue_def, osThreadId thread_id);
    void *osMailAlloc(osMailQId queue_id, uint32_t millisec);
    void *osMailCAlloc(osMailQId queue_id, uint32_t millisec);
    osStatus osMailPut(osMailQId queue_id, void *mail);
    osEvent osMailGet(osMailQId queue_id, uint32_t millisec);
    osStatus osMailFree(osMailQId queue_id, void *mail);

#endif

    // ============= Memory Management Functions ===================================

    osStatus osMemoryLock(uint32_t timeout);

    void osMemoryUnlock(void);

    /*
     * ANSI C offers some basic dynamic memory management functions.
     * These are malloc, free, and realloc. Unfortunately, these routines are
     * not thread-safe, unless a special thread-safe implementation exists
     * in the compiler specific runtime libraries; they can only be used from
     * one task or by multiple tasks if they are called sequentially.
     *
     * Therefore, embOS offer task-safe variants of these routines.
     * These variants have the same names as their ANSI counterparts,
     * but are prefixed OS_; they are called OS_malloc(), OS_free(), OS_realloc().
     *
     * The thread-safe variants that embOS offers use the standard ANSI routines,
     * but they guarantee that the calls are serialized using a resource semaphore.
     *
     * If heap memory management is not supported by the standard C-libraries
     * for a specific CPU, embOS heap memory management is not implemented.
     *
     * Heap type memory management is part of the embOS libraries.
     * It does not use any resources if it is not referenced by the application
     * (that is, if the application does not use any memory management API function).
     *
     * Note that another aspect of these routines may still be a problem:
     * the memory used for the functions (known as heap) may fragment.
     *
     * This can lead to a situation where the total amount of memory is sufficient,
     * but there is not enough memory available in a single block
     * to satisfy an allocation request.
     *
     */

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
    void *osMalloc(size_t size, uint32_t timeout);

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
    void *osCalloc(size_t nelem, size_t elsize, uint32_t timeout);

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
    void *osRealloc(void *ptr, size_t size, uint32_t timeout);

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
    void osFree(void *ptr);

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
    void osEnterInterrupt(void);

    /*
     * Informs RTOS that the end of the interrupt routine has been reached;
     * executes task switching within ISR.
     *
     * If osLeaveInterrupt()is used, it should be the last function to be called
     * in the interrupt handler. If the interrupt has caused a task switch, it will
     * be executed (unless the program which was interrupted was in a critical region).
     *
     */
    void osLeaveInterrupt(void);

    uint32_t osDisableInterrupt(void);

    void osRestoreInterrupt(uint32_t val);

#ifdef __cplusplus
}
#endif

#endif // __CMSIS_OS_H__