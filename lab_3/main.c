#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

const int SIZE = 512;
int shm_id;
void* shmem;

void sig_handler(int sign){
    if(sign == SIGINT){
        shmdt(shmem);
        shmctl(shm_id, IPC_RMID, NULL);
        exit(0);
    }
}


int main(){
  setbuf(stdout, NULL);
  signal(SIGINT, sig_handler);
  key_t shm_key = ftok("/dev/random", 1);
  int exist = 1;
  shm_id = shmget(shm_key, SIZE, 0);

  if (shm_id == -1){
    exist = 0;
    shm_id = shmget(shm_key, SIZE, IPC_CREAT | 0666);
    if(shm_id == -1){
      printf("%s\n", "Error shmget");
    }
  }

  shmem = shmat(shm_id, NULL, 0);
  if (shm_id < 0){
    printf("%s\n", "attach error");
    exit(1);
  }

  if (exist){
    while(1){
      sleep(1);
      printf("%s %s\n", (char*) shmem, "child");
    }
  } else {
    char time_str[100] = "";
    while(1){
      sleep(1);
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      sprintf(time_str, "%d:%d:%d parent time", tm.tm_hour, tm.tm_min, tm.tm_sec);
      printf("%s\n", time_str);
      sprintf((char*) (shmem), "%s ", time_str);
    }
  }

  shmdt(shmem);
  shmctl(shm_id, IPC_RMID, NULL);

  return 0;
}
