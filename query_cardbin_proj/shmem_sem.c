#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_SIZE  2048

/*定义联合体*/
union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

/*函数声明*/
int init_sem(int sem_id,int init_value);/*信号量初始化（赋值）函数*/
int del_sem(int sem_id); /*从系统中删除信号量的函数*/
int sem_p(int sem_id); /*P 操作函数*/
int sem_v(int sem_id); /*V 操作函数*/

int main()
{
	pid_t pid;
	int sem_id;//信号量值
	int shmid;/*要映射的共享内存区标识符*/
	char *shm_addr=NULL;
	char buff[20];//定义20字节大小

	sem_id=semget(ftok(".",'a'),1,0666|IPC_CREAT);/*创建一个信号量*/
	init_sem(sem_id,0);//初始化信号量值

	/*创建共享内存*/
	if((shmid=shmget(IPC_PRIVATE,BUFFER_SIZE,0666))<0)
	{
		perror("shmget");
		exit(1);
	}
	else
	{
		printf("Create shared-memory: %d\n",shmid);
	}

	/*显示共享内存情况*/
	system("ipcs -m");
	pid=fork();//创建子进程
	if(pid==-1)
	{
		perror("fork");
		exit(1);
	}
	else if(pid==0) //子进程处理
	{
		/*映射共享内存*/
		if((shm_addr=shmat(shmid,0,0))==(char *)-1)
		{
			perror("Child: shmat");
			exit(1);
		}
		else
		{
			printf("Child:Attach shared-memmory: %p\n",shm_addr);
		}
		system("ipcs -m"); //执行命令：ipcs -m
		/*通过检查在共享内存的头部是否有标志字符串“WROTE”来确认父进程
		已经向共享内存写入有效数据*/	
		printf("Child:Wait for enable data...\n");

		sem_p(sem_id); //P操作
		/*共享内存的有效数据显示*/
		strcpy(buff,shm_addr);
		printf("Child:Shared-memory:%s\n",buff);
		sem_v(sem_id); //V操作
		del_sem(sem_id); //删除信号量

		/*解除共享内存映射*/
		if((shmdt(shm_addr))<0)
		{
			perror("shmdt");
			exit(1);
		}
		else
		{
			printf("Child: Deattach shared-memory\n");
		}
		system("ipcs -m");//报告此时进程间的通信机制状态

		/*删除共享内存*/
		if(shmctl(shmid,IPC_RMID,NULL)==-1)
		{
			perror("Child:shmctl(IPC_RMID)\n");
			exit(1);
		}
		else
		{
			printf("Delete shared-memory\n");
		}

		system("ipcs -m");
	
	}  /*子进程处理程序结束*/
	else  /*父进程处理*/
	{
		/*映射共享内存*/
		if((shm_addr=shmat(shmid,0,0))==(char *)-1)
		{
			perror("Parent: shmat");
			exit(1);
		}
		else
		{
			printf("Parent:Attach shared-memory: %p\n",shm_addr);
		}

		sleep(1);
	
		printf("\nInput some string:\n");
		fgets(buff,BUFFER_SIZE,stdin);
		strncpy(shm_addr,buff,strlen(buff));
		sem_v(sem_id);//V操作
		
		/*解除共享内存映射*/
		if((shmdt(shm_addr))<0)
		{
			perror("Parent:shmdt");
			exit(1);
		}
		else
		{
			printf("Parent:Deattach shared-memory\n");
		}
		system("ipcs -m");
		waitpid(pid,NULL,0);
		printf("Finished\n");
	} /*父进程处理结束*/

	exit(0);
}  /* main() end*/

/*信号量初始化（赋值）函数*/
int init_sem(int sem_id,int init_value)
{
	union semun sem_union;
	sem_union.val=init_value;  /*init_value为初始值*/
	if(semctl(sem_id,0,SETVAL,sem_union)==-1)
	{
		perror("Initialize semaphore");
		return -1;
	}
	return 0;
}

/*从系统中删除信号量的函数*/
int del_sem(int sem_id)
{
	union semun sem_union;
	if(semctl(sem_id,0,IPC_RMID,sem_union)==-1)
	{
		perror("Delete semaphore");
		return -1;
	}
}

/*P 操作函数*/
int sem_p(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num=0;  /*信号量编号，这里单个信号量的编号应该为0*/
	sem_b.sem_op=-1; /*信号量操作，取值为-1表示P操作*/
	sem_b.sem_flg=SEM_UNDO; /*在进程没释放信号量而退出时，系统自动释放该进程中
							未释放的信号量*/
	if(semop(sem_id,&sem_b,1)==-1)	/*进行Ｐ操作*/
	{
		perror("P operation");
		return -1;
	}
	return 0;
}

/*V 操作函数*/
int sem_v(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num=0;  /*信号量编号，这里单个信号量的编号应该为0*/
	sem_b.sem_op=1; /*信号量操作，取值为+1表示V操作*/
	sem_b.sem_flg=SEM_UNDO; /*在进程没释放信号量而退出时，系统自动释放该进程中
							未释放的信号量*/
	if(semop(sem_id,&sem_b,1)==-1)	/*进行V 操作*/
	{
		perror("V operation");
		return -1;
	}
	return 0;
}

