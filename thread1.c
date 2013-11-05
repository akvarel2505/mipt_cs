#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define FILE_TXT "./thread.txt"

int generate(int *p, int n); //generates array of n numbers

int main()
{
	printf("\n\nMultiplication of matrices with axb and bxc. Enter a,b,c:\n");
	
	int a,b,c;
	scanf("%d%d%d",&a,&b,&c);
	printf("\n");

	int one=sizeof(int);	
	srand(time(NULL));

	int *x,*y;
	x=(int*)malloc(b*one);
	
	if (x==NULL)
	{
		printf("Can't get memory\n");
		exit(-1);
	}

	(void)umask(0);
	int fd=open(FILE_TXT, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd<0)
	{
		printf("Can't open file for writing\n");
		exit(-1);
	}

	int i,j,e_wr;

	// Generation of the first matrix

	printf("This is the first matrix:\n");
	
	
	j = write(fd,&a,one) & write(fd,&b,one) & write(fd,&c,one);

	if (j!=one)
	{
		printf("Can't write in file\n");
		exit(-1);
	}
	
	for (j=0; j<a; j++)
	{
		generate(x,b);
		
		e_wr=write(fd,x,b*one);
		if (e_wr<(b*one))
		{
			printf("Can't write in file\n");
			exit(-1);
		}
	
		for (i=0; i<b; i++) printf("%d ",*(x+i));
		printf("\n");
	}
	
	printf("\n");

	// Generation of the transposed second matrix

	printf("This is the second matrix (transposed):\n");

	for (j=0; j<c; j++)
	{
		generate(x,b);

		e_wr=write(fd,x,b*one);
		if (e_wr<b*one)
		{
			printf("Can't write in file\n");
			exit(-1);
		}
		for (i=0; i<b; i++) printf("%d ",*(x+i));
		printf("\n");
	}

	printf("\n");

	int e_cl=close(fd);
	if (e_cl<0)
	{
		printf("Can't close file\n");
		exit(-1);
	}

}	

int generate(int *p, int n)
{
	int i;
	int one=sizeof(int);
	for (i=0; i<n; i++) *(p+i)=rand()%10;
	return 0; 
}
