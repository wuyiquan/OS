#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//锁与条件变量的定义
pthread_mutex_t mutexQueueNorth,mutexQueueEast,mutexQueueSouth,mutexQueueWest,mutexFirstNorth,mutexFirstEast,mutexFirstSouth,mutexFirstWest;
pthread_mutex_t mutexNorthState,mutexSouthState,mutexWestState,mutexEastState;
pthread_mutex_t A,B,C,D;
pthread_cond_t condQueueNorth,condQueueEast,condQueueSouth,condQueueWest,condFirstNorth,condFirstEast,condFirstSouth,condFirstWest;

int northState = 0;int southState = 0;int westState = 0;int eastState = 0;//四个路口的状态，1为有车，0为没车
int northCar = 0;int southCar = 0;int westCar = 0;int eastCar = 0;//记录每个方向已经leave的车
int northCarCnt = 0;int southCarCnt = 0;int westCarCnt = 0;int eastCarCnt = 0;//记录每个方向一共有多少车
//定义CAR结构用以记录每辆车的信息
typedef struct CAR{
	char dir;//方向
	int numInAll;//所有车辆中的编号
	int numInDir;//自己方向中的编号
}car;
typedef car* carPointer;
void *checkDead(void *arg)//死锁检测
{
	while(1)
    {
		pthread_mutex_lock(&mutexNorthState);//获取路口状态锁
		pthread_mutex_lock(&mutexSouthState);//获取路口状态锁
		pthread_mutex_lock(&mutexWestState);//获取路口状态锁
		pthread_mutex_lock(&mutexEastState);//获取路口状态锁
        if(northState==1 && southState==1 && westState==1 && eastState==1)//如果四个路口都有车
        {
			printf("DEADLOCK: car jam detected, signalling North to go\n");
			pthread_cond_signal(&condFirstNorth);//让北方车先行
		}
		if (northCar+southCar+westCar+eastCar == *(int*)arg)//如果四个方向车都已经走完，退出该while
			break;
		pthread_mutex_unlock(&mutexNorthState);//释放路口状态锁
		pthread_mutex_unlock(&mutexSouthState);//释放路口状态锁
		pthread_mutex_unlock(&mutexWestState);//释放路口状态锁
		pthread_mutex_unlock(&mutexEastState);//释放路口状态锁
		sleep(1);//每次检测后都暂停一下，降低检测频率
		
	}
	pthread_exit(0);//退出线程
}
void *carN(void *arg) 
{
	//将传来的结构指针赋值给一个结构指针
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueNorth);
	while (cP->numInDir != northCar)//判断当前路口应该通行车辆编号是否是该车编号
		pthread_cond_wait(&condQueueNorth, &mutexQueueNorth);
	pthread_mutex_unlock(&mutexQueueNorth);

	//Arrives
	pthread_mutex_lock(&mutexNorthState);
	printf("car %d from North arrives at crossing\n", cP->numInAll);
	northState = 1;//改变路口状态
	sleep(1);
	pthread_mutex_unlock(&mutexNorthState);

	//Wait right to go
	pthread_mutex_lock(&mutexWestState);
	if (westState == 1)
		pthread_cond_wait(&condFirstNorth,&mutexWestState);

	pthread_mutex_lock(&mutexQueueWest);//通知右边的后一辆
	pthread_cond_broadcast(&condQueueWest);
	pthread_mutex_unlock(&mutexQueueWest);

	//Pass
	pthread_mutex_lock(&C);
	pthread_mutex_lock(&D);
	pthread_mutex_unlock(&mutexWestState);

	//Left
	pthread_mutex_lock(&mutexNorthState);
	northState = 0;//改变路口状态
	northCar++;	//该方向已经leave的车辆加一
	pthread_mutex_unlock(&C);
	pthread_mutex_unlock(&D);
	printf("car %d from North leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexNorthState);

	//Signal	
	pthread_mutex_lock(&mutexFirstEast);
	pthread_cond_signal(&condFirstEast);
	pthread_mutex_unlock(&mutexFirstEast);

	pthread_mutex_lock(&mutexEastState);//判断左边路口有没有车，没有的话自己通知后面的车
	if (eastState == 0)
	{
		pthread_mutex_lock(&mutexQueueNorth);
		pthread_cond_broadcast(&condQueueNorth);
		pthread_mutex_unlock(&mutexQueueNorth);
	}
	pthread_mutex_unlock(&mutexEastState);

	pthread_exit(0);
}
void *carS(void *arg) 
{
	//将传来的结构指针赋值给一个结构指针
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueSouth);
	while (cP->numInDir != southCar)//判断当前路口应该通行车辆编号是否是该车编号
		pthread_cond_wait(&condQueueSouth, &mutexQueueSouth);
	pthread_mutex_unlock(&mutexQueueSouth);
	
	//Arrives
	pthread_mutex_lock(&mutexSouthState);
	printf("car %d from South arrives at crossing\n", cP->numInAll);
	southState = 1;//改变路口状态
	sleep(1);
	pthread_mutex_unlock(&mutexSouthState);
	
	//Wait right to go
	pthread_mutex_lock(&mutexEastState);
	if (eastState == 1)
		pthread_cond_wait(&condFirstSouth,&mutexEastState);

	pthread_mutex_lock(&mutexQueueEast);//通知右边的下一辆
	pthread_cond_broadcast(&condQueueEast);
	pthread_mutex_unlock(&mutexQueueEast);
	
	//Pass
	pthread_mutex_lock(&A);
	pthread_mutex_lock(&B);
	pthread_mutex_unlock(&mutexEastState);	

	//Left
	pthread_mutex_lock(&mutexSouthState);
	southState = 0;//改变路口状态
	southCar++;//该方向已经leave的车辆加一
	pthread_mutex_unlock(&A);
	pthread_mutex_unlock(&B);
	printf("car %d from South leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexSouthState);

	//Signal
	pthread_mutex_lock(&mutexFirstWest);
	pthread_cond_signal(&condFirstWest);
	pthread_mutex_unlock(&mutexFirstWest);
	
	pthread_mutex_lock(&mutexWestState);//如果左边没车，自己通知下一辆
	if (westState == 0)
	{
		pthread_mutex_lock(&mutexQueueSouth);
		pthread_cond_broadcast(&condQueueSouth);
		pthread_mutex_unlock(&mutexQueueSouth);
	}
	pthread_mutex_unlock(&mutexWestState);
	
	pthread_exit(0);
}
void *carW(void *arg) 
{
	//将传来的结构指针赋值给一个结构指针
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueWest);
	while (cP->numInDir != westCar)//判断当前路口应该通行车辆编号是否是该车编号
		pthread_cond_wait(&condQueueWest, &mutexQueueWest);
	pthread_mutex_unlock(&mutexQueueWest);
	
	//Arrives
	pthread_mutex_lock(&mutexWestState);
	printf("car %d from West arrives at crossing\n", cP->numInAll);
	westState = 1;//改变路口状态
	sleep(1);
	pthread_mutex_unlock(&mutexWestState);
	
	//Wait right to go
	pthread_mutex_lock(&mutexSouthState);
	if (southState == 1)
		pthread_cond_wait(&condFirstWest,&mutexSouthState);

	pthread_mutex_lock(&mutexQueueSouth);//通知右边的下一辆
	pthread_cond_broadcast(&condQueueSouth);
	pthread_mutex_unlock(&mutexQueueSouth);

	//Pass
	pthread_mutex_lock(&D);
	pthread_mutex_lock(&A);
	pthread_mutex_unlock(&mutexSouthState);

	//Left
	pthread_mutex_lock(&mutexWestState);
	westState = 0;//改变路口状态
	westCar++;//该方向已经leave的车辆加一
	pthread_mutex_unlock(&D);
	pthread_mutex_unlock(&A);
	printf("car %d from West leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexWestState);

	//Signal	
	pthread_mutex_lock(&mutexFirstNorth);
	pthread_cond_signal(&condFirstNorth);
	pthread_mutex_unlock(&mutexFirstNorth);

	pthread_mutex_lock(&mutexNorthState);
	if (northState == 0)//左边已经没有车，只能自己通知下一辆
	{
		pthread_mutex_lock(&mutexQueueWest);
		pthread_cond_broadcast(&condQueueWest);
		pthread_mutex_unlock(&mutexQueueWest);
	}
	pthread_mutex_unlock(&mutexNorthState);
	
	pthread_exit(0);
}
void *carE(void *arg) 
{
	//将传来的结构指针赋值给一个结构指针
	carPointer cP = (carPointer)malloc(sizeof(car));
	cP->numInAll = ((carPointer)arg)->numInAll;
	cP->numInDir = ((carPointer)arg)->numInDir;
	cP->dir = ((carPointer)arg)->dir;

	//Wait to corssing
	pthread_mutex_lock(&mutexQueueEast);
	while (cP->numInDir != eastCar)//判断当前路口应该通行车辆编号是否是该车编号
		pthread_cond_wait(&condQueueEast, &mutexQueueEast);
	pthread_mutex_unlock(&mutexQueueEast);
	
	//Arrives
	pthread_mutex_lock(&mutexEastState);
	printf("car %d from East arrives at crossing\n", cP->numInAll);
	eastState = 1;//改变路口状态
	sleep(1);
	pthread_mutex_unlock(&mutexEastState);

	//Wait right to go
	pthread_mutex_lock(&mutexNorthState);
	if (northState == 1)
		pthread_cond_wait(&condFirstEast,&mutexNorthState);
	
	pthread_mutex_lock(&mutexQueueNorth);//通知右边的下一辆
	pthread_cond_broadcast(&condQueueNorth);
	pthread_mutex_unlock(&mutexQueueNorth);

	//Pass
	pthread_mutex_lock(&B);
	pthread_mutex_lock(&C);
	pthread_mutex_unlock(&mutexNorthState);

	//Left
	pthread_mutex_lock(&mutexEastState);
	eastState = 0;//改变路口状态
	eastCar++;//该方向已经leave的车辆加一
	pthread_mutex_unlock(&B);
	pthread_mutex_unlock(&C);
	printf("car %d from East leaving crossing.\n", cP->numInAll);
	pthread_mutex_unlock(&mutexEastState);

	//Signal	
	pthread_mutex_lock(&mutexFirstSouth);
	pthread_cond_signal(&condFirstSouth);
	pthread_mutex_unlock(&mutexFirstSouth);

	pthread_mutex_lock(&mutexSouthState);//如果左边已经没有车，就自己通知下一辆
	if (southState == 0)
	{
		pthread_mutex_lock(&mutexQueueEast);
		pthread_cond_broadcast(&condQueueEast);
		pthread_mutex_unlock(&mutexQueueEast);
	}
	pthread_mutex_unlock(&mutexSouthState);
	
	pthread_exit(0);
}

