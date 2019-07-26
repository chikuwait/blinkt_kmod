#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
int main(int argc, char *argv[]){;
	char buff=0x00;
	char ledon =0x01;
	int fd,rc,cnt;
	int use_flock = (argc == 5) && (strcmp(argv[4], "-x") == 0);

	fd = open(argv[1],O_RDWR);
	if(fd == -1){
		fprintf(stderr,"オープンできませんでした");
		return -1;
	}

	rc = read(fd,&buff,1);
	cnt = atoi(argv[3]);
	if(cnt== 0){
		printf("0X%X\n",buff);
		return 0;
	}else{
		if(use_flock)
			flock(fd, LOCK_EX);
		for(int i=0;i<cnt;i++){
			buff = buff ^ (ledon << (atoi(argv[2]) - 1));
			buff = buff & (ledon << (atoi(argv[2])-1));
			write(fd,&buff,1);
			usleep(5 * 100000);
		}
		if(use_flock)
			flock_(fd,LOCK_UN);
	}

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
