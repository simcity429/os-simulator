#include "mydatastruct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

PQ* initQueue(PQ* pq){
    pq = (PQ*)malloc(sizeof(PQ));
    if( pq != NULL)    {
      pq->front = NULL;
      pq->rear = NULL;
      pq->num = 0;
      return pq;
    }else{
	printf("\n\n!!! initQueue() !!!\n\n");
	exit(1);
    }
}

void enqueue(PQ* pq, PROCESS_PTR p){
    PROCESS_WRAPPER_PTR tmp =
	(PROCESS_WRAPPER_PTR)malloc(sizeof(PROCESS_WRAPPER));
    tmp->current = p;
    tmp->prev = NULL;
    tmp->next = NULL;
    int flag;
    if (!pq->rear){
	flag = 0;
	pq->front = tmp;
	pq->rear = tmp;
    } else {
	flag = 1;
	pq->rear->next = tmp;
	tmp->prev = pq->rear;
	pq->rear = tmp;
    }
    pq->num++;
}

PROCESS_PTR dequeue(PQ* pq){
    if (!pq->front){
//	printf("No queue entry!\n");
	return NULL;
    } else {
	PROCESS_WRAPPER_PTR tmp = pq->front;
	pq->front = pq->front->next;
	if (pq->front){
	    pq->front->prev = NULL;
	}
	PROCESS_PTR ret = tmp->current;
	pq->num--;
	if(pq->num == 0){
	    pq->rear = NULL;
	}
	free(tmp);
	return ret;
    }
}

PROCESS_WRAPPER_PTR queueDelete(PQ* pq, PROCESS_WRAPPER_PTR obj){
    if (!obj){
	return NULL;
    }
    PROCESS_WRAPPER_PTR prevP = obj->prev;
    PROCESS_WRAPPER_PTR nextP = obj->next;
    if (prevP&&nextP){
	prevP->next = nextP;
	nextP->prev = prevP;
    } else if (!prevP&&nextP){
	nextP->prev = NULL;
    } else if (prevP&&!nextP){
	prevP->next = NULL;
    }
    if (obj == pq->front){
	pq->front = obj->next;
    }else if (obj == pq->rear){
	pq->rear = obj->prev;
    }
    obj->next = NULL;
    obj->prev = NULL;
    pq->num--;
    if (pq->num==0){
	pq->front = NULL;
	pq->rear = NULL;
    }
    return obj;
}


LRUQ* initLRUQ(LRUQ* lq){
    lq = (LRUQ*)malloc(sizeof(LRUQ));
    if (lq){
	lq->num = 0;
	lq->front = NULL;
	lq->rear = NULL;
	return lq;
    } else {
	printf("  >>LRUQ init failed!!!<<  \n");
	exit(1);
    }
}

LRU_STRUCTURE_PTR LRUqueueDelete(LRUQ* lq, LRU_STRUCTURE_PTR obj){
    if (!obj){
	return NULL;
    }
    LRU_STRUCTURE_PTR prevP = obj->prev;
    LRU_STRUCTURE_PTR nextP = obj->next;
    if (prevP&&nextP){
	prevP->next = nextP;
	nextP->prev = prevP;
    } else if (!prevP&&nextP){
	nextP->prev = NULL;
    } else if (prevP&&!nextP){
	prevP->next = NULL;
    }
    if (obj == lq->front){
	lq->front = obj->next;
    }else if (obj == lq->rear){
	lq->rear = obj->prev;
    }
    obj->next = NULL;
    obj->prev = NULL;
    lq->num--;
    if (lq->num==0){
	lq->front = NULL;
	lq->rear = NULL;
    }
    return obj;
}

LRU_STRUCTURE_PTR searchLRUQ(LRUQ* lq, int pid, int allocID){
    if (lq->num == 0){
	return NULL;
    }
    LRU_STRUCTURE_PTR tmp = lq->front;
    while(tmp){
	if (tmp->p->pid == pid && tmp->allocID == allocID){
	    return tmp;
	}
	tmp = tmp->next;
    }
    return NULL;
}

