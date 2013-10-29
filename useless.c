#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define TXT_FILE "./useless.txt"
#define STR_SIZE 256
#define NUMBERS "1234567890"
#define N_CODE 48

struct comm   //command
{
	int t;     // time of slleping
	char *s;   // pointer to the begining of the command
};

int my_strtoint(char *str);
int my_getstr(int n, char *str);
struct comm my_time(char *str);
char *getshell(char **envp);


int main(int argc, char **argv, char **envp)
{
	int start_time=time(NULL);
	
	int fd=open(TXT_FILE,O_RDONLY);
	if (fd<0)
	{
		printf("Can't open file\n");
		exit(-1);
	}

	char *str;
	struct comm rcvd;
	str=(char*)malloc(STR_SIZE);
	int er_rd=my_getstr(fd,str);
	int p_id=1;
	int er_ex;
	char *my_shell=getshell(envp);


	// parent cycle
	while ((er_rd>0)&&(p_id>0))
	{
		rcvd=my_time(str);
		if (rcvd.t<0) printf("Incorrect input: %s",str);
		else
		{
			printf("\nThe command is\n%sTime of sleeping %d\n\n",rcvd.s,rcvd.t);
		
			p_id=fork();
			if (p_id==0)  //child
			{
				int curr_time=time(NULL)-start_time;

				if (curr_time<rcvd.t) sleep(rcvd.t-curr_time);
				else printf("This program is late =(\n");

				er_ex=execl(my_shell,my_shell,"-c",rcvd.s);
				if (er_ex<0)
				{
					printf("Can't execute %s",rcvd.s);
					exit(-1);
				}
				
			}
			if (p_id<0) printf("Can't create process for %s\n",rcvd.s);
		}

		er_rd=my_getstr(fd,str);
	}  

	free(str);

	er_rd=close(fd);
	if (er_rd<0)
	{
		printf("Can't close file\n");
		exit(-1);
	}
}


char *getshell(char **envp)
{
	int i=0;
	while (strstr(*(envp+i),"SHELL=")==NULL) i++;
	return strstr(*(envp+i),"=")+1;
}


struct comm my_time(char *str)
{
	
	int i=0;
	char t[STR_SIZE];
	char buf[2];
	buf[0]=str[0];
	buf[1]='\0';

	struct comm x;

	while ((strstr(NUMBERS,buf)!=NULL) && (i<STR_SIZE))
	{
		t[i]=str[i];
		i++;
		buf[0]=str[i];
	}
	
	if ((str[i]!=' ') || (i==0) || (i==STR_SIZE))
	{
//		printf("This string isn't correct\n");
		x.t = -1;
		x.s = NULL;
	}
	else
	{
		t[i]='\0';		
	
		int t_sleep=my_strtoint(t);
	
		x.t = t_sleep;
		x.s = str+i+1;
	}
	return x;
}


int my_strtoint(char *str)
{
	int log=1;
	int res=0;
	int i;
	int len=strlen(str);
	for(i=len-1; i>=0; i--)
	{
		res+=(str[i]-N_CODE)*log;
		log*=10;
	}
	return res;
}


// Reading the  string from the file

int my_getstr(int fd, char *str)
{
	char *curr;
	int total=STR_SIZE;
	int i=-1;
	int er_rd;

	do
	{
		i++;
		er_rd=read(fd,str+i,1);
		total--;	

		if (total==1)
		{
			total=i+2+STR_SIZE;
			curr=realloc(str,total);
			if (curr==NULL)
			{
				printf("Can't get memory for string\n");
				exit(-1);
			}
			str=curr;
		}

		if (er_rd==-1)
		{
			printf("Can't read from file\n");
			exit(-1);
		}

		if (er_rd==0) return 0; //end of file
		
	} while ((str[i]!='\n') && (str[i]!=EOF));

	str[i+1]='\0';
	return 1;	
}
