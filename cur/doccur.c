#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
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
  PANEL *docPanel, *proAcctPanel, *supAcctPanel, *supInvPanel, *docTypePanel, *docUpdatePanel;
  WINDOW *docWin, *proAcctWin, *supAcctWin, *supInvWin, *docTypeWin, *docUpdateWin;
  FORM *docForm;
  FIELD *docField[11];
  int newRec = 'y';
  int docrow, doccol, parow, pacol, sarow, sacol, invrow, invcol, trow, tcol, urow, ucol;
  int ch;
  int cf;
  int cfUpdate = 0;
  int i = 0, j = 0;
  int list = 2;
  int rows;
  char p;
  int docProAcctID, supAcctID, supInvID, docTypeID;
  char docProAcctIDstr[5], supAcctIDStr[5], supInvIDStr[5], docTypeIDStr[5];
  int val, *params[1], length[1], formats[1];
  char parentType[3][9] = {"PROVIDER", "SUPPLIER", "INVOICE"};
  char parentSelected[9];
  //char parentSelected[1][9];
  int docfParentID, docfOid, docfTypeID, docfStartDt, docfEndDt;
  char docfFileName[30], docfRef[50], docfTitle[100], docfDesc[150], docfCat[30];
  int fExist;
  int upID;
  char updateCatalog[30];                     /* variable to store catalog for update */
  char f[] = "/tmp/";
  char e[strlen(docfFileName)];
  char fn[30];                                /* variable to store filename for update */
  
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
      set_field_type(docField[1],TYPE_REGEXP,"^[A-Za-z0-9 -./]+$");
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
      supAcctWin = newwin(20,50,1,120);
      supInvWin = newwin(20,50,1,120);
      docTypeWin = newwin(20,50,1,120);
      docUpdateWin = newwin(20,50,1,120);

      docPanel = new_panel(docWin);
      proAcctPanel = new_panel(proAcctWin);
      supAcctPanel = new_panel(supAcctWin);
      supInvPanel = new_panel(supInvWin);
      docTypePanel = new_panel(docTypeWin);
      docUpdatePanel = new_panel(docUpdateWin);
      
      update_panels();
      doupdate();

      keypad(docWin, TRUE);
      keypad(proAcctWin, TRUE);
      keypad(supAcctWin, TRUE);
      keypad(supInvWin, TRUE);
      keypad(docTypeWin, TRUE);
      keypad(docUpdateWin, TRUE);

      set_form_win(docForm, docWin);
      set_form_sub(docForm, derwin(docWin, docrow, doccol, 1, 1));
      getmaxyx(docWin, docrow, doccol);
      getmaxyx(proAcctWin, parow, pacol);
      getmaxyx(supAcctWin, sarow, sacol);
      getmaxyx(supInvWin, invrow, invcol);
      getmaxyx(docTypeWin, trow, tcol);
      getmaxyx(docUpdateWin, urow, ucol);
      box(docWin,0,0);
      box(proAcctWin,0,0);
      box(supAcctWin,0,0);
      box(supInvWin,0,0);
      box(docTypeWin,0,0);
      box(docUpdateWin,0,0);
      waddstr(docWin, "Document Import Form");
      waddstr(proAcctWin, "Provider Account");
      waddstr(supAcctWin, "Suppler Account");
      waddstr(supInvWin, "Invoice");
      waddstr(docTypeWin, "Document Type");
      waddstr(docUpdateWin, "Document");      

      if(docWin == NULL || proAcctWin == NULL  || supAcctWin == NULL || supInvWin == NULL || docTypeWin == NULL  || docUpdateWin == NULL)
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
      mvwprintw(docWin, 40,5, "ParentID: F2-Provider F3-Supplier F4-Invoice");
      mvwprintw(docWin, docrow-2, 5, "Press F1 when form complete (F9 for Update)");
      wmove(docWin,3,34);
      wrefresh(docWin);     

      while((ch = wgetch(docWin)) != KEY_F(1))
	{
	  hide_panel(proAcctPanel);
	  hide_panel(supAcctPanel);
	  hide_panel(supInvPanel);
	  hide_panel(docTypePanel);
	  hide_panel(docUpdatePanel);
	  show_panel(docPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, docForm);
	  if(ch == KEY_F(2))
	    {
	      strcpy(parentSelected, parentType[0]);	     /* assign PROVIDER to parentSelected to be used for catalog field */
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
		  // strcpy(docfCat, parentSelected[0]);     //DEBUG
		  //mvwprintw(proAcctWin,3,1,"parent type %s",docfCat); // DEBUG CODE
		  wrefresh(proAcctWin);		
		}
	      noecho();
	      PQclear(res);	      
	    } //F2
	  if(ch == KEY_F(3))
	    {
	      strcpy(parentSelected, parentType[1]);
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
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
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
	      mvwscanw(supAcctWin,11,25, "%5s", &supAcctIDStr);
	      set_field_buffer(docField[0],0, supAcctIDStr);

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      supAcctID = atoi(field_buffer(docField[0],0));
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
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F3
	  if(ch == KEY_F(4))
	    {
	      strcpy(parentSelected, parentType[2]);
	      i = j = rows = 0;
	      list = 2;
	      wclear(supInvWin);
	      box(supInvWin,0,0);
	      waddstr(supInvWin, "Invoice");
	      wmove(supInvWin,1,1);
	      //wrefresh(supInvWin);
	      show_panel(supInvPanel);
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_invoice ORDER BY supplier_invoice_id");	  
	      rows = PQntuples(res);

	      //wrefresh(supInvWin);
	  
	      while((p = wgetch(supInvWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(supInvWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,4));
		      list++;
		      wclrtoeol(supInvWin);  
		    }
		  list = 2;      
		  //wclrtoeol(supInvWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supInvWin);  
		      mvwprintw(supInvWin,10,1,"End of list");
		      box(supInvWin,0,0);
		      mvwprintw(supInvWin,0,0, "Invoice");
		      wmove(supInvWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supInvWin,11,1,"Select Option: ");
	      mvwscanw(supInvWin,11,25, "%5s", &supInvIDStr);
	      set_field_buffer(docField[0],0, supInvIDStr);

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      supInvID = atoi(field_buffer(docField[0],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supInvID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM supplier_invoice WHERE supplier_invoice_id = $1 ORDER BY supplier_invoice_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(supInvWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(supInvWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supInvWin);
		}
	      else
		{
		  mvwprintw(supInvWin,12,1,"Number invalid");
		  wrefresh(supInvWin);		
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F4
	  if(ch == KEY_F(5))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(docTypeWin);
	      box(docTypeWin,0,0);
	      waddstr(docTypeWin, "Document Type");
	      wmove(docTypeWin,1,1);
	      //wrefresh(docTypeWin);
	      show_panel(docTypePanel);
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM document_type ORDER BY type_id");	  
	      rows = PQntuples(res);

	      //wrefresh(docTypeWin);
	  
	      while((p = wgetch(docTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(docTypeWin,list,1,"%s %s ", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(docTypeWin);  
		    }
		  list = 2;      
		  //wclrtoeol(docTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(docTypeWin);  
		      mvwprintw(docTypeWin,10,1,"End of list");
		      box(docTypeWin,0,0);
		      mvwprintw(docTypeWin,0,0, "Document Type");
		      wmove(docTypeWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(docTypeWin,11,1,"Select Option: ");
	      mvwscanw(docTypeWin,11,25, "%5s", &docTypeIDStr);
	      set_field_buffer(docField[3],0, docTypeIDStr);

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      docTypeID = atoi(field_buffer(docField[3],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)docTypeID);
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
		  mvwprintw(docTypeWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(docTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(docTypeWin);
		}
	      else
		{
		  mvwprintw(docTypeWin,12,1,"Number invalid");
		  wrefresh(docTypeWin);		
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F5
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(docUpdateWin);
	      box(docUpdateWin,0,0);
	      waddstr(docUpdateWin, "Document");
	      wmove(docUpdateWin,1,1);
	      //wrefresh(docUpdateWin);
	      show_panel(docUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM documents ORDER BY document_id");	  
	      rows = PQntuples(res);

	      wrefresh(docUpdateWin);
	  
	      while((p = wgetch(docUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(docUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(docUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(docUpdateWin);  
		      mvwprintw(docUpdateWin,10,1,"End of list");
		      box(docUpdateWin,0,0);
		      mvwprintw(docUpdateWin,0,0, "Document");
		      wmove(docUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(docUpdateWin,11,1,"Select Option: ");
	      mvwscanw(docUpdateWin,11,25, "%d", &upID);

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM documents WHERE document_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(docUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(docUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(docUpdateWin);
		  set_field_buffer(docField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(docField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(docField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(docField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(docField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(docField[5],0,PQgetvalue(res,0,6));
		  set_field_buffer(docField[6],0,PQgetvalue(res,0,7));
		  set_field_buffer(docField[7],0,PQgetvalue(res,0,8));
		  set_field_buffer(docField[8],0,PQgetvalue(res,0,9));
		  set_field_buffer(docField[9],0,PQgetvalue(res,0,10));
		  cfUpdate = 1;
		  wrefresh(docUpdateWin);
		}
	      else
		{
		  mvwprintw(docUpdateWin,12,1,"Number invalied");
		  wrefresh(docUpdateWin);		
		  //wrefresh(PARENT_WIN);
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} //while not F1
      hide_panel(proAcctPanel);
      hide_panel(supAcctPanel);
      hide_panel(supInvPanel);
      hide_panel(docTypePanel);
      hide_panel(docUpdatePanel);
      update_panels();
      doupdate();

      form_driver(docForm, REQ_VALIDATION);

      docfParentID = atoi(field_buffer(docField[0],0));
      strcpy(docfFileName, trimWS(field_buffer(docField[1],0)));
      //docfOid = atoi(field_buffer(docField[2],0));
      docfTypeID = atoi(field_buffer(docField[3],0));
      strcpy(docfRef, trimWS(field_buffer(docField[4],0)));
      strcpy(docfTitle, trimWS(field_buffer(docField[5],0)));
      strcpy(docfDesc, trimWS(field_buffer(docField[6],0)));
      docfStartDt = atoi(field_buffer(docField[7],0));
      docfEndDt = atoi(field_buffer(docField[8],0));
      strcpy(docfCat, parentSelected);
      //set_field_buffer(docField[9],0,docfCat);

      if(cfUpdate == 1)
	{
	  set_field_buffer(docField[9],0,docfCat);
	  docfOid = atoi(field_buffer(docField[2],0));	  
	  strcpy(updateCatalog,field_buffer(docField[9],0));
	  strcpy(docfCat,updateCatalog);
	}
      else
	{
	  strcpy(docfCat, parentSelected);
	  set_field_buffer(docField[9],0,docfCat);
	}
			 
      if((form_driver(docForm,REQ_VALIDATION) == E_OK) && docfParentID >=1 && docfTypeID >=1)
	{
	  /* check to see if the file name entered exists */
	  if (cfUpdate == 0)
	    {
	      strcpy(e,docfFileName);
	      strcat(f,e);
	      fExist = checkFileExists(f);
	      if (fExist == 2)
		mvwprintw(docWin,30,5, "Error no file %s",f);
	    }	      
	  else
	    {
	      strcpy(fn, trimWS(field_buffer(docField[1],0)));
	      strcpy(e,fn);
	    }  
	  	    
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
		  documentImportDelete(upID, docfOid);
		  mvwprintw(docWin,34,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{		  
		  /* mvwprintw(docWin, 3,65,  "%d", docfParentID);
		  mvwprintw(docWin, 5,65,  "%s", e);
		  mvwprintw(docWin, 7,65,  "%d", docfOid);
		  mvwprintw(docWin, 9,65,  "%d", docfTypeID);
		  mvwprintw(docWin, 11,65, "%s", docfRef);
		  mvwprintw(docWin, 14,65, "%s", docfTitle);
		  mvwprintw(docWin, 19,65, "%s", docfDesc);
		  mvwprintw(docWin, 25,65, "%d", docfStartDt);
		  mvwprintw(docWin, 27,65, "%d", docfEndDt);
		  mvwprintw(docWin, 29,65, "%s", docfCat); */
		  docImportUpdate(upID, docfParentID, e, docfOid, docfTypeID, docfRef, docfTitle,docfDesc, docfStartDt, docfEndDt,docfCat);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(docWin,34,5, "Data updated");
		  //mvwprintw(docWin,35,5, "cfUpdate %d,upID %d, dtDesc %s", cfUpdate,upID, dtDesc);  //DEBUG
		}
      	      else
		{
		  documentImport(docfParentID, f, e, docfTypeID, docfRef, docfTitle, docfDesc, docfStartDt, docfEndDt, parentSelected);
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

/* Import the document and obtain the OID. 
   Update the document table with all the required fields
   including OID and catalog fields which are not entered by the user */
void documentImport(int dParentID, char f[], char e[], int dTypeID, char dRef[], char dTitle[],char dDesc[], int dStartDt, int dEndDt, char dCatalog[])
{
  Oid objImportID;
  
  PGconn *conn = fdbcon();
  PGresult *res;

/*File which is located in tmp directory is imported
  into pg_largeobject table and Oid number returned.*/
  res = PQexec(conn, "BEGIN");
  PQclear(res);
  objImportID = lo_import(conn, f);  
  res = PQexec(conn, "END");
  PQclear(res);

  if (objImportID > 0)
    {
      docImportInsert(dParentID, e, objImportID, dTypeID, dRef, dTitle, dDesc, dStartDt, dEndDt, dCatalog);
    }
  
  PQfinish(conn);
}

void documentImportDelete(int upID, int objImportID)
{
  PGconn *conn = fdbcon();
  PGresult *res;

  /*This will delete the file from the pg_largeobject and
    pg_largeobject_metadata tables. The binary document
    will no longer be available. The entry in the
    documents table then needs to be deleted. */
  res = PQexec(conn, "BEGIN");
  PQclear(res);
  res = PQexec(conn, "SELECT");
  lo_unlink(conn, objImportID);
  res = PQexec(conn, "END");
  PQclear(res);

  /* Delete corresponding row from documents table */
  docImportDelete(upID);      
  
  PQfinish(conn);  
}

