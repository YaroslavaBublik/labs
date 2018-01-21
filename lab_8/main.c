#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

char* fifo_path = "/tmp/kek";
int create = 1;
int fifo_fd;
char buf[100];

void sig_handler(int signo){
    if(signo == SIGINT){
        if(!create)
            unlink(fifo_path);
        exit(0);
    }
    if(signo == SIGPIPE){
        unlink(fifo_path);
        exit(0);
    }
}

void p_read(){
  fifo_fd = open(fifo_path, O_RDONLY);
  while(1){
      sleep(1);
      read(fifo_fd, &buf, sizeof(char)*100);
      printf("read -> %s \n", buf);
  }
}

void p_write(){
  fifo_fd = open(fifo_path, O_WRONLY);
  char time_str[100] = "";
  while(1){
      sleep(1);
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      printf("write ->");
      sprintf(time_str, "ID:%p %d:%d:%d parent time", pthread_self(), tm.tm_hour, tm.tm_min, tm.tm_sec);
      printf("%s\n", time_str);
      dprintf(fifo_fd, "%s\n", time_str);
  }
}

int main(int argc, char** argv){
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);

    struct stat st;
    if(stat(fifo_path, &st) != 0){
        create = 0;
        printf("No fifo, trying to create new one\n");
        mkfifo(fifo_path, 0666);
    }

    if(create){
        printf("child\n");
        p_read();
    }
    else{
        printf("parent\n");
        p_write();
    }
}
