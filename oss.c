#include <stdio.h>
#include <stdlib.h>
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

   int shmid;
   struct shmseg *shmp;

   struct msgbuf buf;
   int msgid;

   //int queue1[10];

//-------------------------------shared memory-------------------------------------------
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
   if(shmid == -1) {
      perror("Shared memory");
      return 1;
   }

   shmp = shmat(shmid, NULL, 0);
   if(shmp == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }
//---------------------------------------------------------------------------


   if((msgid = msgget((key_t)12345, 0644 | IPC_CREAT)) == -1) {
      perror("oss msgget");
      return 1;
   }
   

   pid_t childpid = 0;
   char* args[] = {"./user", NULL};


   shmp->seconds = 0; //sets system clock to 0
   shmp->nanos = 0;

   buf.mtype = 1;

   srand(time(0));
   int looping = 1;
   
   while(looping) {
      buf.message = (rand() % (5000 - 1000 + 1)) + 1000; //random runtime
      printf("scheduled for %i nanos\n", buf.message);
      if((msgsnd(msgid, &buf, sizeof(buf), 0)) == -1) {
         perror("oss msgsnd");
      }
      
      if(childpid == fork()) {
         execv("./user", args);
      }

      wait(NULL);

      if((msgrcv(msgid, &buf, sizeof(buf), 0, 0)) == -1) {
         perror("oss msgrcv");
      }

      printf("used %i nanos\n", buf.message); //keeps track of total time
      shmp->nanos += buf.message;
      printf("total nanos = %i\n", shmp->nanos);
      if(shmp->nanos >= 10000) {
         looping = 0;
      }

   }




//-------------------------cleanup--------------------------------------------
   if(msgctl(msgid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }

   clearmem(shmid, shmp);
  
   printf("ending master\n");
   return 0;
}
//---------------------------------------------------------------------------

/*
void sig_handler(int signum, int shmid, struct shmseg *shmp) {
   printf("force exiting\n");
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

   return;
}
