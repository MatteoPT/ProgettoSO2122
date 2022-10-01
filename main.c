#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<unistd.h>

char* getDirectory(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + 1);
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  return res;
}

char* getDirStat(char* pid) {
  char* res = (char*) malloc(sizeof("/proc") + strlen(pid) + strlen("/stat") + 1);
  strcpy(res, "/proc");
  strcat(res, "/");
  strcat(res, pid);
  strcat(res, "/stat");
  return res;
}

//Full credits must be given to this question on Stack Overflow
//for the method used to calculate CPU usage percentage
//https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
double getCpuUsage(char* pid, int uptime) {
  char* dirStat;
  dirStat = getDirStat(pid);
  FILE *g = fopen(dirStat, "r");
  long unsigned int utime = 0;
  long unsigned int stime = 0;
  long int cutime = 0;
  long int cstime = 0;
  long long unsigned int starttime = 0;
  long unsigned int hertz = sysconf(_SC_CLK_TCK);
  fscanf(g, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %llu", &utime, &stime, &cutime, &cstime, &starttime);
  fclose(g);
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
  if (fm == NULL) {
    printf("Errore nell'apertura della directory '/proc/meminfo'\n");
    exit(EXIT_FAILURE);
  }
  char memStr[256];
  int mem;
  while(fgets(memStr, 256, fm) != NULL) {
    if (sscanf(memStr, "MemTotal: %d kB", &mem) == 1) {
      res = mem*1024.0;
    }
  }
  fclose(fm);
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
  
  FILE *f = fopen("/proc/uptime", "r");
  int uptime = 0;
  fscanf(f, "%d", &uptime);
  fclose(f);

  printf("IN THREAD\n");
  //Una volta nel thread, stampo i PID di tutti i processi
  while( (proc_r = readdir(dir)) != NULL) {
  
    double cpu_usage = getCpuUsage(proc_r->d_name, uptime);
     
    printf("%s || %.3f % \n", proc_r->d_name, cpu_usage);

    char* directory;
    directory = getDirectory(proc_r->d_name);
    
    DIR* pid_dir;
    
    if( (pid_dir = opendir(directory)) == NULL) {
      //Gestione errori
      printf("Errore nell'apertura di %s\n", directory);
      exit(EXIT_FAILURE);
    }
    
    closedir(pid_dir);
    
    free(directory);
    
  }

  
  closedir(dir);
  
}
