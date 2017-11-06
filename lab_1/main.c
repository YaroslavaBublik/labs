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
            fprintf(fp,"%s", "Дочерний процесс - ID: ");
            fprintf(fp,"%s", strcat(str_id, "\n"));
            for (int i = 0; i < (1024*1024*10)/7; ++i){
                fputs("Ya daun", fp);
            }
        }
        fclose(fp);
    }
    else if (pid) {
        FILE *fp = fopen("file.txt", "w");
    	char str_id[12];
        int status = 0;
        struct tms time;
        sprintf(str_id, "%d", getpid());
        fprintf(fp,"%s", "Родительский процесс, ожидаю - ID: ");
        fprintf(fp,"%s", strcat(str_id, "\n"));
		waitpid(pid, &status, 0);
        times(&time);
        fprintf(fp,"%s", "Родительский процесс, наканецта!\n");
        fprintf(fp,"Time: %f ms\n", (double)(time.tms_cutime));
        fclose(fp);
    }
	else {
		perror("fork");
	}        
}

