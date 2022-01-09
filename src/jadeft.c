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
  struct statement *ptr;
  CDKSCREEN *cdkscreen = 0;
  CDKMENU *menu = 0;
  int submenusize[5], menuloc[5];
  int selection;
  char temp[256];
  const char *mesg[2];
 
  cdkscreen = initCDKScreen(NULL);
  initCDKColor();                          /* needs to come after initCDKscreen */

  menulist[0][0] = "</07/B>File           <!07>";
  menulist[0][1] = "</05>NOT IN USE       <!05>";
  menulist[0][2] = "</05>Exit             <!05>";

  menulist[1][0] = "</07/B>Provider Admin <!07>";
  menulist[1][1] = "</05>Provider Type    <!05>";
  menulist[1][2] = "</05>Provider Insert  <!05>";
  menulist[1][3] = "</05>Provider Account <!05>";

  menulist[2][0] = "</07/B>Supplier Admin <!07>";
  menulist[2][1] = "</05>Supplier Type    <!05>";
  menulist[2][2] = "</05>Supplier Insert  <!05>";
  menulist[2][3] = "</05>Supplier Account <!05>";
  menulist[2][4] = "</05>Invoice          <!05>";

  menulist[3][0] = "</07/B>Other Admin    <!07>";
  menulist[3][1] = "</05>Property         <!05>";
  menulist[3][2] = "</05>Payment period   <!05>";
  menulist[3][3] = "</05>Category         <!05>";
  menulist[3][4] = "</05>Statement Config <!05>";
  menulist[3][5] = "</05>Statement Upload <!05>";

  menulist[4][0] = "</07/B>Documents      <!07>";
  menulist[4][1] = "</05>Document Type    <!05>";
  menulist[4][2] = "</05>Document Insert  <!05>";
  
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

  //  mesg[0]= "                         ";

  menu = newCDKMenu(cdkscreen, menulist, 5, submenusize, menuloc,
		    TOP, A_UNDERLINE, A_REVERSE);

  refreshCDKScreen(cdkscreen);

  while (selection != 1)
    {
      selection = activateCDKMenu(menu,0);
      switch(selection)
	{
	case 100:
	  provTypeInsert();
	  break;
	case 101:
	  provInsert();
	  break;
	case 102:
	  provAccountInsert();
	  break;
	case 200:	
	  suppTypeInsert();
	  break;
	case 201:	 
	  suppInsert();
	  break;
	case 202:
	  suppAccountInsert();	  
	  break;
	case 203:
	  invInsert();
	  break;
	case 300:
	  propertyInsert();
	  break;
	case 301:
	  paymentPeriodInsert();
	  break;
	case 302:
	  categoryInsert();
	  break;
	case 303:
	  statementLinkInsert();
	  break;
	case 304:	  
	  upLoadStatement();	  
	  break;
	case 400:
	  documentTypeInsert();
	  break;
	case 401:
	  documentInsert();
	  break;
	default:
	  sprintf(temp,"No selection");
	  break;
	}
    }

  /*  if (menu->exitType == vEARLY_EXIT)
    {
      mesg[0] = "<C>You hit escape. No menu item was selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
      }*/
  if (menu->exitType == vNORMAL)
    {     
      mesg[0] = "<C>Exit application.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
    }

  destroyCDKMenu(menu);
  destroyCDKScreen(cdkscreen);
  endCDK();
  
  return 0;
}
