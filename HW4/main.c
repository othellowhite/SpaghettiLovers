// OS_HW4.c

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include <pthread.h>
#include <unistd.h>		// POSIX operating system : for Distribute Computing

#include <time.h>

#include "semaphore.h"

#define FORK_1 0x200
#define FORK_2 0x300
#define FORK_3 0x400
#define FORK_4 0x500
#define FORK_5 0x600

#define _TRUE	1
#define _FALSE	0

time_t timechecker;
int Observer_condition = 0;
int forkOnTable[6], philosophierAreNowFull=0;

void ErrHandling (char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

struct params {
	int pid;
	key_t skey;
};



/*************************************************************************************************
 
	Semaphore Error Testing.
 
 *************************************************************************************************/

void *__testSem__(void *data) {
	struct params *param = data;
	int pid = param->pid;
	int semid = initsem(param->skey, 1);
	
	if (semid < 0) ErrHandling("Err : semid init failed. \n");
	
	p(semid);
	printf("[Thread id : %d, Semaphore key :  %x] Enter critical section. \n", pid, semid);
	
	printf("[Thread id : %d, Semaphore key :  %x] Before sleep (0.01msec). \n", pid, semid);
	usleep(10000);
	printf("[Thread id : %d, Semaphore key :  %x] After sleep (0.01msec). \n", pid, semid);
	
	printf("[Thread id : %d, Semaphore key :  %x] Out critical section. \n\n", pid, semid);
	v(semid);
	
	return 0;
}

void testSem() {

	int thrID, status;
	pthread_t p_thread[3];
	
	struct params a;
	struct params b;
	struct params c;
	
	a.pid = 1;
	b.pid = 2;
	c.pid = 3;
	
	printf("\n");
	printf("< Simple Semaphore Testing 1  - Different semaphore Key > \n\n");
	
	a.skey = 0x200;
	b.skey = 0x300;
	c.skey = 0x400;
	
	if ( (thrID = pthread_create(&p_thread[0], NULL, __testSem__, (void*)&a) ) < 0)
		ErrHandling("Err : thread create [id : 1] failed. \n");
	if ( (thrID = pthread_create(&p_thread[1], NULL, __testSem__, (void*)&b) ) < 0)
		ErrHandling("Err : thread create [id : 2] failed. \n");
	if ( (thrID = pthread_create(&p_thread[2], NULL, __testSem__, (void*)&c) ) < 0)
		ErrHandling("Err : thread create [id : 3] failed. \n");
	
	pthread_join(p_thread[0], (void**)&status);
	pthread_join(p_thread[1], (void**)&status);
	pthread_join(p_thread[2], (void**)&status);
	
	printf("< Simple Semaphore Testing 2  - Same semaphore Key > \n\n");
	
	a.skey = 0x200;
	b.skey = 0x200;
	c.skey = 0x200;
	
	if ( (thrID = pthread_create(&p_thread[0], NULL, __testSem__, (void*)&a) ) < 0)
		ErrHandling("Err : thread create [id : 1] failed. \n");
	if ( (thrID = pthread_create(&p_thread[1], NULL, __testSem__, (void*)&b) ) < 0)
		ErrHandling("Err : thread create [id : 2] failed. \n");
	if ( (thrID = pthread_create(&p_thread[2], NULL, __testSem__, (void*)&c) ) < 0)
		ErrHandling("Err : thread create [id : 3] failed. \n");
	
	pthread_join(p_thread[0], (void**)&status);
	pthread_join(p_thread[1], (void**)&status);
	pthread_join(p_thread[2], (void**)&status);
	
}

/*************************************************************************************************
 
	case 1. default
 
*************************************************************************************************/
 
struct philosophier_info {
	int pid;
	char name[30];
	key_t leftHand, rightHand;
};

void *__dining_philosophier_f__(void *data) {
	int pid, i;
	char name[30];
	int leftHand, rightHand;
	
	struct philosophier_info *param = data;
	
	pid = param->pid;
	strcpy(name, param->name);
	if (( leftHand = initsem(param->leftHand, 1) ) < 0) ErrHandling ("init leftHand Err.");
	if (( rightHand = initsem(param->rightHand, 1) ) < 0) ErrHandling ("init rightHand Err.");
	
	
	for(i = 1; i <= 100; i++) {
		
		printf("[Thread ID : %d] \'%s\' wants to pick up the [left fork] ! \n", pid, name);
		p(leftHand);
		printf("[Thread ID : %d] \'%s\' successfully picked up the [left fork] ! \n", pid, name);
		
		printf("[Thread ID : %d] \'%s\' wants to pick up the [right fork] ! \n", pid, name);
		p(rightHand);
		printf("[Thread ID : %d] \'%s\' successfully picked up the [right fork] ! \n", pid, name);
		
		usleep(500000);// eating 0.5 sec
		printf("[Thread ID : %d] \'%s\' successfully eat Spaghetti ! \n", pid, name);
		
		printf("[Thread ID : %d] \'%s\' wants to throw the [right fork] ! \n", pid, name);
		v(rightHand);
		printf("[Thread ID : %d] \'%s\' successfully throw the [right fork] with hand ! \n", pid, name);
		
		printf("[Thread ID : %d] \'%s\' wants to throw the [left fork] ! \n", pid, name);
		v(leftHand);
		printf("[Thread ID : %d] \'%s\' successfully throw the [left fork] with hand ! \n", pid, name);

		printf("\n[Thread ID : %d] \'%s\' ate spaghetti [%d] times !! \n\n", pid, name, i);
	}
	
	return 0;
}

/*************************************************************************************************
 
	case 2. Set observer
 
 *************************************************************************************************/

struct observer_info {
	int pid;
	key_t watch_1, watch_2, watch_3, watch_4, watch_5 ;
};
void *__observer__(void *data) {
	int pid;
	int watch_1, watch_2, watch_3, watch_4, watch_5;
	struct observer_info *observ = data;
	
	
	if (( watch_1 = initsem(observ->watch_1, 1) ) < 0) ErrHandling ("observer watch 1 Err.");
	if (( watch_2 = initsem(observ->watch_2, 1) ) < 0) ErrHandling ("observer watch 2 Err.");
	if (( watch_3 = initsem(observ->watch_3, 1) ) < 0) ErrHandling ("observer watch 3 Err.");
	if (( watch_4 = initsem(observ->watch_4, 1) ) < 0) ErrHandling ("observer watch 4 Err.");
	if (( watch_5 = initsem(observ->watch_1, 1) ) < 0) ErrHandling ("observer watch 5 Err.");
	
	while (philosophierAreNowFull!=5) {
		if (time(NULL) - timechecker > 3 ) {
			
			// 1초 지나도록 0으로 반환이 안되면 데드락상태.
			printf("\n\n ** now deadlock ** \n\n ");
			Observer_condition = _TRUE;
			// 모든 스래드 왼손집기 직후 중지.
			
			v(watch_1); forkOnTable[1]=_TRUE;
			v(watch_2); forkOnTable[2]=_TRUE;
			v(watch_3); forkOnTable[3]=_TRUE;
			v(watch_4); forkOnTable[4]=_TRUE;
			v(watch_5); forkOnTable[5]=_TRUE;
			
			printf("\n#### FORK is now [ (fork1) : %d, (fork2) : %d, (fork3) : %d, (fork4) : %d, (fork5) : %d ] On a Table !! #### \n\n",
				   forkOnTable[1], forkOnTable[2], forkOnTable[3], forkOnTable[4], forkOnTable[5]);
			
			while (!(forkOnTable[1]==forkOnTable[2]==forkOnTable[3]==forkOnTable[4]==forkOnTable[5])) printf(".");
			forkOnTable[1]=forkOnTable[2]=forkOnTable[3]=forkOnTable[4]=forkOnTable[5]=_FALSE;
			
			timechecker = time(NULL);
			Observer_condition = _FALSE;	// philosophier can able to eat spaghetti.
			sleep(2); // observer take a nap 2 secs
			
		}
	}
	
	return 0;
}

void *__dining_philosophier_o__(void *data) {
	int pid, i;
	char name[30];
	int leftHand, rightHand;
	
	int preempted=_FALSE;
	
	struct philosophier_info *param = data;
	
	pid = param->pid;
	strcpy(name, param->name);
	if (( leftHand = initsem(param->leftHand, 1) ) < 0) ErrHandling ("init leftHand Err.");
	if (( rightHand = initsem(param->rightHand, 1) ) < 0) ErrHandling ("init rightHand Err.");
	
	
	for(i = 1; i <= 100; ) {
		
		//printf("[Thread ID : %d] \'%s\' wants to pick up the [left fork] ! \n", pid, name);
		p(leftHand);
		//printf("[Thread ID : %d] \'%s\' successfully picked up the [left fork] ! \n", pid, name);
		
		//printf("[Thread ID : %d] \'%s\' wants to pick up the [right fork] ! \n", pid, name);
		p(rightHand);
		
		if (Observer_condition == _TRUE) {
			preempted = _TRUE;
			v(rightHand);
			timechecker = time(NULL);
			printf("[Thread ID : %d] \'%s\' Preempted !!!!!!!!!!!! \n", pid, name);
			while (Observer_condition==_TRUE) ;
		}
		
		if (preempted == _FALSE) {
			
			//printf("[Thread ID : %d] \'%s\' successfully picked up the [right fork] ! \n", pid, name);
			
			usleep(200000);// eating 0.2 sec
			//printf("[Thread ID : %d] \'%s\' successfully eat Spaghetti ! \n", pid, name);
			
			//printf("[Thread ID : %d] \'%s\' wants to throw the [right fork] ! \n", pid, name);
			v(rightHand);
			//printf("[Thread ID : %d] \'%s\' successfully throw the [right fork] with hand ! \n", pid, name);
			
			//printf("[Thread ID : %d] \'%s\' wants to throw the [left fork] ! \n", pid, name);
			v(leftHand);
			//printf("[Thread ID : %d] \'%s\' successfully throw the [left fork] with hand ! \n", pid, name);
			
			printf("\n[Thread ID : %d] \'%s\' ate spaghetti [%d] times !! \n", pid, name, i);
			
			i++;
		}
		
		preempted = _FALSE;
		timechecker = time(NULL);
	}
	
	philosophierAreNowFull++; // to end program
	return 0;
}
/*************************************************************************************************
 
	case 3. Set eating Sequence.
 
 *************************************************************************************************/

void *__dining_philosophier_s__(void *data) {
	int pid;
	char name[30];
	int leftHand, rightHand;
	
	struct philosophier_info *param = data;
	
	pid = param->pid;
	strcpy(name, param->name);
	if (( leftHand = initsem(param->leftHand, 1) ) < 0) ErrHandling ("init leftHand Err.");
	if (( rightHand = initsem(param->rightHand, 1) ) < 0) ErrHandling ("init rightHand Err.");
	//printf("[Thread ID : %d] \'%s\' wants to pick up the [left fork] ! \n", pid, name);
	p(leftHand);
	//printf("[Thread ID : %d] \'%s\' successfully picked up the [left fork] ! \n", pid, name);
	
	//printf("[Thread ID : %d] \'%s\' wants to pick up the [right fork] ! \n", pid, name);
	p(rightHand);
	//printf("[Thread ID : %d] \'%s\' successfully picked up the [right fork] ! \n", pid, name);
	
	usleep(50000);// eating 0.05 sec
	//printf("[Thread ID : %d] \'%s\' successfully eat Spaghetti ! \n", pid, name);
	
	//printf("[Thread ID : %d] \'%s\' wants to throw the [right fork] ! \n", pid, name);
	v(rightHand);
	//printf("[Thread ID : %d] \'%s\' successfully throw the [right fork] with hand ! \n", pid, name);
	
	//printf("[Thread ID : %d] \'%s\' wants to throw the [left fork] ! \n", pid, name);
	v(leftHand);
	//printf("[Thread ID : %d] \'%s\' successfully throw the [left fork] with hand ! \n", pid, name);
	
	printf("[Thread ID : %d] \'%s\' ate spaghetti !! \n", pid, name);
	
	return 0;
}


/*************************************************************************************************
 
	Dining Philosophier : how they eat ? 
 
		-f : default
		-o : set observer
		-s : set sequence
 
 *************************************************************************************************/

int dining_philosophier(char arg) {

	pthread_t p_thread[6];
	int thrID, status;
	
	struct philosophier_info Descartes;
	struct philosophier_info Voltaire;
	struct philosophier_info Sokrates;
	struct philosophier_info Thales;
	struct philosophier_info Plato;
	
	struct observer_info Observer;
	
	Descartes.pid		= 1;
	strcpy(Descartes.name, "René Descartes");
	Descartes.leftHand	= FORK_1;
	Descartes.rightHand	= FORK_2;
	
	Voltaire.pid		= 2;
	strcpy(Voltaire.name, "François Marie Arouet");
	Voltaire.leftHand	= FORK_2;
	Voltaire.rightHand	= FORK_3;
	
	Sokrates.pid		= 3;
	strcpy(Sokrates.name, "Σωκράτης");
	Sokrates.leftHand	= FORK_3;
	Sokrates.rightHand	= FORK_4;
	
	Thales.pid			= 4;
	strcpy(Thales.name, "Θαλής");
	Thales.leftHand  	= FORK_4;
	Thales.rightHand	= FORK_5;
	
	Plato.pid			= 5;
	strcpy(Plato.name, "Πλάτων");
	Plato.leftHand   	= FORK_5;
	Plato.rightHand		= FORK_1;
	
	if ( !(arg=='f' || arg == 'o' || arg == 's')) return -1;
	
	printf("\n");
	printf("< Dining Philosopher  - There are five Philosopher(s), Deadlock Prevention : [%c] > \n\n", arg);
	
	if (arg == 'f') {

		
		if ( (thrID = pthread_create(&p_thread[0], NULL, __dining_philosophier_f__, (void*)&Descartes) ) < 0)
			ErrHandling("Descatres hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[1], NULL, __dining_philosophier_f__, (void*)&Voltaire) ) < 0)
			ErrHandling("Voltaire hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[2], NULL, __dining_philosophier_f__, (void*)&Sokrates) ) < 0)
			ErrHandling("Sokrates hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[3], NULL, __dining_philosophier_f__, (void*)&Thales) ) < 0)
			ErrHandling("Thales hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[4], NULL, __dining_philosophier_f__, (void*)&Plato) ) < 0)
			ErrHandling("Plato hate spaghetti.");
		
		
		pthread_join(p_thread[0], (void**)&status);
		pthread_join(p_thread[1], (void**)&status);
		pthread_join(p_thread[2], (void**)&status);
		pthread_join(p_thread[3], (void**)&status);
		pthread_join(p_thread[4], (void**)&status);

	}
	else if (arg == 'o') {
		
		Observer.pid		= 6;
		Observer.watch_1	= FORK_1;
		Observer.watch_2	= FORK_2;
		Observer.watch_3	= FORK_3;
		Observer.watch_4	= FORK_4;
		Observer.watch_5	= FORK_5;
		
		timechecker = time(NULL);
		
		
		if ( (thrID = pthread_create(&p_thread[0], NULL, __dining_philosophier_o__, (void*)&Descartes) ) < 0)
			ErrHandling("Descatres hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[1], NULL, __dining_philosophier_o__, (void*)&Voltaire) ) < 0)
			ErrHandling("Voltaire hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[2], NULL, __dining_philosophier_o__, (void*)&Sokrates) ) < 0)
			ErrHandling("Sokrates hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[3], NULL, __dining_philosophier_o__, (void*)&Thales) ) < 0)
			ErrHandling("Thales hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[4], NULL, __dining_philosophier_o__, (void*)&Plato) ) < 0)
			ErrHandling("Plato hate spaghetti.");
		if ( (thrID = pthread_create(&p_thread[5], NULL, __observer__, (void*)&Observer) ) < 0)
			ErrHandling("Plato hate spaghetti.");
		
		pthread_join(p_thread[0], (void**)&status);
		pthread_join(p_thread[1], (void**)&status);
		pthread_join(p_thread[2], (void**)&status);
		pthread_join(p_thread[3], (void**)&status);
		pthread_join(p_thread[4], (void**)&status);
		pthread_join(p_thread[5], (void**)&status);
		
	}
	
	else if (arg == 's') {
		
		int eatingTimes = 500;
		int howManyTheyEat[5] = {0};
		
		while (eatingTimes-->0) {
			printf("\n < eatingTimes %d left >.\n", eatingTimes);
			if (eatingTimes % 5 == 0) {// locked Fork = 0
				
				if ( (thrID = pthread_create(&p_thread[2], NULL, __dining_philosophier_s__, (void*)&Sokrates) ) < 0)
				ErrHandling("someone hate spaghetti.");
				if ( (thrID = pthread_create(&p_thread[4], NULL, __dining_philosophier_s__, (void*)&Plato) ) < 0)
				ErrHandling("someone hate spaghetti.");
				
				howManyTheyEat[2]++;
				howManyTheyEat[4]++;
				
				pthread_join(p_thread[2], (void**)&status);
				pthread_join(p_thread[4], (void**)&status);
				
			}
			else if (eatingTimes % 5 == 1) {// locked Fork = 1
				
				if ( (thrID = pthread_create(&p_thread[3], NULL, __dining_philosophier_s__, (void*)&Thales) ) < 0)
					ErrHandling("someone hate spaghetti.");
				if ( (thrID = pthread_create(&p_thread[0], NULL, __dining_philosophier_s__, (void*)&Descartes) ) < 0)
					ErrHandling("someone hate spaghetti.");
				
				howManyTheyEat[3]++;
				howManyTheyEat[0]++;
				
				pthread_join(p_thread[3], (void**)&status);
				pthread_join(p_thread[0], (void**)&status);
			}
			else if (eatingTimes % 5 == 2) {// locked Fork = 2
				
				if ( (thrID = pthread_create(&p_thread[4], NULL, __dining_philosophier_s__, (void*)&Plato) ) < 0)
					ErrHandling("someone hate spaghetti.");
				if ( (thrID = pthread_create(&p_thread[1], NULL, __dining_philosophier_s__, (void*)&Voltaire) ) < 0)
					ErrHandling("someone hate spaghetti.");
				
				howManyTheyEat[4]++;
				howManyTheyEat[1]++;
				
				pthread_join(p_thread[4], (void**)&status);
				pthread_join(p_thread[1], (void**)&status);
			}
			else if (eatingTimes % 5 == 3) {// locked Fork = 3
				
				if ( (thrID = pthread_create(&p_thread[0], NULL, __dining_philosophier_s__, (void*)&Descartes) ) < 0)
					ErrHandling("someone hate spaghetti.");
				if ( (thrID = pthread_create(&p_thread[2], NULL, __dining_philosophier_s__, (void*)&Sokrates) ) < 0)
					ErrHandling("someone hate spaghetti.");
				
				howManyTheyEat[0]++;
				howManyTheyEat[2]++;
				
				pthread_join(p_thread[0], (void**)&status);
				pthread_join(p_thread[2], (void**)&status);
			}
			else if (eatingTimes % 5 == 4) {// locked Fork = 4
				
				if ( (thrID = pthread_create(&p_thread[1], NULL, __dining_philosophier_s__, (void*)&Voltaire) ) < 0)
					ErrHandling("someone hate spaghetti.");
				if ( (thrID = pthread_create(&p_thread[3], NULL, __dining_philosophier_s__, (void*)&Thales) ) < 0)
					ErrHandling("someone hate spaghetti.");
				
				howManyTheyEat[1]++;
				howManyTheyEat[3]++;
				
				pthread_join(p_thread[1], (void**)&status);
				pthread_join(p_thread[3], (void**)&status);
			}
			printf("--> Descartes : %d , Voltaire : %d, Sokrates : %d, Thales : %d, Plato : %d  times eat Spaghetti.\n",
				   howManyTheyEat[0], howManyTheyEat[1], howManyTheyEat[2], howManyTheyEat[3], howManyTheyEat[4]);
			
		}
	}
	return 0;
}


int main(int argc, char** argv)
{

	if (argc != 2) {
		printf("Usage : %s <Prevention ? No : [f], Set Observer : [o], Set sequent : [s]>\n", argv[0]);
		exit(1);
	}
	
	printf("\n############################################################################\n");
	testSem();
	printf("############################################################################\n\n");
	
	if(dining_philosophier(*argv[1]) < 0) {
		printf("Err : Wrong argument \'%c\'\n", *argv[1]);
		exit(1);
	}
	
	
}

