#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Global variables (assumed to be updated elsewhere)
uint8_t G_DataID = 0;
int32_t G_DataValue = 0;

// Data structure definition
typedef struct {
    uint8_t dataID;
    int32_t DataValue;
} Data_1;

// Task handles and queue handle
TaskHandle_t TaskHandle_1 = NULL;
TaskHandle_t TaskHandle_2 = NULL;
QueueHandle_t Queue1 = NULL;

// Task prototypes
void ExampleTask1(void *pV);
void ExampleTask2(void *pV);

void app_main(void) {
    // Create queue with size 5 for Data_1 structure
    Queue1 = xQueueCreate(5, sizeof(Data_1));
    
    if (Queue1 == NULL) {
        printf("Failed to create queue\n");
        return;
    }
    
    // Create tasks
    xTaskCreate(ExampleTask1, "ExampleTask1", 4096, NULL, 2, &TaskHandle_1);
    xTaskCreate(ExampleTask2, "ExampleTask2", 4096, NULL, 1, &TaskHandle_2);
    
    printf("Tasks and Queue created successfully\n");
}

void ExampleTask1(void *pV) {
    Data_1 sendData;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500);
    
    while (1) {
        // Populate data structure with global variables
        sendData.dataID = G_DataID;
        sendData.DataValue = G_DataValue;
        
        // Send data to queue
        if (xQueueSend(Queue1, &sendData, portMAX_DELAY) == pdPASS) {
            printf("Task1: Sent data - ID: %d, Value: %ld\n", sendData.dataID, sendData.DataValue);
        }
        
        // Exact 500ms delay using vTaskDelayUntil
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void ExampleTask2(void *pV) {
    Data_1 receivedData;
    UBaseType_t originalPriority = uxTaskPriorityGet(NULL);
    UBaseType_t currentPriority = originalPriority;
    bool priorityIncreased = false;
    
    while (1) {
        // Receive data from queue
        if (xQueueReceive(Queue1, &receivedData, portMAX_DELAY) == pdPASS) {
            // Print received data
            printf("Task2: Received data - ID: %d, Value: %ld\n", receivedData.dataID, receivedData.DataValue);
            
            // Apply logic based on data
            if (receivedData.dataID == 0) {
                printf("Task2: dataID==0 - Deleting Task2\n");
                vTaskDelete(NULL);
            }
            
            if (receivedData.dataID == 1) {
                printf("Task2: dataID==1 - Processing DataValue\n");
                // Process DataValue member here
            }
            
            if (receivedData.DataValue == 0) {
                if (!priorityIncreased) {
                    currentPriority += 2;
                    vTaskPrioritySet(NULL, currentPriority);
                    priorityIncreased = true;
                    printf("Task2: DataValue==0 - Priority increased to %ld\n", currentPriority);
                }
            }
            
            if (receivedData.DataValue == 1) {
                if (priorityIncreased) {
                    currentPriority = originalPriority;
                    vTaskPrioritySet(NULL, currentPriority);
                    priorityIncreased = false;
                    printf("Task2: DataValue==1 - Priority reset to %ld\n", currentPriority);
                }
            }
            
            if (receivedData.DataValue == 2) {
                printf("Task2: DataValue==2 - Deleting Task2\n");
                vTaskDelete(NULL);
            }
        }
    }
}
