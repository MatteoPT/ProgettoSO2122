#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>

char* getDirectory(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + 1);
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  return res;
}


int main() {

  DIR* dir;
  struct dirent* proc_r;

  dir = opendir("/proc");
  if (dir == NULL) {
    //Gestione errori
    printf("Errore nell'apertura della directory /proc\n");
    exit(EXIT_FAILURE);
  }
  
  //Cerco il thread corrente
  while( (proc_r = readdir(dir)) != NULL ) {
    printf("directory name -> %s\n", proc_r->d_name);
    if (strcmp(proc_r->d_name, "thread-self") == 0) break;
  }

  printf("IN THREAD\n");
  //Una volta nel thread, stampo i PID di tutti i processi
  while( (proc_r = readdir(dir)) != NULL) {
  
    printf("pid -> %s\n", proc_r->d_name);    
    char* directory = getDirectory(proc_r->d_name);
    
    DIR* pid_dir;
    if( (pid_dir = opendir(directory)) == NULL) {
      //Gestione errori
      printf("Errore nell'apertura di %s\n", directory);
      exit(EXIT_FAILURE);
    }
  
  }
  
  
}
