#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if 1
// Simple led task
void vTaskCode( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    */
    for( ;; ) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();

#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

    TaskHandle_t xHandle = NULL;
    /* Create the task, storing the handle. */
    BaseType_t xReturned = xTaskCreate(
        vTaskCode,       /* Function that implements the task. */
        "Blinky task",   /* Text name for the task. */
        512,             /* Stack size in words, not bytes. */
        ( void * ) 1,    /* Parameter passed into the task. */
        tskIDLE_PRIORITY,/* Priority at which the task is created. */
        &xHandle );   

    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Two tasks with same code but different priorities
void vTaskCode( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("Task %d task\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;
    /* Create 2 task */
    int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskCode, "talking task1", 512, ( void * ) &tid1, 1, &xHandle1 );  
    BaseType_t xReturned2 = xTaskCreate( vTaskCode, "talking task2", 512, ( void * ) &tid2, 1, &xHandle2 );
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Two tasks with different yielding
void vTaskYielding( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("doing something and sleeping ID %d\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Doing more and yielding ID %d\n", taskId);
        taskYIELD();
    }
}

void vTaskUnfriendly( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("doing something and busy waiting ID %d\n", taskId);
        uint64_t start_ticks = time_us_64();
        uint64_t end_ticks = start_ticks + 2000000;
        while (time_us_64() < end_ticks) {
            tight_loop_contents();
        }
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;
     /* Create 2 task */
    int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskYielding, "Yielding Task", 512, ( void * ) &tid1, 2, &xHandle1 );  
    BaseType_t xReturned2 = xTaskCreate( vTaskUnfriendly, "Unfriendly Task", 512, ( void * ) &tid2, 1, &xHandle2 );
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
SemaphoreHandle_t xSemaphore;

// Two tasks sharing resources
void vTaskSem( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("Running Task ID %d\n", taskId);
        if (xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(100) ) != pdTRUE) {
            printf("Task %d failed to capture semaphore\n", taskId);
        } else {
            printf("Task %d doing some job\n", taskId);
            vTaskDelay(pdMS_TO_TICKS(10));

            printf("Task %d returning semaphore\n", taskId);
            xSemaphoreGive(xSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;

    vSemaphoreCreateBinary( xSemaphore );
    printf("Semaphore creation was %s\n", xSemaphore == NULL ? "failed": "succesful");

     /* Create 2 task */
     int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskSem, "Yielding Task 1", 512, ( void * ) &tid1, 1, &xHandle1 );
    BaseType_t xReturned2 = xTaskCreate( vTaskSem, "Yielding Task 2", 512, ( void * ) &tid2, 1, &xHandle2 );  
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif