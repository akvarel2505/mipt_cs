#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <errno.h>
#include <pthread.h>

#define FILE_OUT "./result.txt"
#define FILE_IN "./source.txt"
#define IPC_ID_MSG 1
#define MSG_MAX 32   //4068
#define HELLO_MSG "hello"
#define BYE_MSG "bye!"
#define LAST_BYE_MSG "bye, last client!"

struct mult
{
	int str1;
	int stlb2;
	int wh;        		 //width (height for second matrix)
	int *p1, *p2;  	 //pointers to the matrices
	int *res;      //result of calculation
};

struct thread_arg
{
	int i; //determines client's ID
	int msg_q_id;
	int w;
	int h1;
	int h2;
	int n,user;
	int *res,*m1,*m2;
};

struct msgbuf_matr
{
	long mtype;
	struct
	{
		int width;
		int height1;
		int height2;
		char matrices[MSG_MAX];
	} info;
};

struct msgbuf_str
{
	long mtype;
	char msg[MSG_MAX];
};

void *multiplication(void *num0);
void print_matrix(int *p, int h, int w); 
void receive_matrix(int *m0, int fd, int h, int w);
void my_strncpy(char *dest, char *src, int n);
void err_hand(const char *msg);
void *routine(void *my_th_arg);
void matrix_in_file(int fd, int *m, int h, int w);

int main(int argc, char **argv)
{
	// Receiving the information about matrices
	
	int fd=open(FILE_IN, O_RDONLY);
	if (fd<0) err_hand("Can't open file for reading");
		
	int a[3];
	int i,j,er_rd;
	const int one=sizeof(int);

	//receiving the parameters of matrices
	for (i=0; i<3; i++) receive_matrix(a+i,fd,1,1);
	
	const int h1=a[0]; 
	const int h2=a[2];
	const int w=a[1];
	int s1=h1*w;
	int s2=h2*w;
	int *m1,*m2;

	m1=(int*)malloc(s1*one);
	m2=(int*)malloc(s2*one);

	receive_matrix(m1,fd,h1,w);
	receive_matrix(m2,fd,h2,w);

	er_rd=close(fd);
	if (er_rd<0) printf("Can't close file\n");
	
	// MULTIPLICATION

	// Creating message queues

	key_t key=ftok(FILE_IN, IPC_ID_MSG);
	if (key<0) err_hand("Can't generate a key for message queue");
	int msg_q_id=msgget(key,0666|IPC_CREAT);
	if (msg_q_id<0) err_hand("Can't create message queue");

	const int n=atoi(argv[1]);    //number of clients
	int user, much_work, max_work;
	int last_user=0;
	if (n>=h1)
	{
		much_work=0;
		user=1;	
		max_work=w*(h2+1)*one;
	}
	else
	{
		much_work=1;
		user=h1/n; //number of lines in first matrix for usual (not last) client 
		last_user=user+h1%n;
		max_work = (last_user+h2)*one; //number of bytes in greatest message
	}

//	if (max_work>MSG_MAX) err_hand("Too large matrices");

	const int size_res=h1*h2;

	int *result2;
	result2=(int*)malloc(size_res*one);

	struct thread_arg my_th_arg[n];
	pthread_t my_threads[n];

	for (i=0; i<n; i++)
	{
		printf("Creating thread for client %d...\n",i+1);
		my_th_arg[i].msg_q_id=msg_q_id;
		my_th_arg[i].i=i;
		my_th_arg[i].m1=m1;
		my_th_arg[i].m2=m2;
		my_th_arg[i].w=w;
		my_th_arg[i].h2=h2;
		my_th_arg[i].n=n;
		my_th_arg[i].user=user;

		if ((much_work==1)||(i<h1)) my_th_arg[i].res=result2+i*user*h2;
		else my_th_arg[i].res=result2;

		if ((much_work==1)&&(i==(n-1))) my_th_arg[i].h1=last_user;
		if ((much_work)&&(i!=(n-1))) my_th_arg[i].h1=user;
		if ((much_work==0)&&(i<h1)) my_th_arg[i].h1=1;
		if ((much_work==0)&&(i>=h1)) my_th_arg[i].h1=0;

		er_rd=pthread_create(my_threads+i, NULL, routine, my_th_arg+i);
		if (er_rd!=0) err_hand("Can't create a thread");	
	}

	for (i=0; i<n; i++)
	{
		er_rd=pthread_join(my_threads[i],NULL);
		if (er_rd!=0) err_hand("Can't join a thread\n");
		printf("Thread of client %d was joined successfully\n",i+1);
	}
//последний клиент удаляет все ресурсы IPC. Это должно произойти обязательно после того, как все клиенты закончат работу и с очередями сообщений, и с файлом log.txt. Поэтому сообщение "bye" последнему клиенту отправляем тогда, когда работа всех остальных с IPC точно уж завершилась.
 
	struct msgbuf_str buffer[1];
	buffer[0].mtype=4*n;
	strcpy(buffer[0].msg, LAST_BYE_MSG);
	int sz=strlen(LAST_BYE_MSG);
	er_rd = msgsnd(msg_q_id, &buffer, sz+1, 0);
	if (er_rd<0) err_hand("Can't send bye to last client");	
	
	free(m1);
	free(m2);

	(void)umask(0);
	fd=open(FILE_OUT, O_WRONLY|O_CREAT|O_TRUNC);
	if (fd<0) err_hand("Can't open file");
	matrix_in_file(fd,result2, h1, h2);
	er_rd=close(fd);
	if (er_rd<0) err_hand("Can't close file");

	free(result2);

	printf("Goodbye!!!\n");

	return 0;
}

