#include "mydatastruct.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
    int totalEventNum, timeQuantum, vmemSize, pmemSize, pageSize, feedFreq,
	feedSize;
    int eventCnt = 0;
    int cpuCycle = -1;
    int globalPid = 0;
    int CPUOccupied = 0;
    int inst, para;
    int cycleFlag = 1;
    FILE* input;
    FILE* system;
    FILE* memory;
    FILE* scheduler;
    char* inputName = argv[1];
    char inputNextInst[100];
    int feedCnt = 0;
    int pBust = 0;
    int vmempg, pmempg;
    MTREE_PTR root;
    PQ* tmpq = NULL;
    PQ* rq = NULL;
    PQ* sq = NULL;
    PQ* ioq = NULL;
    PQ* aq = NULL;
    LRUQ* lq = NULL;
    PROCESS_PTR fetchedProcess = NULL;
    tmpq = initQueue(tmpq);
    rq = initQueue(rq);
    sq = initQueue(sq);
    ioq = initQueue(ioq);
    aq = initQueue(aq);
    lq = initLRUQ(lq);
    input = fopen(inputName, "r");
    system = fopen("system.txt", "w");
    memory = fopen("memory.txt", "w");
    scheduler = fopen("scheduler.txt", "w");
    fscanf(input, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &totalEventNum, &timeQuantum, &vmemSize, &pmemSize, &pageSize, &feedFreq, &feedSize);
    vmempg = vmemSize/pageSize;
    pmempg = pmemSize/pageSize;
    feedCnt = feedFreq-1;
    root = createMTREE(NULL, 0, 2*pmempg-1, 1);
    fgets(inputNextInst, sizeof(inputNextInst), input);
    while(1){
	cpuCycle++;
	feedCnt++;
	if (cpuCycle > 100) break;
	if(feedCnt == feedFreq){
	    distributeCPUTime(aq, feedSize);
	    feedCnt = 0;
	}
	sqCtrl(sq, rq);
	while(cycleFlag){
	    char* fetchedEvent = NULL;
	    char tmpstr[100];
	    tmpstr[0] = 0;
	    if (eventCnt < totalEventNum){
		fetchedEvent = eventFetch(inputNextInst, cpuCycle, &eventCnt, input);
		if (fetchedEvent != NULL){
		    strcpy(tmpstr, fetchedEvent);
		    cycleFlag = testCycleFlag(inputNextInst, cpuCycle, eventCnt, totalEventNum);
		} else {
		    cycleFlag = 0;
		}
	    }
	    if (eventCnt == totalEventNum){
		cycleFlag = 0;
	    }
	    int eventType = detEventType(fetchedEvent); /*0 means no event occured,
						      1 means process, and 2 means
						      INPUT*/
	    if (fetchedEvent != NULL) strcpy(fetchedEvent, tmpstr);
	    if(!fetchedEvent) free(fetchedEvent);
	    if(eventType == 1){
		char* pos;
		char* tmp = strtok(fetchedEvent, " ");
		tmp = strtok(NULL, " ");
		if ((pos = strchr(tmp, '\n')) != NULL){
		    *pos ='\0';
		}
		makeProcess(globalPid, feedSize, tmp, tmpq, aq, vmempg, root);
		globalPid++;

	    }else if (eventType == 2){
		char* pos;
		char* tmp = strtok(fetchedEvent, " ");
		int pid;
		strtok(NULL, " ");
		tmp = strtok(NULL, " ");
		if ((pos = strchr(tmp, '\n')) != NULL){
		    *pos = '\0';
		}
		pid = atoi(tmp);
		IOCtrl(ioq, rq, pid);
	    }
	}
	cycleFlag = 1;
	qtoq(rq, tmpq);
	if(!CPUOccupied&&(rq->num)>0){
	    fetchedProcess = fetchProcess(rq, timeQuantum);
	    if (!fetchedProcess){
		distributeCPUTime(aq, feedSize);
		feedCnt = 0;
		fetchedProcess = fetchProcess(rq, timeQuantum);
	    }
	    CPUOccupied = 1;
	    fprintf(scheduler, "%d\t%d\t%s\n", cpuCycle, fetchedProcess->pid, fetchedProcess->name);
	}
/*	printf("ioq: ");
	printPQ(ioq);
	printf("rq: ");
	printPQ(rq);
	printf("sq: ");
	printPQ(sq);
	printf("print LRUQ: ");
	printLRUQ(lq);
	printf("physical memory: ");
	printMTREE(root);*/
	fprintf(system, "%d Cycle: ", cpuCycle);
	if (CPUOccupied == 1){
	    fetchInst(fetchedProcess, &inst, &para);
	    fprintf(system, "Process#%d running code %s line %d(op %d, arg %d)\n", fetchedProcess->pid, fetchedProcess->name, fetchedProcess->pc, inst, para);
	} else {
	    fprintf(system, "\n");
	}
	fprintf(system, "RunQueue: ");
	fprintPQ(system, rq);
	fprintf(system, "SleepList: ");
	fprintPQ(system, sq);
	fprintf(system, "IOWait List: ");
	fprintPQ(system, ioq);
	fprintMTREE(system, root);
	fprintLRUQ(system, lq);
	fprintf(system, "\n");

	if (CPUOccupied == 1){
	    pExec(fetchedProcess, inst, para, cpuCycle, sq, ioq, aq, lq, memory);
	}
	pBust = isBusted(fetchedProcess);
	if(pBust){
/*	    printf("pid: %d busted!, remaining CPUCycle: %d timeQ: %d\n",
		    fetchedProcess->pid, fetchedProcess->remainingCPUCycle,
		    fetchedProcess->remainingQuantum);*/
	    CPUOccupied = 0;
	    if(!fetchedProcess->burst){
		enqueue(rq, fetchedProcess);
	    }
	    fetchedProcess = NULL;
	}
	if (fetchedProcess){
	    if(fetchedProcess->isIOWaiting||fetchedProcess->isSleeping){
//		printf("pid: %d busted!\n", fetchedProcess->pid);
		CPUOccupied = 0;
		fetchedProcess = NULL;
	    }
	}
	pBust = 0;


	if (eventCnt ==	totalEventNum&&!rq->num&&!ioq->num&&!sq->num&&!fetchedProcess){
//	    printf("end!\n");
	    break;
	}
    }
    fclose(input);
    fclose(system);
    fclose(memory);
    fclose(scheduler);
}