void LRUenqueue(LRUQ* lq, PROCESS_PTR p, int allocID){
    LRU_STRUCTURE_PTR tmp = searchLRUQ(lq, p->pid, allocID);
    if (tmp){
	tmp = LRUqueueDelete(lq, tmp);
    } else {
	tmp = (LRU_STRUCTURE_PTR)malloc(sizeof(LRU_STRUCTURE));
	tmp->p = p;
	tmp->allocID = allocID;
	tmp->prev = NULL;
	tmp->next = NULL;
    }
    int flag;
    if (!lq->rear){
	flag = 0;
	lq->front = tmp;
	lq->rear = tmp;
    } else {
	flag = 1;
	lq->rear->next = tmp;
	tmp->prev = lq->rear;
	lq->rear = tmp;
    }
    lq->num++;
}

LRU_STRUCTURE_PTR LRUdequeue(LRUQ* lq){
    if (!lq->front){
//	printf("No queue entry!\n");
	return NULL;
    } else {
	LRU_STRUCTURE_PTR tmp = lq->front;
	lq->front = lq->front->next;
	if (lq->front){
	    lq->front->prev = NULL;
	}
	lq->num--;
	if(lq->num == 0){
	    lq->rear = NULL;
	}
	return tmp;
    }
}

char* eventFetch(char* inputNextInst, int cpuCycle, int* eventCnt, FILE* input){
    char tmp[100];
    strcpy(tmp, inputNextInst);
    char* token = strtok(inputNextInst, " ");
    strcpy(inputNextInst, tmp);
    char* retStr = (char*)malloc(sizeof(char)*100);
    int instCycle;
    instCycle = atoi(token);
    if (instCycle == cpuCycle){
	strcpy(retStr, tmp);
	(*eventCnt)++;
	fgets(inputNextInst, sizeof(char)*100, input);
    } else {
	free(retStr);
	retStr = NULL;
    }
    return retStr;
}

int testCycleFlag(char* inputNextInst, int cpuCycle, int eventCnt, int eventNum){
    char tmp[100];
    strcpy(tmp, inputNextInst);
    char* token = strtok(inputNextInst, " ");
    strcpy(inputNextInst, tmp);
    int instCycle = atoi(token);
    if (eventCnt == eventNum){
	instCycle = -1;
    }
    if (instCycle == cpuCycle){
	return 1;
    } else {
	return 0;
    }
}

int detEventType(char* fetchedEvent){
    if (!fetchedEvent){
	return 0;
    }
    char* token = strtok(fetchedEvent, " ");
    char* pos;
    token = strtok(NULL, " ");
    if ((pos = strchr(token, '\n')) != NULL){
	*pos = '\0';
    }
    if (strcmp(token, "INPUT")){
	return 1;
    } else {
	return 2;
    }
}

void qtoq(PQ* dq, PQ* sq){
    while(sq->num){
	enqueue(dq, dequeue(sq));
    }
}

void distributeCPUTime(PQ* aq, int time){
    int cnt = 0;
//    printf("distribute called!\n");
    PROCESS_WRAPPER_PTR tmp = aq->front;
    while(tmp){
	cnt++;
	if (!tmp->current->burst){
	    tmp->current->remainingQuantum += time;
	}
	tmp = tmp->next;
    }
}

void sqCtrl(PQ* sq, PQ* rq){
    PROCESS_WRAPPER_PTR tmp = sq->front;
    PROCESS_WRAPPER_PTR nextTmp = NULL;
    PROCESS_WRAPPER_PTR p;
    int flag = 0;
    while(tmp){
	tmp->current->sleep--;
	if (tmp->current->sleep == 0){
	    tmp->current->isSleeping = 0;
	    nextTmp = tmp->next;
	    flag = 1;
	    p = queueDelete(sq, tmp);
	    if (!(p->current->isIOWaiting)){
//		printf("pid %d sleep end\n", p->current->pid);
		enqueue(rq, p->current);
	    }
	}
	if (flag){
	    tmp = nextTmp;
	} else {
	    tmp = tmp->next;
	}
	flag = 0;
    }
}

