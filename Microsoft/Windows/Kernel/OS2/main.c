/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];
//Huai
#define TASK_STKSIZE 2048
static void task1(void* p_arg);
static void task2(void* p_arg);

//End of Huai

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif


    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();                                                   /* Initialize uC/OS-II                                  */

    //Huai
    OutFileInit();

    InputFile();
    Task_STK = malloc(TASK_NUMBER * sizeof(int*));

    int n;
    for (n = 0; n < TASK_NUMBER; n++) {
        Task_STK[n] = malloc(TASK_STKSIZE * sizeof(int));
    }

    OSTaskCreateExt(task1,
        &TaskParameter[0],
        &Task_STK[0][TASK_STKSIZE - 1],
        TaskParameter[0].TaskPriority,
        TaskParameter[0].TaskID,
        &Task_STK[0][0],
        TASK_STKSIZE,
        &TaskParameter[0],
        (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR)
    );
    OSTaskCreateExt(task2,
        &TaskParameter[1],
        &Task_STK[1][TASK_STKSIZE - 1],
        TaskParameter[1].TaskPriority,
        TaskParameter[1].TaskID,
        &Task_STK[1][0],
        TASK_STKSIZE,
        &TaskParameter[1],
        (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR)
    );



    //End of Huai

//    OSTaskCreateExt( StartupTask,                               /* Create the startup task                              */
//                     0,
//                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u],
//                     APP_CFG_STARTUP_TASK_PRIO,
//                     APP_CFG_STARTUP_TASK_PRIO,
//                    &StartupTaskStk[0u],
//                     APP_CFG_STARTUP_TASK_STK_SIZE,
//                     0u,
//                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//#if OS_TASK_NAME_EN > 0u
//    OSTaskNameSet(         APP_CFG_STARTUP_TASK_PRIO,
//                  (INT8U *)"Startup Task",
//                           &os_err);
//#endif
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}

//Huai
static void task1(void* p_arg)
{
    task_para_set* task_data;
    task_data = p_arg;
    while (1) {
        printf("Tick: %d, Hello from task%d\n", OSTime, task_data->TaskID);
        if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
            fprintf(Output_fp, "Tick: %d, Hello from task%d\n", OSTime, task_data->TaskID);
            fclose(Output_fp);
        }
        OSTimeDly(task_data->TaskPeriodic);
    }
}
static void task2(void* p_arg)
{
    task_para_set* task_data;
    task_data = p_arg;
    while (1) {
        printf("Tick: %d, Hello from task%d\n", OSTime, task_data->TaskID);
        if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
            fprintf(Output_fp, "Tick: %d, Hello from task%d\n", OSTime, task_data->TaskID);
            fclose(Output_fp);
        }

        OSTimeDly(task_data->TaskPeriodic);
    }
}
//End of Huai

/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  StartupTask (void *p_arg)
{
   (void)p_arg;

    OS_TRACE_INIT();                                            /* Initialize the uC/OS-II Trace recorder               */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0u, 0u, 1u, 0u);
		APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet()));
    }
}

