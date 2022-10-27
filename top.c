#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<pwd.h>

#include "top.h"

#define PAGESIZE  sysconf(_SC_PAGE_SIZE)
#define HERTZ     sysconf(_SC_CLK_TCK)

char* getDirectory(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + 1);
  if (res == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  return res;
}


char* getDirStat(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + strlen("/stat") + 1);
  if (res == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  strcat(res, "/stat");
  return res;
}

char* getDirStatus(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + strlen("/status") + 1);
  if (res == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  strcat(res, "/status");
  return res;
}

char* getDirStatm(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + strlen("/stat") + 2);
  if (res == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  strcat(res, "/statm");
  return res;
}


//Full credits must be given to this question on Stack Overflow
//for the method used to calculate CPU usage percentage
//https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
double getCpuUsage(char* pid, int uptime) {
  char* dirStat = getDirStat(pid);
  FILE *g = fopen(dirStat, "r");
  if (g == NULL) { printf("Errore nell'apertura della directory '/proc/%s/stat'\n", pid); exit(EXIT_FAILURE); }
  long unsigned int utime = 0;
  long unsigned int stime = 0;
  long int cutime = 0;
  long int cstime = 0;
  long long unsigned int starttime = 0;
  fscanf(g, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %llu", &utime, &stime, &cutime, &cstime, &starttime);
  if ( fclose(g) == EOF ) { printf("Errore nella chiusura della directory '/proc/%s/stat'\n", pid); exit(EXIT_FAILURE); }
  free(dirStat);
  long unsigned int total_time = utime + stime + cutime + cstime;
  long unsigned int seconds = uptime - (starttime/HERTZ);
  total_time = total_time / HERTZ;
  double cpu_usage = 100*( (double) total_time / (double) seconds);
  return cpu_usage;
}

void getName(char* pid, char* copy) {
  char* dirStatus = getDirStatus(pid);
  FILE *n = fopen(dirStatus, "r");
  if (n == NULL) { printf("Errore nell'apertura della directory '/proc/%s/status'\n", pid); exit(EXIT_FAILURE); }
  char name[256];
  char nameStr[256];
  while(fgets(nameStr, 256, n) != NULL) {
    if (sscanf(nameStr, "Name: %s", name) == 1) {
      break;
    }
  }
  if ( fclose(n) == EOF ) { printf("Errore nella chiusura della directory '/proc/%s/status'\n", pid); exit(EXIT_FAILURE); }
  free(dirStatus);
  strcpy(copy, name);
  return;
}

void getUser(char* pid, char* copy) {
  char* dirStatus = getDirStatus(pid);
  FILE *u = fopen(dirStatus, "r");
  if (u == NULL) { printf("Errore nell'apertura della directory '/proc/%s/status'\n", pid); exit(EXIT_FAILURE); }
  int user;
  char userStr[256];
  while(fgets(userStr, 256, u) != NULL) {
    if (sscanf(userStr, "Uid: %d %*d %*d %*d", &user) == 1) {
      break;
    }
  }
  if ( fclose(u) == EOF ) { printf("Errore nella chiusura della directory '/proc/%s/status'\n", pid); exit(EXIT_FAILURE); }
  free(dirStatus);
  struct passwd* uspw = getpwuid(user);
  strcpy(copy, uspw->pw_name);
  return;
}


double getMemSize() {
  double res = 0;
  FILE *fm = fopen("/proc/meminfo", "r");
  if (fm == NULL) { printf("Errore nell'apertura della directory '/proc/meminfo'\n"); exit(EXIT_FAILURE); }
  char memStr[256];
  int mem;
  while(fgets(memStr, 256, fm) != NULL) {
    if (sscanf(memStr, "MemTotal: %d kB", &mem) == 1) {
      res = mem*1024.0;
    }
  }
  if (fclose(fm) == EOF) { printf("Errore nella chiusura della directory '/proc/meminfo'\n"); exit(EXIT_FAILURE); }
  return res;
}

double getMemUsage(char* pid) {
  double res = 0;
  double memUsed = 0;
  double memSize = getMemSize();
  char* dirStatm = getDirStatm(pid);
  FILE *fmem = fopen(dirStatm, "r");
  if (fmem == NULL) { printf("Errore nell'apertura della directory '/proc/%s/statm'\n", pid); exit(EXIT_FAILURE); }  
  fscanf(fmem, "%*d %*d %*d %*d %*d %lf", &memUsed);
  if (fclose(fmem) == EOF) { printf("Errore nella chiusura della directory '/proc/%s/statm'\n", pid); exit(EXIT_FAILURE); }
  free(dirStatm);
  memUsed = memUsed * PAGESIZE;
  res = 100 * ((double) memUsed / (double) memSize);
  return res;
}

void print_help() {
  printf("\033[30m\033[47m");
  printf("\n        (H)ELP -> Mostra i comandi disponibili\n");
  printf("        (T)ERMINATE -> Termina il processo    \n");
  printf("        (K)ILL -> Terma il processo immediatamente\n");
  printf("        (S)USPEND -> Mette in pausa il processo\n");
  printf("        (R)ESUME -> Riprende il processo messo in pausa\n");
  printf("        (U)PDATE -> Effettua un refresh della lista\n");
  printf("        (Q)UIT -> Esce del programma\n");
  printf("\033[0m\033[K\n");
}

int terminate(int pid) {
  printf("\n        Termino il processo %d...\n", pid);
  int res = kill( (pid_t) pid, SIGTERM);
  if ( res == -1 ) {
    if (errno == EPERM) {
      printf("        Non hai i permessi necessari per terminare il processo con PID %d\n", pid);
    }
    else if (errno == ESRCH) {
      printf("        Il processo con PID %d non esiste!\n", pid);
    }
  }
  return res;
}

int kill_p(int pid) {
  printf("\n        Termino il processo %d...\n", pid);
  int res = kill( (pid_t) pid, SIGKILL);
  if ( res == -1 ) {
    if (errno == EPERM) {
      printf("        Non hai i permessi necessari per terminare il processo con PID %d\n", pid);
    }
    else if (errno == ESRCH) {
      printf("        Il processo con PID %d non esiste!\n", pid);
    }
  }
  return res;
}

int suspend(int pid) {
  printf("\n        Sospendo il processo %d...\n", pid);
  int res = kill( (pid_t) pid, SIGSTOP);
  if ( res == -1 ) {
    if (errno == EPERM) {
      printf("      Non hai i permessi necessari per sospendere il processo con PID %d\n", pid);
    }
    else if (errno == ESRCH) {
      printf("      Il processo con PID %d non esiste!\n", pid);
    }
  }
  return res;
}

int resume(int pid) {
  printf("\n        Riprendo il processo %d...\n", pid);
  int res = kill( (pid_t) pid, SIGCONT);
  if ( res == -1 ) {
    if (errno == EPERM) {
      printf("      Non hai i permessi necessari per riprendere il processo con PID %d\n", pid);
    }
    else if (errno == ESRCH) {
      printf("      Il processo con PID %d non esiste!\n", pid);
    }
  }
  return res;
}


