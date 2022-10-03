#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>


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
  long unsigned int hertz = sysconf(_SC_CLK_TCK);
  fscanf(g, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %llu", &utime, &stime, &cutime, &cstime, &starttime);
  if ( fclose(g) == EOF ) { printf("Errore nella chiusura della directory '/proc/%s/stat'\n", pid); exit(EXIT_FAILURE); }
  free(dirStat);
  long unsigned int total_time = utime + stime + cutime + cstime;
  long unsigned int seconds = uptime - (starttime/hertz);
  total_time = total_time / hertz;
  double cpu_usage = 100*( (double) total_time / (double) seconds);
  return cpu_usage;
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
  long pageSize = sysconf(_SC_PAGE_SIZE);
  char* dirStatm = getDirStatm(pid);
  FILE *fmem = fopen(dirStatm, "r");
  if (fmem == NULL) { printf("Errore nell'apertura della directory '/proc/%s/statm'\n", pid); exit(EXIT_FAILURE); }  
  fscanf(fmem, "%*d %*d %*d %*d %*d %lf", &memUsed);
  if (fclose(fmem) == EOF) { printf("Errore nella chiusura della directory '/proc/%s/statm'\n", pid); exit(EXIT_FAILURE); }
  free(dirStatm);
  memUsed = memUsed * pageSize;
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


int main() {

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
  printf("        PID             Memory%%         CPU%%          ");
  printf("\033[0m\033[K\n");

  while( (proc_r = readdir(dir)) != NULL) {
  
    double cpu_usage = getCpuUsage(proc_r->d_name, uptime);
    
    double mem_usage = getMemUsage(proc_r->d_name);
     
    printf("        %s\t\t%.3f%\t\t%.3f%\n", proc_r->d_name, mem_usage, cpu_usage);
    
  }

  printf("\033[30m\033[47m");
  printf("        Inserisci h(elp) per i comandi disponibili       ");
  printf("\033[0m\033[K\n");
  
  while(1) {
    char* comm = (char*) malloc(10);
    if (comm == NULL) { printf("Errore malloc\n"); exit(EXIT_FAILURE); }
    int pid; int res_signal;
    printf("        Inserisci un comando: ");
    scanf("%s", comm);
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
    else {
      printf("        ATTENZIONE! Comando non riconosciuto\n");
      printf("        Inserisci h(elp) per una lista dei comandi disponibili\n");        
    }
  }
  
  closedir(dir);
  
  return 0;
  
}
