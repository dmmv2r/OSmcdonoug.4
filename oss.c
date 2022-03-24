#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[]) {

   printf("working\n");


   pid_t childpid = 0;
   char* args[] = {"./user", NULL};


   if(childpid == fork()) {
      printf("execing\n");
      execv("./user", args);
   } //else {
      //wait(NULL);
   //}
   
   printf("waiting\n");
   wait(NULL);


   printf("ending master\n");
   return 0;
}