void IOCtrl(PQ* ioq, PQ* rq, int pid){
    PROCESS_WRAPPER_PTR tmp = ioq->front;
    while(tmp){
	if (tmp->current->pid == pid){
	    tmp->current->isIOWaiting = 0;
	    PROCESS_WRAPPER_PTR p = queueDelete(ioq, tmp);
	    if (!(p->current->isSleeping)){
//		printf("!!! pid %d io end!!!\n", p->current->pid);
		enqueue(rq, p->current);
	    }
	    break;
	}
	tmp = tmp->next;
    }
}

PROCESS_PTR findProcess(PQ* aq, int pid){
    if (aq->num == 0){
	return NULL;
    }
    PROCESS_WRAPPER_PTR tmp = aq->front;
    while (tmp){
	if (tmp->current->pid == pid){
	    return tmp->current;
	}
	tmp = tmp->next;
    }
    return NULL;
}

void makeProcess(int pid, int timeQuantum, char* procName, PQ* rq, PQ* aq, int vmempg,
	MTREE_PTR root){
    FILE* procFile = fopen(procName, "r");
    char tmpbuf[20];
    sprintf(tmpbuf, "%d", pid);
    char* txt = ".txt";
    strcat(tmpbuf, txt);
    PROCESS_PTR newProc = (PROCESS_PTR)malloc(sizeof(PROCESS));
    newProc->root = root;
    newProc->vmSize = vmempg;
    newProc->pstream = procFile;
    newProc->name = procName;
    newProc->outstream = fopen(tmpbuf, "w");
    newProc->pid = pid;
    newProc->burst = 0;
    newProc->sleep = 0;
    newProc->remainingQuantum = timeQuantum;
    newProc->remainingCPUCycle = 0;
    newProc->isIOWaiting = 0;
    newProc->isSleeping = 0;
    newProc->allocID = 0;
    fscanf(procFile, "%d", &(newProc->instNum));
    newProc->pc = 0;
    newProc->vm = (VM_PTR)malloc(sizeof(VM)*vmempg);
    for (int i=0;i<vmempg;i++){
	newProc->vm[i].allocID = -1;
	newProc->vm[i].valid = 0;
    }
    enqueue(aq, newProc);
    enqueue(rq, newProc);
}

PROCESS_PTR fetchProcess(PQ* rq, int time){
    int cnt = 0;
    while(cnt<(rq->num)){
	PROCESS_PTR tmp = dequeue(rq);
	if ((tmp->remainingQuantum)>0){
	    tmp->remainingCPUCycle = time;
	    return tmp;
	} else {
	    enqueue(rq, tmp);
	    cnt++;
	}
    }
    return NULL;
}

void fetchInst(PROCESS_PTR p, int* inst, int* para){
    FILE* pstream = p->pstream;
    fscanf(pstream, "%d\t%d", inst, para);
    p->pc++;
//    printf("executing pid: %d, %d %d\n", p->pid, *inst, *para);
    return;
}

