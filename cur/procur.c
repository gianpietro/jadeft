#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <libpq-fe.h>
//#include <libpq/libpq-fs.h>   
#include <arpa/inet.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"
#include "../inc/jadlib.h"

/* API database connection method */
/*
static PGconn * fdbcon()
{  
  PGconn *connection = PQconnectdb("user=gianpietro dbname=jadedev");
  if(PQstatus(connection) == CONNECTION_BAD)
    {
      PQfinish(connection);
      exit(1);
    }  
  return connection;
}
*/
/* Form navigation keys */
/*
static void keyNavigate(int ch, FORM * f)
{ 
  switch(ch)
    {
    case KEY_DOWN:
      form_driver(f, REQ_NEXT_FIELD);
      form_driver(f, REQ_END_LINE);
      break;
    case KEY_UP:
      form_driver(f, REQ_PREV_FIELD);
      form_driver(f, REQ_END_LINE);
      break;
    case KEY_BACKSPACE:
      form_driver(f, REQ_CLR_FIELD);	  
      break;
    case 10:
      form_driver(f, REQ_VALIDATION);
      form_driver(f, REQ_NEXT_FIELD);
      break;
    default:
      form_driver(f, ch);
      break;
    }
}
*/

/* Trim trailing whitespace from the string entered in form field */
/*
static char * trimWS(char *s)
{
  int i = 0;
  int index = -1;

  while (s[i] != '\0')
    {
      if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
	{
	  index = i;
	}
      i++;
    }
  s[index + 1] = '\0';

  return s;
} 
*/      

