#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/doccur.h"
#include "../inc/doclib.h"
#include "../inc/jadlib.h"


void documentTypeInsert()
{
  FIELD *doctypeField[2];
  FORM *doctypeForm;
  WINDOW *doctypeWin, *doctypeUpdateWin;
  PANEL *doctypePanel, *doctypeUpdatePanel;
  int newRec = 'y';
  int drow, dcol, urow, ucol;
  int list = 2, i = 0, j = 0;
  int range = 5;
  char p;
  int ch;
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  char dtDesc[30];
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
      doctypeField[0] = new_field(1,30,2,24,0,0);  /* description */
      doctypeField[1] = NULL;

      set_field_type(doctypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      doctypeForm = new_form(doctypeField);
      scale_form(doctypeForm, &drow, &dcol);

      doctypeWin = newwin(drow+20,dcol+10,1,1);
      doctypeUpdateWin = newwin(20,50,1,120);

      doctypePanel = new_panel(doctypeWin);
      doctypeUpdatePanel = new_panel(doctypeUpdateWin);
      update_panels();
      doupdate();
      
      keypad(doctypeWin, TRUE);
      keypad(doctypeUpdateWin, TRUE);

      set_form_win(doctypeForm, doctypeWin);
      set_form_sub(doctypeForm, derwin(doctypeWin, drow, dcol, 1, 1));
      getmaxyx(doctypeWin,drow,dcol);
      getmaxyx(doctypeUpdateWin, urow, ucol);
      box(doctypeWin,0,0);
      box(doctypeUpdateWin,0,0);
      waddstr(doctypeWin,"Document Type Form");
      waddstr(doctypeUpdateWin,"Document Type");

      if (doctypeWin == NULL || doctypeUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      post_form(doctypeForm);
      wrefresh(doctypeWin);

      mvwprintw(doctypeWin,3,5, "Description:");
      mvwprintw(doctypeWin,21,5,"Press F1 when form complete");
      wmove(doctypeWin,3,25);
      //mvwprintw(doctypeWin,8,5,"rows %d cols %d", drow, dcol);
      //wrefresh(doctypeWin);
      
      while((ch = wgetch(doctypeWin)) != KEY_F(1))
	{
	  hide_panel(doctypeUpdatePanel);
	  show_panel(doctypePanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, doctypeForm);
	  // I had to set validation for the field at this point as would cause
	  // navigation to stop when entering subsequent data after first entry
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(doctypeUpdateWin);
	      box(doctypeUpdateWin,0,0);
	      waddstr(doctypeUpdateWin, "Document Type");
	      wmove(doctypeUpdateWin,1,1);
	      //wrefresh(doctypeUpdateWin);
	      show_panel(doctypeUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM document_type ORDER BY type_id");	  
	      rows = PQntuples(res);

	      wrefresh(doctypeUpdateWin);
	  
	      while((p = wgetch(doctypeUpdateWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(doctypeUpdateWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(doctypeUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(doctypeUpdateWin);  
		      mvwprintw(doctypeUpdateWin,10,1,"End of list");
		      box(doctypeUpdateWin,0,0);
		      mvwprintw(doctypeUpdateWin,0,0, "Document Type");
		      wmove(doctypeUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(doctypeUpdateWin,11,1,"Select Option: ");
	      mvwscanw(doctypeUpdateWin,11,25, "%d", &upID);

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM document_type WHERE type_id = $1 ORDER BY type_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(doctypeUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(doctypeUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  //wrefresh(doctypeUpdateWin);
		  set_field_buffer(doctypeField[0],0,PQgetvalue(res,0,1));
		  //set_field_buffer(doctypeField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(doctypeUpdateWin,12,1,"Number invalied");
		  wrefresh(doctypeUpdateWin);		
		  //wrefresh(PARENT_WIN);
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} // while F1
      hide_panel(doctypeUpdatePanel);
      update_panels();
      doupdate();

      form_driver(doctypeForm,REQ_VALIDATION);

      strcpy(dtDesc, field_buffer(doctypeField[0],0));

      if ((form_driver(doctypeForm,REQ_VALIDATION) == E_OK))
	{
	  strcpy(dtDesc, trimWS(dtDesc));
	  echo();
	  mvwprintw(doctypeWin,14,5,"Save y/n: ");
	  mvwprintw(doctypeWin,15,5,"(d = delete record)");
	  wmove(doctypeWin,14,16);

	  while((cf = wgetch(doctypeWin)) != 'y')
	    {
	      wmove(doctypeWin,14,16);
	      if (cf == 'n')
		{
		  mvwprintw(doctypeWin,16,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  docTypeDelete(upID);
		  mvwprintw(doctypeWin,16,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  docTypeUpdate(upID, dtDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(doctypeWin,16,5, "Data updated");
		  mvwprintw(doctypeWin,17,5, "cfUpdate %d,upID %d, dtDesc %s", cfUpdate,upID, dtDesc);  //DEBUG
		}
      	      else
		{
		  docTypeInsert(dtDesc);
		  mvwprintw(doctypeWin,17,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(doctypeWin,17,5, "Data invalid");	
	}
      noecho();

      unpost_form(doctypeForm);
      free_form(doctypeForm);
      free_field(doctypeField[0]);

      cfUpdate = 0;

      mvwprintw(doctypeWin,19,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(doctypeWin)) != 'y')
	{
	  wmove(doctypeWin,19,44);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(doctypePanel);
      update_panels();
      doupdate();
      delwin(doctypeWin);	
    } //while newRec
  PQfinish(conn);
  endwin();
}
