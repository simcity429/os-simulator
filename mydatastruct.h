#pragma once
#include <stdio.h>

typedef struct VM* VM_PTR;

typedef struct VM{
    int allocID;
    int valid;
} VM;

typedef struct MTREE* MTREE_PTR;

typedef struct MTREE{
    int leaf;
    int isLeft;
    int isAlloc;
    int startIndex;
    int endIndex;
    int pid;
    int allocID;
    MTREE_PTR parent;
    MTREE_PTR lc;
    MTREE_PTR rc;
} MTREE;

typedef struct PROCESS* PROCESS_PTR;

typedef struct PROCESS{
    FILE* pstream;
    FILE* outstream;
    char* name;
    MTREE_PTR root;
    int pid;
    int vmSize;
    int sleep;
    int remainingQuantum;
    int remainingCPUCycle;
    int isIOWaiting;
    int isSleeping;
    int instNum;
    int allocID;
    int pc;
    int burst;
    char* pageTable;
    VM_PTR vm;
} PROCESS;

typedef struct PROCESS_WRAPPER* PROCESS_WRAPPER_PTR;

typedef struct PROCESS_WRAPPER{
    PROCESS_PTR current;
    PROCESS_WRAPPER_PTR next;
    PROCESS_WRAPPER_PTR prev;
} PROCESS_WRAPPER;

typedef struct {
    int num;
    PROCESS_WRAPPER_PTR front;
    PROCESS_WRAPPER_PTR rear;
} PQ;

typedef struct LRU_STRUCTURE* LRU_STRUCTURE_PTR;

typedef struct LRU_STRUCTURE {
    PROCESS_PTR p;
    int allocID;
    LRU_STRUCTURE_PTR next;
    LRU_STRUCTURE_PTR prev;
} LRU_STRUCTURE;

typedef struct {
    int num;
    LRU_STRUCTURE_PTR front;
    LRU_STRUCTURE_PTR rear;
} LRUQ;

///MTREE function

MTREE_PTR createMTREE(MTREE_PTR parent, int pStart, int pEnd, int isLeft);

MTREE_PTR searchForAlloc(MTREE_PTR parent, MTREE_PTR mp, int isLeft, int
	requestSize, int cpuCycle, int pid, int allocID, FILE* memory);

MTREE_PTR searchForAccess(MTREE_PTR mp, int pid, int allocID, int cpuCycle, int
	type, FILE* memory);

MTREE_PTR memoryMerge(MTREE_PTR mp);

int allocMem(MTREE_PTR mp, int pid, int allocID);

void memFree(MTREE_PTR mp);

//PQ function

PQ* initQueue(PQ* rq);

void enqueue(PQ* pq, PROCESS_PTR p);

PROCESS_PTR dequeue(PQ* pq);

PROCESS_WRAPPER_PTR queueDelete(PQ* pq, PROCESS_WRAPPER_PTR obj);

void qtoq(PQ* dq, PQ* sq);

//LRU function

LRUQ* initLRUQ(LRUQ* lq);

LRU_STRUCTURE_PTR LRUqueueDelete(LRUQ* lq, LRU_STRUCTURE_PTR obj);

LRU_STRUCTURE_PTR searchLRUQ(LRUQ* lq, int pid, int allocID);

void LRUenqueue(LRUQ* lq, PROCESS_PTR p, int allocID);

LRU_STRUCTURE_PTR LRUdequeue(LRUQ* lq);

//fetching event function

char* eventFetch(char* inputNextInst, int cpuCycle, int* eventCnt, FILE* input);

int detEventType(char* fetchedEvent);

int testCycleFlag(char* inputNextInst, int cpuCycle, int eventCnt, int
	totalEventNum);

//etc

void distributeCPUTime(PQ* aq, int time);

void sqCtrl(PQ* sq, PQ* rq);

void IOCtrl(PQ* ioq, PQ* rq, int pid);

PROCESS_PTR findProcess(PQ* aq, int pid);

void makeProcess(int pid, int timeQuantum, char* procName, PQ* rq, PQ* aq, int vmempg,
	MTREE_PTR root);

PROCESS_PTR fetchProcess(PQ* rq, int time);

void fetchInst(PROCESS_PTR p, int* inst, int* para);

void pExec(PROCESS_PTR p, int inst, int para, int cpuCycle, PQ* sq, PQ* ioq,
	PQ* aq, LRUQ* lq, FILE* memory);

int isBusted(PROCESS_PTR p);

//print function

void printPQ(PQ* pq);

void fprintPQ(FILE* stream, PQ* pq);

void printMTREE(MTREE_PTR mp);

void _printMTREE(MTREE_PTR mp, FILE* stream);

void fprintMTREE(FILE* stream, MTREE_PTR mp);

void printvm(VM_PTR vm, int vmSize);

void fprintvm(FILE* stream, VM_PTR vm, int vmSize);

void fprintvalid(FILE* stream, VM_PTR vm, int vmSize);

void printLRUQ(LRUQ* lq);

void fprintLRUQ(FILE* stream, LRUQ* lq);

//vm function

int vmemIsInvalid(VM_PTR vm, int vmSize, int allocID);

void vmemAlloc(VM_PTR vm, int vmSize, int requireSize, int allocID);

void vmemFree(VM_PTR vm, int vmSize, int allocID);

void vmemMakeInvalid(VM_PTR vm, int vmSize, int allocID);

void vmemMakeValid(VM_PTR vm, int vmSize, int allocID);
