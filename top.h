#pragma once

char* getDirectory(char* pid);
char* getDirStat(char* pid);
char* getDirStatm(char* pid);
char* getDirStatus(char* pid);
double getCpuUsage(char* pid, int uptime);
double getMemSize();
double getMemUsage(char* pid);
void getName(char* pid, char* copy);
void getUser(char* pid, char* copy);
void print_help();
int terminate(int pid);
int kill_p(int pid);
int suspend(int pid);
int resume(int pid);