/* Function to display Provider form for data entry */
void provInsert()
{
  FIELD *providerField[3];
  FORM *providerForm;
  WINDOW *proWin;
  int ch;
  int cf;            /* confirm save data */
  int actInd;        /* active_Ind */
  char pname[30];    /* provider_name */ 
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
      providerField[0] = new_field(1, 1, 2, 22, 0, 0);      
      providerField[1] = new_field(1, 30, 4, 22, 0, 0);      
      providerField[2] = NULL;

      /* Field 1 digit allowed in range from 1 to 2 */
      set_field_type(providerField[0],TYPE_INTEGER,1,1,2);
      /* Field allowed values of A-Z a-z and hyphen */
      set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      providerForm = new_form(providerField);
      scale_form(providerForm, &rows, &cols);

      /* Add window which will be associated to form */
      proWin = newwin(rows+15, cols+20,1,1);  
      keypad(proWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(providerForm, proWin);
      set_form_sub(providerForm, derwin(proWin,rows,cols,2,2));
      getmaxyx(proWin,rows,cols);
      
      box(proWin, 0,0);
          
      if (proWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(proWin,"Provider Entry Form");

      post_form(providerForm); 
      wrefresh(proWin);

      //mvwprintw(proWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(proWin,rows-16,cols-65, "Active Ind:");                           
      mvwprintw(proWin,rows-14,cols-65, "Provider Name:");
      mvwprintw(proWin,rows-2,cols-65,"Press F1 when form complete");
      wmove(proWin, rows-16,cols-48);     /* move cursor */

      while((ch = wgetch(proWin)) != KEY_F(1))
	 keyNavigate(ch, providerForm);
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(providerField[0],0));
      strcpy(pname, field_buffer(providerField[1],0));

      if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
	{
	  strcpy(pname, trimWS(pname));  
	  echo();
	  mvwprintw(proWin,rows-8,cols-65,"Save: y/n: ");     

	  while((cf = wgetch(proWin)) != 'y')
	    {
	      wmove(proWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(proWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      providerInsert(actInd,pname);   /* Save data to database */
	      mvwprintw(proWin,rows-6,cols-65, "Data saved");
	    }
	}
      else
	{
	  mvwprintw(proWin,rows-6,cols-65, "Data invalid");
	}
          noecho();

      unpost_form(providerForm);
      free_form(providerForm);
      free_field(providerField[0]);
      free_field(providerField[1]);
      free_field(providerField[2]);

      mvwprintw(proWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proWin)) != 'y')
	{
	  wmove(proWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    } 
  endwin();
}

/* Function to display Provider Type form for data entry */
void provTypeInsert()
{
  FIELD *proTypeField[2];
  FORM *proTypeForm;
  WINDOW *proTypeWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char pdesc[30];
  int cf;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      proTypeField[0] = new_field(1,30,2,18,0,0);
      proTypeField[1] = NULL;
      set_field_type(proTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      proTypeForm = new_form(proTypeField);
      scale_form(proTypeForm, &rows, &cols);
      proTypeWin = newwin(rows+15,cols+5,1,120);  
      keypad(proTypeWin, TRUE);

      set_form_win(proTypeForm, proTypeWin);	
      set_form_sub(proTypeForm, derwin(proTypeWin,rows,cols,2,2));
      getmaxyx(proTypeWin,rows,cols);
      
      box(proTypeWin, 0,0);
          
      if (proTypeWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(proTypeWin,"Provider Type Form");
      post_form(proTypeForm); 
      wrefresh(proTypeWin);

      mvwprintw(proTypeWin,rows-14,cols-46, "Description:");
      mvwprintw(proTypeWin,rows-2,cols-46,"Press F1 when form complete");
      wmove(proTypeWin,rows-14,cols-33);     /* move cursor */

      while((ch = wgetch(proTypeWin)) != KEY_F(1))
	keyNavigate(ch, proTypeForm);

      strcpy(pdesc,field_buffer(proTypeField[0],0));
     
      if ((form_driver(proTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*pdesc)))
	{
	  strcpy(pdesc, trimWS(pdesc));
	  echo();
	  mvwprintw(proTypeWin,rows-8,cols-46, "Save y/n: ");
	  while((cf = wgetch(proTypeWin)) != 'y')
	    {
	      wmove(proTypeWin,rows-8,cols-36);
	      if (cf == 'n')
		{
		  mvwprintw(proTypeWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      proTypeInsert(pdesc);
	      mvwprintw(proTypeWin,rows-6,cols-46, "Data saved");
	    }
	}
	  else
	    {
	      mvwprintw(proTypeWin,rows-6,cols-46, "Data invalid");
	    }
	  noecho();

      unpost_form(proTypeForm);
      free_form(proTypeForm);
      free_field(proTypeField[0]);
      free_field(proTypeField[1]);

      mvwprintw(proTypeWin,rows-4,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proTypeWin)) != 'y')
	{
	  wmove(proTypeWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      endwin();
    }
}


/* Function to display Provider Account Form and retrieve DB records
   for provider and provider type to select for provoder account */
int provAccountInsert()
{
  WINDOW * proListWin, * proAcctWin, *proTypeWin;
  FORM * proAcctForm;
  FIELD * proAcctField[7];
  int i = 0, j = 0;
  int range = 5;
  char p;
  int ch, nrow, ncol, parow, pacol, ptrow, ptcol;
  int list = 2;
  int proID, proTypeID;
  char proIDstr[5];  // store provide_id as str
  char proTypestr[5];
  int rows; // number of tuples returned from sql query
  int val, *params[1], length[1], formats[1];   //PQexecParams
  int pafActiveID, pafID, pafSortCode, pafTypeID;  // field values
  char pafAccountNo[30], pafRef[30];
  int cf; // confirm save to DB
  int newRec = 'y';

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  
  while (newRec == 'y')
    {
      proAcctField[0] = new_field(1,1,1,35,0,0);    /* active_ind */
      proAcctField[1] = new_field(1,5,3,35,0,0);    /* provider_id (fk) */
      proAcctField[2] = new_field(1,30,5,35,0,0);   /* provider_account_no */
      proAcctField[3] = new_field(1,9,7,35,0,0);    /* sort_code */
      proAcctField[4] = new_field(1,30,9,35,0,0);   /* reference */
      proAcctField[5] = new_field(1,5,11,35,0,0);   /* provider_type_id */
      proAcctField[6] = NULL;

      set_field_type(proAcctField[0],TYPE_INTEGER,1,1,2);
      set_field_type(proAcctField[1],TYPE_INTEGER,0,1,99999);
      set_field_type(proAcctField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(proAcctField[3],TYPE_INTEGER,0,1,999999999);
      set_field_type(proAcctField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(proAcctField[5],TYPE_INTEGER,0,1,99999);

      proAcctForm = new_form(proAcctField);
      scale_form(proAcctForm, &parow, &pacol);   

      proAcctWin = newwin(parow+20,pacol+10,1,1);
      proListWin = newwin(20,50,1,120);
      proTypeWin = newwin(20,50,23,120);  
      keypad(proAcctWin, TRUE);
      keypad(proListWin, TRUE);
      keypad(proTypeWin, TRUE);

      set_form_win(proAcctForm,proAcctWin);
      set_form_sub(proAcctForm, derwin(proAcctWin,parow,pacol,1,1));
      getmaxyx(proAcctWin, parow, pacol);
      getmaxyx(proListWin, nrow, ncol);
      getmaxyx(proTypeWin, ptrow, ptcol);
      box(proAcctWin,0,0);
      box(proListWin,0,0);
      box(proTypeWin,0,0);
      waddstr(proAcctWin, "Provider Account Form");
      waddstr(proListWin, "Provider List");
      waddstr(proTypeWin, "Provider Type");

      if(proListWin == NULL || proAcctWin == NULL || proTypeWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  return(1);
	}
  
      post_form(proAcctForm);  
      wrefresh(proAcctWin);

      // mvwprintw(proAcctWin, parow-3,pacol-3, "rows %d cols %d",parow,pacol);
      mvwprintw(proAcctWin, parow-30,pacol-64, "Active Ind:");
      mvwprintw(proAcctWin, parow-28,pacol-64, "Provider ID:");
      mvwprintw(proAcctWin, parow-26,pacol-64, "Account Number:");
      mvwprintw(proAcctWin, parow-24,pacol-64, "Sort Code:");
      mvwprintw(proAcctWin, parow-22,pacol-64, "Reference:");
      mvwprintw(proAcctWin, parow-20,pacol-64, "Provider Type ID:");
      wmove(proAcctWin,parow-30,pacol-39);
      wrefresh(proAcctWin);
  
      while((ch = wgetch(proAcctWin)) != KEY_F(1))
	{
	  keyNavigate(ch, proAcctForm);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(proListWin);
	      box(proListWin,0,0);
	      waddstr(proListWin, "Provider List");
	      wmove(proListWin,1,1);
	      wrefresh(proListWin);
	  
	      res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1");	  
	      rows = PQntuples(res);

	      wrefresh(proListWin);
	  
	      while((p = wgetch(proListWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proListWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;	 
		    }
		  list = 2;      
		  wclrtoeol(proListWin);  //clear current line to right of cursor
		  if  (i == rows)
		    {
		      wclrtobot(proListWin);  // clear current line right of cursor and all lines below
		      mvwprintw(proListWin,10,1,"End of list");
		      box(proListWin,0,0);
		      mvwprintw(proListWin,0,0, "Provider List");
		      wmove(proListWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(proListWin,11,1,"Select Provider: ");
	      mvwscanw(proListWin,11,25, "%5s", &proIDstr);
	      set_field_buffer(proAcctField[1],0, proIDstr);

	      proID = atoi(field_buffer(proAcctField[1],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)proID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      res = PQexecParams(conn, "SELECT * FROM provider WHERE provider_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(proListWin,13,1, "no or rows %d ",rows);
		  mvwprintw(proListWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(proListWin);
		}
	      else
		{
		  mvwprintw(proListWin,12,1,"Number invalied");
		  wrefresh(proListWin);		
		  wrefresh(proAcctWin);
		}
	      noecho();
	      PQclear(res);
	    }
	  if(ch == KEY_F(3))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(proTypeWin);
	      box(proTypeWin,0,0);
	      waddstr(proTypeWin, "Provider Type");
	      wmove(proTypeWin,1,1);
	      wrefresh(proTypeWin);

	      res = PQexec(conn,"SELECT * FROM provider_type");
	      rows = PQntuples(res);

	      wrefresh(proTypeWin);

	      while((p = wgetch(proTypeWin)) == '\n')
		{
		  if ( j + range < rows)
		    j = j + range;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proTypeWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;	 
		    }
		  list = 2;      
		  wclrtoeol(proTypeWin);  //clear current line to right of cursor
		  if  (i == rows)
		    {
		      wclrtobot(proTypeWin);  // clear current line right of cursor and all lines below
		      mvwprintw(proTypeWin,10,1,"End of list");
		      box(proTypeWin,0,0);
		      mvwprintw(proTypeWin,0,0, "Provider Type");
		      wmove(proTypeWin,10,1);
		      break;
		    }
		}
	      echo();  
	      mvwprintw(proTypeWin,11,1,"Select Provider: ");
	      mvwscanw(proTypeWin,11,25, "%5s", &proTypestr);
	      set_field_buffer(proAcctField[5],0, proTypestr);

	      proTypeID = atoi(field_buffer(proAcctField[5],0));
	      PQclear(res);

	      val = htonl((uint32_t)proTypeID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      res = PQexecParams(conn, "SELECT * FROM provider_type WHERE provider_type_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(proTypeWin,13,1, "no or rows %d ",rows);
		  mvwprintw(proTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(proTypeWin);
		}
	      else
		{
		  mvwprintw(proTypeWin,12,1,"Number invalied");
		  wrefresh(proTypeWin);		
		  wrefresh(proAcctWin);
		}
	      noecho();
	      PQclear(res);
	    }  // if (ch == f3)
	}

      /* code goes here for assign buffer value and validate prior to insert */

      pafActiveID = atoi(field_buffer(proAcctField[0],0));
      pafID = atoi(field_buffer(proAcctField[1],0));
      strcpy(pafAccountNo,trimWS(field_buffer(proAcctField[2],0)));
      pafSortCode = atoi(field_buffer(proAcctField[3],0));
      strcpy(pafRef, trimWS(field_buffer(proAcctField[4],0)));
      pafTypeID = atoi(field_buffer(proAcctField[5],0));

      if ((form_driver(proAcctForm,REQ_VALIDATION) == E_OK) && (pafActiveID >= 1))
	{
	  echo();
	  mvwprintw(proAcctWin,parow-10,pacol-64,"Save: y/n: ");     

	  while((cf = wgetch(proAcctWin)) != 'y')
	    {
	      wmove(proAcctWin,parow-10,pacol-53);
	      if (cf == 'n')
		{
		  mvwprintw(proAcctWin,parow-8,pacol-64, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      proAccountInsert(pafActiveID, pafID, pafAccountNo, pafSortCode, pafRef, pafTypeID);
	      mvwprintw(proAcctWin,parow-8, pacol-64, "Data saved");
	    }
	}  
      else
	{
	  mvwprintw(proAcctWin,parow-8,pacol-64, "Data invalid");	
	}
      noecho();
  
      unpost_form(proAcctForm);
      free_form(proAcctForm);
      free_field(proAcctField[0]);
      free_field(proAcctField[1]);
      free_field(proAcctField[2]);
      free_field(proAcctField[3]);
      free_field(proAcctField[4]);
      free_field(proAcctField[5]);
      free_field(proAcctField[6]);

      mvwprintw(proAcctWin,parow-6,pacol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proAcctWin)) != 'y')
	{
	  wmove(proAcctWin,parow-6,pacol-27);  //CHECK COL POSITION OK
	  if(newRec == 'n')
	    break;
	}
      noecho();
    }

  PQfinish(conn); 
  endwin();

  return 0;
  
}



  