void pExec(PROCESS_PTR p, int inst, int para, int cpuCycle, PQ* sq, PQ* ioq,
	PQ* aq, LRUQ* lq, FILE* memory){
    p->remainingQuantum--;
    p->remainingCPUCycle--; 
    if (inst == 0){//allocation
	int requestSize = para;
	int dpid, dallocID;
	MTREE_PTR mp = NULL;
	MTREE_PTR obj = NULL;
	PROCESS_PTR objProcess = NULL;
	LRU_STRUCTURE_PTR lp = NULL;
	vmemAlloc(p->vm, p->vmSize, requestSize, p->allocID);
	LRUenqueue(lq, p, p->allocID);
	while(1){
	    mp = searchForAlloc(NULL, p->root, 1, requestSize, cpuCycle,
		    p->pid, p->allocID, memory);
	    if (mp){
		allocMem(mp, p->pid, p->allocID);
		break;
	    } else {
		lp = LRUdequeue(lq);
		dpid = lp->p->pid;
		dallocID = lp->allocID;
		obj = searchForAccess(p->root, dpid, dallocID, cpuCycle, 2, memory);
		memFree(obj);
		objProcess = findProcess(aq, dpid);
		vmemMakeInvalid(objProcess->vm, objProcess->vmSize, dallocID);
	    }
	}
	p->allocID++;
    } else if (inst == 1){//access
	int rallocID = para;
	int requestSize = vmemIsInvalid(p->vm, p->vmSize, rallocID);
	if (requestSize > 0){    
	    int dpid, dallocID;
	    MTREE_PTR mp = NULL;
	    MTREE_PTR obj = NULL;
	    PROCESS_PTR objProcess = NULL;
	    LRU_STRUCTURE_PTR lp = NULL;
	    vmemMakeValid(p->vm, p->vmSize, rallocID);
	    LRUenqueue(lq, p, rallocID);
	    while(1){
		mp = searchForAlloc(NULL, p->root, 1, requestSize, cpuCycle,
			p->pid, rallocID, memory);
		if (mp){
		    allocMem(mp, p->pid, rallocID);
		    searchForAccess(p->root, p->pid, rallocID, cpuCycle, 1, memory);
		    break;
		} else {
		    lp = LRUdequeue(lq);
		    dpid = lp->p->pid;
		    dallocID = lp->allocID;
		    obj = searchForAccess(p->root, dpid, dallocID, cpuCycle, 2, memory);
		    memFree(obj);
		    objProcess = findProcess(aq, dpid);
		    vmemMakeInvalid(objProcess->vm, objProcess->vmSize, dallocID);
		}
	    }
	} else {
	    searchForAccess(p->root, p->pid, rallocID, cpuCycle, 1, memory);
	    LRUenqueue(lq, p, rallocID);
	}
    } else if (inst == 2){//free
	int dpid, dallocID;
	MTREE_PTR obj;
	LRU_STRUCTURE_PTR ltmp;
	dallocID = para;
	dpid = p->pid;
	vmemFree(p->vm, p->vmSize, dallocID);
	obj = searchForAccess(p->root, dpid, dallocID, cpuCycle, 2, memory);
	memFree(obj);
	ltmp = searchLRUQ(lq, dpid, dallocID);
	LRUqueueDelete(lq, ltmp);
    } else if (inst==4 && p->pc != p->instNum){
	p->sleep = para;
	p->isSleeping = 1;
	enqueue(sq, p);
    } else if (inst == 5 && p->pc != p->instNum){
	p->isIOWaiting = 1;
	enqueue(ioq, p);
    }
    //print [pid].txt
    fprintf(p->outstream, "%d Cycle#Instruction op %d arg %d\n", cpuCycle, inst, para);
    fprintf(p->outstream, "AllocID");
    fprintvm(p->outstream, p->vm, p->vmSize);
    fprintf(p->outstream, "Valid  ");
    fprintvalid(p->outstream, p->vm, p->vmSize);
    fprintf(p->outstream, "\n");

    if (p->pc == p->instNum){
//	printf("pid: %d ended\n", p->pid);
	for (int i=0;i<p->allocID;i++){
	    int dpid, dallocID;
	    MTREE_PTR obj;
	    LRU_STRUCTURE_PTR ltmp;
	    dallocID = i;
	    dpid = p->pid;
	    vmemFree(p->vm, p->vmSize, dallocID);
	    obj = searchForAccess(p->root, dpid, dallocID, cpuCycle, 2, memory);
	    memFree(obj);
	    ltmp = searchLRUQ(lq, dpid, dallocID);
	    LRUqueueDelete(lq, ltmp);
	}
	p->burst = 1;
	fclose(p->pstream);
	fclose(p->outstream);
    }
}

