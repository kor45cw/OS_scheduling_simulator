//
//  main.c
//  os
//
//  Created by SonChang Woo on 2016. 5. 23..
//  Copyright © 2016년 SonChangWoo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Process 정보
typedef struct process {
    int processId;
    int cpuBurstTime;
    int ioBurstTime;
    int arrivalTime;
    int priority;
    
    int finish;
    
    double waitingTime;
    double executionCompleteTime;
} Process;


typedef struct queue {
    Process * process;
    int numberOfProcess;
} Queue;

char * scheduleName[10] = {"FCFS", "SJF_Preemptive", "SJF_NonPreemptive", "Priority_Preemptive", "Priority_NonPreemptive", "RoundRobin", "HRRN", "Multilevel Queue Scheduling", "Multilevel Feedback-Queue Scheduling", "DualCore FCFS"};

Process * CreateProcess(int num);
void PrintProcess(Process * process, int num);
Queue * Config(Process * process, int num);
double * Schedule(Process * process, int schduling ,int num);
void Evaluation(Process * process, int num);
double * FCFS(Queue * queue, int num);
double * HRRN(Queue * queue, int num);
double * SJF_Preemptive(Queue * queue, int num);
double * SJF_NonPreemptive(Queue * queue, int num);
double * Priority_Preemptive(Queue * queue, int num);
double * Priority_NonPreemptive(Queue * queue, int num);
double * RoundRobin(Queue * queue, int num);
double * MQS(Queue * queue, int num);
double * MFQS(Queue * queue, int num);
double * DualCoreFCFS(Queue * queue, int num);
int IoOcuur();
void SortByCpuBurst(Queue * queue);
void SortByHRRN(Queue * queue, int time);
void SortByFinish(Queue * queue);
void SortByPriority(Queue * queue);

int main() {
    int n;
    
    printf("Enter number of process : ");
    scanf("%d", &n);
    
    Process * process;
    process = CreateProcess(n);
    
    PrintProcess(process, n);
    
    Evaluation(process, n);
    
    free(process);
    return 0;
}

Process * CreateProcess(int num) {
    int i=0;
    Process * process = (Process *) malloc(sizeof(Process)*num+1);
    
    srand((unsigned int)time(NULL));
    
    for (i=0;i<num;i++) {
        process[i].processId = i+1;
        //cpu_burst : 2~9
        process[i].cpuBurstTime = rand()%(10)+1;
        
        //arrival Time
        process[i].arrivalTime = rand()%(10)+1;
        
        //ioBurst
        process[i].ioBurstTime = rand()%(10)+1;
        
        //priority
        process[i].priority = rand()%(num*20)+1;
        
        process[i].waitingTime = 0;
        process[i].executionCompleteTime = 0;
        process[i].finish = 0;
    }
    return process;
}

void PrintProcess(Process * process, int num) {
    int i=0;
    printf("PID, ArrivalTime, CpuBurstTime, IOBurstTime, Priority\n");
    
    for (i=0;i<num;i++) {
        printf("%d %6d %10d %10d %10d\n", process[i].processId, process[i].arrivalTime, process[i].cpuBurstTime, process[i].ioBurstTime, process[i].priority);
    }
    
}

Queue * Config(Process * process, int num) {
    Queue * queue = (Queue *) malloc(sizeof(Queue)*5);
    
    // 0 : unstarted, 1 : ready, 2 : waiting, 3 : temp, 4 : temp
    queue[0].process = (Process *) malloc(sizeof(Process)*num+1);
    queue[0].process = process;
    queue[1].process = (Process *) malloc(sizeof(Process)*num*10+1);
    queue[2].process = (Process *) malloc(sizeof(Process)*num*10+1);
    queue[3].process = (Process *) malloc(sizeof(Process)*num*10+1);
    queue[4].process = (Process *) malloc(sizeof(Process)*num*10+1);
    queue[0].numberOfProcess = num;
    queue[1].numberOfProcess = -1;
    queue[2].numberOfProcess = -1;
    queue[3].numberOfProcess = -1;
    queue[4].numberOfProcess = -1;
    
    return queue;
}


