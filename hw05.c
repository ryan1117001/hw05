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
#include <libgen.h>
#include <errno.h>
#include <fcntl.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#define DATE_BUFFER_LEN 80
#define _BSD_SOURCE
#define UTIME_SIZE (sizeof (struct utimebuf))

//adds _rev to create a new backfile name
char* rev_rename(int count,int optind, char* argv[], char* backlocal) {
    //source: http://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform
    //source: stackoverflow.com/questions/5242524/converting-int-to-string-in-c
    char* temp1 = malloc(1024);
    char* temp2 = basename(argv[optind]);
    char* countbuf= malloc(100);
    sprintf(countbuf,"%d",count);
    
    //concatenate strings to create name
    strcat(temp1,backlocal);
    strcat(temp1,temp2);
    strcat(temp1,"_rev");
    strcat(temp1,countbuf);
    return temp1;
}

//adds current time to the end of file
//source: PowerPoint Lecture 16
char* time_rename(int optind, char* argv[], char* backlocal) {
    //source: http://stackoverflow.com/questions/25420933/c-create-txt-file-and-append-current-date-and-time-as-a-name
    char* temp1;
    char* temp2 = basename(argv[optind]);
    char buffer_t[DATE_BUFFER_LEN];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer_t, DATE_BUFFER_LEN, "%Y%m%d%I%M%S", t);
    
    temp1= malloc(strlen(argv[optind])+strlen(buffer_t)+8);
    temp1[0]= '\0';
    //concatenate strings to add time
    strcpy(temp1, backlocal);
    strcat(temp1, temp2);
    strcat(temp1, "_");
    strcat(temp1, buffer_t);
    return temp1;
}

//purely coping a file and its contents
//source: PowerPoint Lecture 16
int copy_file(int optind, char* argv[], char  *copy) {
    int wd, x;
    struct stat s;
    wd=open(argv[optind],O_RDONLY); //used to read the orginal file
    if(access(argv[optind], R_OK)==-1) { //checks if file is acessible
      printf("failure accessing %s\n",argv[optind]);
      exit(EXIT_FAILURE);
    }
    if(stat(argv[optind], &s)==-1){ //checks for stat
      printf("stat error");
      exit(EXIT_FAILURE);
    }
    //printf("%s\n",copy);
    x=open(copy, O_RDWR|O_CREAT , s.st_mode); //used to open and appned to new file 
    if(x==-1){ //open error checking
      char* ster=strerror(errno);
      printf("open failed%s\n", ster);
      exit(EXIT_FAILURE);
    }
    char writer[100]; //buffer to push content into new file
    ssize_t n;
    while(n=read(wd,writer,50)){
        write(x,writer,n);
        printf("..writing..\n");
    }
    fflush(NULL); //cleans out
    if(n==-1){ //checks for read/write 
      printf("read/write failed\n");
      exit(EXIT_FAILURE);
    }
    //closes files to allow for reuse
    close(wd);
    close(x);

    return 0;
}

//changes the time timestamp of the new file to be the same as the old
//source: PowerPoint Lecture 16
void timestamp(int optind, char* argv[], char* copy) {
    //timestamp change initial
    time_t tmod, tstat;
    struct stat s;
    if(stat(argv[optind], &s)==-1){
      printf("stat error");
      exit(EXIT_FAILURE);
    }

    tmod = s.st_mtim.tv_sec; //modify time initialized
    tstat = s.st_ctim.tv_sec; //access time initialized
    struct utimbuf buf;
    //changes the time the buffer holds
    buf.modtime = tmod;
    buf.actime = tstat;

    if(utime(copy,&buf)==-1) { //checks for error and copies it over
      printf("time access failure\n");
      exit(EXIT_FAILURE);
    }
}

//changes ownership
//source: PowerPoint Lecture 16
void ownership(int optind, char* argv[], char* copy) {
    //initials for attribute checking and changing
    struct stat s;
    uid_t owner;
    gid_t group;

    if(stat(argv[optind],&s) == -1) { //checks for stat error
      printf("stat error");
      exit(EXIT_FAILURE);
    }
    //sets the user/group name to variables
    owner = s.st_uid;
    group = s.st_gid;
    if (chown(copy,owner,group) == -1) { //changes the owner and group
      printf("change owner error");
      exit(EXIT_FAILURE);
    }
}

