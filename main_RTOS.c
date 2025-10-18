#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"

// Define the data structure for the queue [cite: 9, 10]
typedef struct {
    uint8_t dataID;
    int32_t DataValue;
} Data_t;

// --- Global Variables & Handles ---
// Task Handles as per the problem statement [cite: 5]
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;

// Queue Handle as per the problem statement [cite: 8]
QueueHandle_t Queue1;

// Global variables to be updated "elsewhere" 
// For simulation, we will update these within the sender task.
volatile uint8_t G_DataID = 1;
volatile int32_t G_DataValue = 0;

// Store the original priority of Task 2
UBaseType_t uxTask2OriginalPriority;

// --- Task Prototypes ---
void ExampleTask1(void *pV); // Sender Task
void ExampleTask2(void *pV); // Receiver Task

// --- Main Application ---
int main(void) {
    // Create the queue with a size of 5 and item size of Data_t [cite: 8, 13]
    Queue1 = xQueueCreate(5, sizeof(Data_t));

    if (Queue1 != NULL) {
        // Create ExampleTask1 with priority 1 
        xTaskCreate(ExampleTask1, "SenderTask", 1000, NULL, 1, &TaskHandle_1);
        
        // Create ExampleTask2 with priority 2 
        xTaskCreate(ExampleTask2, "ReceiverTask", 1000, NULL, 2, &TaskHandle_2);
        
        // Store the original priority of Task 2
        uxTask2OriginalPriority = uxTaskPriorityGet(TaskHandle_2);
        
        // Start the RTOS scheduler
        vTaskStartScheduler();
    }

    // The scheduler should run indefinitely, so this part should not be reached.
    for (;;);
    return 0;
}

/**
 * @brief ExampleTask1: Sends data to Queue1 every 500ms.
 * @param pV Unused parameter.
 */
void ExampleTask1(void *pV) {
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = pdMS_TO_TICKS(500); // 500ms interval [cite: 14]
    Data_t dataToSend;

    // Initialize xNextWakeTime for vTaskDelayUntil
    xNextWakeTime = xTaskGetTickCount();

    for (;;) {
        // Use vTaskDelayUntil for a precise, non-drifting delay 
        vTaskDelayUntil(&xNextWakeTime, xBlockTime);

        // Populate the structure from global variables [cite: 15, 16]
        dataToSend.dataID = G_DataID;
        dataToSend.DataValue = G_DataValue;

        printf("Task 1 (Sender): Sending dataID=%d, DataValue=%d\n", dataToSend.dataID, dataToSend.DataValue);

        // Send data to the queue. Wait for a maximum of 100ms if the queue is full.
        if (xQueueSend(Queue1, &dataToSend, pdMS_TO_TICKS(100)) != pdPASS) {
            printf("Task 1 (Sender): Failed to send to queue.\n");
        }
        
        // --- SIMULATION LOGIC for global variables ---
        // This logic simulates the "updated elsewhere" part to test all conditions.
        if (G_DataValue < 2) {
            G_DataValue++;
        } else {
            G_DataValue = 0;
        }
        
        // After a few cycles, send a dataID = 0 to test task deletion
        if (xTaskGetTickCount() > pdMS_TO_TICKS(5000)) {
            G_DataID = 0; 
        }
        // --- END SIMULATION LOGIC ---
    }
}

/**
 * @brief ExampleTask2: Receives data from the queue and processes it.
 * @param pV Unused parameter.
 */
void ExampleTask2(void *pV) {
    Data_t receivedData;
    UBaseType_t currentPriority;

    for (;;) {
        // Wait indefinitely for data to be available in the queue 
        // Note: The prompt mentions Queue2, which seems to be a typo. I am using Queue1 as intended.
        if (xQueueReceive(Queue1, &receivedData, portMAX_DELAY) == pdPASS) {
            
            // Print the received data at every evaluation [cite: 19]
            printf("Task 2 (Receiver): Received dataID=%d, DataValue=%d\n", receivedData.dataID, receivedData.DataValue);
            
            // --- Logic Implementation as per the problem statement table  ---
            if (receivedData.dataID == 0) {
                printf("Task 2 (Receiver): dataID is 0. Deleting self.\n");
                vTaskDelete(NULL); // Delete this task (ExampleTask2)
            
            } else if (receivedData.dataID == 1) {
                // Allow processing of DataValue member
                if (receivedData.DataValue == 0) {
                    currentPriority = uxTaskPriorityGet(NULL);
                    printf("Task 2 (Receiver): DataValue is 0. Increasing priority.\n");
                    vTaskPrioritySet(NULL, currentPriority + 2); // Increase priority by 2
                
                } else if (receivedData.DataValue == 1) {
                    currentPriority = uxTaskPriorityGet(NULL);
                    // Only decrease priority if it was previously increased
                    if (currentPriority > uxTask2OriginalPriority) {
                        printf("Task 2 (Receiver): DataValue is 1. Decreasing priority.\n");
                        vTaskPrioritySet(NULL, uxTask2OriginalPriority); // Decrease priority to original value
                    } else {
                         printf("Task 2 (Receiver): DataValue is 1. Priority not increased, no action.\n");
                    }
                
                } else if (receivedData.DataValue == 2) {
                    printf("Task 2 (Receiver): DataValue is 2. Deleting self.\n");
                    vTaskDelete(NULL); // Delete this task (ExampleTask2)
                }
            }
        }
    }
}