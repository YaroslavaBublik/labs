#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define BLOCK_SIZE  1024 * 16
#define SHM_SIZE 16 * BLOCK_SIZE

int shm_id;
void* shmem;
int is_slave = 1;

pthread_mutex_t lock;
pthread_cond_t cond;
pthread_t tid_rd[5];
pthread_t tid_wr[1];

int last_block_written = -1;

void sig_handler(int signo){
    if(signo == SIGINT){
        shmdt(shmem);
        shmctl(shm_id, IPC_RMID, NULL);
        exit(0);
    }
}

void* p_write(void* args){
    unsigned block_n = 0;
    while(1){
        pthread_mutex_lock(&lock);
        size_t i = 0;
        printf("ID: %p write -> ", pthread_self());
        for(; i < 1024 * 16; i++){
            ((char*)shmem)[(block_n * BLOCK_SIZE) + i] = '0' + (char)(rand()%9);
        }
        last_block_written = block_n;

        if(++block_n >= 16){
            block_n = 0;
        }
        for(int i = 0; i < 16; i++)
            printf("%c", *((char*)shmem + last_block_written * BLOCK_SIZE + i));
        printf("\n");
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
        sleep(rand()%3);
    }
    return NULL;
}

void* p_read(void* args){
    while(1){
        pthread_mutex_lock(&lock);
        while(last_block_written == -1)
            pthread_cond_wait(&cond, &lock);
        printf("ID: %p read ->",  pthread_self());
        for(int i = 0; i < 16; i++)
            printf("%c", *((char*)shmem + last_block_written * BLOCK_SIZE + i));
        printf("\n");
        sleep(rand()%5);
        last_block_written = -1;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(int argc, char** argv){
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    key_t shm_key = ftok("/dev/random", 234234);

    shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if(shm_id == -1){
        perror("shmget");
    }

    shmem = shmat(shm_id, NULL, 0);
    if(shmem == (void*)-1){
        perror("failed to attach shm");
        exit(-1);
    }

    for(int i = 0; i < 5; i++){
        pthread_create(&(tid_rd[i]), NULL, &p_read, &i);
    }
    pthread_create(&(tid_wr[0]), NULL, &p_write, NULL);
    pthread_join(tid_wr[0], NULL);
}