int main() 
{
char s[100];//输入数组
pthread_t thid[100];//线程数组
pthread_t dead;//死锁检测线程
car cars[100];//车辆数组

pthread_mutex_init(&mutexQueueNorth, NULL);//初始化
pthread_mutex_init(&mutexQueueEast, NULL);//初始化
pthread_mutex_init(&mutexQueueSouth, NULL);//初始化
pthread_mutex_init(&mutexQueueWest, NULL);//初始化

pthread_cond_init(&condQueueNorth, NULL);//初始化
pthread_cond_init(&condQueueEast, NULL);//初始化
pthread_cond_init(&condQueueSouth, NULL);//初始化
pthread_cond_init(&condQueueWest, NULL);//初始化

pthread_mutex_init(&mutexFirstNorth, NULL);//初始化
pthread_mutex_init(&mutexFirstEast, NULL);//初始化
pthread_mutex_init(&mutexFirstSouth, NULL);//初始化
pthread_mutex_init(&mutexFirstWest, NULL);//初始化

pthread_cond_init(&condFirstNorth, NULL);//初始化
pthread_cond_init(&condFirstEast, NULL);//初始化
pthread_cond_init(&condFirstSouth, NULL);//初始化
pthread_cond_init(&condFirstWest, NULL);//初始化

pthread_mutex_init(&mutexNorthState, NULL);//初始化
pthread_mutex_init(&mutexSouthState, NULL);//初始化
pthread_mutex_init(&mutexWestState, NULL);//初始化
pthread_mutex_init(&mutexEastState, NULL);//初始化

pthread_mutex_init(&A,NULL);//初始化
pthread_mutex_init(&B,NULL);//初始化
pthread_mutex_init(&C,NULL);//初始化
pthread_mutex_init(&D,NULL);//初始化

printf("Input the sequence:\n");

scanf("%s",s);
size_t len = strlen(s);
for (int i=0; i<len; i++)
{
	switch (s[i])
	{
		//根据方向给车辆赋予相应信息并创建线程
		case 'w':cars[i].numInDir = westCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carW, &(cars[i]));break;
		case 'e':cars[i].numInDir = eastCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carE, &(cars[i]));break;
		case 'n':cars[i].numInDir = northCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carN, &(cars[i]));break;
		case 's':cars[i].numInDir = southCarCnt++; cars[i].numInAll = i+1; pthread_create(&thid[i], NULL, (void *) carS, &(cars[i]));break;
	}
}
pthread_create(&dead,NULL,checkDead,&len);//创建死锁检测线程

pthread_exit(0);
return 0;
}
