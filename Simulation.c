#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define structure for scheduling parameters
struct SchedulingParameters {
    int burstTime;
    int arrivalTime;
    int executionTime;
    int waitingTime;
    int ioWaitTime;
};

// Define structure for process control block (PCB)
struct ProcessControlBlock {
    int processId;
    char processState[20];
    int programCounter;
    int stackPointer;
    struct SchedulingParameters schedulingParams;
    char ioState[20];
};

// Define structure for queue node
struct QueueNode {
    struct ProcessControlBlock *process;
    struct QueueNode *next;
};

// Define structure for queue
struct ProcessQueue {
    struct QueueNode *front, *rear;
};

// Function to create a new queue node
struct QueueNode* createQueueNode(struct ProcessControlBlock *process) {
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a queue
struct ProcessQueue* createProcessQueue() {
    struct ProcessQueue* queue = (struct ProcessQueue*)malloc(sizeof(struct ProcessQueue));
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to enqueue a process into the queue
void enqueueProcess(struct ProcessQueue* queue, struct ProcessControlBlock *process) {
    struct QueueNode* newNode = createQueueNode(process);
    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }
    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to dequeue a process from the queue
struct ProcessControlBlock* dequeueProcess(struct ProcessQueue* queue) {
    if (queue->front == NULL) return NULL;
    struct QueueNode* tempNode = queue->front;
    struct ProcessControlBlock *process = tempNode->process;
    queue->front = queue->front->next;
    if (queue->front == NULL) queue->rear = NULL;
    free(tempNode);
    return process;
}

// Perform context switch between processes
void performContextSwitch(struct ProcessQueue* readyQueue, struct ProcessQueue* waitingQueue) {
    struct ProcessControlBlock *currentProcess = dequeueProcess(readyQueue);
    struct ProcessControlBlock *nextProcess = dequeueProcess(waitingQueue);

    printf("Context Switch: Process %d -> Process %d\n", currentProcess->processId, nextProcess->processId);

    // Print details of each process
    printf("Process %d Details:\n", currentProcess->processId);
    printf("State: %s\n", currentProcess->processState);
    printf("Burst Time: %d\n", currentProcess->schedulingParams.burstTime);
    printf("Arrival Time: %d\n", currentProcess->schedulingParams.arrivalTime);
    printf("Execution Time: %d\n", currentProcess->schedulingParams.executionTime);
    printf("Waiting Time: %d\n\n", currentProcess->schedulingParams.waitingTime);

    // Enqueue current process back to the ready queue
    enqueueProcess(readyQueue, currentProcess);

    // Update state of next process and enqueue it to ready queue
    strcpy(nextProcess->processState, "RUNNING");
    enqueueProcess(readyQueue, nextProcess);
}

// Function to simulate process execution
void executeProcess(struct ProcessControlBlock *process) {
    strcpy(process->processState, "RUNNING");
    printf("Executing Process %d\n", process->processId);
    int delay = 10000000; while(delay > 0) delay--; // Simulated delay
    process->schedulingParams.executionTime++;
}

int main() {
    // Initialize processes
    struct ProcessControlBlock process1 = {100111, "READY", 0, 0, {10, 0, 0, 0, 5}, "IO_BUSY"};
    struct ProcessControlBlock process2 = {100222, "READY", 0, 0, {18, 0, 0, 0, 0}, "NONE"};
    struct ProcessControlBlock process3 = {100333, "READY", 0, 0, {4, 0, 0, 0, 2}, "IO_BUSY"};
    struct ProcessControlBlock process4 = {100444, "READY", 0, 0, {24, 0, 0, 0, 0}, "NONE"};
    struct ProcessControlBlock process5 = {100555, "READY", 0, 0, {6, 0, 0, 0, 0}, "NONE"};

    // Initialize queues
    struct ProcessQueue* readyQueue = createProcessQueue();
    struct ProcessQueue* waitingQueue = createProcessQueue();

    // Enqueue processes into the ready queue
    enqueueProcess(readyQueue, &process1);
    enqueueProcess(readyQueue, &process2);
    enqueueProcess(readyQueue, &process3);
    enqueueProcess(readyQueue, &process4);
    enqueueProcess(readyQueue, &process5);

    bool allProcessesFinished = false;
    int timeQuantum = 2;
    int time = 0;

    // Round-robin scheduling loop
    while (!allProcessesFinished) {
        struct ProcessControlBlock *currentProcess = dequeueProcess(readyQueue);
        int quantum = timeQuantum;

        while (quantum > 0) {
            time++;
            printf("%d--", time);
            executeProcess(currentProcess);
            
            // Check if the process finished execution
            if (currentProcess->schedulingParams.executionTime >= currentProcess->schedulingParams.burstTime) {
                printf("Process %d Finished Execution\n\n", currentProcess->processId);
                strcpy(currentProcess->processState, "TERMINATED");
                break;
            }
            quantum--;
        }
        
        if (strcmp(currentProcess->processState, "TERMINATED") != 0)
            enqueueProcess(readyQueue, currentProcess); // Enqueue process back to ready queue

        if (readyQueue->front == NULL) allProcessesFinished = true;
    }
    return 0;
}
