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
  int list = 2, i = 0, j = 0;
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

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while (newRec == 'y')
    {
      invoiceField[0] = new_field(1,30,2,24,0,0);  /* invoice_no */
      invoiceField[1] = new_field(1,8,4,24,0,0);  /* start_date */
      invoiceField[2] = new_field(1,8,6,24,0,0);  /* end_date */
      invoiceField[3] = new_field(1,5,8,24,0,0);  /* supplier_acct_id */
      invoiceField[4] = new_field(1,30,10,24,0,0);  /* description */
      invoiceField[5] = new_field(1,10,12,24,0,0);  /* amount */      
      invoiceField[6] = NULL;

      set_field_type(invoiceField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(invoiceField[1],TYPE_INTEGER,0,1,99999999);
      set_field_type(invoiceField[2],TYPE_INTEGER,0,1,99999999);
      set_field_type(invoiceField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(invoiceField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      //set_field_type(invoiceField[5],TYPE_NUMERIC,2,-100000,1000000);

      invoiceForm = new_form(invoiceField);
      scale_form(invoiceForm, &invrow, &invcol);

      invWin = newwin(invrow+20,invcol+10,1,1);
      supAcctWin = newwin(20,50,1,120);
      invUpdateWin = newwin(20,50,1,120);

      invPanel = new_panel(invWin);
      invUpdatePanel = new_panel(invUpdateWin);
      supAcctPanel = new_panel(supAcctWin);
      update_panels();
      doupdate();
      
      keypad(invWin, TRUE);
      keypad(supAcctWin, TRUE);
      keypad(invUpdateWin, TRUE);

      set_form_win(invoiceForm, invWin);
      set_form_sub(invoiceForm, derwin(invWin, invrow, invcol, 1, 1));
      getmaxyx(invWin,invrow,invcol);
      getmaxyx(supAcctWin,sarow,sacol);
      getmaxyx(invUpdateWin, urows, ucols);
      box(invWin,0,0);
      box(supAcctWin,0,0);
      box(invUpdateWin,0,0);
      waddstr(invWin,"Invoice Entry Form");
      waddstr(supAcctWin,"Supplier Account");
      waddstr(invUpdateWin,"invoice");

      if (invWin == NULL || supAcctWin == NULL || invUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      post_form(invoiceForm);
      wrefresh(invWin);

      mvwprintw(invWin,3,5, "Invoice Number:");
      mvwprintw(invWin,5,5, "Start Date:");
      mvwprintw(invWin,7,5, "End Date:");
      mvwprintw(invWin,9,5, "Supplier Acct(F2):");
      mvwprintw(invWin,11,5,"Description:");
      mvwprintw(invWin,13,5,"Amount:");
      mvwprintw(invWin,31,5,"Press F1 when form complete");
      wmove(invWin,3,25);
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
	      list = 2;
	      wclear(supAcctWin);
	      box(supAcctWin,0,0);
	      waddstr(supAcctWin, "Supplier Account");
	      wmove(supAcctWin,1,1);
	      //wrefresh(supAcctWin);
	      show_panel(supAcctPanel);
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_account WHERE active_ind = 1 ORDER BY supplier_acct_id");	  
	      rows = PQntuples(res);

	      //wrefresh(supAcctWin);
	  
	      while((p = wgetch(supAcctWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(supAcctWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supAcctWin);  
		    }
		  list = 2;      
		  //wclrtoeol(supAcctWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supAcctWin);  
		      mvwprintw(supAcctWin,10,1,"End of list");
		      box(supAcctWin,0,0);
		      mvwprintw(supAcctWin,0,0, "Supplier Account");
		      wmove(supAcctWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supAcctWin,11,1,"Select Option: ");
	      mvwscanw(supAcctWin,11,25, "%5s", &supAcctIDstr);
	      set_field_buffer(invoiceField[3],0, supAcctIDstr);

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
		  mvwprintw(supAcctWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(supAcctWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supAcctWin);
		}
	      else
		{
		  mvwprintw(supAcctWin,12,1,"Number invalid");
		  wrefresh(supAcctWin);		
		  //wrefresh(invWin);
		}
	      noecho();
	      PQclear(res);
	    } // F2
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(invUpdateWin);
	      box(invUpdateWin,0,0);
	      waddstr(invUpdateWin, "Invoice");
	      wmove(invUpdateWin,1,1);
	      //wrefresh(invUpdateWin);
	      show_panel(invUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_invoice ORDER BY supplier_invoice_id");	  
	      rows = PQntuples(res);

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
		      mvwprintw(invUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(invUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(invUpdateWin);  
		      mvwprintw(invUpdateWin,10,1,"End of list");
		      box(invUpdateWin,0,0);
		      mvwprintw(invUpdateWin,0,0, "Invoice");
		      wmove(invUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(invUpdateWin,11,1,"Select Option: ");
	      mvwscanw(invUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(invUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(invUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
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
		  mvwprintw(invUpdateWin,12,1,"Number invalied");
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
	  mvwprintw(invWin,25,5,"Save y/n: ");
	  mvwprintw(invWin,26,5,"(d = delete record)");
	  wmove(invWin,25,16);

	  while((cf = wgetch(invWin)) != 'y')
	    {
	      wmove(invWin,25,16);
	      if (cf == 'n')
		{
		  mvwprintw(invWin,27,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  invoiceDelete(upID);
		  mvwprintw(invWin,27,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  invoiceUpdate(upID, invfNo, invfStartDt, invfEndDt, invfSupAcctID, invfDesc, invfAmount);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(invWin,27,5, "Data updated");
		  mvwprintw(invWin,28,5, "cfUpdate %d,upID %d, invfNo %s, invfSupAcctID %d", cfUpdate,upID, invfNo, invfSupAcctID);  //DEBUG
		}
      	      else
		{
		  invoiceInsert(invfNo, invfStartDt, invfEndDt, invfSupAcctID, invfDesc, invfAmount);
		  mvwprintw(invWin,27,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(invWin,27,5, "Data invalid");	
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

      mvwprintw(invWin,30,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(invWin)) != 'y')
	{
	  wmove(invWin,30,44);
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
  
