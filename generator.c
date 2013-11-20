#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define FILE_TXT "./source.txt"
#define MAX_VALUE 1000 

int generate(int *p, int n); //generates array of n numbers
void matrix_generation(int height, int width, int fd);   //generates matrix and writes it in file
void err_hand(const char *msg);

int main()
{
	printf("\n\nMultiplication of matrices with axb and bxc. Enter a,b,c:\n");
	
	int a,b,c;
	scanf("%d%d%d",&a,&b,&c);
	printf("\n");

	int one=sizeof(int);	
	srand(time(NULL));

	(void)umask(0);
	int fd=open(FILE_TXT, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd<0) err_hand("Can't open file for writing");

	int j;

	// Generation of the first matrix

	printf("This is the first matrix:\n");
	
	
	j = write(fd,&a,one) & write(fd,&b,one) & write(fd,&c,one);

	if (j!=one) err_hand("Can't write in file");
	
	matrix_generation(a,b,fd);

	// Generation of the transposed second matrix

	printf("This is the second matrix (transposed):\n");

	matrix_generation(c,b,fd);

	int e_cl=close(fd);
	if (e_cl<0) err_hand("Can't close file");

}	

void err_hand(const char *msg)
{
	printf("%s\n",msg);
	printf("errno=%d\n",errno);
	exit(-1);
};

int generate(int *p, int n)
{
	int i;
	int one=sizeof(int);
	for (i=0; i<n; i++) *(p+i)=-(MAX_VALUE/2)+(rand()%MAX_VALUE);
	return 0; 
};

void matrix_generation(int height, int width, int fd)
{
	int i,j;
	int one=sizeof(int);
	int e_wr;
	int position;

	int *p;
	p=(int*)malloc(width*one);
	if (p==NULL) err_hand("Can't get memory");

	for (j=0; j<height; j++)
	{
		generate(p,width);
		position=0;
		int full_sz=width*one;

		e_wr = write(fd, (char*)p, full_sz);
		while (e_wr>0)
		{
			position+=e_wr;
			e_wr=write(fd, ((char*)p)+position, full_sz-position);	
		}

		if (e_wr<0) err_hand("Can't write in file");

		for (i=0; i<width; i++) printf("%d ",*(p+i));
		printf("\n");
	}
	free(p);
}