int isBusted(PROCESS_PTR p){
    if (!p){
	return 0;
    }
    int io, sleep, cpu, q;
    io = p->isIOWaiting;
    sleep = p->isSleeping;
    cpu = p->remainingCPUCycle;
    q = p->remainingQuantum;
    if((!io&&!sleep&&(cpu==0)||(q==0))||p->burst){
	return 1;
    } else {
	return 0;
    }
}

void printPQ(PQ* pq){
    PROCESS_WRAPPER_PTR tmp = pq->front;
    while(tmp){
	printf("%d--",tmp->current->pid);
	tmp = tmp->next;
    }
    printf("\n");
}

void fprintPQ(FILE* stream, PQ* pq){
    PROCESS_WRAPPER_PTR tmp = pq->front;
    if (!tmp){
	fprintf(stream, "Empty\n");
	return;
    }
    while(tmp){
	fprintf(stream, "%d(%s) ",tmp->current->pid, tmp->current->name);
	tmp = tmp->next;
    }
    fprintf(stream, "\n");
    return;
}

MTREE_PTR createMTREE(MTREE_PTR parent, int pStart, int pEnd, int isLeft){
    int mySize = (pEnd - pStart + 1)/2;
    if (mySize < 1){
	return NULL;
    }
    MTREE_PTR newMTREE = (MTREE_PTR)malloc(sizeof(MTREE));
    newMTREE->leaf = 1;
    newMTREE->isAlloc = 0;
    newMTREE->parent = parent;
    newMTREE->pid = -1;
    newMTREE->allocID = -1;
    newMTREE->isLeft = isLeft;
    newMTREE->lc = NULL;
    newMTREE->rc = NULL;
    if (isLeft){
	newMTREE->startIndex = pStart;
	newMTREE->endIndex = pStart + mySize - 1;
    } else {
	newMTREE->startIndex = pStart + mySize;
	newMTREE->endIndex = pEnd;
    }
    return newMTREE;    
}

MTREE_PTR searchForAlloc(MTREE_PTR parent, MTREE_PTR mp, int isLeft, int
	requestSize, int cpuCycle, int pid, int allocID, FILE* memory){
    int pSize;
    if (parent){
	pSize = parent->endIndex - parent->startIndex + 1;
    }
    int mySize = (mp->endIndex) - (mp->startIndex) + 1;
    int cSize = mySize/2;
    if (mp->leaf){//if mp is leaf node
	if ((requestSize > cSize) && (requestSize <= mySize)){
	    if (mp->isAlloc){//if allocated, stop search
		return NULL;
	    } else {//found fitting block
		fprintf(memory, "%d\t%d\t%d\t%d\n", cpuCycle, pid, allocID, 0);
		return mp;
	    }
	} else if (requestSize <= cSize && !mp->isAlloc){//continue searching
	    mp->leaf = 0;
	    mp->isAlloc = 1;
	    mp->rc = createMTREE(mp, mp->startIndex, mp->endIndex, 0);//spliting block
	    mp->lc = createMTREE(mp, mp->startIndex, mp->endIndex, 1);//and making children
	    return searchForAlloc(mp, mp->lc, 1, requestSize, cpuCycle, pid, allocID, memory);//continue searcing
	} else {//if allocated stop search
	    return NULL;
	}
    } else {//not a leaf node, continue searching
	MTREE_PTR lc, rc;
	lc = searchForAlloc(mp, mp->lc, 1, requestSize, cpuCycle, pid, allocID, memory);
	if (lc){//leftmost first
	    return lc;
	}
	rc = searchForAlloc(mp, mp->rc, 0, requestSize, cpuCycle, pid, allocID, memory);
	return rc;	
    }
}

