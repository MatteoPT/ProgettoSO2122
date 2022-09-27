#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <dlfcn.h>


#include "disastrOS.h"


void initFunction(void* args) {

}

typedef void (*fun_prova_handle)();


fun_prova_handle fun_prova; 
void *fun_prova_lib;

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  
  printf("\nINIZIO PROVA DLOPEN\n");
  
  fun_prova_lib = dlopen("./libs/lib_prova.so", RTLD_LAZY | RTLD_DEEPBIND);

  fun_prova = (fun_prova_handle) (dlsym(fun_prova_lib, "fun_prova"));
    
  fun_prova();

  dlclose(fun_prova_lib);
  
  printf("FINE PROVA DLOPEN\n");
  
  return 0;
}