double * Schedule(Process * process, int schduling, int num) {
    double * resultTime;
    // queue[0] : unstarted, queue[1] : ready, queue[2] : waiting
    Queue * queue;
    queue = Config(process, num);
    
    switch (schduling) {
        case 0:
            resultTime=FCFS(queue, num);
            break;
        case 1:
            resultTime=SJF_Preemptive(queue, num);
            break;
        case 2:
            resultTime=SJF_NonPreemptive(queue, num);
            break;
        case 3:
            resultTime=Priority_Preemptive(queue, num);
            break;
        case 4:
            resultTime=Priority_NonPreemptive(queue, num);
            break;
        case 5:
            resultTime=RoundRobin(queue, num);
            break;
        case 6:
            resultTime=HRRN(queue, num);
            break;
        case 7:
            resultTime=MQS(queue, num);
            break;
        case 8:
            resultTime=MFQS(queue, num);
            break;
        case 9:
            resultTime=DualCoreFCFS(queue, num);
        default:
            break;
    }
    free(queue);
    
    return resultTime;
}

void Evaluation(Process * process, int num) {
    int i=0;
    double * temp = (double *) malloc(sizeof(double)*2+1);
    double avgWaitingTime[10];
    double avgTurnaroundTime[10];
    double resultWaiting, resultTurnaround;
    
    for (i=0;i<10;i++) {
        temp = Schedule(process, i, num);
        avgWaitingTime[i] = temp[1];
        avgTurnaroundTime[i] = temp[0];
    }
    printf("\n");
    
    for (i=0;i<10;i++) {
        resultWaiting = avgWaitingTime[i] / num;
        resultTurnaround = avgTurnaroundTime[i] / num;
        
        printf("%s : avgWaitingTime %lf, avgTurnaroundTime %lf \n", scheduleName[i], resultWaiting, resultTurnaround);
    }
    
    free(temp);
    
    return;
}