MTREE_PTR searchForAccess(MTREE_PTR mp, int pid, int allocID, int cpuCycle, int type, FILE* memory){
    if(!mp){
	return NULL;
    }
    int mypid = mp->pid;
    int myAllocID = mp->allocID;
    if (mypid == pid && allocID == myAllocID){
	fprintf(memory, "%d\t%d\t%d\t%d\n", cpuCycle, pid, allocID, type);
	return mp;
    }
    MTREE_PTR lc, rc;
    lc = searchForAccess(mp->lc, pid, allocID, cpuCycle, type, memory);
    rc = searchForAccess(mp->rc, pid, allocID, cpuCycle, type, memory);
    if (!lc&&!rc){
	return NULL;
    } else {
	if (lc){
	    return lc;
	} else {
	    return rc;
	}
    }
}

MTREE_PTR memoryMerge(MTREE_PTR mp){ //return parent
/* memory merging when mp and mp's sibling are neither allocated
   and recursively executed at mp's parent*/
    if(!mp){
	return NULL;
    }
    int sibAlloc;
    int myAlloc = mp->isAlloc;
    MTREE_PTR parent = mp->parent;
    MTREE_PTR sib;
    if (!parent){
	return NULL;
    }
    if (mp->isLeft){
	sib = parent->rc;
	sibAlloc = sib->isAlloc;
    } else {
	sib = parent->lc;
	sibAlloc = sib->isAlloc;
    }
    if (!myAlloc){
	if (!sibAlloc){
	    free(mp);
	    free(sib);
	    parent->lc = NULL;
	    parent->rc = NULL;
	    parent->leaf = 1;
	    parent->isAlloc = 0;
	    return parent;
	}
    }
    return NULL;
}

void memFree(MTREE_PTR mp){//free memory and do memory merging
    if (!mp){
	return;
    }
    mp->isAlloc = 0;
    mp->pid = -1;
    mp->allocID = -1;
    while(mp){
	mp = memoryMerge(mp);
    }
}

void printMTREE(MTREE_PTR mp){
    if (!mp){
	return;
    }
    printMTREE(mp->lc);
    printMTREE(mp->rc);
    if (mp->leaf){
	int mySize = mp->endIndex - mp->startIndex + 1;
	for (int i=mp->startIndex;i<=mp->endIndex;i++){
	    if (i==mp->startIndex){
		if (mp->isAlloc){
		    printf("|%d#%d", mp->pid, mp->allocID);
		} else {
		    printf("|---");
		}
	    } else {
		if (mp->isAlloc){
		    printf(" %d#%d",mp->pid, mp->allocID);
		} else {
		    printf(" ---");
		}
	    }
	}
	printf("|\n");
    }
}

void _printMTREE(MTREE_PTR mp, FILE* stream){
    if (!mp){
	return;
    }
    _printMTREE(mp->lc, stream);
    _printMTREE(mp->rc, stream);
    if (mp->leaf){
	int mySize = mp->endIndex - mp->startIndex + 1;
	for (int i=mp->startIndex;i<=mp->endIndex;i++){
	    if (i==mp->startIndex){
		if (mp->isAlloc){
		    fprintf(stream, "|%d#%d", mp->pid, mp->allocID);
		} else {
		    fprintf(stream, "|---");
		}
	    } else {
		if (mp->isAlloc){
		    fprintf(stream, " %d#%d",mp->pid, mp->allocID);
		} else {
		    fprintf(stream, " ---");
		}
	    }
	}
    }
}

void fprintMTREE(FILE* stream, MTREE_PTR mp){
    _printMTREE(mp, stream);
    fprintf(stream, "|\n");
}

