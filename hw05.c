#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int main(int argc, char* argv[]){
  	//argument checking and usage information
    if (argc == 0) {
      printf("Usage Information: ./hw05 [-d file_location] [-h] [-t] [-m] <name_of_file>");
      exit(EXIT_SUCCESS);
    }

    bool opt_d, opt_h=false;
  	char* backLocation="~/backups/";
  	int opt;
  	/*
  		getops loop
  	*/
  	char buffer[EVENT_BUF_LEN];
  	int x, wd;
    char* p;
    struct inotify_event* event;
  	int fd = inotify_init();
  	
    //INTIAL BACK UP

    //fd inisilization
    if ( fd < 0 ) {
      printf("inotify init failed\n");
      exit(EXIT_FAILURE);
    }
    wd = inotify_add_watch(fd,argv[1], IN_MODIFY);
    if (wd == -1) {
      printf("wd return failure");
      return(EXIT_FAILURE);
    }
    while(1) {
  		x=read(fd, buffer, EVENT_BUF_LEN);
  		if ( x < 0 ) {
    		printf("read failed\n");
        exit(EXIT_FAILURE);
  		}
      for (p = buffer; p<buffer+x; ) {
        event = (struct intoify_event*) p;
        if ((event->mask & IN_MODIFY) != 0) {
          //MAKE ANOTHER COPY
        }
      }
      p += sizeof(struct inotify_event) + event->len;
  	}
    return EXIT_SUCCESS; //will not go through this.
}