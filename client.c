#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_IN "./source.txt"
#define FILE_LOG "./log.txt"
#define IPC_ID_MSG 1
#define IPC_ID_SHM 2
#define IPC_ID_SEM 3
#define MSG_MAX 4068 

struct mult
{
	int str1;
	int stlb2;
	int wh;         //width or heigh
	int *p1, *p2;   //pointers to the matrices
	int *res;       //result of calculation
};


struct msgbuf_str
{
	long mtype;
	char msg[MSG_MAX];
};


struct msgbuf_matr
{
	long mtype;
	struct
	{
		int width;
		int height1;
		int height2;
		int matrices[MSG_MAX];
	} info;
};


void *multiplication(void *num0);
int char_to_int(unsigned char *x);
void print_matrix(int *p, int h, int w);
void my_strncpy(char *dest, char *src, int n);
void er_hand(const char *msg);
void write_in_file(int fd, int id, const char *msg);
void work_with_log(int fd, int sem_id, struct sembuf my_sem_buf, int client_id, const char *msg);

int main()
{
	const int one=sizeof(int);
	int client_id, client_num;

	key_t key_sem=ftok(FILE_IN, IPC_ID_SEM);	
	key_t key_msg=ftok(FILE_IN, IPC_ID_MSG);
	key_t key_shm=ftok(FILE_IN, IPC_ID_SHM);
	if ((key_sem<0)||(key_msg<0)||(key_shm<0)) er_hand("Can't generate a key");


	(void)umask(0);
	int er_sem, er_rd, fd;
	char *cl_id;
	int *id;
	
	struct sembuf my_sem_buf;
	my_sem_buf.sem_num=0;
	my_sem_buf.sem_flg=0;

	int sem_id=semget(key_sem, 1, 0666|IPC_CREAT|IPC_EXCL);
	if ((sem_id<0)&&(errno!=EEXIST))
	{
		er_hand("Can't create semapfore");
	}
	
	(void)umask(0);
	// AP: зачем вам shared memory?
	int shm_id=shmget(key_shm, one, 0666|IPC_CREAT);
	if (shm_id<0) er_hand("Can't create shared memory");
	
	cl_id=shmat(shm_id, NULL, 0);
	if (cl_id==(char*)(-1)) er_hand("Can't attach shared memory");
	id=(int*)cl_id;
	
	if (sem_id>0) //the first client
	{
		// semaphore has been created, value=0 (man)
		//critical section
		client_num=client_id=1;
		(*id)=5;

		(void)umask(0);
		fd=open(FILE_LOG, O_WRONLY|O_CREAT|O_APPEND|O_TRUNC,0666);
		if (fd<0) er_hand("Can't open file");

		write_in_file(fd, client_id, "I have begun working and created a semaphore\n");			
		//end of critical section
		my_sem_buf.sem_op=1;
		er_sem=semop(sem_id, &my_sem_buf, 1);
		if (er_sem<0) er_hand("Can't operate with semaphore 1");
	}
	if ((sem_id<0)&&(errno==EEXIST)) //not the first client
	{
		sem_id=semget(key_sem, 1, 0666);
		if (sem_id<0) er_hand("Can't get access to semaphore");

		my_sem_buf.sem_op=-1;
		er_sem=semop(sem_id, &my_sem_buf, 1);
		if (er_sem<0) er_hand("Can't operate with semaphore 2a");

		//critical section
		client_id=*id;
		(*id)+=4;
		client_num=1+client_id/4;

		(void)umask(0);
		fd=open(FILE_LOG, O_WRONLY|O_APPEND);
		if (fd<0) er_hand("Can't open file");

		write_in_file(fd, client_num, "I have begun working\n");
		//end of critical section
		my_sem_buf.sem_op=1;
		er_sem=semop(sem_id, &my_sem_buf, 1);		
		if (er_sem<0) er_hand("Can't operate with semaphore 2b");
	}

	er_rd=shmdt(cl_id);
	if (er_rd<0) printf("Can't detach shared memory\n errno=%d\n",errno);

	(void)umask(0);
	int msg_id=msgget(key_msg, 0666|IPC_CREAT);
	if (msg_id<0) er_hand("Can't create message queue");

	struct msgbuf_str buffer[1];
	struct msgbuf_matr buf2[1];

	char hello[]="hello";
	strcpy(buffer[0].msg, hello);
	buffer[0].mtype=client_id;

	er_rd=msgsnd(msg_id, &buffer, strlen(hello)+1,0);
	if (er_rd<0) er_hand("Can't send a message");

	work_with_log(fd, sem_id, my_sem_buf, client_num, "I have sent hello message to server\n");
	
	// AP: аналогично сереверу - а если длинная матрица? нужно уметь получать любой размер
	er_rd = msgrcv(msg_id, &buf2, MSG_MAX, client_id+1, 0);
	if (er_rd<0) er_hand("Can't receive message from server");
	
	work_with_log(fd, sem_id, my_sem_buf, client_num, "I have received work from server\n");

	int h1=buf2[0].info.height1;
	int h2=buf2[0].info.height2;
	int w=buf2[0].info.width;
	int *m;
	m=buf2[0].info.matrices;	

	// MULTIPLICATION

	struct mult job;
	job.p1 = m;
	job.p2 = m + h1*w;
	job.wh = w;
	int *result;
	result=(int*)malloc(h1*h2*one);
	
	int i,j;
	for (i=0; i<h1; i++)
	{	
		job.str1=i;
		for (j=0; j<h2; j++)
		{
			job.res = result+i*h2+j;
			job.stlb2 = j;
			multiplication(&job);
		}
	}

	work_with_log(fd, sem_id, my_sem_buf, client_num, "Hurray! I have done my work!\n");

	my_strncpy(buffer[0].msg,(char*)(result), h1*h2*one);
	buffer[0].mtype=client_id+2;

	// AP: аналогично - нужно уметь отправлять результат любой длины
	er_rd=msgsnd(msg_id, &buffer, h1*h2*one, 0);
	if (er_rd<0) er_hand("Can't send a message with work");

	free(result);

	work_with_log(fd, sem_id, my_sem_buf, client_num, "I have sent my work to server\n");

	er_rd=msgrcv(msg_id, &buffer, MSG_MAX, client_id+3, 0);
	if (er_rd<0) printf("Can't receive last message from server\n");

	if (strstr(buffer[0].msg,"last client")>0)
	{
		if ((semctl(sem_id,0,IPC_RMID,NULL)<0)||
		(shmctl(shm_id,IPC_RMID, NULL)<0)||
		(msgctl(msg_id,IPC_RMID,NULL)<0)) er_hand("Can't remove IPC");
		else printf("All IPCs have been deleted successfully\n");	
	}
	else
	{
		buffer[0].mtype=client_id;
		er_rd=msgsnd(msg_id, &buffer, 0, 0);
		if (er_rd<0) er_hand("Can't send to server");
	}
	printf("Client %d finished it's work successfully\n",client_num);
	return 0;
}

