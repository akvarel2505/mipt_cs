#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/mman.h>

#define NPAR 8
#define FTOK_FILE "./mmap_writer.c"
#define DEST_FILE "./buffer.txt"

struct info_file
{
	char *i[NPAR];
};
/*
0 name
1 type
2 size
3 owner
4 group
5 permissions
6 acess time
7 modification time
*/		

struct info_to_send
{
	int size;
	char *str;
};

struct my_msgbuf
{
	long mtype;
};

void err_hand(const char *msg);
char *make_file_name(const char *dir_path, const char *name);
struct info_file work_with_file(char *name);
char *get_size(off_t sz);
char *get_time(time_t t);
char *get_type(mode_t mode);
char *get_mode(mode_t mode);
char *get_owner(uid_t id);
char *get_group(gid_t id);
struct info_to_send work_with_struct(struct info_file);
void *remap(int fd, void *map_file, int *position, int *rest, int *npages);

int main(int argc, char **argv)
{
	int err,i;

	if (argc<2) err_hand("Too few parameters for the program");
	
	//creating a message queue for signaling about each new record in the file
	key_t key=ftok(FTOK_FILE, 1);
	if (key<0) err_hand("Can't generate a key!");
	(void)umask(0);
	int msg_id=msgget(key, 0666|IPC_CREAT);	
	if (msg_id<0) err_hand("Can't get access to message queue");
	struct my_msgbuf my_msg;
	my_msg.mtype=1;	

	//opening file
	(void)umask(0);
	int fd=open(DEST_FILE, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd<0) printf("Can't open file");
	
	const off_t PG_SZ=sysconf(_SC_PAGE_SIZE);
	int position=0; //position in mapped file
	int rest=2*PG_SZ;
	int total=0;    //total size of file
	int npages=2; //number of pages in file
	

	ftruncate(fd, 2*PG_SZ);
	void *map_file;
	map_file=mmap(NULL, 2*PG_SZ, PROT_WRITE, MAP_SHARED, fd, 0);	
	if ((map_file+1)==NULL) err_hand("Can't map file in memory");
	
	//begin working with directory
	DIR *dir;
	dir=opendir(argv[1]);
	if (dir==NULL) err_hand("Can't open the directory");
	
	struct dirent *curr_file;
	struct info_file info;
	struct info_to_send result;
	char *file_name;

	curr_file=readdir(dir);
	while (curr_file!=0)
	{
		file_name=make_file_name(argv[1], curr_file->d_name);
		
		info=work_with_file(file_name);
		result=work_with_struct(info);
		
		if (rest<sizeof(int)) map_file=remap(fd, map_file, &position, &rest, &npages);
	
		*((int*)(map_file+position))=result.size;
		position+=sizeof(int);
		total+=sizeof(int);
		rest-=sizeof(int);
		
		if (rest<result.size) map_file=remap(fd, map_file, &position, &rest, &npages);

		strncpy((char*)(map_file+position), result.str, result.size);
		position+=result.size;
		total+=result.size;
		rest-=result.size;
				
		//sending message
		err=msgsnd(msg_id, &my_msg, 0, 0);
		if (err<0) err_hand("Can't send message");
		
		//free
		for (i=2; i<NPAR; i++) free(info.i[i]);
		free(info.i[0]);	
		free(result.str);

		curr_file=readdir(dir);
	}
		
	err=munmap(map_file, 2*PG_SZ);
	if (err<0) err_hand("Can't unmap file finally");
		
	//sending message about the end
	my_msg.mtype=npages;
	err=msgsnd(msg_id, &my_msg, 0, 0);
	if (err<0) err_hand("Can't send final message");

	err=closedir(dir)|close(fd);
	if (err!=0) err_hand("Can't close the directory or the file");
};

struct info_to_send work_with_struct(struct info_file inf_f)
{

	char *def[NPAR];
	def[3]="\nowner: ";	
	def[4]="\ngroup: ";	
	def[0]="\nname: ";	
	def[2]="\nsize (bytes): ";	
	def[5]="\npermissions: ";	
	def[1]="\ntype: ";	
	def[6]="\nlast access: ";	
	def[7]="modificated: ";	
		
	int i,pos;
	int sz=0; //full size of formated infornation
	int sz_def[NPAR], sz_info[NPAR];

	for (i=0; i<NPAR; i++)
	{
		sz_def[i]=strlen(def[i]);
		sz_info[i]=strlen(inf_f.i[i]);
		sz+=sz_def[i]+sz_info[i];
	}
	char *str;
	str=malloc(sz+1);
	str[sz]=0;
	pos=0;

