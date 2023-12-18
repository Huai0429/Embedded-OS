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
static void task3(void* p_arg);

//End of Huai

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask(void* p_arg);


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
int cmp(task_para_set* TP1, task_para_set* TP2)
{
    /*int x = TP1->TaskExecutionTime;
    int y = TP2->TaskExecutionTime;*/
    int x = TP1->TaskPeriodic;
    int y = TP2->TaskPeriodic;
    //printf("%d %d\n", x, y);
    if (x > y) return x > y;
    else if (x == y) return TP1->TaskPriority < TP2->TaskPriority ? 0 : 1;
    return x > y;
}
int  main(void)
{
#if OS_TASK_NAME_EN > 0u
    CPU_INT08U  os_err;
#endif
    ResCtr = 2;
    //R1_PRIO = 63;
    //R2_PRIO = 63;


    CPU_IntInit();

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();                                                   /* Initialize uC/OS-II                                  */

    //Huai
    OutFileInit();

    InputFile();
    Task_STK = malloc(TASK_NUMBER * sizeof(int*));
    TaskCtr = calloc(TASK_NUMBER*4+1, sizeof(int));
    ResPrio = calloc(ResCtr, sizeof(int));
    for (int i = 0; i < ResCtr; i++) ResPrio[i] = 63;
    ResHeldBy = calloc(ResCtr, sizeof(int));

    R1InUse = 0;
    R2InUse = 0;
    int n;
    for (n = 0; n < TASK_NUMBER; n++) {
        Task_STK[n] = malloc(TASK_STKSIZE * sizeof(int));
    }
    qsort(TaskParameter, TASK_NUMBER, sizeof(TaskParameter[0]), cmp);
    for (int i = 0; i < TASK_NUMBER; i++) {
        printf("%d %d %d %d %d %d %d %d\n", TaskParameter[i].TaskID, TaskParameter[i].TaskArriveTime, TaskParameter[i].TaskExecutionTime, 
            TaskParameter[i].TaskPeriodic,TaskParameter[i].R1Lock, TaskParameter[i].R1UnLock, TaskParameter[i].R2Lock, TaskParameter[i].R2UnLock);
    }
    /*TaskParameter[0].TaskPriority = 0;
    OSTaskCreateExt(task1,
        &TaskParameter[0],
        &Task_STK[0][TASK_STKSIZE - 1],
        TaskParameter[0].TaskPriority,
        TaskParameter[0].TaskID,
        &Task_STK[0][0],
        TASK_STKSIZE,
        &TaskParameter[0],
        (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR),
        TaskParameter[0].TaskExecutionTime,
        TaskParameter[0].TaskArriveTime,
        TaskParameter[0].TaskPeriodic
    );
    TaskParameter[1].TaskPriority = 1;
    OSTaskCreateExt(task2,
        &TaskParameter[1],
        &Task_STK[1][TASK_STKSIZE - 1],
        TaskParameter[1].TaskPriority,
        TaskParameter[1].TaskID,
        &Task_STK[1][0],
        TASK_STKSIZE,
        &TaskParameter[1],
        (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR),
        TaskParameter[1].TaskExecutionTime,
        TaskParameter[1].TaskArriveTime,
        TaskParameter[1].TaskPeriodic
    );*/


    for (int i = 0; i < TASK_NUMBER; i++) {
        TaskParameter[i].TaskPriority = (i + 1) * 3;
        //TaskParameter[i].TaskPriority = i;
        //printf("Task Prio %d \n", TaskParameter[i].TaskPriority);
        
        OSTaskCreateExt(task1,
            &TaskParameter[i],
            &Task_STK[i][TASK_STKSIZE - 1],
            TaskParameter[i].TaskPriority,
            TaskParameter[i].TaskID,
            &Task_STK[i][0],
            TASK_STKSIZE,
            &TaskParameter[i],
            (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR),
            TaskParameter[i].TaskExecutionTime,
            TaskParameter[i].TaskArriveTime,
            TaskParameter[i].TaskPeriodic,
            TaskParameter[i].R1Lock,
            TaskParameter[i].R1UnLock,
            TaskParameter[i].R2Lock,
            TaskParameter[i].R2UnLock
        );
    }
    for (int i = TASK_NUMBER - 1; i >= 0; i--) {
        if ((TaskParameter[i].R1Lock != 0 || TaskParameter[i].R1UnLock != 0) && ResPrio[0] > TaskParameter[i].TaskPriority) {
            ResPrio[0] = TaskParameter[i].TaskPriority - 1;
        }
        if ((TaskParameter[i].R2Lock != 0 || TaskParameter[i].R2UnLock != 0) && ResPrio[1] > TaskParameter[i].TaskPriority) {
            ResPrio[1] = TaskParameter[i].TaskPriority - 2;
        }
    }
    printf("Mutex Create on Prio %d %d \n", ResPrio[0], ResPrio[1]);
    INT8U err;
    R1 = OSMutexCreate(ResPrio[0], &err);
    R2 = OSMutexCreate(ResPrio[1], &err);


    printf("\n================TCB linked list================\n");
    printf("Task\t Prev_TCB_addr \t TCB_addr\t Next_TCB_addr\n");
    OS_TCB* head = OSTCBHead;
    while (head != 0) {
        printf("%2d\t %11x\t   %6x\t      %6x\t \n", head->OSTCBPrio == 63 ? head->OSTCBPrio : head->OSTCBPrio + 1, head->OSTCBPrev, head, head->OSTCBNext);
        head = head->OSTCBNext;
    }
    //next_task_pri = (OSUnMapTbl[OSRdyGrp] << 3) + OSUnMapTbl[OSRdyTbl[OSUnMapTbl[OSRdyGrp]]];

    printf("Tick\tEvent\t      CurrentTask ID\tNextTask ID\t  ResponseTime\t  BlockingTime \t PreemptionTime   \n");

    /*if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
        fclose(Output_fp);
    }*/


    //End of Huai

    OSTimeSet(0);
    OSStart();
    /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }
}

