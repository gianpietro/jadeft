#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
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
  int ch;
  int cf, cfUpdate = 0;
  int range = 5, list = 2, i = 0, j = 0;
  int actInd;
  char sname[30];
  int newRec = 'y';
  int rows, cols, urows, ucols;
  char p;
  int trows, val, upID, *params[1], length[1],  formats[1];
 
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

    while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      supplierField[0] = new_field(1, 1, 2, 22, 0, 0);      
      supplierField[1] = new_field(1, 30, 4, 22, 0, 0);      
      supplierField[2] = NULL;

      /* Field 1 digit allowed in range from 1 to 2 */
      set_field_type(supplierField[0],TYPE_INTEGER,1,1,2);
      /* Field allowed values of A-Z a-z and hyphen */
      set_field_type(supplierField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      supplierForm = new_form(supplierField);
      scale_form(supplierForm, &rows, &cols);

      /* Add window which will be associated to form */
      supWin = newwin(rows+20, cols+10,1,1);
      supUpdateWin = newwin(20,50,1,120);
      keypad(supWin, TRUE);
      keypad(supUpdateWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(supplierForm, supWin);
      set_form_sub(supplierForm, derwin(supWin,rows,cols,1,1));
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

      waddstr(supWin,"Supplier Entry Form");

      post_form(supplierForm); 
      wrefresh(supWin);


      //mvwprintw(supWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(supWin,24, 5, "rows %d cols %d", rows, cols);
      mvwprintw(supWin,3,5, "Active Ind:");                           
      mvwprintw(supWin,5,5, "Supplier Name:");
      mvwprintw(supWin,23,5,"Press F1 when form complete");
      wmove(supWin,3,23);     /* move cursor */

      while((ch = wgetch(supWin)) != KEY_F(1))
	{
	  keyNavigate(ch, supplierForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(supUpdateWin);
	      box(supUpdateWin,0,0);
	      waddstr(supUpdateWin, "Supplier");
	      wmove(supUpdateWin,1,1);
	      wrefresh(supUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier WHERE active_ind = 1 ORDER BY supplier_id");	  
	      trows = PQntuples(res);

	      wrefresh(supUpdateWin);
	  
	      while((p = wgetch(supUpdateWin)) == '\n')
		{
		  if ( j + range < trows)
		    j = j + range;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supUpdateWin);
		      // box(supUpdateWin,0,0);		     
		    }
		  list = 2;      
		  //wclrtoeol(supUpdateWin);  
		  if  (i == trows)
		    {
		      wclrtobot(supUpdateWin);  
		      mvwprintw(supUpdateWin,10,1,"End of list");
		      box(supUpdateWin,0,0);
		      mvwprintw(supUpdateWin,0,0, "Supplier");
		      wmove(supUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supUpdateWin,11,1,"Select Supplier: ");
	      mvwscanw(supUpdateWin,11,25, "%d", &upID);
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
		  mvwprintw(supUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(supUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supUpdateWin);
		  set_field_buffer(supplierField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(supplierField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(supUpdateWin,12,1,"Number invalied");
		  wrefresh(supUpdateWin);		
		  wrefresh(supWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
      } //while F1
	
      form_driver(supplierForm,REQ_VALIDATION);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(supplierField[0],0));
      strcpy(sname, field_buffer(supplierField[1],0));

      if ((form_driver(supplierForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*sname)))
	{
	  strcpy(sname, trimWS(sname));  
	  echo();
	  mvwprintw(supWin,17,5,"Save: y/n: ");     
          mvwprintw(supWin,18,5,"(d = delete record)");
          wmove(supWin,17,16);
	  while((cf = wgetch(supWin)) != 'y')
	    {
	      wmove(supWin,17,16);
	      if (cf == 'n')
		{
	 	  mvwprintw(supWin,19,5, "Data not saved");
		  break;
		}
	      /* NEW CODE FOR DELETE */
	      if (cf == 'd')
		{  
		      supplierDelete(upID);
		      mvwprintw(supWin,19,5, "Record deleted");                
		      break;
		 }		 
	      /* END OF NEW CODE FOR DELETE */
           }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  supplierUpdate(upID,actInd,sname);
		  mvwprintw(supWin,19,5, "cfUpdate %d,upID %d actInd %d sname %s", cfUpdate,upID,actInd, sname);
		}
	      else
		{
		  supplierInsert(actInd,sname);   /* Save data to database */
	          mvwprintw(supWin,19,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(supWin,19,5, "Data invalid");
	}
      noecho();
      
      unpost_form(supplierForm);
      free_form(supplierForm);
      free_field(supplierField[0]);
      free_field(supplierField[1]);
      free_field(supplierField[2]);

      cfUpdate = 0;
      mvwprintw(supWin,22,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supWin)) != 'y')
	{
	  wmove(supWin,22,44);	  
	  if(newRec == 'n')
	    break;
	}
      noecho();
    }
    PQfinish(conn);  
  endwin();
}


void suppTypeInsert()
{
  FIELD *supTypeField[2];
  FORM *supTypeForm;
  WINDOW *supTypeWin, *supTypeUpdateWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char sdesc[30];
  int cf;
  int cfUpdate = 0;
  int range = 5, list = 2, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      supTypeField[0] = new_field(1,30,2,18,0,0);
      supTypeField[1] = NULL;
      set_field_type(supTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      supTypeForm = new_form(supTypeField);
      scale_form(supTypeForm, &rows, &cols);
      supTypeWin = newwin(rows+15,cols+5,1,120);
      supTypeUpdateWin = newwin(20,50,30,120);
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

      waddstr(supTypeWin,"Supplier Type Form");
      post_form(supTypeForm); 
      wrefresh(supTypeWin);

      mvwprintw(supTypeWin,rows-14,cols-46, "Description:");
      mvwprintw(supTypeWin,rows-2,cols-46,"Press F1 when form complete");
      wmove(supTypeWin,rows-14,cols-33);     /* move cursor */

      while((ch = wgetch(supTypeWin)) != KEY_F(1))
	{
	  keyNavigate(ch, supTypeForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(supTypeUpdateWin);
	      box(supTypeUpdateWin,0,0);
	      waddstr(supTypeUpdateWin, "Supplier Type");
	      wmove(supTypeUpdateWin,1,1);
	      wrefresh(supTypeUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_type ORDER BY supplier_type_id");	  
	      trows = PQntuples(res);

	      wrefresh(supTypeUpdateWin);
	  
	      while((p = wgetch(supTypeUpdateWin)) == '\n')
		{
		  if ( j + range < trows)
		    j = j + range;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supTypeUpdateWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(supTypeUpdateWin);
		    }
		  list = 2;      
		  if  (i == trows)
		    {
		      wclrtobot(supTypeUpdateWin);  
		      mvwprintw(supTypeUpdateWin,10,1,"End of list");
		      box(supTypeUpdateWin,0,0);
		      mvwprintw(supTypeUpdateWin,0,0, "Supplier Type");
		      wmove(supTypeUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supTypeUpdateWin,11,1,"Select Supplier: ");
	      mvwscanw(supTypeUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(supTypeUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(supTypeUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(supTypeUpdateWin);
		  set_field_buffer(supTypeField[0],0,PQgetvalue(res,0,1));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(supTypeUpdateWin,12,1,"Number invalied");
		  wrefresh(supTypeUpdateWin);		
		  wrefresh(supTypeWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9	
	} // While F1
      
      form_driver(supTypeForm,REQ_VALIDATION);

      strcpy(sdesc,field_buffer(supTypeField[0],0));
     
      if ((form_driver(supTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*sdesc)))
	{
	  strcpy(sdesc, trimWS(sdesc));
	  echo();
	  mvwprintw(supTypeWin,rows-8,cols-46, "Save y/n: ");
	  mvwprintw(supTypeWin,rows-7,cols-46,"(d = delete record)");
	  wmove(supTypeWin,rows-8,cols-34);
	  while((cf = wgetch(supTypeWin)) != 'y')
	    {
	      wmove(supTypeWin,rows-8,cols-34);
	      if (cf == 'n')
		{
		  mvwprintw(supTypeWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  //supTypeDelete(upID);
		  mvwprintw(supTypeWin,rows-6,cols-46, "Record deleted");                
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  supTypeUpdate(upID,sdesc); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(supTypeWin,rows-6,cols-46, "Data updated");
		  mvwprintw(supTypeWin,20,5, "cfUpdate %d,upID %d sdesc %s", cfUpdate,upID,sdesc);  //DEBUG
		}
	      else
		{
		  supTypeInsert(sdesc);
		  mvwprintw(supTypeWin,rows-6,cols-46, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(supTypeWin,rows-6,cols-46, "Data invalid");
	}
      noecho();

      unpost_form(supTypeForm);
      free_form(supTypeForm);
      free_field(supTypeField[0]);
      free_field(supTypeField[1]);

      cfUpdate = 0;
      mvwprintw(supTypeWin,rows-4,cols-46,"Do you want to update record y/n: ");
      echo();
      while((newRec = wgetch(supTypeWin)) != 'y')
	{
	  wmove(supTypeWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    }  //while newRec=y
  PQfinish(conn);
  endwin(); 
}


void paymentPeriodInsert()
{
  FIELD *payPerField[2];
  FORM *payPerForm;
  WINDOW *payPerWin, *payPerUpdateWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char payPer[30];
  int cf;
  int cfUpdate = 0;
  int range = 5, list = 2, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      payPerField[0] = new_field(1,30,2,18,0,0);
      payPerField[1] = NULL;
      set_field_type(payPerField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      payPerForm = new_form(payPerField);
      scale_form(payPerForm, &rows, &cols);
      payPerWin = newwin(rows+15,cols+5,1,120);
      payPerUpdateWin = newwin(20,50,30,120);
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

      waddstr(payPerWin,"Payment Period Form");
      post_form(payPerForm); 
      wrefresh(payPerWin);

      mvwprintw(payPerWin,rows-14,cols-46, "Pay Period:");
      mvwprintw(payPerWin,rows-2,cols-46,"Press F1 when form complete");
      wmove(payPerWin,rows-14,cols-33);     /* move cursor */

      while((ch = wgetch(payPerWin)) != KEY_F(1))
	{
	  keyNavigate(ch, payPerForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(payPerUpdateWin);
	      box(payPerUpdateWin,0,0);
	      waddstr(payPerUpdateWin, "Pay Period");
	      wmove(payPerUpdateWin,1,1);
	      wrefresh(payPerUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM payment_period ORDER BY payment_period_id");	  
	      trows = PQntuples(res);

	      wrefresh(payPerUpdateWin);
	  
	      while((p = wgetch(payPerUpdateWin)) == '\n')
		{
		  if ( j + range < trows)
		    j = j + range;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(payPerUpdateWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(payPerUpdateWin);
		    }
		  list = 2;      
		  if  (i == trows)
		    {
		      wclrtobot(payPerUpdateWin);  
		      mvwprintw(payPerUpdateWin,10,1,"End of list");
		      box(payPerUpdateWin,0,0);
		      mvwprintw(payPerUpdateWin,0,0, "Pay Period");
		      wmove(payPerUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(payPerUpdateWin,11,1,"Select Option: ");
	      mvwscanw(payPerUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(payPerUpdateWin,13,1, "no or rows %d ",trows);  //DEBUG
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(payPerUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(payPerUpdateWin);
		  set_field_buffer(payPerField[0],0,PQgetvalue(res,0,1));
		  //set_field_buffer(payPerField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(payPerUpdateWin,12,1,"Number invalied");
		  wrefresh(payPerUpdateWin);		
		  wrefresh(payPerWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
	} //while F1
      
      form_driver(payPerForm,REQ_VALIDATION);

      strcpy(payPer,field_buffer(payPerField[0],0));
     
      if ((form_driver(payPerForm,REQ_VALIDATION) == E_OK) && (!isspace(*payPer)))
	{
	  strcpy(payPer, trimWS(payPer));
	  echo();
	  mvwprintw(payPerWin,rows-8,cols-46, "Save y/n: ");
	  mvwprintw(payPerWin,rows-7,cols-46,"(d = delete record)");
	  wmove(payPerWin,rows-8,cols-36);
	  while((cf = wgetch(payPerWin)) != 'y')
	    {
	      wmove(payPerWin,rows-8,cols-36);
	      if (cf == 'n')
		{
		  mvwprintw(payPerWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  //payPerDelete(upID);
		  mvwprintw(payPerWin,rows-6,cols-46, "Record deleted");                
		  break;
		}
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  payPerUpdate(upID,payPer); 
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(payPerWin,rows-6,cols-46, "Data updated");
		  mvwprintw(payPerWin,rows-5,cols-46, "cfUpdate %d,upID %d payPer %s", cfUpdate,upID,payPer);  //DEBUG
		}
	      else
		{
		  payPeriodInsert(payPer);
		  mvwprintw(payPerWin,rows-6,cols-46, "Data saved");	
		}
	    }
	}
      else
	{
	  mvwprintw(payPerWin,rows-6,cols-46, "Data invalid");
	}
      noecho();

      unpost_form(payPerForm);
      free_form(payPerForm);
      free_field(payPerField[0]);
      free_field(payPerField[1]);

      cfUpdate = 0;

      mvwprintw(payPerWin,rows-4,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(payPerWin)) != 'y')
	{
	  wmove(payPerWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    }
  PQfinish(conn);
  endwin();    
}

void propertyInsert()
{
  FIELD *propertyField[5];
  FORM *propertyForm;
  WINDOW *prtWin, *prtUpdateWin;
  int ch;
  int cf;                 /* confirm save data */
  int actInd;             /* active_Ind */
  char prtPostCode[30];   /* post_code */
  char prtAddress[30];
  char prtCity[30];
  int newRec= 'y';        /* Add another new record */
  int rows, cols;
  int cfUpdate = 0;
  int range = 5, list = 2, i = 0, j = 0;
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1],  formats[1];

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      propertyField[0] = new_field(1, 1, 2, 22, 0, 0);      // active_ind
      propertyField[1] = new_field(1, 10, 4, 22, 0, 0);     // post_code
      propertyField[2] = new_field(1, 30, 6, 22, 0, 0);     // address
      propertyField[3] = new_field(1, 30, 8, 22, 0, 0);     // city
      propertyField[4] = NULL;

      set_field_type(propertyField[0],TYPE_INTEGER,1,1,2);
      set_field_type(propertyField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[3],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      propertyForm = new_form(propertyField);
      scale_form(propertyForm, &rows, &cols);

      /* Add window which will be associated to form */
      prtWin = newwin(31,81,1,1);
      prtUpdateWin = newwin(20,50,1,120);

      keypad(prtWin, TRUE);
      keypad(prtUpdateWin, TRUE);

      /* Set main and sub windows */
      set_form_win(propertyForm, prtWin);
      set_form_sub(propertyForm, derwin(prtWin,rows,cols,1,1));
      getmaxyx(prtWin,rows,cols);
            
      box(prtWin, 0,0);
      box(prtUpdateWin,0,0);
          
      if (prtWin == NULL)// || prtUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(prtWin,"Property Entry Form");
      mvwprintw(prtWin,29,1,"row %d col %d", rows, cols);

      post_form(propertyForm);
      wrefresh(prtWin);

      mvwprintw(prtWin,3,5, "Active Ind:");                           
      mvwprintw(prtWin,5,5, "Post Code:");
      mvwprintw(prtWin,7,5, "Address:");
      mvwprintw(prtWin,9,5, "City:");
            
      mvwprintw(prtWin,28,1,"Press F1 when form complete");
      wmove(prtWin,3,23);     /* move cursor */

      while((ch = wgetch(prtWin)) != KEY_F(1))
	{
	  keyNavigate(ch, propertyForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(prtUpdateWin);
	      box(prtUpdateWin,0,0);
	      waddstr(prtUpdateWin, "Property");
	      wmove(prtUpdateWin,1,1);
	      wrefresh(prtUpdateWin);

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM property WHERE active_ind = 1 ORDER BY property_id");	  
	      trows = PQntuples(res);

	      wrefresh(prtUpdateWin);
	  
	      while((p = wgetch(prtUpdateWin)) == '\n')
		{
		  if ( j + range < trows)
		    j = j + range;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(prtUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(prtUpdateWin);
		    }
		  list = 2;      
		  if  (i == trows)
		    {
		      wclrtobot(prtUpdateWin);  
		      mvwprintw(prtUpdateWin,10,1,"End of list");
		      box(prtUpdateWin,0,0);
		      mvwprintw(prtUpdateWin,0,0, "Property");
		      wmove(prtUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(prtUpdateWin,11,1,"Select Option: ");
	      mvwscanw(prtUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(prtUpdateWin,13,1, "no or rows %d ",trows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(prtUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(prtUpdateWin);
		  set_field_buffer(propertyField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(propertyField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(propertyField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(propertyField[3],0,PQgetvalue(res,0,4));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(prtUpdateWin,12,1,"Number invalied");
		  wrefresh(prtUpdateWin);		
		  wrefresh(prtWin);
		}
	      noecho();
	      PQclear(res);
	    } //while F9 
	} //while F1
	  
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
	  mvwprintw(prtWin,15,5,"Save: y/n: ");
	  mvwprintw(prtWin,16,5,"(d = delete record)");
	  wmove(prtWin,15,18);
	  while((cf = wgetch(prtWin)) != 'y')
	    {
	      wmove(prtWin,15,18);
	      if (cf == 'n')
		{
		  mvwprintw(prtWin,22,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  prtDelete(upID);
		  mvwprintw(prtWin,rows-6,cols-65, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  prtUpdate(upID, actInd, prtPostCode, prtAddress, prtCity); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(prtWin,rows-6,cols-65, "Data updated");
		  mvwprintw(prtWin,rows-5,cols-65, "cfUpdate %d,upID %d actInd %d sname %s", cfUpdate,upID,actInd, prtPostCode);  //DEBUG
		}
	      else 
		{
		  prtInsert(actInd, prtPostCode, prtAddress, prtCity);
	          mvwprintw(prtWin,rows-6,cols-65, "Data saved");		  
		}
	    }
	}
      else
	{
	  mvwprintw(prtWin,22,5, "Data invalid");
	}
      noecho();

      unpost_form(propertyForm);
      free_form(propertyForm);
      free_field(propertyField[0]);
      free_field(propertyField[1]);
      free_field(propertyField[2]);
      free_field(propertyField[3]);

      cfUpdate = 0;

      mvwprintw(prtWin,25,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(prtWin)) != 'y')
	{
	  wmove(prtWin,25,42);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      delwin(prtWin);
    }  //while newrec = y
  PQfinish(conn);
  endwin();  
}


int suppAccountInsert()
{
  WINDOW * supAcctWin, * supWin, * supTypeWin, * prtWin, * payWin, * paWin, *supUpdateWin;
  FORM * supAcctForm;
  FIELD * supAcctField[13];
  int i = 0, j = 0;
  int range = 5;
  char p;
  int ch;
  int srow, scol, strow, stcol, sarow, sacol, prrow, prcol, pyrow, pycol, parow, pacol, urows, ucols;
  int list = 2;
  int supID, prtID, supTID, payID, proANo;
  char supIDstr[5], prtIDstr[5], supTIDstr[5], payIDstr[5], proANostr[30];
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  int safActiveID, safSupID, safPrtID, safSupTypeID, safStartDt, safEndDt, safPayID, safProAcctID;
  float safAmount;
  char safSupAcctRef[30], safComment[30], safAlias[30];
  int cf;
  int newRec = 'y';
  int cfUpdate = 0;

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  
  while (newRec == 'y')
    {
      supAcctField[0] = new_field(1,1,2,33,0,0);    /* active_ind */
      supAcctField[1] = new_field(1,30,4,33,0,0);   /* supplier_acct_ref */
      supAcctField[2] = new_field(1,5,6,33,0,0);    /* supplier_id */
      supAcctField[3] = new_field(1,5,8,33,0,0);    /* property_id */
      supAcctField[4] = new_field(1,5,10,33,0,0);   /* supplier_type_id */
      supAcctField[5] = new_field(1,8,12,33,0,0);   /* start_date */
      supAcctField[6] = new_field(1,8,14,33,0,0);   /* end_date */
      supAcctField[7] = new_field(1,5,16,33,0,0);   /* payment_period_id */
      supAcctField[8] = new_field(1,10,18,33,0,0);  /* amount */
      supAcctField[9] = new_field(1,30,20,33,0,0);  /* comment */
      supAcctField[10] = new_field(1,10,22,33,0,0); /* alias */
      supAcctField[11] = new_field(1,5,24,33,0,0);  /* provider_acct_id */
      supAcctField[12] = NULL;

      set_field_type(supAcctField[0],TYPE_INTEGER,1,1,2);
      set_field_type(supAcctField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[2],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[4],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[5],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[6],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[7],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[8],TYPE_NUMERIC,2,-100000,1000000);
      set_field_type(supAcctField[9],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[10],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[11],TYPE_INTEGER,0,1,99999);

      supAcctForm = new_form(supAcctField);
      scale_form(supAcctForm, &sarow, &sacol);

      supAcctWin = newwin(sarow+15, sacol+10,1,1);
      supWin = newwin(20,50,1,120);
      prtWin = newwin(20,50,1,120);
      supTypeWin = newwin(20,50,1,120);
      payWin = newwin(20,50,1,120);
      paWin = newwin(20,50,1,120);
      supUpdateWin = newwin(40,73,1,120);
      
      keypad(supAcctWin, TRUE);
      keypad(supWin, TRUE);
      keypad(prtWin, TRUE);
      keypad(supTypeWin, TRUE);
      keypad(payWin, TRUE);
      keypad(paWin, TRUE);
      keypad(supUpdateWin, TRUE);

      set_form_win(supAcctForm,supAcctWin);
      set_form_sub(supAcctForm, derwin(supAcctWin,sarow,sacol,1,1));
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
      waddstr(supAcctWin, "Supplier Account Form");
      waddstr(supWin, "Supplier");
      waddstr(prtWin, "Property");
      waddstr(supTypeWin, "Supplier Type");
      waddstr(payWin, "Payment Period");
      waddstr(paWin, "Provider Account");
      waddstr(supUpdateWin, "Supplier Account");

      if(supAcctWin == NULL || supWin == NULL || prtWin == NULL ||supTypeWin == NULL || payWin == NULL || paWin == NULL || supUpdateWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  return(1);
	}

      post_form(supAcctForm);
      wrefresh(supAcctWin);

      mvwprintw(supAcctWin, 37, 5,"row %d col %d", sarow, sacol);

      mvwprintw(supAcctWin, 3,5, "Active Ind:");
      mvwprintw(supAcctWin, 5,5, "Supplier Account No:");
      mvwprintw(supAcctWin, 7,5, "Supplier ID(F2):");
      mvwprintw(supAcctWin, 9,5, "Property ID(F3):");
      mvwprintw(supAcctWin, 11,5, "Supplier Type ID(F4):");
      mvwprintw(supAcctWin, 13,5, "Start Date:");
      mvwprintw(supAcctWin, 15,5, "End Date:");
      mvwprintw(supAcctWin, 17,5, "Payment Period(F5):");
      mvwprintw(supAcctWin, 19,5, "Amount:");
      mvwprintw(supAcctWin, 21,5, "Comment:");
      mvwprintw(supAcctWin, 23,5, "Alias:");
      mvwprintw(supAcctWin, 25,5, "Provider Account ID(F6):");
      wmove(supAcctWin,3,34);
      wrefresh(supAcctWin);

      while((ch = wgetch(supAcctWin)) != KEY_F(1))
      {
	keyNavigate(ch, supAcctForm);
	if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(supWin);
	      box(supWin,0,0);
	      waddstr(supWin, "Supplier");
	      wmove(supWin,1,1);
	      wrefresh(supWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier WHERE active_ind = 1 ORDER BY supplier_id");	  
	      rows = PQntuples(res);

	      wrefresh(supWin);
	  
	      while((p = wgetch(supWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supWin);  
		    }
		  list = 2;      
		  //wclrtoeol(supWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supWin);  
		      mvwprintw(supWin,10,1,"End of list");
		      box(supWin,0,0);
		      mvwprintw(supWin,0,0, "Supplier");
		      wmove(supWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supWin,11,1,"Select Supplier: ");
	      mvwscanw(supWin,11,25, "%5s", &supIDstr);
	      set_field_buffer(supAcctField[2],0, supIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      supID = atoi(field_buffer(supAcctField[2],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supID);
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
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(supWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(supWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supWin);
		}
	      else
		{
		  mvwprintw(supWin,12,1,"Number invalied");
		  wrefresh(supWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } //F2
	if(ch == KEY_F(3))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(prtWin);
	      box(prtWin,0,0);
	      waddstr(prtWin, "Property");
	      wmove(prtWin,1,1);
	      wrefresh(prtWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM property WHERE active_ind = 1 ORDER BY property_id");	  
	      rows = PQntuples(res);

	      wrefresh(prtWin);
	  
	      while((p = wgetch(prtWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(prtWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(prtWin);  
		    }
		  list = 2;      
		  //wclrtoeol(prtWin);  
		  if  (i == rows)
		    {
		      wclrtobot(prtWin);  
		      mvwprintw(prtWin,10,1,"End of list");
		      box(prtWin,0,0);
		      mvwprintw(prtWin,0,0, "Property");
		      wmove(prtWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(prtWin,11,1,"Select Property: ");
	      mvwscanw(prtWin,11,25, "%5s", &prtIDstr);
	      set_field_buffer(supAcctField[3],0, prtIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      prtID = atoi(field_buffer(supAcctField[3],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)prtID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM property WHERE property_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(prtWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(prtWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(prtWin);
		}
	      else
		{
		  mvwprintw(prtWin,12,1,"Number invalied");
		  wrefresh(prtWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F3
	if(ch == KEY_F(4))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(supTypeWin);
	      box(supTypeWin,0,0);
	      waddstr(supTypeWin, "Supplier Type");
	      wmove(supTypeWin,1,1);
	      wrefresh(supTypeWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_type ORDER BY supplier_type_id");	  
	      rows = PQntuples(res);

	      wrefresh(supTypeWin);
	  
	      while((p = wgetch(supTypeWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supTypeWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(supTypeWin);  
		    }
		  list = 2;      
		  //wclrtoeol(supTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supTypeWin);  
		      mvwprintw(supTypeWin,10,1,"End of list");
		      box(supTypeWin,0,0);
		      mvwprintw(supTypeWin,0,0, "Supplier Type");
		      wmove(supTypeWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supTypeWin,11,1,"Select Supplier Type: ");
	      mvwscanw(supTypeWin,11,25, "%5s", &supTIDstr);
	      set_field_buffer(supAcctField[4],0, supTIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      supTID = atoi(field_buffer(supAcctField[4],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supTID);
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
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(supTypeWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(supTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(supTypeWin);
		}
	      else
		{
		  mvwprintw(supTypeWin,12,1,"Number invalied");
		  wrefresh(supTypeWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F4
	if(ch == KEY_F(5))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(payWin);
	      box(payWin,0,0);
	      waddstr(payWin, "Payment Period");
	      wmove(payWin,1,1);
	      wrefresh(payWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM payment_period ORDER BY payment_period_id");	  
	      rows = PQntuples(res);

	      wrefresh(payWin);
	  
	      while((p = wgetch(payWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(payWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(payWin);  
		    }
		  list = 2;      
		  // wclrtoeol(payWin);  
		  if  (i == rows)
		    {
		      wclrtobot(payWin);  
		      mvwprintw(payWin,10,1,"End of list");
		      box(payWin,0,0);
		      mvwprintw(payWin,0,0, "Payment Period");
		      wmove(payWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(payWin,11,1,"Select Pay Period: ");
	      mvwscanw(payWin,11,25, "%5s", &payIDstr);
	      set_field_buffer(supAcctField[7],0, payIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      payID = atoi(field_buffer(supAcctField[7],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)payID);
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
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(payWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(payWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(payWin);
		}
	      else
		{
		  mvwprintw(payWin,12,1,"Number invalied");
		  wrefresh(payWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } //F5
	if(ch == KEY_F(6))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(paWin);
	      box(paWin,0,0);
	      waddstr(paWin, "Provider Account");
	      wmove(paWin,1,1);
	      wrefresh(paWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(paWin);
	  
	      while((p = wgetch(paWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		       /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(paWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,2),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(paWin);  
		    }
		  list = 2;      
		  //wclrtoeol(paWin);  
		  if  (i == rows)
		    {
		      wclrtobot(paWin);  
		      mvwprintw(paWin,10,1,"End of list");
		      box(paWin,0,0);
		      mvwprintw(paWin,0,0, "Provider Account");
		      wmove(paWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(paWin,11,1,"Select Provider Account: ");
	      mvwscanw(paWin,11,25, "%5s", &proANostr);
	      set_field_buffer(supAcctField[11],0, proANostr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      proANo = atoi(field_buffer(supAcctField[11],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)proANo);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_account WHERE provider_acct_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(paWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(paWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(paWin);
		}
	      else
		{
		  mvwprintw(paWin,12,1,"Number invalied");
		  wrefresh(paWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } // F6
	if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(supUpdateWin);
	      box(supUpdateWin,0,0);
	      waddstr(supUpdateWin, "Supplier Account");
	      wmove(supUpdateWin,1,1);
	      wrefresh(supUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM supplier_account WHERE active_ind = 1 ORDER BY supplier_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(supUpdateWin);
	  
	      while((p = wgetch(supUpdateWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(supUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(supUpdateWin);  
		      mvwprintw(supUpdateWin,10,1,"End of list");
		      box(supUpdateWin,0,0);
		      mvwprintw(supUpdateWin,0,0, "Supplier Account");
		      wmove(supUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(supUpdateWin,11,1,"Select Option: ");
	      mvwscanw(supUpdateWin,11,25, "%d", &upID);

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM supplier_account WHERE supplier_acct_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(supUpdateWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(supUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supUpdateWin);
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
		  mvwprintw(supUpdateWin,12,1,"Number invalied");
		  wrefresh(supUpdateWin);		
		  wrefresh(supAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
      } //while not F1

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

      if ((form_driver(supAcctForm,REQ_VALIDATION) == E_OK) && (safActiveID >= 1))
	{
	  echo();
	  mvwprintw(supAcctWin,32,5,"Save: y/n: ");
	  mvwprintw(supUpdateWin,33,5,"(d = delete record)");
	  wmove(supUpdateWin,32,5);

	  while((cf = wgetch(supAcctWin)) != 'y')
	    {
	      wmove(supAcctWin,32,16);
	      if (cf == 'n')
		{
		  mvwprintw(supAcctWin,34,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  supAccountDelete(upID);
		  mvwprintw(supAcctWin,34,5, "Record deleted");                
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
		  mvwprintw(supAcctWin,19,5, "Data updated");
		  mvwprintw(supAcctWin,20,5, "cfUpdate %d,upID %d, safSupAcctRef %d, safSupID %d", cfUpdate,upID, safSupAcctRef, safSupID);  //DEBUG
		}
	      else
		{
		  supAccountInsert(safActiveID, safSupAcctRef, safSupID, safPrtID, safSupTypeID, safStartDt,
				   safEndDt, safPayID, safAmount, safComment, safAlias, safProAcctID);
		  mvwprintw(supAcctWin,34,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(supAcctWin,34,5, "Data invalid");	
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

      mvwprintw(supAcctWin,37,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supAcctWin)) != 'y')
	{
	  wmove(supAcctWin,37,44);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      delwin(supAcctWin);
    } //while newRec = y

  PQfinish(conn);
  endwin();      
  
  return 0;
}
    
