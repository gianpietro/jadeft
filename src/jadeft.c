#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
//#include <form.h>
#include "../inc/fdbcon.h"
#include "../inc/prolib.h"
#include "../inc/procur.h"
#include "../inc/supcur.h"

int main (void) {
  int x;

  system("clear");   // stdlib.h
  //  connectToDB();

  printf("Enter 1 to continue: ");
  scanf("%d",&x);

  // provTypeInsert();
  // provInsert();  
  // provAccountInsert();
  // suppInsert();
  // suppTypeInsert();
  // paymentPeriodInsert();
   propertyInsert();


  // suppAccountInsert();

  
  // system("clear");
   printf("Completed Enter 1 to exit: ");
  scanf("%d", &x);
  
  return 0;
}