	for (i=0; i<NPAR; i++)
	{
		strncpy(str+pos, def[i], sz_def[i]);
		pos+=sz_def[i];
		strncpy(str+pos, inf_f.i[i], sz_info[i]);
		pos+=sz_info[i];
	}

	struct info_to_send result;
	result.size=sz;
	result.str=str;

	return result; 
};

struct info_file work_with_file(char *name)
{
	struct stat buf;
	struct info_file res;

	int err=lstat(name, &buf);
	if (err==-1) err_hand("Error with system call stat");

	res.i[0]=name;
	res.i[1]=get_type(buf.st_mode);
	res.i[2]=get_size(buf.st_size);
	res.i[7]=get_time(buf.st_mtime);
	res.i[6]=get_time(buf.st_atime);
	res.i[3]=get_owner(buf.st_uid);
	res.i[4]=get_group(buf.st_gid);
	res.i[5]=get_mode(buf.st_mode);

	return res;
};

char *get_size(off_t sz)
{
	off_t x=sz;
	int razr=0;
	while (x>0)
	{
		razr++;
		x=x/10;
	}
	if (razr==0) razr++;
	x=sz;

	char *res;
	int len=razr;
	int i;
	res=malloc(razr+1);
	for (i=razr-1; i>=0; i--)
	{
		res[i]=x%10+48;
		x/=10;
	}
	res[razr]=0;
	return res;
}

char *get_type(mode_t mode)
{
	char *c;
	if S_ISREG(mode) c="regular";
	if S_ISLNK(mode) c="link";
	if S_ISDIR(mode) c="directory";
	if S_ISCHR(mode) c="character device";
	if S_ISBLK(mode) c="block device";
	if S_ISFIFO(mode) c="FIFO";
	if S_ISSOCK(mode) c="socket";
	return c;
};

char *get_mode(mode_t mode)
{
	int x = mode & (S_IRWXU|S_IRWXG|S_IRWXO);
	char *res;
	const int len=9;
	res=malloc(len+1);
	strcpy(res,"rwxrwxrwx");
	
	int mask=1;
	int i,buf;
	for (i=len-1; i>=0; i--)
	{
		buf = x & mask;
		if (buf==0) res[i]='-';
		mask*=2;
	}
	res[len]=0;

	return res;		
};

char *get_owner(uid_t id)
{
	struct passwd *p;
	p=getpwuid(id);
	if (p==NULL) err_hand("Can't get user name");
	char *res;
	res=malloc(strlen(p->pw_name)+1);
	strcpy(res, p->pw_name);
	return res;
};

char *get_group(gid_t id)
{
	struct group *p;
	p=getgrgid(id);
	if (p==NULL) err_hand("Can't get group name");
	char *res;
	res=malloc(strlen(p->gr_name)+1);
	strcpy(res, p->gr_name);
	return res;
};


char *get_time(time_t t)
{
	char *res, *c;
	c=ctime(&t);
	res=malloc(strlen(c)+1);
	strcpy(res,c);
	return res;
};

void *remap(int fd, void *map_file, int *position, int *rest, int *npages)
{
/*
	The main idea of this function:

	before: page_PAGE_PAGE_page_page
	after:  page_page_PAGE_PAGE_page

	PAGE - mapped, page - not mapped memory.
*/
	off_t PG_SZ=sysconf(_SC_PAGE_SIZE);
	int np=(*npages);
	void *res;

	int err=munmap(map_file, 2*PG_SZ);
	if (err<0) err_hand("Can't unmap file");

	np++;
	ftruncate(fd, np*PG_SZ); //add one more memory page to file

	res=mmap(NULL, 2*PG_SZ, PROT_WRITE, MAP_SHARED, fd, (np-2)*PG_SZ);
	if ((res+1)==NULL) err_hand("Can't remap file");

	(*rest)+=PG_SZ;
	(*npages)=np;
	(*position)-=PG_SZ;

	return res;
};

char *make_file_name(const char *dir_path, const char *name)
{
	char *res;
	int len_dir=strlen(dir_path);
	int len_file=strlen(name);
	res=malloc(len_dir+len_file+2);

	strncpy(res, dir_path, len_dir);
	strcpy(res+len_dir+1, name);
	*(res+len_dir)='/';

	return res;
};

void err_hand(const char *msg)
{
	printf("%s\nerrno=%d\n", msg, errno);
	exit(-1);
};
