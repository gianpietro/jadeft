#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/supcur.h"
#include "../inc/jadlib.h"


void suppInsert()
{
  FIELD *supplierField[3];
  FORM *supplierForm;
  WINDOW *supWin;
  int ch;
  int cf;
  int actInd;
  char sname[30];
  int newRec = 'y';
  int rows, cols;


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
      supWin = newwin(rows+15, cols+20,1,1);  
      keypad(supWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(supplierForm, supWin);
      set_form_sub(supplierForm, derwin(supWin,rows,cols,2,2));
      getmaxyx(supWin,rows,cols);
      
      box(supWin, 0,0);
          
      if (supWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(supWin,"Supplier Entry Form");

      post_form(supplierForm); 
      wrefresh(supWin);

      //mvwprintw(supWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(supWin,rows-16,cols-65, "Active Ind:");                           
      mvwprintw(supWin,rows-14,cols-65, "Supplier Name:");
      mvwprintw(supWin,rows-2,cols-65,"Press F1 when form complete");
      wmove(supWin, rows-16,cols-48);     /* move cursor */

      while((ch = wgetch(supWin)) != KEY_F(1))
	 keyNavigate(ch, supplierForm);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(supplierField[0],0));
      strcpy(sname, trimWS(field_buffer(supplierField[1],0)));

      if ((form_driver(supplierForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*sname)))
	{
	  //strcpy(sname, trimWS(sname));  
	  echo();
	  mvwprintw(supWin,rows-8,cols-65,"Save: y/n: ");     

	  while((cf = wgetch(supWin)) != 'y')
	    {
	      wmove(supWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(supWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      // providerInsert(actInd,sname);   /* Save data to database */
	      mvwprintw(supWin,rows-6,cols-65, "Data saved");
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

      mvwprintw(supWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supWin)) != 'y')
	{
	  wmove(supWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    } 
  endwin();
}


void suppTypeInsert()
{
  FIELD *supTypeField[2];
  FORM *supTypeForm;
  WINDOW *supTypeWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char sdesc[30];
  int cf;

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
      keypad(supTypeWin, TRUE);

      set_form_win(supTypeForm, supTypeWin);	
      set_form_sub(supTypeForm, derwin(supTypeWin,rows,cols,2,2));
      getmaxyx(supTypeWin,rows,cols);
      
      box(supTypeWin, 0,0);
          
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
	keyNavigate(ch, supTypeForm);

      strcpy(sdesc,field_buffer(supTypeField[0],0));
     
      if ((form_driver(supTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*sdesc)))
	{
	  strcpy(sdesc, trimWS(sdesc));
	  echo();
	  mvwprintw(supTypeWin,rows-8,cols-46, "Save y/n: ");
	  while((cf = wgetch(supTypeWin)) != 'y')
	    {
	      wmove(supTypeWin,rows-8,cols-36);
	      if (cf == 'n')
		{
		  mvwprintw(supTypeWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      //proTypeInsert(sdesc);
	      mvwprintw(supTypeWin,rows-6,cols-46, "Data saved");
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

      mvwprintw(supTypeWin,rows-4,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supTypeWin)) != 'y')
	{
	  wmove(supTypeWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      endwin();
    }
}

void paymentPeriodInsert()
{
  FIELD *payPerField[2];
  FORM *payPerForm;
  WINDOW *payPerWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char payPer[30];
  int cf;

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
      keypad(payPerWin, TRUE);

      set_form_win(payPerForm, payPerWin);	
      set_form_sub(payPerForm, derwin(payPerWin,rows,cols,2,2));
      getmaxyx(payPerWin,rows,cols);
      
      box(payPerWin, 0,0);
          
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
	keyNavigate(ch, payPerForm);

      strcpy(payPer,field_buffer(payPerField[0],0));
     
      if ((form_driver(payPerForm,REQ_VALIDATION) == E_OK) && (!isspace(*payPer)))
	{
	  strcpy(payPer, trimWS(payPer));
	  echo();
	  mvwprintw(payPerWin,rows-8,cols-46, "Save y/n: ");
	  while((cf = wgetch(payPerWin)) != 'y')
	    {
	      wmove(payPerWin,rows-8,cols-36);
	      if (cf == 'n')
		{
		  mvwprintw(payPerWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      //proTypeInsert(sdesc);
	      mvwprintw(payPerWin,rows-6,cols-46, "Data saved");
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

      mvwprintw(payPerWin,rows-4,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(payPerWin)) != 'y')
	{
	  wmove(payPerWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      endwin();
    }
}

void propertyInsert()
{
  FIELD *propertyField[5];
  FORM *propertyForm;
  WINDOW *prtWin;
  int ch;
  int cf;            /* confirm save data */
  int actInd;        /* active_Ind */
  char prtPostCode[10];    /* post_code */
  char prtAddress[30];
  char prtCity[30];
  //int index , i;
  int newRec= 'y';   /* Add another new record */
  int rows, cols;
    
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
      propertyField[2] = new_field(1, 30, 6, 22, 0, 0);        // address
      propertyField[3] = new_field(1, 30, 8, 22, 0, 0);        // city
      propertyField[4] = NULL;

      set_field_type(propertyField[0],TYPE_INTEGER,1,1,2);
      set_field_type(propertyField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(propertyField[3],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      propertyForm = new_form(propertyField);
      scale_form(propertyForm, &rows, &cols);

      /* Add window which will be associated to form */
      prtWin = newwin(rows+15, cols+20,1,1);  
      keypad(prtWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(propertyForm, prtWin);
      set_form_sub(propertyForm, derwin(prtWin,rows,cols,1,1));
      getmaxyx(prtWin,rows,cols);
      
      box(prtWin, 0,0);
          
      if (prtWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(prtWin,"Property Entry Form");

      post_form(propertyForm); 
      wrefresh(prtWin);

      //mvwprintw(prtWin,y+2,x+5,"Jade Finacial Tracker");
      //mvwprintw(prtWin,rows-4, cols-65, "rows %d cols %d", rows, cols);
      mvwprintw(prtWin,rows-21,cols-65, "Active Ind:");                           
      mvwprintw(prtWin,rows-19,cols-65, "Post Code:");
      mvwprintw(prtWin,rows-17,cols-65, "Address:");
      mvwprintw(prtWin,rows-15,cols-65, "City:");
            
      mvwprintw(prtWin,rows-2,cols-65,"Press F1 when form complete");
      wmove(prtWin, rows-21,cols-48);     /* move cursor */

      while((ch = wgetch(prtWin)) != KEY_F(1))
	 keyNavigate(ch, propertyForm);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(propertyField[0],0));
      strcpy(prtPostCode, field_buffer(propertyField[1],0));
      strcpy(prtAddress, trimWS(field_buffer(propertyField[2],0)));
      strcpy(prtCity, trimWS(field_buffer(propertyField[3],0)));

      if ((form_driver(propertyForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*prtPostCode)))
	{
	  strcpy(prtPostCode, trimWS(prtPostCode));  
	  echo();
	  mvwprintw(prtWin,rows-8,cols-65,"Save: y/n: ");     

	  while((cf = wgetch(prtWin)) != 'y')
	    {
	      wmove(prtWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(prtWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      //providerInsert(actInd,pname);   /* Save data to database */
	      mvwprintw(prtWin,rows-6,cols-65, "Data saved");
	    }
	}
      else
	{
	  mvwprintw(prtWin,rows-6,cols-65, "Data invalid");
	}
          noecho();

      unpost_form(propertyForm);
      free_form(propertyForm);
      free_field(propertyField[0]);
      free_field(propertyField[1]);
      free_field(propertyField[2]);

      mvwprintw(prtWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(prtWin)) != 'y')
	{
	  wmove(prtWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    } 
  endwin();
}
    
int suppAccountInsert()
{
  WINDOW * supAcctWin, * supWin, * supTypeWin, * prtWin, * payWin;
  FORM * supAcctForm;
  FIELD * supAcctField[13];
  int i = 0, j = 0;
  int range = 5;
  char p;
  int ch;
  int srow, scol, strow, stcol, sarow, sacol, prrow, prcol, parow, pacol;
  int list = 2;
  int supID, prtID, supTID, payID, proANo;
  char supIDstr[5], prtIDstr[5], supTIDstr[5], payIDstr[5], proANostr[30];
  int rows;
  int val, *params[1], length[1],  formats[1];
  int safActiveID, safSupID, safPrtID, safSupTypeID, safStartDt, safEndDt, safPayID;
  float safAmount;
  char safSupAcctRef[30], safComment[50], safAlias[10], safProAcctNo[30];
  int cf;
  int newRec = 'y';

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  
  while (newRec == 'y')
    {
      supAcctField[0] = new_field(1,1,1,35,0,0); /* active_ind */
      supAcctField[1] = new_field(1,30,3,35,0,0); /* supplier_acct_ref */
      supAcctField[2] = new_field(1,5,5,35,0,0); /* supplier_id */
      supAcctField[3] = new_field(1,5,7,35,0,0); /* property_id */
      supAcctField[4] = new_field(1,5,9,35,0,0); /* supplier_type_id */
      supAcctField[5] = new_field(1,8,11,35,0,0); /* start_date */
      supAcctField[6] = new_field(1,8,13,35,0,0); /* end_date */
      supAcctField[7] = new_field(1,5,15,35,0,0); /* payment_period_id */
      supAcctField[8] = new_field(1,10,17,35,0,0); /* amount */
      supAcctField[9] = new_field(1,50,19,35,0,0); /* comment */
      supAcctField[11] = new_field(1,30,21,35,0,0); /* provider_acct_no */
      supAcctField[10] = new_field(1,10,23,35,0,0); /* alias */
      supAcctField[12] = NULL;

      set_field_type(supAcctField[0],TYPE_INTEGER,1,1,2);
      set_field_type(supAcctField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[2],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[4],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[5],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[6],TYPE_INTEGER,0,1,99999999);
      set_field_type(supAcctField[7],TYPE_INTEGER,0,1,99999);
      set_field_type(supAcctField[8],TYPE_NUMERIC,0,-100000.00,1000000.00);
      set_field_type(supAcctField[9],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[10],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(supAcctField[11],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      supAcctForm = new_form(supAcctField);
      scale_form(supAcctForm, &sarow, &sacol);

      supAcctWin = newwin(sarow+20, sacol+10,1,1);
      supWin = newwin(20,50,1,120);
      prtWin = newwin(20,50,1,120);
      supTypeWin = newwin(20,50,1,120);
      payWin = newwin(20,50,1,120);
      
      keypad(supAcctWin, TRUE);
      keypad(supWin, TRUE);
      keypad(prtWin, TRUE);
      keypad(supTypeWin, TRUE);
      keypad(payWin, TRUE);

      set_form_win(supAcctForm,supAcctWin);
      set_form_sub(supAcctForm, derwin(supAcctWin,sarow,sacol,1,1));
      getmaxyx(supAcctWin,sarow,sacol);
      getmaxyx(supWin,srow,scol);
      getmaxyx(prtWin,prrow,prcol);
      getmaxyx(supTypeWin, strow, stcol);
      getmaxyx(payWin, parow, pacol);
      box(supAcctWin,0,0);
      box(supWin,0,0);
      box(prtWin,0,0);
      box(supTypeWin, 0,0);
      box(payWin, 0,0);
      waddstr(supAcctWin, "Supplier Account Form");
      waddstr(supWin, "Supplier");
      waddstr(prtWin, "Property");
      waddstr(supTypeWin, "Supplier Type");
      waddstr(payWin, "Payment Period");

      if(supAcctWin == NULL || supWin == NULL || prtWin == NULL ||supTypeWin == NULL || payWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  return(1);
	}

      post_form(supAcctForm);
      wrefresh(supAcctWin);

      mvwprintw(supAcctWin, sarow-30, sacol-64, "Active Ind:");
      mvwprintw(supAcctWin, sarow-28, sacol-64, "Supplier Account No:");
      mvwprintw(supAcctWin, sarow-26, sacol-64, "Supplier ID(F2):");
      mvwprintw(supAcctWin, sarow-24, sacol-64, "Property ID(F3):");
      mvwprintw(supAcctWin, sarow-22, sacol-64, "Supplier Type ID(F4):");
      mvwprintw(supAcctWin, sarow-20, sacol-64, "Start Date:");
      mvwprintw(supAcctWin, sarow-18, sacol-64, "End Date:");
      mvwprintw(supAcctWin, sarow-16, sacol-64, "Payment Period(F5):");
      mvwprintw(supAcctWin, sarow-14, sacol-64, "Amount:");
      mvwprintw(supAcctWin, sarow-12, sacol-64, "Comment:");
      mvwprintw(supAcctWin, sarow-10, sacol-64, "Provider Account No(F6):");
      mvwprintw(supAcctWin, sarow-8, sacol-64, "Alias:");
      wmove(supAcctWin, sarow-30, sacol-39);
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
	      res = PQexec(conn,"SELECT * FROM supplier WHERE active_ind = 1");	  
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
		    }
		  list = 2;      
		  wclrtoeol(supWin);  
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
	      res = PQexec(conn,"SELECT * FROM property WHERE active_ind = 1");	  
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
		    }
		  list = 2;      
		  wclrtoeol(prtWin);  
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
	      res = PQexec(conn,"SELECT * FROM supplier_type");	  
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
		    }
		  list = 2;      
		  wclrtoeol(supTypeWin);  
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
	      res = PQexec(conn,"SELECT * FROM payment_period");	  
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
		    }
		  list = 2;      
		  wclrtoeol(payWin);  
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
	



	
      } //while not F1

    /* code goes here for assign buffer value and validate prior to insert */



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

      mvwprintw(supAcctWin,sarow-6,sacol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(supAcctWin)) != 'y')
	{
	  wmove(supAcctWin,sarow-6,sacol-64);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      




      
		   
    } //while newRec = y

  PQfinish(conn);
  endwin();      
  
  return 0;
}
    
