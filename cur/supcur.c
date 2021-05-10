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
    