void *routine(void *my_th_arg)
{
	struct thread_arg *x;
	x=(struct thread_arg*)my_th_arg;

	int i=x[0].i;
	int msg_q_id=x[0].msg_q_id;
	int w=x[0].w;
	int h1=x[0].h1;
	int h2=x[0].h2;
	int n=x[0].n;
	int user=x[0].user;
	int *res,*m1,*m2;
	res=x[0].res;
	m1=x[0].m1;
	m2=x[0].m2;	

	struct msgbuf_str buffer[1];
	struct msgbuf_matr buf2[1];
	int one=sizeof(int);
	int sz;
	int s2=h2*w*one;
	int work=h1*w*one+s2;
	if (h1==0) work=0;

	int er_rd=msgrcv(msg_q_id, &buffer, MSG_MAX, 4*i+1, 0);
	if (er_rd<0)
	{
		printf("Can't receive message from client No %d\n",i);
		exit(-1);
	}

	buf2[0].mtype=4*i+2;
	buf2[0].info.width = w;
	buf2[0].info.height1 = h1;
	buf2[0].info.height2 = h2;

	int info_to_send=w*one*(h1+h2);
	char *matr_buf;
	matr_buf=(char*)malloc(info_to_send);
	my_strncpy(matr_buf, (char*)(m1 + user*i*w), h1*w*one);
	my_strncpy(matr_buf + h1*w*one, (char*)(m2), s2);

	
	int pos=0;
	int need;
	while (pos<info_to_send)
	{
		if (work>0)
		{
			need=info_to_send-pos;
			if (need>MSG_MAX) need=MSG_MAX;
			my_strncpy((char*)buf2[0].info.matrices, matr_buf+pos, need);
			pos+=need;
		}

		if (work==0)
		{ 
			pos=info_to_send;
			need=0;
		}

		er_rd = msgsnd(msg_q_id, &buf2, need+3*one, 0);
		if (er_rd<0)
		{
			printf("Can't send message to client No %d",i+1);
			exit(-1);
		}
	}//while	

	free(matr_buf);
	
	info_to_send=h1*h2*one;
	matr_buf=(char*)malloc(info_to_send);
	pos=0;

	do
	{
		er_rd=msgrcv(msg_q_id, &buffer, MSG_MAX, 4*i+3, 0);
		if (er_rd<0) err_hand("Can't receive done work from client");

		my_strncpy(matr_buf+pos, buffer[0].msg, er_rd);
		pos+=er_rd;
	} while (pos<info_to_send);

	my_strncpy((char*)(res), matr_buf, info_to_send);
	free(matr_buf);

	buffer[0].mtype=4*i+4;
	if (i!=n-1)
	{
		strcpy(buffer[0].msg, BYE_MSG);
		sz=strlen(BYE_MSG);
		er_rd = msgsnd(msg_q_id, &buffer, sz+1, 0);
		if (er_rd<0) printf("Can't send last message to client, errno=%d\n",errno);
		er_rd = msgrcv(msg_q_id, &buffer, MSG_MAX, 4*i+1, 0);
		if (er_rd<0) err_hand("Can't receive last message");
	}	
	return NULL;
}

void err_hand(const char *msg)
{
	printf("%s\n",msg);
	printf("errno=%d\n",errno);
	exit(-1);
}

void my_strncpy(char *dest, char *src, int n)
{
	int i;
	for (i=0; i<n; i++) *(dest+i)=*(src+i);
}

void receive_matrix(int *m0, int fd, int h, int w)
{
	int position=0;
	int ind_err=1;
	int one=sizeof(int);
	int size=h*w*one;
	char *m;
	m=(char*)m0;
	
	while (ind_err>0)
	{
		ind_err=read(fd, m+position, size-position);
		position+=ind_err;
	}
	if (ind_err<0) err_hand("Can't read from file");
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
			buf=*((int*)(p+i*w+j));			
			printf("%d ", buf);
		}
		printf("\n");
	}
	printf("\n");
};

void matrix_in_file(int fd, int *m, int h, int w)
{
	int sz=h*w*2+h; ///assume that all values are numbers
	int new_sz;
	int x,signum;
	char *res,*buf;
	res=(char*)malloc(sz);
	int i,j,k,bufer,razr;
	int pos=0;
	for (i=0; i<h; i++)
	{
		for (j=0; j<w; j++)
		{

			x=*(m+i*w+j);
			if (x<0)
			{
				x=-x;
				bufer=x;
				signum=1;
			}
			else
			{
				bufer=x;
				signum=0;
			}

			k=0;
			while(bufer>0)
			{
				k++;
				bufer/=10;
			}				
			bufer=x;	
			razr=k+signum; 
			buf=malloc(razr);
			if (signum) buf[0]='-';

			for (k=razr-1; k>=signum; k--)
			{
				buf[k]=48+bufer%10;
				bufer/=10;
			}

			new_sz=razr+pos+1;
			if (new_sz>sz)
			{
				res=realloc((char*)res, new_sz);
				sz=new_sz;
			}
			k=0;
			while (pos<new_sz-1)
			{
				*(res+pos)=*(buf+k);
				k++;
				pos++;
			}
			free(buf);
			*(res+pos)=' ';
			pos++;
		}
		if (pos+1>sz)
		{
			res=realloc((char*)res, pos+1);
			sz++;
		}
		*(res+pos)='\n';
		pos++;
	}
	pos=0;
	int g;
	do
	{
		g=write(fd,res+pos,sz-pos);
		pos+=g;
	} while (g>0);
	if (g<0) err_hand("Can't write in file");
	free(res);	
};