//changes permissions
//source: PowerPoint Lecture 16
void perm(int optind, char* argv[], char* copy) {
  //initial stat and permission holder variable
  struct stat s;
  mode_t mode;

  if (stat(argv[optind],&s) == -1) { //checks for stat error
      printf("stat error");
      exit(EXIT_FAILURE);
     }
     mode = s.st_mode; //sets mode to permission bits
  if (chmod(copy,mode) == -1) { //changes the the permisions to be the same and checks for error
    printf("permission error");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char* argv[]){
    //argument checking and usage information
    if (argc == 1) {
      printf("Usage Information: %s [-d file_location] [-h] [-t] [-m] <name_of_file>\n", argv[0]);
      exit(EXIT_SUCCESS);
    }

    //getopt initials
    bool opt_d = false, opt_h=false, opt_t = false, opt_m = false;
    int opt=0;
    char* d_arg=NULL;

    //getopt that changes the boolean flag
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

    //backup initials
    struct stat s;
    char* backLocation=malloc(1024);
    char* temp = getenv("HOME");
    strcpy(backLocation, temp);
    strcat(backLocation, "/backup/");
    //change in backup location
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

    //Help information
    if(opt_h){
      printf("\n\nSYNOPSIS\n%s [-d file_location] [-h] [-t] [-m] <name_of_file>\n", argv[0]);
      printf("%s SOURCE_FILE\n", argv[0]);
      printf("\nDESCRIPTION\n%s accepts a file name as an argument\n", argv[0]);
      printf("\nOPTIONS\n-d: assigns the backup file path to the 'file_location' argument string provided\n-h: prints this helpful information\n");
      printf("-t: appends the duplicatoin time to the file name\n-m disables metadata duplication\n");
      return EXIT_SUCCESS;
    }
    //check for backup location
    if(access(backLocation, F_OK) == -1){
      if(mkdir(backLocation, S_IRWXU)==-1){
        printf("Cannot make directory%s\n", backLocation );
        exit(EXIT_FAILURE);
      }
    }
    //naming of the back up file
    char* pathEnd;
    char* backFile=malloc(1024);
    if(argv[optind]!=NULL){
      pathEnd=basename(argv[optind]);
    }
    else{ //if null, argument was not there or usable
      printf("Usage Information: %s [-d file_location] [-h] [-t] [-m] <name_of_file>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
    //attaches the original file name (not path) to new backup file
    backFile=strcpy(backFile, backLocation);
    strcat(backFile, pathEnd);
    //printf("%s\n",argv[optind]);
    //printf("%s\n",backFile);

    //concatenate the time to the end if true
    printf("Backup Location: %s\n", backLocation);
    if(opt_t == true) {
        backFile = time_rename(optind,argv,backLocation);
        printf("Backup file name: %s\n", backFile);
    }
    //concatenate the rev to the end if false
    if (opt_t == false) {
        backFile = rev_rename(0,optind,argv,backLocation);
        printf("Backup file name: %s\n", backFile);
    }

    //initial copy of file
    //copies file and ideally returns 0 when successful
    if (copy_file(optind,argv,backFile) != 0) {
      printf("copy error\n");
      return EXIT_FAILURE;
    }
    //backup file exists so metadata can be copied
    if(!opt_m){ //default false, will copy metadata
      printf("Metdata on\n");
      timestamp(optind,argv,backFile);
      ownership(optind,argv,backFile);
      perm(optind,argv,backFile);
    }
    else printf("Metadata off\n"); //if true, metadata is not copied

  int fd,wd;
  struct inotify_event *event;
  char buf[EVENT_BUF_LEN];
  ssize_t numRead;
  char *p;

  //Source: The Linux Programming Interface, Michael Kerrisk pg382
  //creation of inotify instance
  fd = inotify_init();
  if (fd == -1) {
    printf("fd returned -1\n");
    return EXIT_FAILURE;
  }
  //watch added on original file to see if it is deleted or written on
  wd = inotify_add_watch(fd,argv[optind], IN_DELETE_SELF | IN_CLOSE_WRITE);
  if (wd == -1) {
    printf("wd returned failure\n");
    return EXIT_FAILURE;
  }
  int count = 1;
  //loop forever to read events
  printf("Watch has begun\n"); //simply tells user that watch has begun
  for (;;) {
    //buffer to read the event
    numRead = read(fd,buf,EVENT_BUF_LEN);
    if (numRead <= 0) {
      printf("read error\n");
      return EXIT_FAILURE;
    }
    for (p = buf; p<buf+numRead;) {
      event = (struct inotify_event*) p; //casting event to p pointer into buffer
      if ((event->mask & IN_DELETE_SELF) != 0) { //when deleted file, stops watching and exits
          printf("File Deleted\n");
          return EXIT_SUCCESS;
      }
      if ((event->mask & IN_CLOSE_WRITE) != 0) { //when file is written upon, enters loop
        printf("File Modified, copy will be made\n"); 
        if (opt_t == true) { //renames file with a new time
          backFile = time_rename(optind, argv, backLocation);
        }
        else { //renames file with new rev number
          backFile = rev_rename(count,optind,argv, backLocation);
          count++;
        }
        copy_file(optind,argv,backFile); //copies the content to a new file
        if (!opt_m) { //copies the metadata again
          timestamp(optind,argv,backFile);
          ownership(optind,argv,backFile);
          perm(optind,argv,backFile);
        }
        printf("File has been copied\n");
      }
      p += sizeof(struct inotify_event) + event->len; //moves the buffer along
    }
  }
    return EXIT_SUCCESS; //will not get here
}

