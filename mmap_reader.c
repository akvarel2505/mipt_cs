#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define DEST_FILE "./buffer.txt"
#define FTOK_FILE "./mmap_writer.c"

struct my_msgbuf
{
	long mtype;
};

void err_hand(const char *msg);
void *remap(int fd, void *map_file, int *position, int *rest, int *npages);

int main()
{
	int err;

	//opening file
	(void)umask(0);
	int fd=open(DEST_FILE, O_RDWR|O_CREAT, 0777);
	if (fd<0) err_hand("Can't open file");

	void *map_file;
	off_t PG_SZ=sysconf(_SC_PAGE_SIZE);
	// AP: а почему вы используете именно 2 страницы памяти, а например не какой-то свой произвольный размер буфера?
	map_file=mmap(NULL, 2*PG_SZ, PROT_READ, MAP_SHARED, fd, 0);
	if ((map_file+1)==NULL) err_hand("Can't map file in memory");
	int position=0;
	int rest=2*PG_SZ;
	int npages=2;

	//preparation to work with message queue
	int key=ftok(FTOK_FILE, 1);
	if (key<0) err_hand("Can't generate a key");

	(void)umask(0);
	// AP: а что означает "исполнение" для очереди сообщений?
	int msg_id=msgget(key,0777|IPC_CREAT);
	if (msg_id<0) err_hand("Cant get access to message queue");

	struct my_msgbuf rcv_msg;	

	err=msgrcv(msg_id, &rcv_msg, 0, 0, 0);
	if (err<0) err_hand("Can't receive message");

	int fragm_sz;
	char *str;
	do
	{
		//reading length of the string
		if (rest<sizeof(int)) map_file = remap(fd, map_file, &position, &rest, &npages);
		fragm_sz=*((int*)(map_file+position));
		position+=sizeof(int);
		rest-=sizeof(int);

		str=(char*)realloc(str, fragm_sz+1);
		str[fragm_sz]=0;

		//reading string
		if (rest<fragm_sz) map_file = remap(fd, map_file, &position, &rest, &npages);
		strncpy(str, (char*)(map_file+position), fragm_sz);
		position+=fragm_sz;
		rest-=fragm_sz;

		//printing result
		printf("%s\n",str);	

		//receiving permission for reading next piece of information
		err=msgrcv(msg_id, &rcv_msg, 0, 0, 0);
		if (err<0) err_hand("Can't receive message");

	} while (rcv_msg.mtype==1);

	free(str);
	err=close(fd)|munmap(map_file, 2*PG_SZ);
	if (err!=0) err_hand("Can't finish the work with file");

	if (rcv_msg.mtype>1)
		err=msgctl(msg_id, IPC_RMID, NULL);
	if (err<0) err_hand("Can't remove message queue");

	printf("Reader has finished its work successfully\n\n");
};

void *remap(int fd, void *map_file, int *position, int *rest, int *npages)
{
//this one is almost similar to function "remap" in mmap_writer.c except ftruncate

	off_t PG_SZ=sysconf(_SC_PAGE_SIZE);
	int np=(*npages);
	void *res;

	int err=munmap(map_file, 2*PG_SZ);
	if (err<0) err_hand("Can't unmap file");

	np++;

//	ftruncate(fd, np*PG_SZ); //add one more memory page to file
//it is not nessesary here, because writer has already increased size of file

	res=mmap(NULL, 2*PG_SZ, PROT_WRITE, MAP_SHARED, fd, (np-2)*PG_SZ);
	if ((res+1)==NULL) err_hand("Can't remap file");

	(*rest)+=PG_SZ;
	(*npages)=np;
	(*position)-=PG_SZ;

	return res;
};

void err_hand(const char *msg)
{
	printf("%s\nerrno=%d\n", msg, errno);
	exit(-1);
};
