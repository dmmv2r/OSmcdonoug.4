#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h>

#define SHM_KEY 52
#define PERMS 0644

struct shmseg {
   unsigned int seconds;
   unsigned int nanos;
};

struct msgbuf {
   long mtype; //process id
   int message; //runtime
};

//void sig_handler(int signum, int shmid, struct shmseg *shmp);
void clearmem(int shmid, struct shmseg *shmp);

int main(int argc, char* argv[]) {
   //signal(SIGINT, sig_handler);

   printf("in user\n");
   int shmid;
   struct shmseg *shmp;

   struct msgbuf buf;
   int msgid;
   
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
   if(shmid == -1) {
      perror("shared memory");
      return 1;
   }

   shmp = shmat(shmid, NULL, 0);
   if(shmp == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }
   


   if((msgid = msgget((key_t)12345, 0644 | IPC_CREAT)) == -1) {
      perror("user msgget");
      return 1;
   }

   if((msgrcv(msgid, &buf, sizeof(buf), 0, 0)) == -1) {
      perror("user msgrcv");
      exit(1);
   }
 
   srand(time(0));  
   int block;
   block = (rand() % (5 - 1 + 1)) + 1; //decides if process is randomly blocked
   if(block >= 3) {
      printf("blocked\n");
      sleep(2);
   }

   int used; //decides randomly how much of the alotted runtime was used
   used = (rand() % (buf.message - 1 + 1)) + 1;
   buf.message = buf.message - used;   

   if((msgsnd(msgid, &buf, sizeof(buf), 0)) == -1) {
      perror("user msgsnd");
   }
  
   printf("leaving user\n");
   printf("\n");

   return 0;
}

/*
void sig_handler(int signum, int shmid, struct shmseg *shmp) {
   printf("force ending\n");
   clearmem(shmid, shmp);
   exit(1);
}*/

void clearmem(int shmid, struct shmseg *shmp) {
   if(shmdt(shmp) == -1) {
      perror("shmdt");
      return;
   }

   if(shmctl(shmid, IPC_RMID, 0) == -1) {
      perror("shmctl");
      return;
   }
}
