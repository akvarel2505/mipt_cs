#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define CLIENT "./client"
#define SERVER "./server"
#define FILE_IN "./source.txt"

void err_hand(const char *msg);

int main(int argc, char **argv)
{
	if (argc!=2) err_hand("Wrong number of parameters");

	char c=*(argv[1]);
	int n;

	if ((c>'0')||(c<'9')) n=atoi(argv[1]);
	else err_hand("The parameter of the program must be a number");

	int er=fork();
	if (er>0)
	{
		int i=0;
		er=1;		
		while ((i<(n-1))&&(er>0))
		{
			er=fork();
			i++;
			if (er<0) err_hand("Can't fork in client cycle");			}
	
		er=execl(CLIENT, CLIENT, NULL);
		if (er<0) err_hand("Can't start a client");		
	}
	if (er==0)
	{
		int er_serv=execlp(SERVER,SERVER,argv[1],NULL);
		if (er_serv<0) err_hand("Can't execute a server");
	}
	if (er<0) err_hand("Fork error");

}

void err_hand(const char *msg)
{
	printf("%s\n",msg);
	printf("errno=%d\n",errno);
	exit(-1);
};
