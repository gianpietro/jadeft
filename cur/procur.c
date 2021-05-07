#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <libpq-fe.h>
//#include <libpq/libpq-fs.h>   
#include <arpa/inet.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"

/* API database connection method */
PGconn * fdbcon()
{  
  PGconn *connection = PQconnectdb("user=gianpietro dbname=jadedev");
  if(PQstatus(connection) == CONNECTION_BAD)
    {
      PQfinish(connection);
      exit(1);
    }  
  return connection;
}

/* Form navigation keys */
void keyNavigate(int ch, FORM * f)
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

/* Trim trailing whitespace from the string entered in form field */
char * trimWS(char *s)
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

/* Function to display Provider form for data entry */
void providerWindow()
{
  FIELD *providerField[3];
  FORM *providerForm;
  WINDOW *proWin;
  int ch;
  int cf;            /* confirm save data */
  int actInd;        /* active_Ind */
  char pname[30];    /* provider_name */ 
  int index , i;
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
      providerField[1] = new_field(1, 30, 4, 22, 0, 0);   //DEBUG CODE needs to be 30
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
void providerTypeWindow()
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
int proSelect()
{
  WINDOW * proListWin, * proAcctWin;
  FORM * proAcctForm;
  FIELD * proAcctField[3];
  int i = 0, j = 0;
  int range = 5;
  char p;
  int ch, nrow, ncol, parow, pacol;
  int list = 2;
  int proID;
  char proIDstr[5];  // store provide_id as str
  PGconn *conn =  fdbcon();
  PGresult *res;
  int rows; // number of tuples returned from sql query
  int val, *params[1], length[1], formats[1];   //PQexecParams

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  proAcctField[0] = new_field(1,1,2,25,0,0);  // active_ind.
  proAcctField[1] = new_field(1,5,4,25,0,0);  // provider_id fk
  proAcctField[2] = NULL;

  set_field_type(proAcctField[0],TYPE_INTEGER,1,1,2);

  proAcctForm = new_form(proAcctField);
  scale_form(proAcctForm, &parow, &pacol);   

  proAcctWin = newwin(parow+30,pacol+40,1,1);
  proListWin = newwin(25,50,1,120);
  keypad(proAcctWin, TRUE);
  keypad(proListWin, TRUE);

  set_form_win(proAcctForm,proAcctWin);
  set_form_sub(proAcctForm, derwin(proAcctWin,parow,pacol,1,1));
  getmaxyx(proAcctWin, parow, pacol);
  getmaxyx(proListWin, nrow, ncol);
  box(proAcctWin,0,0);
  box(proListWin,0,0);
  waddstr(proAcctWin, "Provider Account Form");
  waddstr(proListWin, "Provider List");

  if(proListWin == NULL || proAcctWin == NULL)
    {
        endwin();
        puts("Unable to create window");
        return(1);
    }
  
  post_form(proAcctForm);  
  wrefresh(proAcctWin);

  mvwprintw(proAcctWin, parow-32,pacol-68, "Active Ind:");
  mvwprintw(proAcctWin, parow-30,pacol-68, "Provider ID:");
  wmove(proAcctWin,parow-32,pacol-45);
  
  while((ch = wgetch(proAcctWin)) != KEY_F(1))
    {
      keyNavigate(ch, proAcctForm);
      if(ch == 's')
	{
	  i = j = rows = 0;
	  list = 2;
	  //wmove(proListWin,1,1);
	  // wclrtobot(proListWin);
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
	  //wrefresh(proListWin);	      
	  
          noecho();
	  PQclear(res);
	}
      // if(ch == 'z')
      /* {
          The  code for the provider type box
          will go here. This should follow the same 
          as the above -  if (ch == 's') - for a proTypeWin
          which will list the records in the provider_type
          table so the description of the provider can be added
          to the provider account form  
	  } */
    } 

  unpost_form(proAcctForm);
  free_form(proAcctForm);
  free_field(proAcctField[0]);
  free_field(proAcctField[1]);
  free_field(proAcctField[2]);

  PQfinish(conn); 
  endwin();

  return 0;
}



  
