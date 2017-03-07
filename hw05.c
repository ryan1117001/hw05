#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>


#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#define DATE_BUFFER_LEN 80
#define _BSD_SOURCE
#define UTIME_SIZE (sizeof (struct utimebuf))

char* rev_rename(int count, char* dupFile){
    //source: http://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
    char countbuf[10];
    sprintf(countbuf,"%d",count);
    char rename[] = "_rev";
    strcat(dupFile,rename);
    strcat(dupFile,countbuf);
    return dupFile;
}

int main(int argc, char* argv[]){
  	//argument checking and usage information
    if (argc == 1) {
      printf("Usage Information: %s [-d file_location] [-h] [-t] [-m] <name_of_file>", argv[0]);
      exit(EXIT_SUCCESS);
    }
    bool opt_d = false, opt_h=false, opt_t = false, opt_m = false;
  	char* backLocation=malloc(1024);
  	strcpy(backLocation, "/home/kofi/backup/");
  	int opt=0;
  	char* d_arg=NULL;
  	struct stat s;
  	while((opt = getopt(argc, argv, "d:htm")) != -1){
  		switch(opt) {
  			case 'd':
  				opt_d = true;
  				d_arg = strdup(optarg);
  				break;
  			case 'h':
  				opt_h = true;
  				break;
  			case 't':
  				opt_t = true;
  				break;
  			case 'm':
  				opt_m = true;
  				break;
  			default:
  				printf("Please enter options d,h,t or m.\n Option 'd' requires an argument\n");
  				return EXIT_SUCCESS;
  		}
  	}
  	char* dupFile;
  	if(argv[optind]!=NULL){
  		dupFile = argv[optind];
  	}
  	else{
  		printf("file not specified");
  		exit(EXIT_FAILURE);
  	}
  	if(opt_d){
  		//source: http://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
  		//checks if d_arg path exists
  		if(access(d_arg, F_OK) != -1 ){
  			if(stat(d_arg, &s)!=-1){
  				int mt=s.st_mode;
  				if(mt& S_IFMT==S_IFDIR){
  					const char* backLocation = d_arg;
  					printf("Your backup directory is: %s\n", backLocation);
  				}
  		}
  	}
  		else {
  			printf("BAD: Not a valid directory. Your backup folder will default to %s\n", backLocation);
  		}
	}
	if(opt_h){
		printf("\n\nSYNOPSIS\n%s [-d file_location] [-h] [-t] [-m] <name_of_file>\n", argv[0]);
		printf("%s SOURCE_FILE\n", argv[0]);
		printf("\nDESCRIPTION\n%s accepts a file name as an argument\n", argv[0]);
		printf("\nOPTIONS\n-d: assigns the backup file path to the 'file_location' argument string provided\n-h: prints this helpful information\n");
		printf("-t: appends the duplicatoin time to the file name\n-m disables metadata duplication\n");
		return EXIT_SUCCESS;
	}
	if(opt_t){
		//source: http://stackoverflow.com/questions/25420933/c-create-txt-file-and-append-current-date-and-time-as-a-name
		
		char buffer_t[DATE_BUFFER_LEN];
		time_t now = time(NULL);
		struct tm *t = localtime(&now);
		strftime(buffer_t, DATE_BUFFER_LEN, "%Y%m%d%I%M%S", t);
		
		dupFile= malloc(strlen(argv[optind])+strlen(buffer_t)+8);
		dupFile[0]= '\0';
		strcpy(dupFile, argv[optind]);
		strcat(dupFile, "_");
		strcat(dupFile, buffer_t);
		printf("Your duplicate file is named: %s\n", dupFile);
	}
	if(opt_m){
		//instert opt m instructions
	}

  if (opt_t == false) {
    //source: http://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
    /*char rename[] = "_rev";
    strcat(dupFile,rename);
    strcat(dupFile,"0");
    printf("Name of file is: %s\n", dupFile);*/
    dupFile = rev_rename(0,dupFile);
    printf("Name of file is: %s\n", dupFile);
  }
	if(access(backLocation, F_OK) == -1){
		if(mkdir(backLocation, S_IRWXU)==-1){
			printf("cant make directory%s\n", backLocation );
			exit(EXIT_FAILURE);
		}
	}
	char* pathEnd;
    pathEnd=basename(dupFile);
  	char buffer[EVENT_BUF_LEN];
  	int x, wd;
    char* p;
    struct inotify_event* event;
  	int fd = inotify_init();
  	
    //INITIAL BACK UP

    //fd inisilization
    if ( fd < 0 ) {
      	printf("inotify init failed\n");
    	exit(EXIT_FAILURE);
    }
    if(access(argv[optind], R_OK)==-1){
    	printf("failure accessing %s",argv[optind]);
    	exit(EXIT_FAILURE);
    }
    if(stat(dupFile, &s)!=-1){
    printf("%s\n",pathEnd);
    strcat(backLocation, pathEnd);
    printf("%s\n", backLocation);
    //int t=umask(s.st_mode);
    }
    else{
      printf("%d\n",errno );
    }
    x=open(backLocation, O_RDWR|O_CREAT , s.st_mode);
    if(x==-1){
      char* ster=strerror(errno);
    	printf("open failed%s\n", ster);
    	exit(EXIT_FAILURE);
    }
    //printf("yah\n");
    if(read(x, backLocation, sizeof(backLocation))==-1){
        printf("write failed\n");
        exit(EXIT_FAILURE);
    }
    fflush(NULL);
    if(x==-1){
    	printf("read/write failed");
    	exit(EXIT_FAILURE);
    }
    time_t tmod, tstat;
    printf("YAH");
    
    	tmod=s.st_mtim.tv_sec;
    	tstat=s.st_ctim.tv_sec;
    	struct utimbuf buf;
    	buf.modtime=tmod;
    	buf.actime=tstat;
    	utime(backLocation,&buf);
	
		printf("time access failure");
		exit(EXIT_FAILURE);
	printf("almost there");
    if(x==-1){
    	printf("read/write failed");
    	exit(EXIT_FAILURE);
    }
    wd = inotify_add_watch(fd,argv[optind], IN_MODIFY);
    if (wd == -1) {
      printf("wd return failure\n");
      return(EXIT_FAILURE);
    }
    printf("just a bit further....");
    if (fstat(wd, &s)<0){
    	printf("stat failure");
    	exit(EXIT_FAILURE);
    }
    

    while(1) {
  		x=read(fd, buffer, EVENT_BUF_LEN);
  		if ( x < 0 ) {
    		printf("read failed\n");
        	exit(EXIT_FAILURE);
  		}
      int rev_num = 1;
      for (p = buffer; p<buffer+x; ) {
        event = (struct inotify_event*) p;
        if ((event->mask & IN_MODIFY) != 0) {
          //MAKE ANOTHER COPY

          //while loop for each new copy of backup where
          //rename is necessary
          bool copy_rename = false;
          while(copy_rename == false){
            rev_rename(rev_num,dupFile);
            rev_num++;
          }
        }
        p += sizeof(struct inotify_event) + event->len;
      }
      
  	}
    return EXIT_SUCCESS; //will not go through this.
}