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
  //int range = 5;
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
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
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

void documentInsert()
{
  PANEL *docPanel, *proAcctPanel;
  WINDOW *docWin, *proAcctWin;
  FORM *docForm;
  FIELD *docField[11];
  int newRec = 'y';
  int docrow, doccol, parow, pacol;
  int ch;
  int cf;
  int cfUpdate = 0;
  int i = 0, j = 0;
  int list = 2;
  int rows;
  char p;
  int docProAcctID;
  char docProAcctIDstr[5];
  int val, *params[1], length[1], formats[1];


  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while ( newRec == 'y')
    {
      docField[0] = new_field(1,5,2,33,0,0);    /* parent_id   */
      docField[1] = new_field(1,30,4,33,0,0);   /* file_name   */
      docField[2] = new_field(1,5,6,33,0,0);    /* oid_value   */
      docField[3] = new_field(1,5,8,33,0,0);    /* type_id     */
      docField[4] = new_field(2,25,10,33,0,0);  /* reference   */
      docField[5] = new_field(4,25,13,33,0,0);  /* title       */
      docField[6] = new_field(5,30,18,33,0,0);  /* description */
      docField[7] = new_field(1,8,24,33,0,0);   /* start_date  */
      docField[8] = new_field(1,8,26,33,0,0);   /* end_date    */
      docField[9] = new_field(1,30,28,33,0,0);  /* catalog     */
      docField[10] = NULL;
      
      set_field_type(docField[0],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[2],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[5],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[6],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[7],TYPE_INTEGER,0,1,99999999);
      set_field_type(docField[8],TYPE_INTEGER,0,1,99999999);
      set_field_type(docField[9],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      field_opts_off(docField[2], O_ACTIVE);   /* field is inactive cursor skips over */
      field_opts_off(docField[9], O_ACTIVE);
      
      docForm = new_form(docField);
      scale_form(docForm, &docrow, &doccol);

      docWin = newwin(docrow+15,doccol+10,1,1);
      proAcctWin = newwin(20,50,1,120);

      docPanel = new_panel(docWin);
      proAcctPanel = new_panel(proAcctWin);
      update_panels();
      doupdate();

      keypad(docWin, TRUE);
      keypad(proAcctWin, TRUE);

      set_form_win(docForm, docWin);
      set_form_sub(docForm, derwin(docWin, docrow, doccol, 1, 1));
      getmaxyx(docWin, docrow, doccol);
      getmaxyx(proAcctWin, parow, pacol);
      box(docWin,0,0);
      box(proAcctWin,0,0);
      waddstr(docWin, "Document Import Form");
      waddstr(proAcctWin, "Provider Account");


      if(docWin == NULL || proAcctWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}
      
      post_form(docForm);
      wrefresh(docWin);

      mvwprintw(docWin, 3,5,  "ParentID:");
      mvwprintw(docWin, 5,5,  "File Name:");
      mvwprintw(docWin, 7,5,  "OID:");
      mvwprintw(docWin, 9,5,  "Type(F5):");
      mvwprintw(docWin, 11,5, "Reference:");
      mvwprintw(docWin, 14,5, "Title:");
      mvwprintw(docWin, 19,5, "Description:");
      mvwprintw(docWin, 25,5, "Start Date:");
      mvwprintw(docWin, 27,5, "End Date:");
      mvwprintw(docWin, 29,5, "Catalog:");
      mvwprintw(docWin,40,5, "ParentID: F2-Provider F3-Supplier F4-Invoice");
      wmove(docWin,3,34);
      wrefresh(docWin);     

      while((ch = wgetch(docWin)) != KEY_F(1))
	{
	  show_panel(docPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, docForm);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(proAcctWin);
	      box(proAcctWin,0,0);
	      waddstr(proAcctWin, "Provider Account");
	      wmove(proAcctWin,1,1);
	      show_panel(proAcctPanel);
	      update_panels();
	      doupdate();

	      res = PQexec(conn, "SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");
	      rows = PQntuples(res);

	      while((p = wgetch(proAcctWin)) == '\n')
		{
		  if(j + RANGE < rows)
		    j = j + RANGE;
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      mvwprintw(proAcctWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(proAcctWin);
		    }
		  list = 2;
		  if (i == rows)
		    {
		      wclrtobot(proAcctWin);  
		      mvwprintw(proAcctWin,10,1,"End of list");
		      box(proAcctWin,0,0);
		      mvwprintw(proAcctWin,0,0, "Provider Account");
		      wmove(proAcctWin,10,1);
		      break;		      
		    }		  
		}
	      echo();
	      mvwprintw(proAcctWin,11,1,"Select Provider Account: ");
	      mvwscanw(proAcctWin,11,25, "%5s", &docProAcctIDstr);
	      set_field_buffer(docField[0],0, docProAcctIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      docProAcctID = atoi(field_buffer(docField[0],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)docProAcctID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_account WHERE active_ind = 1 and provider_acct_id = $1 ORDER BY provider_acct_id;"
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
		  mvwprintw(proAcctWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(proAcctWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,3));
		  wrefresh(proAcctWin);
		}
	      else
		{
		  mvwprintw(proAcctWin,12,1,"Number invalied");
		  wrefresh(proAcctWin);		
		  //wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);	      
	    } //F2	  
	} //while not F1

      update_panels();
      doupdate();

      form_driver(docForm, REQ_VALIDATION);

      if((form_driver(docForm,REQ_VALIDATION) == E_OK))
	{
	  echo();
	  mvwprintw(docWin,32,5, "Save y/n: ");
	  mvwprintw(docWin,33,5, "(d = delete record)");
	  wmove(docWin,32,16);

	  while((cf = wgetch(docWin)) != 'y')
	    {
	      wmove(docWin,32,16);
	      if (cf == 'n')
		{
		  mvwprintw(docWin,34,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  //docDelete(upID);
		  mvwprintw(docWin,34,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  //docUpdate(upID, dtDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(docWin,34,5, "Data updated");
		  //mvwprintw(docWin,35,5, "cfUpdate %d,upID %d, dtDesc %s", cfUpdate,upID, dtDesc);  //DEBUG
		}
      	      else
		{
		  //docInsert(dtDesc);
		  mvwprintw(docWin,34,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(docWin,34,5, "Data invalid");
	}
      noecho();

      unpost_form(docForm);
      free_form(docForm);
      free_field(docField[0]);
      free_field(docField[1]);
      free_field(docField[2]);
      free_field(docField[3]);
      free_field(docField[4]);
      free_field(docField[5]);
      free_field(docField[6]);
      free_field(docField[7]);
      free_field(docField[8]);
      free_field(docField[9]);

      cfUpdate = 0;

      mvwprintw(docWin,37,5, "Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(docWin)) != 'y')
	{
	  wmove(docWin,37,44);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(docPanel);
      update_panels();
      doupdate();
      delwin(docWin);      
    } //while newRec == 1

  PQfinish(conn);
  endwin();

} //end
