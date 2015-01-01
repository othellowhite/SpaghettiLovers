
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <pthread.h>
#include <unistd.h>		// POSIX operating system : for Distribute Computing

#define ONE_THREAD '1'
#define TWO_THREAD '2'
#define VAL_LENGTH  4

FILE* sharedMemory;

static runTimes;

void errHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void initMemory() {
	
	sharedMemory = fopen("./physicalMemory.txt", "w+");
	fprintf(sharedMemory, "0\n");
	runTimes = 0;
	return;
}

void *__simpleThread__(void *data) {
	int id;
	char tmp[10];
	int memoryValue;
	int i = 0;
	id = *((int*)data);
	
	for (i = 0; i < 100; i++) {
		
		sleep(1);
		
		if (runTimes < 100) fseek(sharedMemory, -3, SEEK_END);
		else if (runTimes < 1000) fseek(sharedMemory, -4, SEEK_END);
		else errHandling("value overflow");
		
		fscanf(sharedMemory, "%d", &memoryValue);
		fprintf(sharedMemory, "%d\n",memoryValue+1);
		
		printf("[%d][Thread ID : %d] get %d, put %d \n", ++runTimes, id, memoryValue, memoryValue+1);
	}
	return 0;
}

void threadRace(char thrNum) {
	pthread_t p_thread[2];
	int thrID, status;
	int a = 1;
	int b = 2;

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
	
	else errHandling("wrong Thread num.");
}

int main(int argc, char **argv){

	if (argc != 2) errHandling("Usage : ./filename <number(s) of thread(s) : 1 or 2> ");
	initMemory();
	switch (*argv[1]) {
		case ONE_THREAD:
			threadRace(ONE_THREAD);
			break;
		case TWO_THREAD:
			threadRace(TWO_THREAD);
			break;
		default:
			errHandling("wrong ipt");
	}
	
	return 0;
}

