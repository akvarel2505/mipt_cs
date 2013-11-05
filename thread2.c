#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_TXT "./thread.txt"

int main()
{
	// Receive the information
	
	int fd=open(FILE_TXT,O_RDONLY);
	if (fd<0)
	{
		printf("Can't open file for reading\n");
		exit(-1);
	}

	int a,b,c;
	int one=sizeof(int);

	int er_rd = read(fd,&a,one) & read(fd,&b,one) & read(fd,&c,one);
	if (er_rd!=one)
	{
		printf("Can't read from file\n");
		exit(-1);
	}

	const int h1=a;  //static???
	const int h2=c;
	const int w=b;
	int s1=a*b*one;
	int s2=c*b*one;

	int *m1,*m2;
	m1=(int*)malloc(s1);
	m2=(int*)malloc(s2);

	if ((m1==NULL)||(m2==NULL))
	{
		printf("Can't get memory for the matrices\n");
		exit(-1);
	}

	er_rd = read(fd,m1,s1) + read(fd,m2,s2);
	if (er_rd!=(s1+s2))
	{
		printf("Can't read from file\n");
		exit(-1);
	}

	er_rd=close(fd);
	if (er_rd<0) printf("Can't close file\n");
		
	printf("These matrices have been received from file:\n\n");

	int i,j;

	for (i=0; i<a; i++)
	{
		for (j=0; j<b; j++) printf("%d ",*(m1+i*b+j));
		printf("\n");
	}	
	printf("\n");

	for (i=0; i<c; i++)
	{
		for (j=0; j<b; j++) printf("%d ",*(m2+i*b+j));
		printf("\n");
	}
	printf("\n");




/*
	er_rd=execlp("/bin/rm","/bin/rm",FILE_TXT,NULL);
	if (er_rd<0)
	{
		printf("Can't remove the file!\n");
		exit(-1);
	}
*/

}
