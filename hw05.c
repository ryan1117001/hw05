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
  	bool opt_d, opt_h=false;
  	char* backLocation="~/backups/";
  	int opt;
  	/*
  		getops loop
  		*/
  	char buffer[EVENT_BUF_LEN];
  	int x, i = 0;
	char* p;
	struct inotify_event* event;
  	int fd = inotify_init();
  	if ( fd < 0 ) {
    printf("inotify init failed\n");
    exit(EXIT_FAILURE);
    /*
    fd inisilization
    add watches
    */
      	while(1){
  		x=read(fd, buffer, EVENT_BUF_LEN);
  		if ( x < 0 ) {
    		printf("read failed\n");

  		}
  		/*
  		for loop for event handling
  		*/
  	}
}