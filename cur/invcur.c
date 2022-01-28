#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/invcur.h"
#include "../inc/invlib.h"
#include "../inc/jadlib.h"


void invInsert()
{
  FIELD *invoiceField[7];
  FORM *invoiceForm;
  WINDOW *invWin, *supAcctWin, *invUpdateWin;
  PANEL *invPanel, *invUpdatePanel, *supAcctPanel;
  int newRec = 'y';
  int invrow, invcol, sarow, sacol, urows, ucols;
  int list = 6, i = 0, j = 0;
  int range = 5;
  char p;
  int ch;
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  char supAcctIDstr[5];
  int supAcctID;
  int invfStartDt, invfEndDt, invfSupAcctID;
  float invfAmount;
  char invfNo[30], invfDesc[30];
  int cfUpdate = 0;
  int cf;
  const char *titleOne = "Invoice Entry Form";
  const char *titleTwo = "Supplier Account";
  const char *titleThree = "Invoice";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  int fldColor = 0;

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')
    {
      invoiceField[0] = new_field(1,30,2,28,0,0);  /* invoice_no */
      invoiceField[1] = new_field(1,8,4,28,0,0);  /* start_date */
      invoiceField[2] = new_field(1,8,6,28,0,0);  /* end_date */
      invoiceField[3] = new_field(1,5,8,28,0,0);  /* supplier_acct_id */
      invoiceField[4] = new_field(1,30,10,28,0,0);  /* description */
      invoiceField[5] = new_field(1,10,12,28,0,0);  /* amount */      
      invoiceField[6] = NULL;

      for (fldColor = 0; fldColor < 6; fldColor++)
	{
	  set_field_fore(invoiceField[fldColor], COLOR_PAIR(9));
	  set_field_back(invoiceField[fldColor], COLOR_PAIR(9));
	}

      set_field_type(invoiceField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(invoiceField[1],TYPE_INTEGER,0,1,99999999);
      set_field_type(invoiceField[2],TYPE_INTEGER,0,1,99999999);
      set_field_type(invoiceField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(invoiceField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      //set_field_type(invoiceField[5],TYPE_NUMERIC,2,-100000,1000000);

      invoiceForm = new_form(invoiceField);
      scale_form(invoiceForm, &invrow, &invcol);

      invWin = newwin(LINES-15, COLS/3,LINES-(LINES-4),COLS/15);
      supAcctWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      invUpdateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      invPanel = new_panel(invWin);
      invUpdatePanel = new_panel(invUpdateWin);
      supAcctPanel = new_panel(supAcctWin);
      wbkgd(invWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();
      
      keypad(invWin, TRUE);
      keypad(supAcctWin, TRUE);
      keypad(invUpdateWin, TRUE);

      set_form_win(invoiceForm, invWin);
      set_form_sub(invoiceForm, derwin(invWin, invrow, invcol, 2, 2));
      getmaxyx(invWin,invrow,invcol);
      getmaxyx(supAcctWin,sarow,sacol);
      getmaxyx(invUpdateWin, urows, ucols);
      box(invWin,0,0);
      box(supAcctWin,0,0);
      box(invUpdateWin,0,0);
      wattron(invWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(invWin,1,(sacol-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */      
      wattroff(invWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      //waddstr(invWin,"Invoice Entry Form");
      //waddstr(supAcctWin,"Supplier Account");
      //waddstr(invUpdateWin,"invoice");

      if (invWin == NULL || supAcctWin == NULL || invUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      post_form(invoiceForm);
      wrefresh(invWin);

      mvwprintw(invWin,invrow-(invrow-4),invcol-(invcol-5), "Invoice Number:");
      mvwprintw(invWin,invrow-(invrow-6),invcol-(invcol-5), "Start Date:");
      mvwprintw(invWin,invrow-(invrow-8),invcol-(invcol-5), "End Date:");
      mvwprintw(invWin,invrow-(invrow-10),invcol-(invcol-5),"Supplier Acct(F2):");
      mvwprintw(invWin,invrow-(invrow-12),invcol-(invcol-5),"Description:");
      mvwprintw(invWin,invrow-(invrow-14),invcol-(invcol-5),"Amount:");
      mvwprintw(invWin,invrow-2,invcol-(invcol-5),"Press F1 when form complete (F9 for Update)");
      wmove(invWin,invrow-(invrow-4),invcol-(invcol-30));
      //mvwprintw(invWin,24,5,"rows %d cols %d", invrow, invcol);
      wrefresh(invWin);
      
      while((ch = wgetch(invWin)) != KEY_F(1))
	{
	  hide_panel(supAcctPanel);
	  hide_panel(invUpdatePanel);
	  show_panel(invPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, invoiceForm);
	  // I had to set validation for the field at this point as would cause
	  // navigation to stop when entering subsequent data after first entry
	  set_field_type(invoiceField[5],TYPE_NUMERIC,2,-100000,1000000);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(supAcctWin);
	      box(supAcctWin,0,0);
	      //waddstr(supAcctWin, "Supplier Account");
	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supAcctWin,1,(sacol-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supAcctWin,1,1);
	      //wrefresh(supAcctWin);
	      show_panel(supAcctPanel);
	      wbkgd(supAcctWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_account WHERE active_ind = 1 ORDER BY supplier_acct_id");	  
	      rows = PQntuples(res);

	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supAcctWin, 4, 1, "ID    Supplier Account");  //+3
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */

	      //wrefresh(supAcctWin);
	  
	      while((p = wgetch(supAcctWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(supAcctWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supAcctWin);  
		    }
		  list = 6;      
		  //wclrtoeol(supAcctWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supAcctWin);  
		      mvwprintw(supAcctWin,13,1,"End of list");
		      box(supAcctWin,0,0);
		      //mvwprintw(supAcctWin,0,0, "Supplier Account");
		      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supAcctWin,1,(sacol-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supAcctWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(supAcctWin,sarow-7,1,"Select Option: ");
	      mvwscanw(supAcctWin,sarow-7, sacol-45, "%5s", &supAcctIDstr);
	      set_field_buffer(invoiceField[3],0, supAcctIDstr);
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      supAcctID = atoi(field_buffer(invoiceField[3],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supAcctID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
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
		  set_field_buffer(invoiceField[3],0,PQgetvalue(res,0,0));
		  //mvwprintw(supAcctWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(supAcctWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supAcctWin);
		}
	      else
		{
		  set_field_buffer(invoiceField[3],0,"");
		  wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supAcctWin,sarow-6,1,"Number invalid");
		  wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(supAcctWin);		
		  //wrefresh(invWin);
		}
	      noecho();
	      PQclear(res);
	    } // F2
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(invUpdateWin);
	      box(invUpdateWin,0,0);
	      //waddstr(invUpdateWin, "Invoice");
	      wattron(invUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(invUpdateWin,1,(ucols-lenThree)/2, titleThree);     /*SET_SUB_WIM_TITLE */
	      wattroff(invUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(invUpdateWin,1,1);
	      //wrefresh(invUpdateWin);
	      show_panel(invUpdatePanel);
	      wbkgd(invUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_invoice ORDER BY supplier_invoice_id");	  
	      rows = PQntuples(res);

	      wattron(invUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(invUpdateWin, 4, 1, "ID    Invoice_no      Supplier Account");  //+3
	      wattroff(invUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */	

	      wrefresh(invUpdateWin);
	  
	      while((p = wgetch(invUpdateWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(invUpdateWin,list,1,"%-5s %-15s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(invUpdateWin);
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(invUpdateWin);  
		      mvwprintw(invUpdateWin,13,1,"End of list");
		      box(invUpdateWin,0,0);
		      wattron(invUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(invUpdateWin,1,(ucols-lenThree)/2, titleThree);    /* SET_SUB_WIN_TITLE */
		      wattroff(invUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      //mvwprintw(invUpdateWin,0,0, "Invoice");
		      wmove(invUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(invUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(invUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(invUpdateWin,urows-7, ucols-45, "%d", &upID);
	      wattroff(invUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM supplier_invoice WHERE supplier_invoice_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(invUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(invUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(invUpdateWin);
		  set_field_buffer(invoiceField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(invoiceField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(invoiceField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(invoiceField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(invoiceField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(invoiceField[5],0,PQgetvalue(res,0,6));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(invUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(invUpdateWin,urows-6,1,"Number invalid");
		  wattroff(invUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(invUpdateWin);		
		  //wrefresh(PARENT_WIN);
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} // while F1
      hide_panel(supAcctPanel);
      hide_panel(invUpdatePanel);
      update_panels();
      doupdate();

      form_driver(invoiceForm,REQ_VALIDATION);

      strcpy(invfNo, field_buffer(invoiceField[0],0));
      invfStartDt = atoi(field_buffer(invoiceField[1],0));
      invfEndDt = atoi(field_buffer(invoiceField[2],0));
      invfSupAcctID = atoi(field_buffer(invoiceField[3],0));
      strcpy(invfDesc, trimWS(field_buffer(invoiceField[4],0)));
      invfAmount = atof(field_buffer(invoiceField[5],0));

      if ((form_driver(invoiceForm,REQ_VALIDATION) == E_OK) && (!isspace(*invfNo)))
	{
	  strcpy(invfNo, trimWS(invfNo));
	  echo();
	  wattron(invWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(invWin,invrow-10,invcol-64,"Save y/n: ");
	  wattroff(invWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(invWin,invrow-9,invcol-64,"(d = delete record)");
	  wmove(invWin,invrow-10,invcol-53);

	  while((cf = wgetch(invWin)) != 'y')
	    {
	      wmove(invWin,invrow-10,invcol-53);
	      if (cf == 'n')
		{
		  mvwprintw(invWin,invrow-8,invcol-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  invoiceDelete(upID);
		  wattron(invWin,A_BOLD | A_BLINK);
		  mvwprintw(invWin,invrow-8,invcol-64, "Record deleted");
		  wattroff(invWin,A_BOLD | A_BLINK);
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  invoiceUpdate(upID, invfNo, invfStartDt, invfEndDt, invfSupAcctID, invfDesc, invfAmount);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(invWin,A_BOLD | A_BLINK);
		  mvwprintw(invWin,invrow-8,invcol-64, "Data updated");
		  wattroff(invWin,A_BOLD | A_BLINK);
		  //mvwprintw(invWin,28,5, "cfUpdate %d,upID %d, invfNo %s, invfSupAcctID %d", cfUpdate,upID, invfNo, invfSupAcctID);  //DEBUG
		}
      	      else
		{
		  invoiceInsert(invfNo, invfStartDt, invfEndDt, invfSupAcctID, invfDesc, invfAmount);
		  wattron(invWin,A_BOLD | A_BLINK);
		  mvwprintw(invWin,invrow-8,invcol-64, "Data saved");
		  wattroff(invWin,A_BOLD | A_BLINK);
		}
	    }
	}
      else
	{
	  mvwprintw(invWin,invrow-8,invcol-64, "Data invalid");	
	}
      noecho();

      unpost_form(invoiceForm);
      free_form(invoiceForm);
      free_field(invoiceField[0]);
      free_field(invoiceField[1]);
      free_field(invoiceField[2]);
      free_field(invoiceField[3]);
      free_field(invoiceField[4]);
      free_field(invoiceField[5]);
      //free_field(invoiceField[6]);

      cfUpdate = 0;

      mvwprintw(invWin,invrow-6,invcol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(invWin)) != 'y')
	{
	  wmove(invWin,invrow-6,invcol-27);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(invPanel);
      update_panels();
      doupdate();
      delwin(invWin);	
    } //while newRec
  PQfinish(conn);
  endwin();
}
  