int allocMem(MTREE_PTR mp, int pid, int allocID){
    if (!mp){
	return 0;
    }
    mp->isAlloc = 1;
    mp->pid = pid;
    mp->allocID = allocID;
    return 1;
}

int vmemIsInvalid(VM_PTR vm, int vmSize, int allocID){
    int flag = 0;
    int size = 0;
    for (int i=0;i<vmSize;i++){
	if (vm[i].allocID == allocID && vm[i].valid == 0){
	    size++;
	}else if (vm[i].allocID == allocID && vm[i].valid == 1){
	    flag = 1;
	}
    }
    if (flag){
	return 0;
    } else if (!flag && !size){
	return -1;
    } else {
	return size;
    }
}

void vmemAlloc(VM_PTR vm, int vmSize, int requireSize, int allocID){
    int tmp;
    if (!vm){
	return;
    }
    for(int i=0;i<vmSize;){;
	tmp = i+1;
	int j;
	for (j=i;j<requireSize+i;j++){
	    if (vm[j].allocID != -1){
		tmp = j;
		break;
	    }
	}
	if (j==requireSize+i){
	    for (int k=i;k<requireSize+i;k++){
		vm[k].allocID = allocID;
		vm[k].valid = 1;
	    }
	    return;
	}
	if (tmp < i+1){
	    i += 1;
	} else {
	    i = tmp;
	}
    }
    printf("  >>vmemAlloc fail<<  \n");
    return;
}

void vmemFree(VM_PTR vm, int vmSize, int allocID){
    if (!vm){
	return;
    }
    for (int i=0;i<vmSize;i++){
	if (vm[i].allocID == allocID){
	    vm[i].allocID = -1;
	    vm[i].valid = 0;
	}
    }
}

void vmemMakeInvalid(VM_PTR vm, int vmSize, int allocID){
    if (!vm){
	return;
    }
    for (int i=0;i<vmSize;i++){
	if (vm[i].allocID == allocID){
	    vm[i].valid = 0;
	}
    }
}

void vmemMakeValid(VM_PTR vm, int vmSize, int allocID){
    if (!vm){
	return;
    }
    for (int i=0;i<vmSize;i++){
	if (vm[i].allocID == allocID){
	    vm[i].valid = 1;
	}
    }
}

void printvm(VM_PTR vm, int vmSize){
    for (int i=0;i<vmSize;i++){
	if(vm[i].allocID == -1){
	    printf("-");
	} else {
	    printf("%d", vm[i].allocID);
	}
    }
    printf("\n");
}

void printLRUQ(LRUQ* lq){
    LRU_STRUCTURE_PTR tmp = lq->front;
    while(tmp){
	printf("(%d#%d)--",tmp->p->pid, tmp->allocID);
	tmp = tmp->next;
    }
    printf("\n");
}

void fprintLRUQ(FILE* stream, LRUQ* lq){
    fprintf(stream, "LRU:");
    LRU_STRUCTURE_PTR tmp = lq->front;
    while(tmp){
	fprintf(stream, " (%d:%d)",tmp->p->pid, tmp->allocID);
	tmp = tmp->next;
    }
    fprintf(stream, "\n");
}

void fprintvm(FILE* stream, VM_PTR vm, int vmSize){
    fprintf(stream, "|");
    for (int i=0;i<vmSize;i++){
	if(vm[i].allocID == -1){
	    fprintf(stream, "-");
	} else {
	    fprintf(stream, "%d", vm[i].allocID);
	}
	if ((i%4) == 3){
	    fprintf(stream, "|");
	}
    }
    fprintf(stream, "\n");
}

void fprintvalid(FILE* stream, VM_PTR vm, int vmSize){
    fprintf(stream, "|");
    for (int i=0;i<vmSize;i++){
	fprintf(stream, "%d", vm[i].valid);
	if ((i%4) == 3){
	    fprintf(stream, "|");
	}
    }
    fprintf(stream, "\n");
}

