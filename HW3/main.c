
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <pthread.h>
#include <unistd.h>		// POSIX operating system : for Distribute Computing

#include "semaphore.h"

#define ONE_THREAD '1'
#define TWO_THREAD '2'
#define THR_THREAD '3'
#define VAL_LENGTH  4

FILE* sharedMemory;

static int runTimes;

void errHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void initMemory() {
	
	sharedMemory = fopen("./physicalMemory.txt", "w+");
	fprintf(sharedMemory, "[Thread ID : X] 0\n");
	runTimes = 0;
	return;
}

struct params {
	int pid;
	key_t skey;
};

void *__simpleThread__(void *data) {
	int pid;
	int memoryValue;
	int i = 0;
	int semid;
	
	struct params *param = data;
	pid = param->pid;
	if (( semid = initsem(param->skey, 1) ) < 0) errHandling ("init Semid Err.");
	

	for (i = 0; i < 100; i++) {
	
		
		/****************** critical section begin ********************/
		
		p(semid);
		
		printf("[%d][Thread ID : %d] -> In critical section. \n",++runTimes, pid);
			   
		usleep(50000); // 0.05 second waits.
		
		if (runTimes < 100) fseek(sharedMemory, -3, SEEK_END);
		else if (runTimes < 1000) fseek(sharedMemory, -4, SEEK_END);
		else errHandling("value overflow"); // it will not happen in this code.
		
		fscanf(sharedMemory, "%d", &memoryValue);
		fprintf(sharedMemory, "[Thread ID : %d] %d\n", pid, memoryValue+1);
		
		printf("[%d][Thread ID : %d] get %d, put %d . Leaving critical section.\n\n",
			   runTimes, pid, memoryValue, memoryValue+1);
		
		v(semid);
		
		/******************* critical section end *********************/
		
	}
	return 0;
}

void threadRace(char thrNum) {
	pthread_t p_thread[3];
	int thrID, status;
	
	/*
	int a = 1;
	int b = 2;
	int c = 3;
	*/
	struct params a;
	struct params b;
	struct params c;
	
	a.pid = 1;
	b.pid = 2;
	c.pid = 3;
	
	a.skey = b.skey = c.skey = 0x200; // [in sample code]
	
	if (thrNum == ONE_THREAD) {
		
		thrID = pthread_create(&p_thread[0], NULL, __simpleThread__, (void*)&a);
		if (thrID<0) errHandling("Err : thread create failed");
		
		// wait for thread end
		pthread_join(p_thread[0], (void**)&status);
	}
	
	else if (thrNum == TWO_THREAD){
		
		thrID = pthread_create(&p_thread[0], NULL, __simpleThread__, (void*)&a);
		if (thrID<0) errHandling("Err : Thread[id : 1] create failed");
		thrID = pthread_create(&p_thread[1], NULL, __simpleThread__, (void*)&b);
		if (thrID<0) errHandling("Err : Thread[id : 2] create failed");
		
		// wait for thread end
		pthread_join(p_thread[0], (void**)&status);
		pthread_join(p_thread[1], (void**)&status);
	}
	else if (thrNum == THR_THREAD){
		
		thrID = pthread_create(&p_thread[0], NULL, __simpleThread__, (void*)&a);
		if (thrID<0) errHandling("Err : Thread[id : 1] create failed");
		thrID = pthread_create(&p_thread[1], NULL, __simpleThread__, (void*)&b);
		if (thrID<0) errHandling("Err : Thread[id : 2] create failed");
		thrID = pthread_create(&p_thread[2], NULL, __simpleThread__, (void*)&c);
		if (thrID<0) errHandling("Err : Thread[id : 3] create failed");
		
		// wait for thread end
		pthread_join(p_thread[0], (void**)&status);
		pthread_join(p_thread[1], (void**)&status);
		pthread_join(p_thread[2], (void**)&status);
	}
	
	else errHandling("wrong Thread num.");
}

int main(int argc, char **argv){
	
	if (argc != 2) errHandling("Usage : ./filename <number(s) of thread(s) : 1 to 3> ");
	initMemory();
	switch (*argv[1]) {
		case ONE_THREAD:
			threadRace(ONE_THREAD);
			break;
		case TWO_THREAD:
			threadRace(TWO_THREAD);
			break;
		case THR_THREAD:
			threadRace(THR_THREAD);
			break;
		default:
			errHandling("wrong ipt");
	}
	
	return 0;
}