void er_hand(const char *msg)
{
	printf("%s\n",msg);
	printf("errno=%d\n",errno);
	exit(-1);
};

void write_in_file(int fd, int id, const char *msg)
{
	int er;
	int pos=0;
	int size,k;
	
	int i=id;
	int j=0;
	char *x;
	while (i>0)
	{
		i/=10;
		j++;
	}
	k=j;
	size=j+strlen(msg)+1;
	x=(char*)malloc(size);
	i=id;
	x[j]=' ';
	j--;
	while (i>0)
	{
		x[j]=(i%10)+48;
		i/=10;
		j--;
	}
	for (i=k+1; i<size; i++) x[i]=msg[i-k-1];		
	do 
	{
		er=write(fd,x+pos,size-pos);
		pos+=er;		
	} while (er>0);
	if (er<0) er_hand("Can't write in file");
	free(x);
};

void work_with_log(int fd, int sem_id, struct sembuf my_sem_buf, int client_id, const char *msg)
{
	int er_sem;

	my_sem_buf.sem_op=-1;
	er_sem=semop(sem_id, &my_sem_buf, 1);
	if (er_sem<0)
	{
		printf("%d\n",client_id);
//		er_hand("Can't operate with semaphore");
		er_hand(msg);
	}

	write_in_file(fd, client_id, msg);

	my_sem_buf.sem_op=1;
	er_sem=semop(sem_id, &my_sem_buf, 1);		
	if (er_sem<0)// er_hand("Can't operate with semaphore");
		er_hand(msg);
};

void my_strncpy(char *dest, char *src, int n)
{
	int i;
	for (i=0; i<n; i++) *(dest+i)=*(src+i);
};

void *multiplication(void *num0)
{
	int i;
	int res=0;
	int one=sizeof(int);
	int *str, *stlb;
	struct mult *num;
	num=(struct mult *)num0;
	str = (*num).p1 + (*num).wh * (*num).str1;
	stlb = (*num).p2 + (*num).wh * (*num).stlb2;

	for (i=0; i<(*num).wh; i++)
	{
		res += *(str+i) * *(stlb+i);
	} 
	*(num[0].res)=res;
	
	return NULL;
};

void print_matrix(int *p, int h, int w)
{
	int i,j,k;
	int one=sizeof(int);
	int buf;
	for (i=0; i<h; i++)
	{
		for (j=0; j<w; j++)
		{
			buf=*(p+i*w+j);			
			printf("%d ", buf);
		}
		printf("\n");
	}
	printf("\n");
};

int char_to_int(unsigned char *x)
{
	int one=sizeof(int);
	int buf=0;
	int log2=1;
	int k;
	for (k=0; k<one; k++)
	{
		buf+=(*(x+k))*log2;
		log2*=256;
	}
	return buf;
};


