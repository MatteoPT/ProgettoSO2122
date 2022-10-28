#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

#include "top.h"

#define PAGESIZE  sysconf(_SC_PAGE_SIZE)
#define HERTZ     sysconf(_SC_CLK_TCK)


int main() {

N:

  DIR* dir;
  struct dirent* proc_r;

  dir = opendir("/proc");
  if (dir == NULL) {
    if (errno == EACCES) printf("Accesso negato a '/proc'\n");
    else if (errno == EMFILE || errno == ENFILE) printf("Raggiunto il limite di file descriptors aperti\n");
    else if (errno == ENOMEM) printf("Memoria disponibile non sufficiente per completare l'operazione\n");
    else printf("Errore nell'apertura di '/proc'\n");
    exit(EXIT_FAILURE);
  }
  
  while( (proc_r = readdir(dir)) != NULL ) {
    if (strcmp(proc_r->d_name, "thread-self") == 0) break;
  }
  
  int uptime = 0;
  FILE *f = fopen("/proc/uptime", "r");
  if (f != NULL) {
    fscanf(f, "%d", &uptime);
    fclose(f);
  }
  
  printf("\033[30m\033[42m");
  printf("        PID             Memory%%         CPU%%            STATUS          USER            NAME                ");
  printf("\033[0m\033[K\n");

  while( (proc_r = readdir(dir)) != NULL) {
  
    char* pid = proc_r->d_name;
  
    double cpu_usage = getCpuUsage(pid, uptime);
    
    double mem_usage = getMemUsage(pid);
    
    char name[256];
    
    char user[256];
    
    getUser(pid, user);
    
    getName(pid, name);
    
    char status = getStatus(pid);
   
     
    printf("        %s\t\t%.3f\t\t%.3f\t\t%c\t\t%s\t\t%s\n", pid, mem_usage, cpu_usage, status, user, name);

  }

  printf("\033[30m\033[47m");
  printf("        Inserisci h(elp) per i comandi disponibili       ");
  printf("\033[0m\033[K\n");
  
  while(1) {
    char* comm = (char*) malloc(10);
    if (comm == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
    int pid; int res_signal;
    printf("        Inserisci un comando: ");
    scanf("%s", comm); int i;
    for (i = 0 ; i<strlen(comm); i++) { comm[i] = tolower(comm[i]); }
    if (strcmp(comm, "h") == 0 || strcmp(comm, "help") == 0) {
      free(comm);
      print_help();
    }
    else if (strcmp(comm, "q") == 0 || strcmp(comm, "quit") == 0) {
      free(comm);
      break;
    }
    else if (strcmp(comm, "t") == 0 || strcmp(comm, "terminate") == 0) {
      free(comm);
      printf("        Inserisci il pid: ");
      scanf("%d", &pid);
      res_signal = terminate(pid);
      if (res_signal == 0) printf("        Terminato con successo!\n");
    }
    else if (strcmp(comm, "k") == 0 || strcmp(comm, "kill") == 0) {
      free(comm);
      printf("        Inserisci il pid: ");
      scanf("%d", &pid);
      res_signal = kill_p(pid);
      if (res_signal == 0) printf("        Terminato con successo!\n");
    }
    else if (strcmp(comm, "s") == 0 || strcmp(comm, "suspend") == 0) {
      free(comm);
      printf("          Inserisci il pid: ");
      scanf("%d", &pid);
      res_signal = suspend(pid);
      if (res_signal == 0) printf("        Sospeso con successo!\n");
    }
    else if (strcmp(comm, "r") == 0 || strcmp(comm, "resume") == 0) {
      free(comm);
      printf("        Inserisci il pid: ");
      scanf("%d", &pid);
      res_signal = terminate(pid);
      if (res_signal == 0) printf("        Ripreso con successo!\n");
    }
    else if (strcmp(comm, "u") == 0 || strcmp(comm, "update") == 0) {
      free(comm);
      goto N;
    }
    else {
      printf("        ATTENZIONE! Comando non riconosciuto\n");
      printf("        Inserisci h(elp) per una lista dei comandi disponibili\n");        
    }
  }
  
  closedir(dir);
  
  return 0;
  
}