double * FCFS(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag = 1; // 0 : cpu running, 1 : cpu is empty, 2 : io time
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("FCFS (First Come First Served) : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        // current에 아무것도 없으면
        if (flag == 1) {
            // ready queue에서 하나 가져옴
            if (countOfReady>0) {
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        // ready queue에 프로세스 있으면 waiting time 증가
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish = 0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * HRRN(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("HRRN(Highest Response Ratio Next) : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        if (flag == 1) {
            if (countOfReady>0) {
                SortByHRRN(&queue[1], currentTime);
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * SJF_Preemptive(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag = 1; // 1 : cpu is empty
    int newProcessCheck = 0;
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("SJF_P (Shortest Job First P) : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
                newProcessCheck = 1;
            }
        }
        // 새 인풋 들어왔을 때 확인
        if (flag == 0 && newProcessCheck == 1) {
            usingProcess.finish = 0;
            queue[1].process[++queue[1].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfReady++;
        }
        
        if (flag == 1) {
            // ready queue에서 하나 가져옴
            if (countOfReady>0) {
                SortByCpuBurst(&queue[1]);
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
    
}

double * SJF_NonPreemptive(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("SJF_NP (Shortest Job Fisrt NP) : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        if (flag == 1) {
            // ready queue에서 하나 가져옴
            if (countOfReady>0) {
                SortByCpuBurst(&queue[1]);
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * Priority_Preemptive(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int newProcessCheck = 0;
    int i;
    int flag = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("Priority P : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
                newProcessCheck = 1;
            }
        }
        
        if (flag == 0 && newProcessCheck==1) {
            usingProcess.finish = 0;
            queue[1].process[++queue[1].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfReady++;
        }
        
        if (flag == 1) {
            if (countOfReady>0) {
                SortByPriority(&queue[1]);
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * Priority_NonPreemptive(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("Priority NP : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        if (flag == 1) {
            if (countOfReady>0) {
                SortByPriority(&queue[1]);
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                countOfReady--;
            }
        }
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * RoundRobin(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int quantum = 2;
    int i;
    int flag = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("RoundRobin (quantum 2) : ");
    
    while (num != endCount) {
        // 현재 time에 도착한 process를 ready queue로 이동
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        if (flag == 1) {
            if (countOfReady>0) {
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                quantum = 2;
                countOfReady--;
            }
        }
        
        quantum = quantum-1;
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");

            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag = 1;
                quantum = 2;
                countOfReady++;
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
                quantum =2;
            }
        }
        
        if (quantum<=0 && flag == 0) {
            usingProcess.finish = 0;
            queue[1].process[++queue[1].numberOfProcess] = usingProcess;
            flag = 1;
            quantum = 2;
            countOfReady++;
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            //            printf("IO \n");
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * MQS(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfRR = 0, countOfFCFS = 0;
    int quantum = 2;
    int i, newProcessCheck = 0;
    int flag = 1, groundFlag = 0; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("Multilevel Queue Scheduling : ");
    
    // foreground = RR (1~3) queue[1]
    // background = FCFS (4~10) queue[3]
    
    while(num != endCount) {
        
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                if (queue[0].process[i].cpuBurstTime <= 3) {
                    queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                    countOfRR++;
                    newProcessCheck = 1;
                } else {
                    queue[3].process[++queue[3].numberOfProcess] = queue[0].process[i];
                    countOfFCFS++;
                }
            }
        }
        
        
        if (flag == 0 && newProcessCheck == 1 && groundFlag ==0) {
            usingProcess.finish = 0;
            queue[3].process[++queue[3].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfFCFS++;
        }
        
        if (flag ==1) {
            if (countOfRR>0) {
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                quantum = 2;
                groundFlag = 1;
                countOfRR--;
            } else if (countOfFCFS>0) {
                SortByFinish(&queue[3]);
                usingProcess = queue[3].process[0];
                queue[3].process[0].finish = 1;
                flag = 0;
                groundFlag = 0;
                countOfFCFS--;
            }
        }
        
        if (groundFlag == 1) {
            quantum = quantum-1;
        }
        
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        if (queue[3].numberOfProcess != -1) {
            for (i=0; i<=queue[3].numberOfProcess;i++) {
                queue[3].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            printf("z");
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                if (queue[2].process[queue[2].numberOfProcess].cpuBurstTime <= 3) {
                    queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                    flag = 1;
                    groundFlag = 1;
                    quantum = 2;
                    newProcessCheck = 1;
                    countOfRR++;
                } else {
                    queue[3].process[++queue[3].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                    flag = 1;
                    quantum = 2;
                    groundFlag = 0;
                    newProcessCheck = 0;
                    countOfFCFS++;
                }
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
                quantum =2;
            }
        }
        
        if (quantum<=0 && flag == 0) {
            usingProcess.finish = 0;
            if (usingProcess.cpuBurstTime <= 3) {
                queue[1].process[++queue[1].numberOfProcess] = usingProcess;
                flag = 1;
                countOfRR++;
            } else {
                queue[3].process[++queue[3].numberOfProcess] = usingProcess;
                flag = 1;
                countOfFCFS++;
            }
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            //            printf("IO \n");
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * MFQS(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0, countOfQueueNum3 = 0, countOfFCFS = 0;
    int quantum1 = 2;
    int quantum2 = 4;
    int i, newProcessCheck = 0;
    int flag = 1; // 1 : cpu is empty
    int groundFlag = 0; // 0 : FCFS, 1: quantum1, 2: quantum2
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess;
    
    
    printf("Multilevel Feedback-Queue Scheduling : ");
    
    // queue num 0 : quantum 2 -> queue[1]
    // queue num 1 : quantum 4 -> queue[3]
    // queue num 2 : FCFS -> queue[4]
    // queue 0 가 비었으면 queue 1, queue0과 1모두가 비었으면 2를 실행
    // quantum 2동안 끝내지 못하면, queue 1로 이동, 1에서도 4동안 끝내지 못하면 2로 이동
    
    while(num != endCount) {
        
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                if (queue[0].process[i].cpuBurstTime <= 2) {
                    queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                    countOfReady++;
                    newProcessCheck = 1;
                } else if (queue[0].process[i].cpuBurstTime <= 6) {
                    queue[3].process[++queue[3].numberOfProcess] = queue[0].process[i];
                    countOfQueueNum3++;
                    if (newProcessCheck == 0) {
                        newProcessCheck = 2;
                    }
                } else {
                    queue[4].process[++queue[4].numberOfProcess] = queue[0].process[i];
                    countOfFCFS++;
                }
            }
        }
        
        if (flag == 0 && newProcessCheck == 1 && groundFlag ==0) {
            usingProcess.finish = 0;
            queue[4].process[++queue[4].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfFCFS++;
        } else if (flag == 0 && newProcessCheck == 1 && groundFlag == 2) {
            usingProcess.finish = 0;
            queue[3].process[++queue[3].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfQueueNum3++;
        }
        
        if (flag == 0 && newProcessCheck == 2 && groundFlag ==0) {
            usingProcess.finish = 0;
            queue[4].process[++queue[4].numberOfProcess] = usingProcess;
            flag = 1;
            newProcessCheck = 0;
            countOfFCFS++;
        }
        
        
        if (flag ==1) {
            if (countOfReady>0) {
                SortByFinish(&queue[1]);
                usingProcess = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag = 0;
                groundFlag = 1;
                quantum1 = 2;
                countOfReady--;
            } else if (countOfQueueNum3>0) {
                SortByFinish(&queue[3]);
                usingProcess = queue[3].process[0];
                queue[3].process[0].finish = 1;
                flag = 0;
                groundFlag = 2;
                quantum2 = 4;
                countOfQueueNum3--;
            } else if (countOfFCFS>0) {
                SortByFinish(&queue[4]);
                usingProcess = queue[4].process[0];
                queue[4].process[0].finish = 1;
                flag = 0;
                groundFlag = 0;
                countOfFCFS--;
            }
        }
        
        if (groundFlag == 1) {
            quantum1 = quantum1 -1;
        } else if (groundFlag == 2) {
            quantum2 = quantum2 -1;
        }
        
        
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        if (queue[3].numberOfProcess != -1) {
            for (i=0; i<=queue[3].numberOfProcess;i++) {
                queue[3].process[i].waitingTime++;
            }
        }
        
        if (queue[4].numberOfProcess != -1) {
            for (i=0; i<=queue[4].numberOfProcess;i++) {
                queue[4].process[i].waitingTime++;
            }
        }
        
        // waiting queue에 프로세스가 있으면
        if (flag == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag = 2;
            
            printf("z");
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                if (queue[2].process[queue[2].numberOfProcess].cpuBurstTime > 6) {
                    queue[4].process[++queue[4].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                    flag = 1;
                    countOfFCFS++;
                } else if (queue[2].process[queue[2].numberOfProcess].cpuBurstTime <= 2) {
                    queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                    flag = 1;
                    countOfReady++;
                } else {
                    queue[3].process[++queue[3].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                    flag = 1;
                    countOfQueueNum3++;
                }
            }
        }
        
        if (flag == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess.cpuBurstTime--;
            flag = 0;
            printf("%d", usingProcess.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess.cpuBurstTime == 0) {
                endCount++;
                flag = 1;
                usingProcess.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess.executionCompleteTime - usingProcess.arrivalTime;
                waitingTime += usingProcess.waitingTime;
            }
        }
        
        
        // quantum1이 다 줄었는데도 끝나지 않았으니
        if (quantum1<=0 && flag == 0 && groundFlag == 1) {
            usingProcess.finish = 0;
            queue[3].process[++queue[3].numberOfProcess] = usingProcess;
            flag = 1;
            quantum1 = 2;
            countOfQueueNum3++;
        }
        
        // quantum2가 다 줄었는데도 끝나지 않았으니
        if (quantum2<=0 && flag == 0 && groundFlag == 2) {
            usingProcess.finish = 0;
            queue[4].process[++queue[4].numberOfProcess] = usingProcess;
            flag = 1;
            quantum2 = 4;
            countOfFCFS++;
        }
        
        if (IoOcuur() == 1 && flag == 0) {
            //            printf("IO \n");
            usingProcess.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess;
            flag=2;
        }
        
        currentTime = currentTime+1;
        
    }
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

double * DualCoreFCFS(Queue * queue, int num) {
    double * calTime = (double *) malloc(sizeof(double)*3);
    int * printCore1 = (int *) malloc(sizeof(int)*num);
    int * printCore2 = (int *) malloc(sizeof(int)*num);
    int countCore1 = 0, countCore2= 0;
    int currentTime = 0;
    int endCount = 0;
    int countOfReady = 0;
    int i;
    int flag1 = 1, flag2 = 1; // 1 : cpu is empty
    double turnaroundTime = 0, waitingTime = 0;
    Process usingProcess1, usingProcess2;
    
    printf("DualCore FCFS : ");
    
    while(num != endCount) {
        
        for (i=0;i<num;i++) {
            if (currentTime == queue[0].process[i].arrivalTime) {
                queue[1].process[++queue[1].numberOfProcess] = queue[0].process[i];
                countOfReady++;
            }
        }
        
        if (flag1 == 1) {
            if (countOfReady>0) {
                SortByFinish(&queue[1]);
                usingProcess1 = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag1 = 0;
                countOfReady--;
            }
        }
        
        if (flag2 == 1) {
            if (countOfReady>0) {
                SortByFinish(&queue[1]);
                usingProcess2 = queue[1].process[0];
                queue[1].process[0].finish = 1;
                flag2 = 0;
                countOfReady--;
            }
        }
        
        
        if (queue[1].numberOfProcess != -1) {
            for (i=0; i<=queue[1].numberOfProcess;i++) {
                queue[1].process[i].waitingTime++;
            }
        }
        
        if (flag1 == 2) {
            queue[2].process[queue[2].numberOfProcess].ioBurstTime--;
            flag1 = 2;
            printCore1[countCore1] = "z";
            countCore1++;
            
            // io Burst Finish
            if (queue[2].process[queue[2].numberOfProcess].ioBurstTime == 0) {
                queue[2].process[queue[2].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[2].process[queue[2].numberOfProcess];
                flag1 = 1;
                countOfReady++;
            }
        }
        
        if (flag2 == 2) {
            queue[3].process[queue[3].numberOfProcess].ioBurstTime--;
            flag2 = 2;
            printCore2[countCore2] = "z";
            countCore2++;
            
            // io Burst Finish
            if (queue[3].process[queue[3].numberOfProcess].ioBurstTime == 0) {
                queue[3].process[queue[3].numberOfProcess].ioBurstTime = rand()%10+1;
                queue[1].process[++queue[1].numberOfProcess] = queue[3].process[queue[3].numberOfProcess];
                flag2 = 1;
                countOfReady++;
            }
        }
        
        if (flag1 == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess1.cpuBurstTime--;
            flag1 = 0;
            printCore1[countCore1] = usingProcess1.processId;
            countCore1++;
            //            printf("%d", usingProcess1.processId);
            
            // 프로세스가 끝났을 때
            if (usingProcess1.cpuBurstTime == 0) {
                endCount++;
                flag1 = 1;
                usingProcess1.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess1.executionCompleteTime - usingProcess1.arrivalTime;
                waitingTime += usingProcess1.waitingTime;
            }
        }
        
        if (flag2 == 0) {
            // cpu에 프로세스가 있으니 그것을 계속 실행
            usingProcess2.cpuBurstTime--;
            flag2 = 0;
            printCore2[countCore2] = usingProcess2.processId;
            countCore2++;
            //            printf("%d", usingProcess2.processId);
            
            
            // 프로세스가 끝났을 때
            if (usingProcess2.cpuBurstTime == 0) {
                endCount++;
                flag2 = 1;
                usingProcess2.executionCompleteTime = currentTime;
                
                turnaroundTime += usingProcess2.executionCompleteTime - usingProcess2.arrivalTime;
                waitingTime += usingProcess2.waitingTime;
            }
        }
        
        if (IoOcuur() == 1 && flag1 == 0) {
            //            printf("IO \n");
            usingProcess1.finish=0;
            queue[2].process[++queue[2].numberOfProcess] = usingProcess1;
            flag1 = 2;
        }
        
        if (IoOcuur() == 1 && flag2 == 0) {
            //            printf("IO \n");
            usingProcess2.finish=0;
            queue[3].process[++queue[3].numberOfProcess] = usingProcess2;
            flag2 = 2;
        }
        
        currentTime = currentTime +1;
    }
    
    printf("\ncore 1 : ");
    for (i=0;i<countCore1;i++) {
        printf("%d", printCore1[i]);
    }
    
    printf("\ncore 2 : ");
    for (i=0;i<countCore2;i++) {
        printf("%d", printCore2[i]);
    }
    
    
    printf("\n");
    calTime[0] = turnaroundTime;
    calTime[1] = waitingTime;
    return calTime;
}

int IoOcuur() {
    int io = rand()%100+1;
    
    if (io < 10) {
        return 1;
    } else {
        return 0;
    }
}

void SortByCpuBurst(Queue * queue) {
    int i,j;
    Process temp;
    //Ready Queue에 Process를 CpuBurst Time 순서로 정렬
    for(i=0; i <= queue->numberOfProcess; i++){
        for(j=i+1; j <= queue->numberOfProcess; j++) {
            // 앞에꺼 보다 뒤에꺼가 cpuBurstTime이 작으면 순서바꿈 -> 작은 순서로 정렬
            if(queue->process[j].cpuBurstTime < queue->process[i].cpuBurstTime) {
                temp = queue->process[j];
                queue->process[j] = queue->process[i];
                queue->process[i] = temp;
            }
        }
    }
}

void SortByHRRN(Queue * queue, int time) {
    int i, j;
    int waitI, waitJ;
    int cpuI, cpuJ;
    double resultI, resultJ;
    Process temp;
    
    for (i=0; i<=queue->numberOfProcess;i++) {
        for (j=i+1; j<= queue->numberOfProcess; j++) {
            waitI = time - queue->process[i].arrivalTime;
            cpuI = queue->process[i].cpuBurstTime;
            waitJ = time - queue->process[j].arrivalTime;
            cpuJ = queue->process[j].cpuBurstTime;
            resultI = (double) (waitI+cpuI) / cpuI;
            resultJ = (double) (waitJ+cpuJ) / cpuJ;
            if (resultJ > resultI) {
                //                printf("result I, J : %lf %lf \n", resultI, resultJ);
                temp = queue->process[j];
                queue->process[j] = queue->process[i];
                queue->process[i] = temp;
            }
        }
    }
}

void SortByFinish(Queue * queue) {
    int i, j;
    Process temp;
    //Ready Queue에 initial Process를 Not Finish 순서로 정렬
    for(i=0; i <= queue->numberOfProcess; i++){
        for(j=i+1; j <= queue->numberOfProcess; j++) {
            if(queue->process[j].finish < queue->process[i].finish) {
                temp = queue->process[j];
                queue->process[j] = queue->process[i];
                queue->process[i] = temp;
            }
        }
    }
}

void SortByPriority(Queue * queue) {
    int i, j;
    Process temp;
    //Ready Queue에 initial Process를 Priority 순서로 정렬
    for(i=0; i <= queue->numberOfProcess; i++){
        for(j=i+1; j <= queue->numberOfProcess; j++) {
            if(queue->process[j].priority < queue->process[i].priority) {
                temp = queue->process[j];
                queue->process[j] = queue->process[i];
                queue->process[i] = temp;
            }
        }
    }
}