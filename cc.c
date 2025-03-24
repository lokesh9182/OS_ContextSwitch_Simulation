#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define structure for scheduling parameters
struct scheduling_parameters {
    int burst_time;
    int arrival_time;
    int execution_time;
    int waiting_time;
    int io_w_time;
};

// Define structure for process control block (PCB)
struct PCB {
    int pid;
    char state[20];
    int pc;
    int sp;
    struct scheduling_parameters sch_param;
    char iostate[20];
};

// Define structure for queue node
struct Node {
    struct PCB *process;
    struct Node *next;
};

// Define structure for queue
struct Queue {
    struct Node *front, *rear;
};

// Function to create a new queue node
struct Node* createNode(struct PCB *process) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->process = process;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a queue
struct Queue* createQueue() {
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// Function to enqueue a process into the queue
void enqueue(struct Queue* q, struct PCB *process) {
    struct Node* newNode = createNode(process);
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

// Function to dequeue a process from the queue
struct PCB* dequeue(struct Queue* q) {
    if (q->front == NULL) return NULL;
    struct Node* temp = q->front;
    struct PCB *process = temp->process;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return process;
}

// perform context switch between processes
void contextSwitch(struct Queue* readyQueue, struct Queue* waitingQueue) {
    struct PCB *current_process = dequeue(readyQueue);
    struct PCB *next_process = dequeue(waitingQueue);

    printf("Context Switch: Process %d -> Process %d\n", current_process->pid, next_process->pid);

    // Print details of each process
    printf("Process %d Details:\n", current_process->pid);
    printf("State: %s\n", current_process->state);
    printf("Burst Time: %d\n", current_process->sch_param.burst_time);
    printf("Arrival Time: %d\n", current_process->sch_param.arrival_time);
    printf("Execution Time: %d\n", current_process->sch_param.execution_time);
    printf("Waiting Time: %d\n\n", current_process->sch_param.waiting_time);

    // Enqueue current process to the end of the ready queue
    enqueue(readyQueue, current_process);

    // Update state of next process and enqueue it to ready queue
    strcpy(next_process->state, "RUNNING");
    enqueue(readyQueue, next_process);
}

// Function to simulate process execution
void executeProcess(struct PCB *process) {
    strcpy(process->state, "RUNNING");
    printf("Executing Process %d\n", process->pid);
    int x=10000000;while(x>0)x--; // delay of one second
    process->sch_param.execution_time++;
    return;
}

int main() {
    // Initialize processes
    struct PCB process1 = {100111, "READY", 0, 0, {10, 0, 0, 0,5}, "IO_BUSY"};
    struct PCB process2 = {100222, "READY", 0, 0, {18, 0, 0, 0,0}, "NONE"};
    struct PCB process3 = {100333, "READY", 0, 0, {4, 0, 0, 0,2}, "IO_BUSY"};
    struct PCB process4 = {100444, "READY", 0, 0, {24, 0, 0, 0,0}, "NONE"};
    struct PCB process5 = {100555, "READY", 0, 0, {6, 0, 0, 0,0}, "NONE"};

    // Initialize queues
    struct Queue* readyQueue = createQueue();
    struct Queue* waitingQueue = createQueue();

    // Enqueue processes into the ready queue
    enqueue(readyQueue, &process1);
    enqueue(readyQueue, &process2);
    enqueue(readyQueue, &process3);
    enqueue(readyQueue, &process4);
    enqueue(readyQueue, &process5);

    bool allProcessesFinished = false;
    
    // Round-robin scheduling loop
    int time_quantum = 2;
    int time=0;
    while (!allProcessesFinished) {
        struct PCB *current_process = dequeue(readyQueue);
        int tq = 2;
        while(tq>0){
            time++;
            printf("%d--",time);
            executeProcess(current_process);
            // Check if the process finished execution
            if (current_process->sch_param.execution_time >= current_process->sch_param.burst_time) {
                printf("Process %d Finished Execution\n\n", current_process->pid);
                strcpy(current_process->state, "TERMINATED");
                break;
            } 
            tq--;
        }
        if(strcmp(current_process->state,"TERMINATED") != 0)
            enqueue(readyQueue, current_process);//Enqueue process back to ready queue

        if (readyQueue->front == NULL) allProcessesFinished = true;

    
    }
    return 0;
}
