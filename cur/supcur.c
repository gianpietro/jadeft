#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/supcur.h"
#include "../inc/suplib.h"
#include "../inc/jadlib.h"

void suppInsert()
{
  FIELD *supplierField[3];
  FORM *supplierForm;
  WINDOW *supWin, *supUpdateWin;
  PANEL *supPanel, *mainPanel;
  int ch;
  int cf, cfUpdate = 0;
  int list = 2, i = 0, j = 0;
  int actInd;
  char sname[30];
  int newRec = 'y';
  int rows, cols, urows, ucols;
  char p;
  int trows, val, upID, *params[1], length[1],  formats[1];
  char *titleOne = "Supplier Type Form";
  char *titleTwo = "Supplier Type";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
 
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);        /* INIT_PAIR_FOR_WIN_COLOR */
  init_pair(9,COLOR_WHITE,COLOR_BLACK);      /* INIT_PAIR_FOR_FIELD_COLOR */

    while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      supplierField[0] = new_field(1, 1, 4, 22, 0, 0);      
      supplierField[1] = new_field(1, 30, 6, 22, 0, 0);      
      supplierField[2] = NULL;

      set_field_fore(supplierField[0], COLOR_PAIR(9));    /* SET_FIELD_FOREGROUND */
      set_field_back(supplierField[0], COLOR_PAIR(9));    /* SET_FIELD_BACKGROUND */
      set_field_fore(supplierField[1], COLOR_PAIR(9));
      set_field_back(supplierField[1], COLOR_PAIR(9));

      /* Field 1 digit allowed in RANGE from 1 to 2 */
      set_field_type(supplierField[0],TYPE_INTEGER,1,1,2);
      /* Field allowed values of A-Z a-z and hyphen */
      set_field_type(supplierField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      supplierForm = new_form(supplierField);
      scale_form(supplierForm, &rows, &cols);

      /* Add window which will be associated to form */
      supWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      supUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);

      supPanel = new_panel(supUpdateWin);
      mainPanel = new_panel(supWin);
      wbkgd(supWin, COLOR_PAIR(1));        /* MAIN_WINDOW_BACKGROUND_COLOR */    
      update_panels();
      doupdate();
      
      keypad(supWin, TRUE);
      keypad(supUpdateWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(supplierForm, supWin);
      set_form_sub(supplierForm, derwin(supWin,rows,cols,2,2));
      getmaxyx(supWin,rows,cols);
      getmaxyx(supUpdateWin, urows, ucols);
      box(supWin, 0,0);
      box(supUpdateWin,0,0);
          
      if (supWin == NULL || supUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      // waddstr(supWin,"Supplier Entry Form");
      wattron(supWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(supWin,1,(cols-lenOne)/2, titleOne);   /* SET_MAIN_WIND_TITLE */  
      wattroff(supWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */

      post_form(supplierForm); 
      wrefresh(supWin);

      //mvwprintw(supWin,y+2,x+5,"Jade Finacial Tracker");
      //mvwprintw(supWin,24, 5, "rows %d cols %d", rows, cols);
      mvwprintw(supWin,rows-(rows-6),cols-(cols-5), "Active Ind:");                           
      mvwprintw(supWin,rows-(rows-8),cols-(cols-5), "Supplier Name:");
      mvwprintw(supWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(supWin,rows-(rows-6),cols-(cols-24));     /* move cursor */

      while((ch = wgetch(supWin)) != KEY_F(1))	
	{
	  hide_panel(supPanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, supplierForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(supUpdateWin);
	      box(supUpdateWin,0,0);
	      //waddstr(supUpdateWin, "Supplier");
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supUpdateWin,1,(cols-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supUpdateWin,1,1);
	      show_panel(supPanel);
	      wbkgd(supUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(supUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier WHERE active_ind = 1 ORDER BY supplier_id");	  
	      trows = PQntuples(res);

	      wrefresh(supUpdateWin);
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supUpdateWin, 4, 1, "ID    Supplier");  //+3
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	      while((p = wgetch(supUpdateWin)) == '\n')
		{		 
		  if ( j + RANGE < trows)  
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supUpdateWin);
		      box(supUpdateWin,0,0);		     
		    }
		  list = 6;      
		  //wclrtoeol(supUpdateWin);  
		  if  (i == trows)
		    {
		      wclrtobot(supUpdateWin);  
		      mvwprintw(supUpdateWin,urows-8,1,"End of list");
		      box(supUpdateWin,0,0);
		      // mvwprintw(supUpdateWin,0,0, "Supplier");
		      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supUpdateWin,1,(cols-lenTwo)/2,titleTwo);    /* SET_SUB_WIN_TITLE */ 
		      //mvwprintw(proUpdateWin,0,0, "Provider");
		      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supUpdateWin,urows-8,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(supUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(supUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      //mvwscanw(supUpdateWin,11,25, "%5s", &supIDstr);
	      //set_field_buffer(supplierField[2],0, supIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      //supID = atoi(field_buffer(supplierField[2],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier WHERE supplier_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  // mvwprintw(supUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  // mvwprintw(supUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(supUpdateWin);
		  set_field_buffer(supplierField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(supplierField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supUpdateWin,urows-6,1,"Number invalid");
		  wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(supUpdateWin);		
		  //wrefresh(supWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
      } //while F1
      hide_panel(supPanel);
      update_panels();
      doupdate();
	
      form_driver(supplierForm,REQ_VALIDATION);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(supplierField[0],0));
      strcpy(sname, field_buffer(supplierField[1],0));

      if ((form_driver(supplierForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*sname)))
	{
	  strcpy(sname, trimWS(sname));  
	  echo();
	  wattron(supWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(supWin,rows-8,cols-65,"Save: y/n: ");
	  wattroff(supWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
          mvwprintw(supWin,rows-7,cols-65,"(d = delete record)");
          wmove(supWin,rows-8,cols-54);	 
	  while((cf = wgetch(supWin)) != 'y')
	    {
	      wmove(supWin,rows-8,cols-54);
	      if (cf == 'n')
		{
	 	  mvwprintw(supWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	      /* NEW CODE FOR DELETE */
	      if (cf == 'd')
		{  
		      supplierDelete(upID);
		      wattron(supWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		      mvwprintw(supWin,rows-6,cols-65, "Record deleted");
		      wattroff(supWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		      break;
		 }		 
	      /* END OF NEW CODE FOR DELETE */
           }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  supplierUpdate(upID,actInd,sname);
		  wattron(supWin, A_BOLD | A_BLINK);
		  mvwprintw(supWin,rows-6,cols-65, "Data updated");
		  wattroff(supWin, A_BOLD | A_BLINK);
		  //mvwprintw(supWin,20,5, "cfUpdate %d,upID %d actInd %d sname %s", cfUpdate,upID,actInd, sname);
		}
	      else
		{
		  supplierInsert(actInd,sname);   /* Save data to database */
		  wattron(supWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
	          mvwprintw(supWin,rows-6,cols-65, "Data saved");
		  wattroff(supWin, A_BOLD | A_BLINK);    /* ATTOFF_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(supWin,rows-6,cols-65, "Data invalid");
	}
      noecho();
      
      unpost_form(supplierForm);
      free_form(supplierForm);
      free_field(supplierField[0]);
      free_field(supplierField[1]);
      free_field(supplierField[2]);

      cfUpdate = 0;
      mvwprintw(supWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supWin)) != 'y')
	{
	  wmove(supWin,rows-4,cols-28);	  
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(supWin);
    }
    PQfinish(conn);  
  endwin();
}


void suppTypeInsert()
{
  FIELD *supTypeField[2];
  FORM *supTypeForm;
  WINDOW *supTypeWin, *supTypeUpdateWin;
  PANEL *supTypePanel, *mainPanel;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char sdesc[30];
  int cf;
  int cfUpdate = 0;
  int list = 2, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];
  char *titleOne = "Supplier Type Form";
  char *titleTwo = "Supplier Type";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      supTypeField[0] = new_field(1,30,6,22,0,0);
      supTypeField[1] = NULL;
      set_field_type(supTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      set_field_fore(supTypeField[0], COLOR_PAIR(9));    /* SET_FIELD_FOREGROUND */
      set_field_back(supTypeField[0], COLOR_PAIR(9));    /* SET_FIELD_BACKGROUND */

      supTypeForm = new_form(supTypeField);
      scale_form(supTypeForm, &rows, &cols);
      supTypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      supTypeUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);

      supTypePanel = new_panel(supTypeUpdateWin);
      mainPanel = new_panel(supTypeWin);
      wbkgd(supTypeWin, COLOR_PAIR(1));        /* MAIN_WINDOW_BACKGROUND_COLOR */    
      update_panels();
      doupdate();
      
      keypad(supTypeWin, TRUE);
      keypad(supTypeUpdateWin, TRUE);
      
      set_form_win(supTypeForm, supTypeWin);	
      set_form_sub(supTypeForm, derwin(supTypeWin,rows,cols,2,2));
      getmaxyx(supTypeWin,rows,cols);
      getmaxyx(supTypeUpdateWin, urows, ucols);
      box(supTypeWin,0,0);
      box(supTypeUpdateWin,0,0);
          
      if (supTypeWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      //waddstr(supTypeWin,"Supplier Type Form");
      wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(supTypeWin,1,(cols-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
      wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      post_form(supTypeForm); 
      wrefresh(supTypeWin);

      mvwprintw(supTypeWin,rows-(rows-8),cols-(cols-5), "Description:");
      mvwprintw(supTypeWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(supTypeWin,rows-(rows-8),cols-(cols-24));     /* move cursor */

      while((ch = wgetch(supTypeWin)) != KEY_F(1))
	{
	  hide_panel(supTypePanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();	  
	  keyNavigate(ch, supTypeForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(supTypeUpdateWin);
	      box(supTypeUpdateWin,0,0);
	      // waddstr(supTypeUpdateWin, "Supplier Type");
	      wattron(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supTypeUpdateWin,1,(cols-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supTypeUpdateWin,1,1);
	      show_panel(supTypePanel);
	      wbkgd(supTypeUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	  
	      wrefresh(supTypeUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_type ORDER BY supplier_type_id");	  
	      trows = PQntuples(res);

	      wrefresh(supTypeUpdateWin);
	      wattron(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supTypeUpdateWin, 4, 1, "ID    Type");  //+3
	      wattroff(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(supTypeUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supTypeUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(supTypeUpdateWin);
		      box(supTypeUpdateWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(supTypeUpdateWin);  
		      mvwprintw(supTypeUpdateWin,urows-8,1,"End of list");
		      box(supTypeUpdateWin,0,0);
		      //mvwprintw(supTypeUpdateWin,0,0, "Supplier Type");
     	              wattron(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supTypeUpdateWin,1,(cols-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */ 		      
		      wattroff(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supTypeUpdateWin,urows-8,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(supTypeUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(supTypeUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier_type WHERE supplier_type_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  //mvwprintw(supTypeUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(supTypeUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  //wrefresh(supTypeUpdateWin);
		  set_field_buffer(supTypeField[0],0,PQgetvalue(res,0,1));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supTypeUpdateWin,urows-6,1,"Number invalid");
		  wattroff(supTypeUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(supTypeUpdateWin);		
		  //wrefresh(supTypeWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9	
	} // While F1
      hide_panel(supTypePanel);
      update_panels();
      doupdate();	  
      
      form_driver(supTypeForm,REQ_VALIDATION);

      strcpy(sdesc,field_buffer(supTypeField[0],0));
     
      if ((form_driver(supTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*sdesc)))
	{
	  strcpy(sdesc, trimWS(sdesc));
	  echo();
	  wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(supTypeWin,rows-8,cols-65, "Save y/n: ");
	  wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
	  mvwprintw(supTypeWin,rows-7,cols-65,"(d = delete record)");
	  wmove(supTypeWin,rows-8,cols-54);
	  while((cf = wgetch(supTypeWin)) != 'y')
	    {
	      wmove(supTypeWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(supTypeWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  supTypeDelete(upID);
		  wattron(supTypeWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		  mvwprintw(supTypeWin,rows-6,cols-65, "Record deleted");
		  wattroff(supTypeWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  supTypeUpdate(upID,sdesc); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(supTypeWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(supTypeWin,rows-6,cols-65, "Data updated");
		  wattroff(supTypeWin, A_BOLD | A_BLINK);
		  //mvwprintw(supTypeWin,20,5, "cfUpdate %d,upID %d sdesc %s", cfUpdate,upID,sdesc);  //DEBUG
		}
	      else
		{
		  supTypeInsert(sdesc);
		  wattron(supTypeWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(supTypeWin,rows-6,cols-65, "Data saved");
		  wattroff(supTypeWin, A_BOLD | A_BLINK);    /* ATTOFF_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(supTypeWin,rows-6,cols-65, "Data invalid");
	}
      noecho();

      unpost_form(supTypeForm);
      free_form(supTypeForm);
      free_field(supTypeField[0]);
      free_field(supTypeField[1]);

      cfUpdate = 0;
      mvwprintw(supTypeWin,rows-4,cols-65,"Do you want to update record y/n: ");
      echo();
      while((newRec = wgetch(supTypeWin)) != 'y')
	{
	  wmove(supTypeWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(supTypeWin);
    }  //while newRec=y
  PQfinish(conn);
  endwin();
}


void paymentPeriodInsert()
{
  FIELD *payPerField[2];
  FORM *payPerForm;
  WINDOW *payPerWin, *payPerUpdateWin;
  PANEL *payPerPanel, *mainPanel;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char payPer[30];
  int cf;
  int cfUpdate = 0;
  int list = 2, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];
  char *titleOne = "Payment Period Form";
  char *titleTwo = "Pay Period";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      payPerField[0] = new_field(1,30,6,22,0,0);
      payPerField[1] = NULL;

      set_field_fore(payPerField[0], COLOR_PAIR(9));
      set_field_back(payPerField[0], COLOR_PAIR(9));
      
      set_field_type(payPerField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      payPerForm = new_form(payPerField);
      scale_form(payPerForm, &rows, &cols);
      payPerWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      payPerUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);

      payPerPanel = new_panel(payPerUpdateWin);
      mainPanel = new_panel(payPerWin);
      wbkgd(payPerWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();
      
      keypad(payPerWin, TRUE);
      keypad(payPerUpdateWin, TRUE);

      set_form_win(payPerForm, payPerWin);	
      set_form_sub(payPerForm, derwin(payPerWin,rows,cols,2,2));
      getmaxyx(payPerWin,rows,cols);
      getmaxyx(payPerUpdateWin, urows, ucols);
      
      box(payPerWin, 0,0);
      box(payPerUpdateWin,0,0);
          
      if (payPerWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      //waddstr(payPerWin,"Payment Period Form");
      wattron(payPerWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(payPerWin,1,(cols-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
      wattroff(payPerWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      post_form(payPerForm); 
      wrefresh(payPerWin);

      mvwprintw(payPerWin,rows-(rows-8),cols-(cols-5), "Pay Period:");
      mvwprintw(payPerWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(payPerWin,rows-(rows-8),cols-(cols-24));     /* move cursor */

      while((ch = wgetch(payPerWin)) != KEY_F(1))
	{
	  hide_panel(payPerPanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, payPerForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(payPerUpdateWin);
	      box(payPerUpdateWin,0,0);
	      //waddstr(payPerUpdateWin, "Pay Period");
	      wattron(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(payPerUpdateWin,1,(cols-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(payPerUpdateWin,1,1);
	      show_panel(payPerPanel);
	      wbkgd(payPerUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(payPerUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM payment_period ORDER BY payment_period_id");	  
	      trows = PQntuples(res);

	      wrefresh(payPerUpdateWin);
	      wattron(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(payPerUpdateWin, 4, 1, "ID    Period");  //+3
	      wattroff(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(payPerUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(payPerUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(payPerUpdateWin);
		      box(payPerUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(payPerUpdateWin);  
		      mvwprintw(payPerUpdateWin,13,1,"End of list");
		      box(payPerUpdateWin,0,0);
		      // mvwprintw(payPerUpdateWin,0,0, "Pay Period");
		      wattron(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(payPerUpdateWin,1,(ucols-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */ 
		      //mvwprintw(proUpdateWin,0,0, "Provider");
		      wattroff(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(payPerUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(payPerUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(payPerUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM payment_period WHERE payment_period_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  //mvwprintw(payPerUpdateWin,13,1, "no or rows %d ",trows);  //DEBUG
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(payPerUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  //wrefresh(payPerUpdateWin);
		  set_field_buffer(payPerField[0],0,PQgetvalue(res,0,1));
		  //set_field_buffer(payPerField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(payPerUpdateWin,urows-6,1,"Number invalid");
		  wattroff(payPerUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(payPerUpdateWin);		
		  //wrefresh(payPerWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
	} //while F1
      hide_panel(payPerPanel);
      update_panels();
      doupdate();
      
      form_driver(payPerForm,REQ_VALIDATION);

      strcpy(payPer,field_buffer(payPerField[0],0));
     
      if ((form_driver(payPerForm,REQ_VALIDATION) == E_OK) && (!isspace(*payPer)))
	{
	  strcpy(payPer, trimWS(payPer));
	  echo();
	  wattron(payPerWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(payPerWin,rows-8,cols-65, "Save y/n: ");
	  wattroff(payPerWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
	  mvwprintw(payPerWin,rows-7,cols-65,"(d = delete record)");
	  wmove(payPerWin,rows-8,cols-54);
	  while((cf = wgetch(payPerWin)) != 'y')
	    {
	      wmove(payPerWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(payPerWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  payPerDelete(upID);
		  wattron(payPerWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		  mvwprintw(payPerWin,rows-6,cols-65, "Record deleted");
		  wattroff(payPerWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		  break;
		}
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  payPerUpdate(upID,payPer);
		  wattron(payPerWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(payPerWin,rows-6,cols-65, "Data updated");
		  wattroff(payPerWin, A_BOLD | A_BLINK);
		  //mvwprintw(payPerWin,rows-5,cols-46, "cfUpdate %d,upID %d payPer %s", cfUpdate,upID,payPer);  //DEBUG
		}
	      else
		{
		  payPeriodInsert(payPer);
		  wattron(payPerWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(payPerWin,rows-6,cols-65, "Data saved");
		  wattroff(payPerWin, A_BOLD | A_BLINK);    /* ATTOFF_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(payPerWin,rows-6,cols-65, "Data invalid");
	}
      noecho();

      unpost_form(payPerForm);
      free_form(payPerForm);
      free_field(payPerField[0]);
      free_field(payPerField[1]);

      cfUpdate = 0;

      mvwprintw(payPerWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(payPerWin)) != 'y')
	{
	  wmove(payPerWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(payPerWin);
    }
  PQfinish(conn);
  endwin();
}

void propertyInsert()
{
  FIELD *propertyField[5];
  FORM *propertyForm;
  WINDOW *prtWin, *prtUpdateWin;
  PANEL *prtPanel, *mainPanel;
  int ch;
  int cf;                 /* confirm save data */
  int actInd;             /* active_Ind */
  char prtPostCode[30];   /* post_code */
  char prtAddress[30];
  char prtCity[30];
  int newRec= 'y';        /* Add another new record */
  int rows, cols;
  int cfUpdate = 0;
  int list = 6, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];
  char *titleOne = "Property Entry Form";
  char *titleTwo = "Property";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);  

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      propertyField[0] = new_field(1, 1, 4, 28, 0, 0);      // active_ind
      propertyField[1] = new_field(1, 10, 6, 28, 0, 0);     // post_code
      propertyField[2] = new_field(1, 30, 8, 28, 0, 0);     // address
      propertyField[3] = new_field(1, 30, 10, 28, 0, 0);     // city
      propertyField[4] = NULL;

      set_field_fore(propertyField[0], COLOR_PAIR(9));
      set_field_back(propertyField[0], COLOR_PAIR(9));
      set_field_fore(propertyField[1], COLOR_PAIR(9));
      set_field_back(propertyField[1], COLOR_PAIR(9));
      set_field_fore(propertyField[2], COLOR_PAIR(9));
      set_field_back(propertyField[2], COLOR_PAIR(9));

      set_field_type(propertyField[0],TYPE_INTEGER,1,1,2);
      set_field_type(propertyField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[3],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      propertyForm = new_form(propertyField);
      scale_form(propertyForm, &rows, &cols);      

      /* Add window which will be associated to form */
      prtWin = newwin(LINES-25, COLS/3,LINES-(LINES-4),COLS/15);
      prtUpdateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      prtPanel = new_panel(prtUpdateWin);
      mainPanel = new_panel(prtWin);
      wbkgd(prtWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();

      keypad(prtWin, TRUE);
      keypad(prtUpdateWin, TRUE);

      /* Set main and sub windows */
      set_form_win(propertyForm, prtWin);
      set_form_sub(propertyForm, derwin(prtWin,rows,cols,2,2));
      getmaxyx(prtWin,rows,cols);
      getmaxyx(prtUpdateWin,urows,ucols);
      box(prtWin,0,0);
      box(prtUpdateWin,0,0);
      wattron(prtWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(prtWin,1,(cols-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
      wattroff(prtWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
          
      if (prtWin == NULL || prtUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      //waddstr(prtWin,"Property Entry Form");
      //mvwprintw(prtWin,29,1,"row %d col %d", rows, cols);

      post_form(propertyForm);
      wrefresh(prtWin);

      mvwprintw(prtWin,rows-(rows-6),cols-(cols-5), "Active Ind:");                           
      mvwprintw(prtWin,rows-(rows-8),cols-(cols-5), "Post Code:");
      mvwprintw(prtWin,rows-(rows-10),cols-(cols-5), "Address:");
      mvwprintw(prtWin,rows-(rows-12),cols-(cols-5), "City:");            
      mvwprintw(prtWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(prtWin,rows-(rows-6),cols-(cols-30));     /* move cursor */

      while((ch = wgetch(prtWin)) != KEY_F(1))
	{
	  hide_panel(prtPanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, propertyForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(prtUpdateWin);
	      box(prtUpdateWin,0,0);
	      //waddstr(prtUpdateWin, "Property");
	      wattron(prtUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(prtUpdateWin,1,(ucols-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(prtUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(prtUpdateWin,1,1);
	      show_panel(prtPanel);
	      wbkgd(prtUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(prtUpdateWin);

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM property WHERE active_ind = 1 ORDER BY property_id");	  
	      trows = PQntuples(res);

	      wrefresh(prtUpdateWin);
	      wattron(prtUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(prtUpdateWin, 4, 1, "ID     Post Code");  //+3
	      wattroff(prtUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(prtUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(prtUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(prtUpdateWin);
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(prtUpdateWin);  
		      mvwprintw(prtUpdateWin,13,1,"End of list");
		      box(prtUpdateWin,0,0);
		      //mvwprintw(prtUpdateWin,0,0, "Property");
		      wattron(prtUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(prtUpdateWin,1,(ucols-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(prtUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(prtUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(prtUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(prtUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(prtUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(prtUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM property WHERE property_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  // mvwprintw(prtUpdateWin,13,1, "no or rows %d ",trows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  // mvwprintw(prtUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(prtUpdateWin);
		  set_field_buffer(propertyField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(propertyField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(propertyField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(propertyField[3],0,PQgetvalue(res,0,4));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(prtUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(prtUpdateWin,urows-6,1,"Number invalid");
		  wattroff(prtUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(prtUpdateWin);		
		  wrefresh(prtWin);
		}
	      noecho();
	      PQclear(res);
	    } //while F9 
	} //while F1
      hide_panel(prtPanel);
      update_panels();
      doupdate();
	  
      form_driver(propertyForm,REQ_VALIDATION);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(propertyField[0],0));
      strcpy(prtPostCode, field_buffer(propertyField[1],0));
      strcpy(prtAddress, trimWS(field_buffer(propertyField[2],0)));
      strcpy(prtCity, trimWS(field_buffer(propertyField[3],0)));

      if ((form_driver(propertyForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*prtPostCode)))
	{
	  strcpy(prtPostCode, trimWS(prtPostCode));
	  echo();
	  wattron(prtWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(prtWin,rows-10,cols-64,"Save: y/n: ");
	  wattroff(prtWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(prtWin,rows-9,cols-64,"(d = delete record)");
	  wmove(prtWin,rows-10,cols-53);
	  while((cf = wgetch(prtWin)) != 'y')
	    {
	      wmove(prtWin,rows-10,cols-53);
	      if (cf == 'n')
		{
		  mvwprintw(prtWin,rows-10,cols-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  prtDelete(upID);
		  wattron(prtWin,A_BOLD | A_BLINK);
		  mvwprintw(prtWin,rows-8,cols-64, "Record deleted");
		  wattroff(prtWin,A_BOLD | A_BLINK);
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  prtUpdate(upID, actInd, prtPostCode, prtAddress, prtCity); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(prtWin,rows-8,cols-64, "Data updated");
		  // mvwprintw(prtWin,rows-5,cols-65, "cfUpdate %d,upID %d actInd %d sname %s", cfUpdate,upID,actInd, prtPostCode);  //DEBUG
		}
	      else 
		{
		  prtInsert(actInd, prtPostCode, prtAddress, prtCity);
		  wattron(prtWin,A_BOLD | A_BLINK);
	          mvwprintw(prtWin,rows-8,cols-64, "Data saved");
		  wattroff(prtWin,A_BOLD | A_BLINK);
		}
	    }
	}
      else
	{
	  mvwprintw(prtWin,rows-8,cols-64, "Data invalid");
	}
      noecho();

      unpost_form(propertyForm);
      free_form(propertyForm);
      free_field(propertyField[0]);
      free_field(propertyField[1]);
      free_field(propertyField[2]);
      free_field(propertyField[3]);

      cfUpdate = 0;

      mvwprintw(prtWin,rows-6,cols-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(prtWin)) != 'y')
	{
	  wmove(prtWin,rows-6,cols-27);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(prtWin);
    }  //while newrec = y
  PQfinish(conn);
  endwin();
}


int suppAccountInsert()
{
  PANEL *supUpdatePanel, *supPanel, *prtPanel, *supTypePanel, *payPanel, *paPanel, *mainPanel;
  WINDOW * supAcctWin, * supWin, * supTypeWin, * prtWin, * payWin, * paWin, *supUpdateWin;
  FORM * supAcctForm;
  FIELD * supAcctField[13];
  int i = 0, j = 0;
  char p;
  int ch;
  int srow, scol, strow, stcol, sarow, sacol, prrow, prcol, pyrow, pycol, parow, pacol, urows, ucols;
  int list = 6;
  int supID, prtID, supTID=0, payID, proANo;
  char supIDstr[5], prtIDstr[5], supTIDstr[5], payIDstr[5], proANostr[30];
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  int safActiveID, safSupID, safPrtID, safSupTypeID=0, safStartDt, safEndDt, safPayID, safProAcctID;
  float safAmount;
  char safSupAcctRef[30], safComment[30], safAlias[30];
  int cf;
  int newRec = 'y';
  int cfUpdate = 0;
  char *titleOne = "Supplier Account Form";
  char *titleTwo = "Supplier";
  char *titleThree = "Property";
  char *titleFour = "Supplier Type";
  char *titleFive = "Payment Period";
  char *titleSix = "Provider Account";
  char *titleNine = "Supplier Account";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  int lenFour = strlen(titleFour);
  int lenFive = strlen(titleFive);
  int lenSix = strlen(titleSix);
  int lenNine = strlen(titleNine);
  int fldColor = 0;

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  
  
  while (newRec == 'y')
    {
      supAcctField[0] = new_field(1,1,2,28,0,0);    /* active_ind        */
      supAcctField[1] = new_field(1,30,4,28,0,0);   /* supplier_acct_ref */
      supAcctField[2] = new_field(1,5,6,28,0,0);    /* supplier_id       */
      supAcctField[3] = new_field(1,5,8,28,0,0);    /* property_id       */
      supAcctField[4] = new_field(1,5,10,28,0,0);   /* supplier_type_id  */
      supAcctField[5] = new_field(1,8,12,28,0,0);   /* start_date        */
      supAcctField[6] = new_field(1,8,14,28,0,0);   /* end_date          */
      supAcctField[7] = new_field(1,5,16,28,0,0);   /* payment_period_id */
      supAcctField[8] = new_field(1,10,18,28,0,0);  /* amount            */
      supAcctField[9] = new_field(1,30,20,28,0,0);  /* comment           */
      supAcctField[10] = new_field(1,30,22,28,0,0); /* alias             */
      supAcctField[11] = new_field(1,5,24,28,0,0);  /* provider_acct_id  */
      supAcctField[12] = NULL; 

      for (fldColor = 0; fldColor < 12; fldColor++)
	{
	  set_field_fore(supAcctField[fldColor], COLOR_PAIR(9));
	  set_field_back(supAcctField[fldColor], COLOR_PAIR(9));
	}
            

      set_field_type(supAcctField[0],TYPE_INTEGER,1,1,2);
      set_field_type(supAcctField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[2],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[4],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[5],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[6],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[7],TYPE_INTEGER,0,1,99999);
      // set_field_type(supAcctField[8],TYPE_NUMERIC,2,-100000,1000000);
      set_field_type(supAcctField[9],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[10],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[11],TYPE_INTEGER,0,1,99999);

      supAcctForm = new_form(supAcctField);
      scale_form(supAcctForm, &sarow, &sacol);

      supAcctWin = newwin(LINES-10, COLS/3,LINES-(LINES-4),COLS/15);
      supWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      prtWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      supTypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      payWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      paWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      supUpdateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      supUpdatePanel = new_panel(supUpdateWin);
      supPanel = new_panel(supWin);
      prtPanel = new_panel(prtWin);
      supTypePanel = new_panel(supTypeWin);
      payPanel = new_panel(payWin);
      paPanel = new_panel(paWin);
      mainPanel = new_panel(supAcctWin);
      wbkgd(supAcctWin, COLOR_PAIR(1));     
      //hide_panel(supUpdatePanel);
      //hide_panel(supPanel);
      update_panels();
      doupdate();

      
      keypad(supAcctWin, TRUE);
      keypad(supWin, TRUE);
      keypad(prtWin, TRUE);
      keypad(supTypeWin, TRUE);
      keypad(payWin, TRUE);
      keypad(paWin, TRUE);
      keypad(supUpdateWin, TRUE);

      set_form_win(supAcctForm,supAcctWin);
      set_form_sub(supAcctForm, derwin(supAcctWin,sarow,sacol,2,2));
      getmaxyx(supAcctWin,sarow,sacol);
      getmaxyx(supWin,srow,scol);
      getmaxyx(prtWin,prrow,prcol);
      getmaxyx(supTypeWin, strow, stcol);
      getmaxyx(payWin, pyrow, pycol);
      getmaxyx(paWin, parow, pacol);
      getmaxyx(supUpdateWin, urows, ucols);
      box(supAcctWin,0,0);
      box(supWin,0,0);
      box(prtWin,0,0);
      box(supTypeWin, 0,0);
      box(payWin, 0,0);
      box(paWin, 0,0);
      box(supUpdateWin,0,0);
      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(supAcctWin,1,(sacol-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */      
      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      //waddstr(supAcctWin, "Supplier Account Form");
      //waddstr(supWin, "Supplier");
      // waddstr(prtWin, "Property");
      //waddstr(supTypeWin, "Supplier Type");
      //waddstr(payWin, "Payment Period");
      //waddstr(paWin, "Provider Account");
      //waddstr(supUpdateWin, "Supplier Account");
      

      if(supAcctWin == NULL || supWin == NULL || prtWin == NULL ||supTypeWin == NULL || payWin == NULL || paWin == NULL || supUpdateWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  return(1);
	}

      post_form(supAcctForm);
      wrefresh(supAcctWin);

      // mvwprintw(supAcctWin, 37, 5,"row %d col %d", sarow, sacol);

      mvwprintw(supAcctWin, sarow-(sarow-4),sacol-(sacol-5),  "Active Ind:");
      mvwprintw(supAcctWin, sarow-(sarow-6),sacol-(sacol-5),  "Supplier Account No:");
      mvwprintw(supAcctWin, sarow-(sarow-8),sacol-(sacol-5),  "Supplier ID(F2):");
      mvwprintw(supAcctWin, sarow-(sarow-10),sacol-(sacol-5),  "Property ID(F3):");
      mvwprintw(supAcctWin, sarow-(sarow-12),sacol-(sacol-5), "Supplier Type ID(F4):");
      mvwprintw(supAcctWin, sarow-(sarow-14),sacol-(sacol-5), "Start Date:");
      mvwprintw(supAcctWin, sarow-(sarow-16),sacol-(sacol-5), "End Date:");
      mvwprintw(supAcctWin, sarow-(sarow-18),sacol-(sacol-5), "Payment Period(F5):");
      mvwprintw(supAcctWin, sarow-(sarow-20),sacol-(sacol-5), "Amount:");
      mvwprintw(supAcctWin, sarow-(sarow-22),sacol-(sacol-5), "Comment:");
      mvwprintw(supAcctWin, sarow-(sarow-24),sacol-(sacol-5), "Alias:");
      mvwprintw(supAcctWin, sarow-(sarow-26),sacol-(sacol-5), "Provider Account ID(F6):");
      mvwprintw(supAcctWin, sarow-2,sacol-(sacol-5),"Press F1 when form complete (F9 for Update)");
      wmove(supAcctWin,sarow-(sarow-4),sacol-(sacol-30));
      wrefresh(supAcctWin);

      while((ch = wgetch(supAcctWin)) != KEY_F(1))
      {
	hide_panel(supUpdatePanel);
	hide_panel(supPanel);
	hide_panel(prtPanel);
	hide_panel(supTypePanel);
	hide_panel(payPanel);
	hide_panel(paPanel);
	show_panel(mainPanel);
	update_panels();
	doupdate();
	keyNavigate(ch, supAcctForm);   
	set_field_type(supAcctField[8],TYPE_NUMERIC,2,-100000,1000000);
	if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(supWin);
	      box(supWin,0,0);
	      //waddstr(supWin, "Supplier");
	      wattron(supWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supWin,1,(scol-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(supWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supWin,1,1);
	      //wrefresh(supWin);
	      show_panel(supPanel);
	      wbkgd(supWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier WHERE active_ind = 1 ORDER BY supplier_id");	  
	      rows = PQntuples(res);

	      //wrefresh(supWin);
	      wattron(supWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supWin, 4, 1, "ID    Name");  //+3
	      wattroff(supWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(supWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supWin);
		      box(supWin,0,0); 
		    }
		  list = 6;      
		  //wclrtoeol(supWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supWin);  
		      mvwprintw(supWin,13,1,"End of list");
		      box(supWin,0,0);
		      //mvwprintw(supWin,0,0, "Supplier");
		      wattron(supWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supWin,1,(scol-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(supWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(supWin,srow-7,1,"Select Option: ");
	      mvwscanw(supWin,srow-7, scol-45, "%5s", &supIDstr);
	      set_field_buffer(supAcctField[2],0, supIDstr);
	      wattroff(supWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      supID = atoi(field_buffer(supAcctField[2],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier WHERE active_ind = 1 and supplier_id = $1 ORDER BY supplier_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      //mvwprintw(supWin,11,1,"rows %d", rows);
	      if (rows == 1)
		{
		  set_field_buffer(supAcctField[2],0,PQgetvalue(res,0,0));
		  //mvwprintw(supWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(supWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supWin);
		}
	      else
		{
		  set_field_buffer(supAcctField[2],0,"");
		  wattron(supWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supWin,srow-6,1,"Number invalid");
		  wattroff(supWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(supWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	      //hide_panel(supPanel);
	      //update_panels();
	      //doupdate();
	    } //F2
	if(ch == KEY_F(3))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(prtWin);
	      box(prtWin,0,0);
	      //waddstr(prtWin, "Property");
	      wattron(prtWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(prtWin,1,(prcol-lenThree)/2, titleThree);     /*SET_SUB_WIM_TITLE */
	      wattroff(prtWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(prtWin,1,1);
	      show_panel(prtPanel);
	      wbkgd(prtWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      wrefresh(prtWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM property WHERE active_ind = 1 ORDER BY property_id");	  
	      rows = PQntuples(res);

	      wrefresh(prtWin);
	      wattron(prtWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(prtWin, 4, 1, "ID    Post Code");  //+3
	      wattroff(prtWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(prtWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(prtWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(prtWin);
		      box(prtWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  //wclrtoeol(prtWin);  
		  if  (i == rows)
		    {
		      wclrtobot(prtWin);  
		      mvwprintw(prtWin,13,1,"End of list");
		      box(prtWin,0,0);
		      //mvwprintw(prtWin,0,0, "Property");
		      wattron(prtWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(prtWin,1,(prcol-lenThree)/2, titleThree);    /* SET_SUB_WIN_TITLE */
		      wattroff(prtWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(prtWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(prtWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(prtWin,prrow-7,1,"Select Option: ");
	      mvwscanw(prtWin,prrow-7,prcol-45, "%5s", &prtIDstr);
	      set_field_buffer(supAcctField[3],0, prtIDstr);
              wattroff(prtWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      prtID = atoi(field_buffer(supAcctField[3],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)prtID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM property WHERE active_ind = 1 AND property_id = $1 ORDER BY property_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(supAcctField[3],0,PQgetvalue(res,0,0));
		  //mvwprintw(prtWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(prtWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(prtWin);
		}
	      else
		{
		  set_field_buffer(supAcctField[3],0,"");
		  wattron(prtWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(prtWin,prrow-6,1,"Number invalid");
		  wattroff(prtWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(prtWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F3
	if(ch == KEY_F(4))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(supTypeWin);
	      box(supTypeWin,0,0);
	      //waddstr(supTypeWin, "Supplier Type");
	      wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supTypeWin,1,(stcol-lenFour)/2, titleFour);     /*SET_SUB_WIM_TITLE */
	      wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supTypeWin,1,1);
	      show_panel(supTypePanel);
	      wbkgd(supTypeWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(supTypeWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_type ORDER BY supplier_type_id");	  
	      rows = PQntuples(res);

	      wrefresh(supTypeWin);
	      wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supTypeWin, 4, 1, "ID    Description");  //+3
	      wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	      while((p = wgetch(supTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supTypeWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(supTypeWin);
		      box(supTypeWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  //wclrtoeol(supTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supTypeWin);  
		      mvwprintw(supTypeWin,13,1,"End of list");
		      box(supTypeWin,0,0);
		      //mvwprintw(supTypeWin,0,0, "Supplier Type");
		      wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supTypeWin,1,(stcol-lenFour)/2, titleFour);    /* SET_SUB_WIN_TITLE */
		      wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supTypeWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(supTypeWin,strow-7,1,"Select Option: ");
	      mvwscanw(supTypeWin,strow-7,stcol-45, "%5s", &supTIDstr);
	      set_field_buffer(supAcctField[4],0, supTIDstr);
              wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      supTID = atoi(field_buffer(supAcctField[4],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supTID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier_type WHERE supplier_type_id = $1 ORDER BY supplier_type_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(supAcctField[4],0,PQgetvalue(res,0,0));
		  //mvwprintw(supTypeWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(supTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(supTypeWin);
		}
	      else
		{
		  set_field_buffer(supAcctField[4],0,"");
		  wattron(supTypeWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supTypeWin,strow-6,1,"Number invalid");
		  wattroff(supTypeWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(supTypeWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F4
	if(ch == KEY_F(5))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(payWin);
	      box(payWin,0,0);
	      //waddstr(payWin, "Payment Period");
	      wattron(payWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(payWin,1,(pycol-lenFive)/2, titleFive);     /*SET_SUB_WIM_TITLE */
	      wattroff(payWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(payWin,1,1);
	      show_panel(payPanel);
	      wbkgd(payWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(payWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM payment_period ORDER BY payment_period_id");	  
	      rows = PQntuples(res);

	      wrefresh(payWin);
	      wattron(payWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(payWin, 4, 1, "ID    Period");  //+3
	      wattroff(payWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	      while((p = wgetch(payWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(payWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(payWin);
		      box(payWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  // wclrtoeol(payWin);  
		  if  (i == rows)
		    {
		      wclrtobot(payWin);  
		      mvwprintw(payWin,13,1,"End of list");
		      box(payWin,0,0);
		      //mvwprintw(payWin,0,0, "Payment Period");
		      wattron(payWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(payWin,1,(pycol-lenFive)/2, titleFive);    /* SET_SUB_WIN_TITLE */
		      wattroff(payWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(payWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(payWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(payWin,pyrow-7,1,"Select Option: ");
	      mvwscanw(payWin,pyrow-7,pycol-45, "%5s", &payIDstr);
	      set_field_buffer(supAcctField[7],0, payIDstr);
	      wattroff(payWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      payID = atoi(field_buffer(supAcctField[7],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)payID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM payment_period WHERE payment_period_id = $1 ORDER BY payment_period_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(supAcctField[7],0,PQgetvalue(res,0,0));
		  //mvwprintw(payWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(payWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(payWin);
		}
	      else
		{
		  set_field_buffer(supAcctField[7],0,"");
	      //mvwprintw(payWin,12,1,"Number invalid");
	          wattron(payWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(payWin,pyrow-6,1,"Number invalid");
		  wattroff(payWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(payWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } //F5
	if(ch == KEY_F(6))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(paWin);
	      box(paWin,0,0);
	      //waddstr(paWin, "Provider Account");
	      wattron(paWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(paWin,1,(pacol-lenSix)/2, titleSix);     /*SET_SUB_WIM_TITLE */
	      wattroff(paWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(paWin,1,1);
	      show_panel(paPanel);
	      wbkgd(paWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(paWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(paWin);
	      wattron(paWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(paWin, 4, 1, "ID    Provider_ID     Account Number");  //+3
	      wattroff(paWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(paWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(paWin,list,1,"%-5s %-15s %-15s", PQgetvalue(res,i,0),PQgetvalue(res,i,2),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(paWin);
		      box(paWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  //wclrtoeol(paWin);  
		  if  (i == rows)
		    {
		      wclrtobot(paWin);  
		      mvwprintw(paWin,13,1,"End of list");
		      box(paWin,0,0);
		      //mvwprintw(paWin,0,0, "Provider Account");
		      wattron(paWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(paWin,1,(pacol-lenSix)/2, titleSix);    /* SET_SUB_WIN_TITLE */
		      wattroff(paWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(paWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(paWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(paWin,parow-7,1,"Select Option: ");
	      mvwscanw(paWin,parow-7,pacol-45, "%5s", &proANostr);
	      set_field_buffer(supAcctField[11],0, proANostr);
              wattroff(paWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      proANo = atoi(field_buffer(supAcctField[11],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)proANo);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_account WHERE active_ind = 1 AND provider_acct_id = $1 ORDER BY provider_acct_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(supAcctField[11],0,PQgetvalue(res,0,0));
		  //mvwprintw(paWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(paWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(paWin);
		}
	      else
		{
		  set_field_buffer(supAcctField[11],0,"");
		  //mvwprintw(paWin,12,1,"Number invalid");
		  wattron(paWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(paWin,parow-6,1,"Number invalid");
		  wattroff(paWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(paWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F6
	if(ch == KEY_F(9))
	    {	      
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(supUpdateWin);
	      box(supUpdateWin,0,0);
	      //waddstr(supUpdateWin, "Supplier Account");
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supUpdateWin,1,(ucols-lenNine)/2, titleNine);     /*SET_SUB_WIM_TITLE */
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supUpdateWin,1,1);
	      //wrefresh(supUpdateWin);
	      show_panel(supUpdatePanel);
	      wbkgd(supUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();
	      //wrefresh(supAcctWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_account WHERE active_ind = 1 ORDER BY supplier_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(supUpdateWin);
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supUpdateWin, 4, 1, "ID    Supplier_ID     Supplier Account");  //+3
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */	  
	      while((p = wgetch(supUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supUpdateWin,list,1,"%-5s %-15s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,3),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supUpdateWin);
		      box(supUpdateWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(supUpdateWin);  
		      mvwprintw(supUpdateWin,13,1,"End of list");
		      box(supUpdateWin,0,0);
		      //mvwprintw(supUpdateWin,0,0, "Supplier Account");
		      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supUpdateWin,1,(ucols-lenNine)/2, titleNine);    /* SET_SUB_WIN_TITLE */
		      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(supUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(supUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier_account WHERE active_ind = 1 AND supplier_acct_id = $1 ORDER BY supplier_acct_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(supUpdateWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(supUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(supUpdateWin);
		  set_field_buffer(supAcctField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(supAcctField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(supAcctField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(supAcctField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(supAcctField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(supAcctField[5],0,PQgetvalue(res,0,6));
		  set_field_buffer(supAcctField[6],0,PQgetvalue(res,0,7));
		  set_field_buffer(supAcctField[7],0,PQgetvalue(res,0,8));
		  set_field_buffer(supAcctField[8],0,PQgetvalue(res,0,9));
		  set_field_buffer(supAcctField[9],0,PQgetvalue(res,0,10));
		  set_field_buffer(supAcctField[10],0,PQgetvalue(res,0,11));
		  set_field_buffer(supAcctField[11],0,PQgetvalue(res,0,12));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(supUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supUpdateWin,urows-6,1,"Number invalid");
		  wattroff(supUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(supUpdateWin);		
		  //wrefresh(supAcctWin);		  
		}
	      noecho();
	      PQclear(res);
	    } //F9	
      } //while not F1
      hide_panel(supUpdatePanel);
      hide_panel(supPanel);
      hide_panel(prtPanel);
      hide_panel(supTypePanel);
      hide_panel(payPanel);
      hide_panel(paPanel);
      update_panels();
      doupdate();

    /* code goes here for assign buffer value and validate prior to insert */
      form_driver(supAcctForm,REQ_VALIDATION);
     
      safActiveID = atoi(field_buffer(supAcctField[0],0));
      strcpy(safSupAcctRef, trimWS(field_buffer(supAcctField[1],0)));
      safSupID = atoi(field_buffer(supAcctField[2],0));
      safPrtID = atoi(field_buffer(supAcctField[3],0));
      safSupTypeID = atoi(field_buffer(supAcctField[4],0));
      safStartDt = atoi(field_buffer(supAcctField[5],0));
      safEndDt = atoi(field_buffer(supAcctField[6],0));
      safPayID = atoi(field_buffer(supAcctField[7],0));
      safAmount = atof(field_buffer(supAcctField[8],0));
      strcpy(safComment,trimWS(field_buffer(supAcctField[9],0)));
      strcpy(safAlias, trimWS(field_buffer(supAcctField[10],0)));
      safProAcctID = atoi(field_buffer(supAcctField[11],0));

      /*
      mvwprintw(supAcctWin, 3,65, "%d",safActiveID);
      mvwprintw(supAcctWin, 5,65, "%s",safSupAcctRef);
      mvwprintw(supAcctWin, 7,65, "%d",safSupID);
      mvwprintw(supAcctWin, 9,65, "%d",safPrtID);
      mvwprintw(supAcctWin, 11,65, "%d",safSupTypeID);
      mvwprintw(supAcctWin, 13,65, "%d",safStartDt);
      mvwprintw(supAcctWin, 15,65, "%d",safEndDt);
      mvwprintw(supAcctWin, 17,65, "%d",safPayID);
      mvwprintw(supAcctWin, 19,60, "%3.2f",safAmount);
      mvwprintw(supAcctWin, 21,65, "%s",safComment);
      mvwprintw(supAcctWin, 23,65, "%s",safAlias);
      mvwprintw(supAcctWin, 25,65, "%d",safProAcctID);
      */

      if ((form_driver(supAcctForm,REQ_VALIDATION) == E_OK) && (safActiveID >= 1))
	{
	  echo();
	  wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(supAcctWin,sarow-10,sacol-64,"Save y/n: ");
	  wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(supAcctWin,sarow-9,sacol-64,"(d = delete record)");
	  wmove(supAcctWin,sarow-10,pacol-53);

	  while((cf = wgetch(supAcctWin)) != 'y')
	    {
	      wmove(supAcctWin,sarow-10,pacol-53);
	      if (cf == 'n')
		{
		  mvwprintw(supAcctWin,sarow-8,pacol-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  supAccountDelete(upID);
		  wattron(supAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(supAcctWin,sarow-8,pacol-64, "Record deleted");
		  wattroff(supAcctWin,A_BOLD | A_BLINK);
		  break;
		}	      
	    }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  supAccountUpdate(upID, safActiveID, safSupAcctRef, safSupID, safPrtID, safSupTypeID, safStartDt,
		                   safEndDt, safPayID, safAmount, safComment, safAlias, safProAcctID);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(supAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(supAcctWin,sarow-8,pacol-64, "Data updated");
		  wattroff(supAcctWin,A_BOLD | A_BLINK);
		  //mvwprintw(supAcctWin,35,5, "cfUpdate %d,upID %d, safSupAcctRef %s, safSupID %d", cfUpdate,upID, safSupAcctRef, safSupID);  //DEBUG
		}
	      else
		{
		  supAccountInsert(safActiveID, safSupAcctRef, safSupID, safPrtID, safSupTypeID, safStartDt,
				   safEndDt, safPayID, safAmount, safComment, safAlias, safProAcctID);
		  wattron(supAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(supAcctWin,sarow-8,pacol-64, "Data saved");
		  wattroff(supAcctWin,A_BOLD | A_BLINK);
		}
	    }
	}
      else
	{
	  mvwprintw(supAcctWin,sarow-8,pacol-64, "Data invalid");	
	}
      noecho();

      /* free form and fields */
      
      unpost_form(supAcctForm);
      free_form(supAcctForm);
      free_field(supAcctField[0]);
      free_field(supAcctField[1]);
      free_field(supAcctField[2]);
      free_field(supAcctField[3]);
      free_field(supAcctField[4]);
      free_field(supAcctField[5]);
      free_field(supAcctField[6]);
      free_field(supAcctField[7]);
      free_field(supAcctField[8]);
      free_field(supAcctField[9]);
      free_field(supAcctField[10]);
      free_field(supAcctField[11]);

      cfUpdate = 0;

      mvwprintw(supAcctWin,sarow-6,pacol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supAcctWin)) != 'y')
	{
	  wmove(supAcctWin,sarow-6,pacol-27);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(supAcctWin);
    } //while newRec = y

  PQfinish(conn);
  endwin();      
  
  return 0;
}
    
