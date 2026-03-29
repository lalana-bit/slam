#include "free_demo.h"


#define TASK_START_STACK_SIZE 128
#define TASK_START_PRIORITY 1

#define TASK1_STACK_SIZE 256
#define TASK1_PRIORITY 2

#define TASK2_STACK_SIZE 128
#define TASK2_PRIORITY 3

#define TASK3_STACK_SIZE 128
#define TASK3_PRIORITY 4


void task_start(void *param);
TaskHandle_t task_start_handle;
StackType_t task_start_stack[TASK_START_STACK_SIZE];
StaticTask_t task_start_tcb;

void task1(void *param);
TaskHandle_t task1_handle;
StackType_t task1_stack[TASK1_STACK_SIZE];
StaticTask_t task1_tcb;

void task2(void *param);
TaskHandle_t task2_handle;
StackType_t task2_stack[TASK2_STACK_SIZE];  // 静态任务的栈空间本体，存放任务的运行时栈内容，数组形式
StaticTask_t task2_tcb;                     // 静态任务的TCB结构体函数控制块，（存放任务优先级，栈顶指针，链表节点，任务状态等各种内核管理信息）

void task3(void *param);
TaskHandle_t task3_handle;
StackType_t task3_stack[TASK3_STACK_SIZE];
StaticTask_t task3_tcb;

int fputc(int ch,FILE *f){
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,100);
    return ch;
}

//分配空闲任务的资源
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
StaticTask_t idle_task_tcb;

void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize){
    *ppxIdleTaskTCBBuffer = &idle_task_tcb;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


//分配软件定时器任务的资源

StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];
StaticTask_t timer_task_tcb;

void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize){
    *ppxTimerTaskTCBBuffer = &timer_task_tcb;
    *ppxTimerTaskStackBuffer = timer_task_stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

// // 启动task_start任务
// void freertos_start(void){
//      xTaskCreate((TaskFunction_t) task_start,
// 						(char *) "task_start",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
// 						(configSTACK_DEPTH_TYPE) TASK_START_STACK_SIZE,
// 						(void *) NULL,
// 						(UBaseType_t) TASK_START_PRIORITY,
// 						(TaskHandle_t *) &task_start_handle );
//     //开启任务调度器，会自动创建idle任务和软件定时器任务（如果开启的话）
//     vTaskStartScheduler();
// }

void freertos_start(void){
   task_start_handle= xTaskCreateStatic((TaskFunction_t) task_start,
                                        (const char *) "task_start",
                                        (uint32_t) TASK_START_STACK_SIZE,
                                        (void *) NULL,
                                        (UBaseType_t) TASK_START_PRIORITY,
                                        (StackType_t *) task_start_stack, 
                                        (StaticTask_t *) &task_start_tcb);
    //开启任务调度器，静态创建任务需实现2个分配资源的接口函数
    vTaskStartScheduler();
}


void task_start(void *param){
    (void)param;

    taskENTER_CRITICAL();
    task1_handle= xTaskCreateStatic(task1,"task1",TASK1_STACK_SIZE,NULL,TASK1_PRIORITY,task1_stack,&task1_tcb);
    task2_handle= xTaskCreateStatic(task2,"task2",TASK2_STACK_SIZE,NULL,TASK2_PRIORITY,task2_stack,&task2_tcb);
    task3_handle= xTaskCreateStatic(task3,"task3",TASK3_STACK_SIZE,NULL,TASK3_PRIORITY,task3_stack,&task3_tcb);
    taskEXIT_CRITICAL(); 

    vTaskDelete(NULL);
}


void task1(void *param){
    (void)param;
    while(1){
        HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
        vTaskDelay(2000);
    }
}

void task2(void *param){
    (void)param;
    while(1){
        HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
        vTaskDelay(800);
    }
}

uint8_t Get_KeyNum(void);
char task_info[100]="";
uint8_t keynum=0;
uint8_t flag=0;
uint8_t i=0;
void task3(void *param){
    (void)param;
    while(1){
        if(keynum != Get_KeyNum()){
            keynum = Get_KeyNum();
            switch(keynum){
                case 1:
                    vTaskSuspend(task1_handle);
                    break;
                case 2:
                    vTaskResume(task1_handle);
                    break;
                case 3:
                    flag=(flag+1)%2;
                    if(flag){
                        vTaskSuspendAll();
                    }
                    else{
                        xTaskResumeAll();
                    }
                    break;
                default:
                    break;
            }
        }
        if(!flag){ // flag=0表示调度器正常，flag=1表示调度器挂起
            i++;
            OLED_Clear();
            OLED_ShowNum(0,0,i,3,16,1,1);
            vTaskDelay(400); // 仅调度器正常时调用，避免卡死
        }
        else{
            // 调度器挂起时，短暂延时（用硬件延时，而非FreeRTOS延时）
            // 比如用HAL_Delay（STM32）或简单的空循环，避免CPU占用100%
            HAL_Delay(10); 
        }
    }
}

uint8_t Get_KeyNum(void){
    uint8_t keynum=0;
    if(HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_SET)
        keynum = 1;
    else if(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == GPIO_PIN_SET)
        keynum = 2;
    else if(HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin)==GPIO_PIN_RESET)
        keynum = 3;
    return keynum;
}
