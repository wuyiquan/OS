#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
pthread_mutex_t mutexQueueNorth,mutexQueueEast,mutexQueueSouth,mutexQueueWest,mutexFirstNorth,mutexFirstEast,mutexFirstSouth,mutexFirstWest;
pthread_mutex_t mutexNorthState,mutexSouthState,mutexWestState,mutexEastState;
pthread_mutex_t A,B,C,D;
pthread_cond_t condQueueNorth,condQueueEast,condQueueSouth,condQueueWest,condFirstNorth,condFirstEast,condFirstSouth,condFirstWest;
int num=0;
int northState = 0;int southState = 0;int westState = 0;int eastState = 0;
int northCar = 0;int southCar = 0;int westCar = 0;int eastCar = 0;
int northCarCnt = 0;int southCarCnt = 0;int westCarCnt = 0;int eastCarCnt = 0;
typedef struct CAR{
	char dir;
	int numInAll;
	int numInDir;
}car;
typedef car* carPointer;
void *checkDead(void *arg)
{
	while(1)
    {
		sleep(1);
		pthread_mutex_lock(&mutexNorthState);
		pthread_mutex_lock(&mutexSouthState);
		pthread_mutex_lock(&mutexWestState);
		pthread_mutex_lock(&mutexEastState);
        if(northState==1 && southState==1 && westState==1 && eastState==1)
        {
            printf("DEADLOCK: car jam detected, signalling North to go\n");
            pthread_cond_signal(&condFirstNorth);
		}
		if (northCar+southCar+westCar+eastCar == *(int*)arg)
			break;
		pthread_mutex_unlock(&mutexNorthState);
		pthread_mutex_unlock(&mutexSouthState);
		pthread_mutex_unlock(&mutexWestState);
		pthread_mutex_unlock(&mutexEastState);
		
	}
	pthread_exit(0);
}
void *carN(void *arg) 
{
	
	//pthread_cleanup_push(pthread_mutex_unlock, &mutex);
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueNorth);
	while (cP->numInDir != northCar)
		pthread_cond_wait(&condQueueNorth, &mutexQueueNorth);

	//Arrives
	pthread_mutex_lock(&mutexNorthState);
	printf("car %d from North arrives at crossing\n", cP->numInAll);
	northState = 1;
	sleep(1);
	pthread_mutex_unlock(&mutexNorthState);
	//printf("Nhere1 %d.\n", cP->numInAll);
	//Wait right to go
	pthread_mutex_lock(&mutexWestState);//Only one will wait, so it's safe to not use mutexFirstNorth.
	if (westState == 1)
		pthread_cond_wait(&condFirstNorth,&mutexWestState);
	pthread_mutex_lock(&mutexNorthState);
	//printf("Nhere2 %d.\n", cP->numInAll);
	//Pass
	pthread_mutex_lock(&C);
	pthread_mutex_lock(&D);
	pthread_mutex_unlock(&mutexWestState);
	//printf("Nhere3 %d.\n", cP->numInAll);
	//Left
	pthread_mutex_lock(&mutexFirstEast);
	northState = 0;
	northCar++;
	pthread_cond_signal(&condFirstEast);
	
	pthread_mutex_unlock(&mutexFirstEast);
	pthread_mutex_unlock(&C);
	pthread_mutex_unlock(&D);
	printf("car %d from North leaving crossing.\n", cP->numInAll);
	
	pthread_mutex_unlock(&mutexNorthState);
	sleep(1);
	pthread_cond_broadcast(&condQueueNorth);
	pthread_mutex_unlock(&mutexQueueNorth);
	
	//printf("Nhere7 %d.\n", cP->numInAll);
	//pthread_cleanup_pop(0);
	pthread_exit(0);
}
void *carS(void *arg) 
{
	
	//pthread_cleanup_push(pthread_mutex_unlock, &mutex);
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueSouth);
	while (cP->numInDir != southCar)
		pthread_cond_wait(&condQueueSouth, &mutexQueueSouth);

	
	//Arrives
	pthread_mutex_lock(&mutexSouthState);
	printf("car %d from South arrives at crossing\n", cP->numInAll);
	southState = 1;
	sleep(1);
	pthread_mutex_unlock(&mutexSouthState);
	
	//Wait right to go
	pthread_mutex_lock(&mutexEastState);
	if (eastState == 1)
		pthread_cond_wait(&condFirstSouth,&mutexEastState);
	
	pthread_mutex_lock(&mutexSouthState);
	//Pass
	pthread_mutex_lock(&A);
	pthread_mutex_lock(&B);
	//sleep(1);
	pthread_mutex_unlock(&mutexEastState);

	//Left
	

	pthread_mutex_lock(&mutexFirstWest);

	southState = 0;
	southCar++;
	pthread_cond_signal(&condFirstWest);
	
	pthread_mutex_unlock(&mutexFirstWest);
	pthread_mutex_unlock(&A);
	pthread_mutex_unlock(&B);
	printf("car %d from South leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexSouthState);
	sleep(1);
	pthread_cond_broadcast(&condQueueSouth);
	pthread_mutex_unlock(&mutexQueueSouth);
	
	
	//pthread_cleanup_pop(0);
	pthread_exit(0);
}
void *carW(void *arg) 
{
	
	//pthread_cleanup_push(pthread_mutex_unlock, &mutex);
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueWest);
	while (cP->numInDir != westCar)
		pthread_cond_wait(&condQueueWest, &mutexQueueWest);

	
	//Arrives
	pthread_mutex_lock(&mutexWestState);
	printf("car %d from West arrives at crossing\n", cP->numInAll);
	westState = 1;
	sleep(1);
	pthread_mutex_unlock(&mutexWestState);
	
	//Wait right to go
	pthread_mutex_lock(&mutexSouthState);
	if (southState == 1)
		pthread_cond_wait(&condFirstWest,&mutexSouthState);
	

	//Pass
	pthread_mutex_lock(&mutexWestState);
	pthread_mutex_lock(&D);
	pthread_mutex_lock(&A);
	//sleep(1);
	pthread_mutex_unlock(&mutexSouthState);

	//Left
	
	pthread_mutex_lock(&mutexFirstNorth);

	westState = 0;
	westCar++;
	pthread_cond_signal(&condFirstNorth);
	
	pthread_mutex_unlock(&mutexFirstNorth);
	pthread_mutex_unlock(&D);
	pthread_mutex_unlock(&A);
	printf("car %d from West leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexWestState);
	sleep(1);
	pthread_cond_broadcast(&condQueueWest);
	pthread_mutex_unlock(&mutexQueueWest);
	
	
	//pthread_cleanup_pop(0);
	pthread_exit(0);
}
void *carE(void *arg) 
{
	
	//pthread_cleanup_push(pthread_mutex_unlock, &mutex);
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;


	//Wait to corssing
	pthread_mutex_lock(&mutexQueueEast);
	while (cP->numInDir != eastCar)
		pthread_cond_wait(&condQueueEast, &mutexQueueEast);
	
	//Arrives
	pthread_mutex_lock(&mutexEastState);
	printf("car %d from East arrives at crossing\n", cP->numInAll);
	eastState = 1;
	sleep(1);
	pthread_mutex_unlock(&mutexEastState);
	//printf("Ehere1 %d.\n", cP->numInAll);

	//Wait right to go
	pthread_mutex_lock(&mutexNorthState);
	//printf("Ehere2 %d.\n", cP->numInAll);
	if (northState == 1)
		pthread_cond_wait(&condFirstEast,&mutexNorthState);
	//printf("Ehere3 %d.\n", cP->numInAll);

	//Pass
	pthread_mutex_lock(&mutexEastState);
	pthread_mutex_lock(&B);
	//printf("Efuckhere4 %d.\n", cP->numInAll);
	pthread_mutex_lock(&C);
	//printf("Ehere5 %d.\n", cP->numInAll);
	//sleep(1);
	pthread_mutex_unlock(&mutexNorthState);
	//printf("Ehere6 %d.\n", cP->numInAll);
	//Left
	
	pthread_mutex_lock(&mutexFirstSouth);

	eastState = 0;
	eastCar++;
	pthread_cond_signal(&condFirstSouth);
	
	pthread_mutex_unlock(&mutexFirstSouth);
	pthread_mutex_unlock(&B);
	pthread_mutex_unlock(&C);
	printf("car %d from East leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexEastState);
	sleep(1);
	pthread_cond_broadcast(&condQueueEast);
	pthread_mutex_unlock(&mutexQueueEast);
	
	

	//pthread_cleanup_pop(0);
	pthread_exit(0);
}

int main() 
{
char s[100];	
pthread_t thid[100];
pthread_t dead;
car cars[100];

pthread_mutex_init(&mutexQueueNorth, NULL);
pthread_mutex_init(&mutexQueueEast, NULL);
pthread_mutex_init(&mutexQueueSouth, NULL);
pthread_mutex_init(&mutexQueueWest, NULL);

pthread_cond_init(&condQueueNorth, NULL);
pthread_cond_init(&condQueueEast, NULL);
pthread_cond_init(&condQueueSouth, NULL);
pthread_cond_init(&condQueueWest, NULL);

pthread_mutex_init(&mutexFirstNorth, NULL);
pthread_mutex_init(&mutexFirstEast, NULL);
pthread_mutex_init(&mutexFirstSouth, NULL);
pthread_mutex_init(&mutexFirstWest, NULL);

pthread_cond_init(&condFirstNorth, NULL);
pthread_cond_init(&condFirstEast, NULL);
pthread_cond_init(&condFirstSouth, NULL);
pthread_cond_init(&condFirstWest, NULL);

pthread_mutex_init(&mutexNorthState, NULL);
pthread_mutex_init(&mutexSouthState, NULL);
pthread_mutex_init(&mutexWestState, NULL);
pthread_mutex_init(&mutexEastState, NULL);

pthread_mutex_init(&A,NULL);
pthread_mutex_init(&B,NULL);
pthread_mutex_init(&C,NULL);
pthread_mutex_init(&D,NULL);

printf("Input the sequence:\n");

scanf("%s",s);
size_t len = strlen(s);

for (int i=0; i<len; i++)
{
	switch (s[i])
	{
		case 'w':cars[i].numInDir = westCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carW, &(cars[i]));break;
		case 'e':cars[i].numInDir = eastCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carE, &(cars[i]));break;
		case 'n':cars[i].numInDir = northCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carN, &(cars[i]));break;
		case 's':cars[i].numInDir = southCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carS, &(cars[i]));break;
	}
}
pthread_create(&dead,NULL,checkDead,&len);

pthread_exit(0);
return 0;

}
