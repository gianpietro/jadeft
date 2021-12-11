#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <cdk.h>
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

static const char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS];

int main (void) {
   //int x;
  //struct statement *ptr;
  //system("clear");
  
  CDKSCREEN *cdkscreen = 0;
  CDKMENU *menu = 0;
  int submenusize[2], menuloc[2];
  int selection;
  char temp[256];
  const char *mesg[2];
  initCDKColor();
 
  cdkscreen = initCDKScreen(NULL);

  menulist[0][0] = "</B>File         <!B>";
  menulist[0][1] = "NOT IN USE       ";
  menulist[0][2] = "Exit             ";

  menulist[1][0] = "Provider         ";
  menulist[1][1] = "Provider Type    ";
  menulist[1][2] = "Provider Insert  ";
  menulist[1][3] = "Provider Account ";

  submenusize[0] = 3;
  submenusize[1] = 4;

  menuloc[0] = LEFT;
  menuloc[1] = LEFT;

  mesg[0]= "                         ";

  menu = newCDKMenu(cdkscreen, menulist, 2, submenusize, menuloc,
		    TOP, A_UNDERLINE, A_REVERSE);

  refreshCDKScreen(cdkscreen);

  selection = activateCDKMenu(menu, 0);

  while (selection != 1)
    {
      switch(selection)
	{
	case 100:
	  provTypeInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 101:
	  provInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 102:
	  provAccountInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	}
    } 
  
  /*  initscr();  
   cbreak();
 noecho();
 keypad(stdscr, TRUE);*/


  //printw("Lines %d Cols %d\n", LINES, COLS);
  //  printf("Enter 1 to continue: ");
  //refresh();
  // scanf("%d",&x);
  //refresh();

  /* provider schema */
  //provInsert();  
  //provTypeInsert();   
  //provAccountInsert();

  /* supplier schema */
  //suppInsert();
  //suppTypeInsert();
  //propertyInsert();
  //paymentPeriodInsert();
  //suppAccountInsert();
  

  /* invoice schema */
  //invInsert();

  /*document schema */
  //documentTypeInsert();
  //documentInsert();

  /* statement schema */     
  //categoryInsert();
  //statementLinkInsert();
  
    /*upload stmt functions */
  /*  ptr = upLoadStatement();
    printStmtFile(ptr);
    statementInsert(ptr); 
    freeStatement(ptr); */

  sprintf(temp,"selection %d", selection);
  mesg[0] = temp;
  popupLabel(cdkscreen, (CDK_CSTRING2) mesg, 1);

  destroyCDKMenu(menu);
  destroyCDKScreen(cdkscreen);
  endCDK();

  
    //  printf("Completed Enter 1 to exit: ");
  //refresh();
  //scanf("%d", &x);

  //endwin();
  
  return 0;
}
