#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
//#include <form.h>
//#include <ncurses.h>
//#include <panel.h>
#include "../inc/fdbcon.h"
#include "../inc/prolib.h"
#include "../inc/procur.h"
#include "../inc/supcur.h"
#include "../inc/invcur.h"
#include "../inc/doccur.h"
#include "../inc/catcur.h"
#include "../inc/stmuplf.h"
#include "../inc/stmuplibf.h"

int main (void) {
  int x;
  struct statement *ptr;
  system("clear");   // stdlib.h
  //  connectToDB();

  /*  initscr();  
   cbreak();
 noecho();
 keypad(stdscr, TRUE);*/


  //printw("Lines %d Cols %d\n", LINES, COLS);
  printf("Enter 1 to continue: ");
  //refresh();
  scanf("%d",&x);
  //refresh();

  /* provider schema */
  // provInsert();  
  // provTypeInsert();   
  // provAccountInsert();

  /* supplier schema */
  // suppInsert();
  //   suppTypeInsert();
  // propertyInsert();
  // paymentPeriodInsert();
  // suppAccountInsert();
  // provAccountInsert();

  /* invoice schema */
  // invInsert();

  /*document schema */
  //documentTypeInsert();
  // documentInsert();

  /* statement schema */

  categoryInsert();

  // fStmtName();
   ptr = upLoadStatement();
   printStmtFile(ptr);

   statementInsert(ptr);

      
   //printStatement(ptr);

   // printStatement_new(ptr);
  //categoryInsert();

  // statementLinkInsert();
  //upLoadStatement();

  //  categoryInsert();
  

    /* Upload Statment */
  //upLoadStatement();

  //  upLoadStatement();

    
   // categoryInsert();

  freeStatement(ptr);
  
  printf("Completed Enter 1 to exit: ");
  //refresh();
  scanf("%d", &x);


  //endwin();
  
  return 0;
}
