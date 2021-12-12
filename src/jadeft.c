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
  struct statement *ptr;
  //system("clear");
  
  CDKSCREEN *cdkscreen = 0;
  CDKMENU *menu = 0;
  int submenusize[5], menuloc[5];
  int selection;
  char temp[256];
  const char *mesg[2];
  initCDKColor();
 
  cdkscreen = initCDKScreen(NULL);

  menulist[0][0] = "</B>File         <!B>";
  menulist[0][1] = "NOT IN USE       ";
  menulist[0][2] = "Exit             ";

  menulist[1][0] = "Provider Admin   ";
  menulist[1][1] = "Provider Type    ";
  menulist[1][2] = "Provider Insert  ";
  menulist[1][3] = "Provider Account ";

  menulist[2][0] = "Supplier Admin   ";
  menulist[2][1] = "Supplier         ";
  menulist[2][2] = "Supplier Type    ";
  menulist[2][3] = "Supplier Account ";
  menulist[2][4] = "Invoice          ";

  menulist[3][0] = "Other Admin      ";
  menulist[3][1] = "Property         ";
  menulist[3][2] = "Payment period   ";
  menulist[3][3] = "Category         ";
  menulist[3][4] = "Statement Config ";
  menulist[3][5] = "Statement Upload ";

  menulist[4][0] = "Documents        ";
  menulist[4][1] = "Document Type    ";
  menulist[4][2] = "Document Insert  ";
  
  submenusize[0] = 3;
  submenusize[1] = 4;
  submenusize[2] = 5;
  submenusize[3] = 6;
  submenusize[4] = 3;

  menuloc[0] = LEFT;
  menuloc[1] = LEFT;
  menuloc[2] = LEFT;
  menuloc[3] = LEFT;
  menuloc[4] = LEFT;

  mesg[0]= "                         ";

  menu = newCDKMenu(cdkscreen, menulist, 5, submenusize, menuloc,
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
	case 200:
	  suppInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 201:
	  suppTypeInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 202:
	  suppAccountInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 300:
	  propertyInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 301:
	  paymentPeriodInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 302:
	  categoryInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 303:
	  statementLinkInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 304:
	  ptr = upLoadStatement();
	  printStmtFile(ptr);
	  statementInsert(ptr); 
	  freeStatement(ptr);
	  selection = activateCDKMenu(menu, 0);
	case 400:
	  documentTypeInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	case 401:
	  documentInsert();
	  selection = activateCDKMenu(menu, 0);
	  break;
	default:
	  sprintf(temp,"No selection");
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
