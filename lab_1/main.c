#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
int main()
{
    pid_t pid;
    pid = fork();
    if (!pid){
    	FILE *fp = fopen("file.txt", "w");
        if(fp != NULL) {
            char str_id[12];
            sprintf(str_id, "%d", getpid());
            printf("%s", "Дочерний процесс - ID: ");
            printf("%s", strcat(str_id, "\n"));
            for (int i = 0; i < (1024*1024*10)/7; ++i){
                fputs("Ya daun", fp);
            }
        }
        fclose(fp);
    }
    else if (pid) {
    	char str_id[12];
        int status = 0;
        struct tms time;
        sprintf(str_id, "%d", getpid());
        printf("%s", "Родительский процесс, ожидаю - ID: ");
        printf("%s", strcat(str_id, "\n"));
		waitpid(pid, &status, 0);
        times(&time);
        printf("%s", "Родительский процесс, наканецта!\n");
        printf("Time: %f ms\n", (double)(time.tms_cutime));
    }
	else {
		perror("fork");
	}        
}

