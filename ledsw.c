#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){;
	char buff;
	int fd = open("hook",O_RDONLY);
	int rc = read(fd,&buff,1);
	printf("read: %0x",buff);

/*
	FILE *fd = fopen("hook","rb+") ;
	char buf;	
	if(fd == NULL){
		fprintf(stderr,"オープンできませんでした");
		return -1;
	}
	buf = fread(&buf,sizeof(char),1,fd);
	printf("data = %x\n", buf);
	fclose(fd);*/
	return 0;
}
