#include <form.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libpq-fe.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"

/* API database connection method */
PGconn * fdbcon()
{  
  PGconn *connection = PQconnectdb("user=gianpietro dbname=jadedev");
  return connection;
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
  //int y = 2;         /* sub_form window row */
  //int x = 2;         /* fsub_orm window col */
    
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
      
      /* 
      int r, c;
      for (r=0; r < rows; r++)
	 mvwprintw(proWin,r, 1,"%d", r); // DEBUG CODE      
      for(c=0; c < cols; c++)
            mvwprintw(proWin,1, c,"%d", c); // DEBUG CODE
      */
      
      post_form(providerForm); 
      wrefresh(proWin);

      //mvwprintw(proWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(proWin,rows-16,cols-65, "Active Ind:");                           
      mvwprintw(proWin,rows-14,cols-65, "Provider Name:");
      mvwprintw(proWin,rows-2,cols-65,"Press F1 when form complete");
      wmove(proWin, rows-16,cols-48);     /* move cursor */

      //keyNavigation(proWin, providerForm, ch);
      
      while((ch = wgetch(proWin)) != KEY_F(1))
	{
	  switch(ch)
	    {
	    case KEY_DOWN:
	      form_driver(providerForm, REQ_NEXT_FIELD);
	      form_driver(providerForm, REQ_END_LINE);
	      break;
	    case KEY_UP:
	      form_driver(providerForm, REQ_PREV_FIELD);
	      form_driver(providerForm, REQ_END_LINE);
	      break;
	    case KEY_BACKSPACE:
	      form_driver(providerForm, REQ_CLR_FIELD);
	    case 10:                                           // ASCII value for new line feed(Enter Key) 
	      form_driver(providerForm, REQ_VALIDATION);
	      form_driver(providerForm, REQ_NEXT_FIELD);
	      break;
	    default:
	      form_driver(providerForm, ch);
	      break;
	    }
	}
      

      /* Assign data entered in field */
      actInd = atoi(field_buffer(providerField[0],0));
      strcpy(pname, field_buffer(providerField[1],0));

      if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
	{
	  strcpy(pname, trimWS(pname));  
	  echo();
	  mvwprintw(proWin,rows-8,cols-65,"Save: y/n: ");     
	  //wmove(proWin,rows-8,cols-54);
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

      //mvwprintw(proWin,rows-4,cols-65,"Provider Entry Form");
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
  //int y = 2;   /* UL corner form subwindow */
  //int x = 2;   /* UL corner form subwindow */
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
      //mvwprintw(proTypeWin,rows-12, cols-50,"row %d col %d", rows, cols);
      post_form(proTypeForm); 
      wrefresh(proTypeWin);

      mvwprintw(proTypeWin,rows-14,cols-46, "Description:");
      //mvwprintw(proTypeWin,y+2,x+5, "Description:");
      mvwprintw(proTypeWin,rows-2,cols-46,"Press F1 when form complete");
      wmove(proTypeWin,rows-14,cols-33);     /* move cursor */
      
      while((ch = wgetch(proTypeWin)) != KEY_F(1))
	{
	  switch(ch)
	    {
	    case KEY_DOWN:
	      form_driver(proTypeForm, REQ_NEXT_FIELD);
	      form_driver(proTypeForm, REQ_END_LINE);
	      break;
	    case KEY_UP:
	      form_driver(proTypeForm, REQ_PREV_FIELD);
	      form_driver(proTypeForm, REQ_END_LINE);
	      break;
	    case KEY_BACKSPACE:
	      form_driver(proTypeForm, REQ_CLR_FIELD);
	    case 10:                                           /* ASCII value for new line feed(Enter Key) */
	      form_driver(proTypeForm, REQ_VALIDATION);
	      form_driver(proTypeForm, REQ_NEXT_FIELD);
	      break;
	    default:
	      form_driver(proTypeForm, ch);
	      break;
	    }
	}      

      strcpy(pdesc,field_buffer(proTypeField[0],0));
     
      if ((form_driver(proTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*pdesc)))
	{
	  strcpy(pdesc, trimWS(pdesc));
	  echo();
	  //mvwprintw(proTypeWin,rows-4,cols-46,"input val:%s",pdesc);	    
	  mvwprintw(proTypeWin,rows-8,cols-46, "Save y/n: ");
	  //wmove(proTypeWin,rows-8,cols-34);
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

      //mvwprintw(proTypeWin,rows-6,cols-46,"Provide Type Entry Form");
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


/* Function to display Provider data from database */
int proSelect()
{
  int i = 0, j = 0;
  int range = 5;
  char p;
  WINDOW * proListWin;
  int nrow, ncol;
  int list = 2;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  proListWin = newwin(25,50,1,120);  
  if(proListWin == NULL)
    {
        endwin();
        puts("Unable to create window");
        return(1);
    }
  getmaxyx(proListWin, nrow, ncol);
  box(proListWin,0,0);
  waddstr(proListWin, "Provider data");
  wrefresh(proListWin);
  
  PGconn *conn =  fdbcon();
  
  if(PQstatus(conn) == CONNECTION_BAD)
    {
      PQfinish(conn);
      exit(1);
    }

  PGresult *res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1");

  int rows = PQntuples(res);

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
	  mvwprintw(proListWin,0,0, "Provider data");
	  wmove(proListWin,10,1);
  	      mvwprintw(proListWin,11,1,"Select Provider: ");
	  wrefresh(proListWin);
	  //break;
	}
    }  
  //  mvwprintw(proListWin,11,1,"Select Provider: ");
  //wrefresh(proListWin);
  getch();
  
  PQclear(res);
  PQfinish(conn);

  endwin();

  return 0;
}

/*
int proListWindow()
{
  WINDOW * proListWin;
  int rows, cols;
  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  proListWin = newwin(25,50,1,120);  
  if(proListWin == NULL)
    {
        endwin();
        puts("Unable to create window");
        return(1);
    }
  getmaxyx(proListWin, rows, cols);
  box(proListWin,0,0);
  waddstr(proListWin, "Provider data");
  wrefresh(proListWin);
  
  proSelect(proListWin, 1,1);
  

  endwin();

  return 0;

  
}
*/

  
