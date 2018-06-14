#include "cpipetest.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int vcl = 1;
int fps = 60;
int res = 720;

int main(int argc, char** argv) {
  pthread_t thread[1];
  int result_code;
  unsigned index;

  //Start the thread
  result_code = pthread_create(&thread[0], NULL, localSocket_thread, NULL);
  assert(!result_code);

  //THIS IS ONLY TO KEEP MAIN RUNNING
  while(1){

  }
  exit(EXIT_SUCCESS);
}

int getVCL(){
	return vcl;
}

int getFPS(){
	return fps;
}

int getRES(){
	return res;
}

int setVCL(int vcl2){
	vcl = vcl2;
	return 1;
}

int setFPS(int fps2){
	fps = fps2;
	return 1;
}

int setRES(int res2){
	res = res2;
	return 1;
}
