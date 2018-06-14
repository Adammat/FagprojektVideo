For at få thread til at virke skal følgende ting være tilstede:

Includes:
#include "cpipetest.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Funktioner:
int getVCL();
int getFPS();
int getRES();
int setVCL(int vcl2);
int setFPS(int fps2);
int setRES(int res2);

Kode i main: (Kan måske optimeres)
pthread_t thread[1];
  int result_code;
  unsigned index;

  //Start the thread
  result_code = pthread_create(&thread[0], NULL, localSocket_thread, NULL);
  assert(!result_code);



COMPILING (VIGTIGT!):
Koden skal compiles med koden:
gcc -pthread -o program threadMainTest.c cpipetest.c

Hvor: program er output programmet (.o)
      thredMainTest.c er main koden der starter threaten.
      cpipetest.c er koden der indeholder localSocket_thread()

I cpiptesttest VIGTIGT!:
Thread programmet skal include headeren til main programmet, som inkludere de ovenstående funktioner.