//Huai
static void task1(void* p_arg)
{

    task_para_set* task_data;
    task_data = p_arg;
    INT8U perr;
    while (1) {
        OSTCBCur->StartTime = OSTimeGet();
        int temp = OSTimeGet();
        OSTCBCur->Executed = 0;
        OSTCBCur->NextReadyTime = OSTCBCur->ArrivesTime + (TaskCtr[OSTCBCur->OSTCBId] + 1) * OSTCBCur->PeriodicTime;
        //printf("%2d task %d Enter Ready Time %d\n", OSTimeGet(), OSTCBCur->OSTCBId, OSTCBCur->NextReadyTime);
        while (!OSTCBCur->Executed /*|| !OSTCBCur->MissDeadline*/) {

            //ResourceBelong();
            //if (OSTimeGet() == OSTCBCur->R1Lock + OSTCBCur->StartTime + OSTCBCur->PreemptionTime && OSTCBCur->R1UnLock > 0 && !R1InUse) {
            //    //printf("%2d R1 lock %d %d %d\n", OSTimeGet(),OSTCBCur->R1Lock, OSTCBCur->StartTime, OSTCBCur->PreemptionTime);
            //    /*printf("%2d\tLockResource\ttask(%2d)(%2d)\t R1\n", OSTimeGet(), OSTCBCur->OSTCBId, TaskCtr[OSTCBCur->OSTCBPrio]); */
            //    //printf("%2d Lock R1 %d\n", OSTimeGet(), OSTCBCur->OSTCBId);
            //    R1InUse = 1;
            //    OSMutexPend(R1, OSTCBCur->R1UnLock - OSTCBCur->R1Lock, &perr);
            //}
            //if (OSTimeGet() == OSTCBCur->R2Lock + OSTCBCur->StartTime + OSTCBCur->PreemptionTime && OSTCBCur->R2UnLock > 0 && !R2InUse) {
            //    /*printf("%2d\tLockResource\ttask(%2d)(%2d)\t R2\n", OSTimeGet(), OSTCBCur->OSTCBId, TaskCtr[OSTCBCur->OSTCBPrio]);*/
            //    //printf("%2d Lock R2 %d\n", OSTimeGet(), OSTCBCur->OSTCBId);
            //    R2InUse = 1;
            //    OSMutexPend(R2, OSTCBCur->R2UnLock - OSTCBCur->R2Lock, &perr);
            //}
            //if (OSTimeGet() == OSTCBCur->R1UnLock + OSTCBCur->StartTime + OSTCBCur->PreemptionTime && OSTCBCur->R1UnLock > 0 && R1InUse) {
            //    /* printf("%2d\tUnlockResource\ttask(%2d)(%2d)\t R1\n", OSTimeGet(), OSTCBCur->OSTCBId, TaskCtr[OSTCBCur->OSTCBPrio]);*/
            //    R1InUse = 0;
            //    OSMutexPost(R1);
            //}
            //if (OSTimeGet() == OSTCBCur->R2UnLock + OSTCBCur->StartTime + OSTCBCur->PreemptionTime && OSTCBCur->R2UnLock > 0 && R2InUse) {
            //    /*printf("%2d\tUnLockResource\ttask(%2d)(%2d)\t R2\n", OSTimeGet(), OSTCBCur->OSTCBId, TaskCtr[OSTCBCur->OSTCBPrio]);*/
            //    R2InUse = 0;
            //    OSMutexPost(R2);
            //}
        }

        OSTCBCur->EndTime = OSTimeGet();
        OSTCBCur->DelayTime = OSTCBCur->NextReadyTime - OSTimeGet();
        //printf("Delay: %d %d %d\n", OSTimeGet(), OSTCBCur->NextReadyTime, OSTimeGet() - OSTCBCur->NextReadyTime);
        OSTimeDly(OSTCBCur->NextReadyTime - OSTimeGet());
    }
}
static void task2(void* p_arg)
{
    task_para_set* task_data;
    task_data = p_arg;

    while (1) {
        int StartTime = OSTimeGet();
        while (!OSTCBCur->Executed) {
            //printf("break task2\n"); 
            OSTCBCur->DelayTime = OSTimeGet();
            //cnt2++;
            //printf("%2d\t Completion\t task(%2d)(%2d)\t ", OSTimeGet(), task_data->TaskID, cnt2);
            //printf("%2d\t Completion\n", OSTimeGet());
            //printf("D2 %d %d %d %d\n", task_data->TaskPeriodic, EndTime, OSTCBCur->NextReadyTime, abs(EndTime - OSTCBCur->NextReadyTime));
            OSTimeDly(abs(OSTimeGet() - OSTCBCur->NextReadyTime));

        }
        //printf("executing task\n");

    }

    //OSTimeDly(task_data->TaskPeriodic);
    /*while (1) {
        if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
            printf("%d\t task(%2d)(%2d)\t", OSTime, task_data->TaskID,cnt1);
            fprintf(Output_fp, "%d\t task(%2d)(%2d)\t", OSTime, task_data->TaskID,cnt1);
            fclose(Output_fp);
        }
        cnt1++;
        OSTimeDly(task_data->TaskPeriodic);
    }*/

}
static void task3(void* p_arg)
{
    /*task_para_set* task_data;
    task_data = p_arg;
    while (1) {
        if ((Output_err = fopen_s(&Output_fp, "./Output.txt", "a")) == 0) {
            printf("%d\t task(%2d)(%2d)\t", OSTime, task_data->TaskID, cnt2);
            fprintf(Output_fp, "%d\t task(%2d)(%2d)\t", OSTime, task_data->TaskID, cnt2);
            fclose(Output_fp);
        }
        cnt2++;
        OSTimeDly(task_data->TaskPeriodic);
    }*/
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

static  void  StartupTask(void* p_arg)
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

