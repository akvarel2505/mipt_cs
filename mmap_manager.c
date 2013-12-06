#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define RD "./reader"
#define WRT "./writer"
void err_hand(const char *msg)
{
	printf("%s\nerrno=%d\n",msg,errno);
	exit(-1);
};

int main(int argc, char **argv)
{
	if (argc<2) err_hand("Too few parameters");
	int err, pid;
	
	pid=fork();
	if (pid>0) err=execl(WRT, WRT, argv[1], NULL);
	if (pid==0) err=execl(RD, RD, NULL);
	if (pid<0) err_hand("Can't fork");
	
	if (err<0)
	{
		printf("child pid=%d",pid);
		err_hand("Can't exec");
	}
}
