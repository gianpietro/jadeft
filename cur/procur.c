#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"
#include "../inc/jadlib.h"


/* Function to display Provider form for data entry */
void provInsert()
{
  FIELD *providerField[3];
  FORM *providerForm;
  WINDOW *proWin, *proUpdateWin;
  PANEL *proPanel, *mainPanel;
  int ch;
  int cf;            /* confirm save data */
  int actInd;        /* active_Ind */
  char pname[30];    /* provider_name */ 
  int newRec= 'y';   /* Add another new record */
  int rows, cols;
  int cfUpdate = 0;
  int list = 3, i = 0, j = 0; 
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1], formats[1];
  char *formTitle = "Provider Entry Form";
  int titleLen = strlen(formTitle);
  char *provTitle = "Provider";
  int provLen = strlen(provTitle);
    

  PGconn *conn =  fdbcon();
  PGresult *res;
     
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  /* found issue when color pair number is the same as used in cdk menu widget
     jadeft.c file. It makes menu lose its color i.e. 5 and 7 */ 
  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_YELLOW);
  init_pair(3,COLOR_YELLOW,COLOR_MAGENTA);
  init_pair(4,COLOR_WHITE,COLOR_CYAN);  
  init_pair(6,COLOR_BLACK,COLOR_YELLOW);
  init_pair(8,COLOR_BLACK,COLOR_WHITE); 
  init_pair(9,COLOR_WHITE,COLOR_BLACK);

    while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      providerField[0] = new_field(1, 1, 4, 22, 0, 0);      
      providerField[1] = new_field(1, 30, 6, 22, 0, 0);      
      providerField[2] = NULL;

      set_field_fore(providerField[0], COLOR_PAIR(9));
      set_field_back(providerField[0], COLOR_PAIR(9));
      set_field_fore(providerField[1], COLOR_PAIR(9));
      set_field_back(providerField[1], COLOR_PAIR(9));

      /* Field 1 digit allowed in RANGE from 1 to 2 */
      set_field_type(providerField[0],TYPE_INTEGER,1,1,2);
      /* Field allowed values of A-Z a-z and hyphen */
      set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      providerForm = new_form(providerField);
      scale_form(providerForm, &rows, &cols);

      /* Add window which will be associated to form */
      proWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-2),COLS/2);             // (rows+15, cols+20,1,1);
      proUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+3, COLS/2);       // (20,50,1,120);

      proPanel = new_panel(proUpdateWin);
      mainPanel = new_panel(proWin);
      wbkgd(proWin, COLOR_PAIR(1));
     
      update_panels();
      doupdate();
      
      keypad(proWin, TRUE);
      keypad(proUpdateWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(providerForm, proWin);
      set_form_sub(providerForm, derwin(proWin,rows,cols,2,2));
      getmaxyx(proWin,rows,cols);
      getmaxyx(proUpdateWin, urows, ucols);
      
      box(proWin, 0,0);
      box(proUpdateWin,0,0);

      if (proWin == NULL || proUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}
      
      wattron(proWin,A_BOLD | COLOR_PAIR(1));
      //waddstr(proWin,"Provider Entry Form");
      mvwprintw(proWin,1,(cols-titleLen)/2, formTitle);     
      //wprintw(proWin,"row %d cols %d\n", rows, cols);   //DEBUG
      wattroff(proWin,A_BOLD | COLOR_PAIR(1));

      post_form(providerForm); 
      wrefresh(proWin);

      //mvwprintw(proWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(proWin,rows-(rows-6),cols-(cols-5), "Active Ind:");                           
      mvwprintw(proWin,rows-(rows-8),cols-(cols-5), "Provider Name:");
      mvwprintw(proWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(proWin, rows-(rows-6),cols-(cols-24));     /* move cursor */
      //mvwprintw(proWin,15,4,"rows %d, cols %d", rows, cols );

      while((ch = wgetch(proWin)) != KEY_F(1))
	{
	  hide_panel(proPanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, providerForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 6;    //+3  
	      wclear(proUpdateWin);
	      box(proUpdateWin,0,0);
	      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(proUpdateWin,1,(cols-provLen)/2, provTitle);     
	      // waddstr(proUpdateWin, "Provider");
	      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      wmove(proUpdateWin,1,1);
	      show_panel(proPanel);
	      wbkgd(proUpdateWin, COLOR_PAIR(1));
	      update_panels();
	      doupdate();
	      //wrefresh(proUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1 ORDER BY provider_id");	  
	      trows = PQntuples(res);

	      wrefresh(proUpdateWin);
	      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(proUpdateWin, 4, 1, "ID    Provider");  //+3
	      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      while((p = wgetch(proUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(proUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;		     
		      wclrtoeol(proUpdateWin);
		      box(proUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(proUpdateWin);  
		      mvwprintw(proUpdateWin,13,1,"End of list");   //+3
		      box(proUpdateWin,0,0);
		      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		      mvwprintw(proUpdateWin,1,(cols-provLen)/2, provTitle);     
		      //mvwprintw(proUpdateWin,0,0, "Provider");
		      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		      wmove(proUpdateWin,13,1);  //+3
		      break;
		    }
		}	  
	      echo();
	      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(proUpdateWin,14,1,"Select Option: ");  //+3	      
	      mvwscanw(proUpdateWin,14,25, "%d", &upID);  //+3
	      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider WHERE provider_id = $1 ORDER BY provider_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  //mvwprintw(proUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(proUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(proUpdateWin);
		  set_field_buffer(providerField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(providerField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		  mvwprintw(proUpdateWin,15,1,"Number invalied"); //+3
		  wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		  wrefresh(proUpdateWin);		
		  //wrefresh(proWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
	} //while F1
      hide_panel(proPanel);
      update_panels();
      doupdate();
      
      form_driver(providerForm,REQ_VALIDATION);	 /* Validates data in form and buffers and assigned values in fields */	  
    
      /* Assign data entered in field */
      actInd = atoi(field_buffer(providerField[0],0));
      strcpy(pname, field_buffer(providerField[1],0));

      if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
	{
	  strcpy(pname, trimWS(pname));  
	  echo();
	  wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(proWin,rows-8,cols-65,"Save: y/n: ");
	  wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(proWin,rows-7,cols-65,"(d = delete record)");
	  wmove(proWin,rows-8,cols-54);

	  while((cf = wgetch(proWin)) != 'y')
	    {
	      wmove(proWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(proWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  providerDelete(upID);
		  wattron(proWin, A_BOLD | A_BLINK);
		  mvwprintw(proWin,rows-6,cols-65, "Record deleted");
		  wattroff(proWin, A_BOLD | A_BLINK);
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  providerUpdate(upID,actInd,pname); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(proWin, A_BOLD | A_BLINK);
		  mvwprintw(proWin,rows-6, cols-65, "Data updated");
		  wattroff(proWin, A_BOLD | A_BLINK);
		  //mvwprintw(proWin,20,5, "cfUpdate %d,upID %d actInd %d pname %s", cfUpdate,upID,actInd, pname);  //DEBUG
		}
	      else
		{
		  providerInsert(actInd,pname);   /* Save data to database */
		  wattron(proWin, A_BOLD | A_BLINK);
		  mvwprintw(proWin,rows-6,cols-65, "Data saved");
		  wattroff(proWin, A_BOLD | A_BLINK);
		}
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

      cfUpdate = 0;

      mvwprintw(proWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proWin)) != 'y')
	{
	  wmove(proWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(proWin);
    }
    PQfinish(conn);    
  endwin();
}

/* Function to display Provider Type form for data entry */
void provTypeInsert()
{
  FIELD *proTypeField[2];
  FORM *proTypeForm;
  WINDOW *proTypeWin, *proTypeUpdateWin;
  PANEL *proTypePanel, *mainPanel;
  int ch;
  char newRec = 'y';
  int rows, cols;
  char pdesc[30];
  int cf;
  int cfUpdate = 0;
  int list = 2, i = 0, j = 0;
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
      proTypeField[0] = new_field(1,30,2,18,0,0);
      proTypeField[1] = NULL;
      set_field_type(proTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      proTypeForm = new_form(proTypeField);
      scale_form(proTypeForm, &rows, &cols);
      proTypeWin = newwin(rows+15,cols+5,1,120);
      proTypeUpdateWin = newwin(20,50,30,120);

      proTypePanel = new_panel(proTypeUpdateWin);
      mainPanel = new_panel(proTypeWin);
      update_panels();
      doupdate();
      
      keypad(proTypeWin, TRUE);
      keypad(proTypeUpdateWin, TRUE);

      set_form_win(proTypeForm, proTypeWin);	
      set_form_sub(proTypeForm, derwin(proTypeWin,rows,cols,2,2));
      getmaxyx(proTypeWin,rows,cols);
      getmaxyx(proTypeUpdateWin, urows, ucols);
      
      box(proTypeWin, 0,0);
      box(proTypeUpdateWin,0,0);
          
      if (proTypeWin == NULL || proTypeUpdateWin == NULL)
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
	{
	  hide_panel(proTypePanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();	  
	  keyNavigate(ch, proTypeForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = trows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(proTypeUpdateWin);
	      box(proTypeUpdateWin,0,0);
	      waddstr(proTypeUpdateWin, "Provider Type");
	      wmove(proTypeUpdateWin,1,1);
	      show_panel(proTypePanel);
	      update_panels();
	      doupdate();	  
	      wrefresh(proTypeUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM provider_type ORDER BY provider_type_id");	  
	      trows = PQntuples(res);

	      wrefresh(proTypeUpdateWin);
	  
	      while((p = wgetch(proTypeUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(proTypeUpdateWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(proTypeUpdateWin);
		    }
		  list = 2;      
		  if  (i == trows)
		    {
		      wclrtobot(proTypeUpdateWin);  
		      mvwprintw(proTypeUpdateWin,10,1,"End of list");
		      box(proTypeUpdateWin,0,0);
		      mvwprintw(proTypeUpdateWin,0,0, "Provider Type");
		      wmove(proTypeUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(proTypeUpdateWin,11,1,"Select Option: ");
	      mvwscanw(proTypeUpdateWin,11,25, "%d", &upID);

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_type WHERE provider_type_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      trows = PQntuples(res);
	      if (trows == 1)
		{
		  mvwprintw(proTypeUpdateWin,13,1, "no or rows %d ",trows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(proTypeUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(proTypeUpdateWin);
		  set_field_buffer(proTypeField[0],0,PQgetvalue(res,0,1));
		  //set_field_buffer(proTypeField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(proTypeUpdateWin,12,1,"Number invalied");
		  wrefresh(proTypeUpdateWin);		
		  //wrefresh(proTypeWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9
	} //while F1
      hide_panel(proTypePanel);
      update_panels();
      doupdate();	  
      
      form_driver(proTypeForm,REQ_VALIDATION);

      strcpy(pdesc,field_buffer(proTypeField[0],0));
     
      if ((form_driver(proTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*pdesc)))
	{
	  strcpy(pdesc, trimWS(pdesc));
	  echo();
	  mvwprintw(proTypeWin,rows-8,cols-46, "Save y/n: ");
	  mvwprintw(proTypeWin,rows-7,cols-65,"(d = delete record)");
	  wmove(proTypeWin,rows-8,cols-55);
	  while((cf = wgetch(proTypeWin)) != 'y')
	    {
	      wmove(proTypeWin,rows-8,cols-36);
	      if (cf == 'n')
		{
		  mvwprintw(proTypeWin,rows-6,cols-46, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  proTypeDelete(upID);
		  mvwprintw(proTypeWin,rows-6,cols-46, "Record deleted");                
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  proTypeUpdate(upID,pdesc); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(proTypeWin,19,5, "Data updated");
		  mvwprintw(proTypeWin,20,5, "cfUpdate %d,upID %d pdesc %s", cfUpdate,upID,pdesc);  //DEBUG
		}
	      else
		{
		  proTypeInsert(pdesc);
		  mvwprintw(proTypeWin,rows-6,cols-46, "Data saved");
		}
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

      cfUpdate = 0;

      mvwprintw(proTypeWin,rows-4,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proTypeWin)) != 'y')
	{
	  wmove(proTypeWin,rows-4,cols-9);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(proTypeWin);
      //endwin();
    }
  PQfinish(conn);
  endwin();
}


/* Function to display Provider Account Form and retrieve DB records
   for provider and provider type to select for provoder account */
int provAccountInsert()
{
  WINDOW *proListWin, *proAcctWin, *proTypeWin, *proAccountUpdateWin;
  FORM *proAcctForm;
  FIELD *proAcctField[7];
  PANEL *proPanel, *proTypePanel, *proAccountUpdatePanel, *mainPanel;
  int list = 2, i = 0, j = 0;
  //int RANGE = 5;
  char p;
  int ch, nrow, ncol, parow, pacol, ptrow, ptcol, urows, ucols;
  int proID, proTypeID;
  char proIDstr[5];  // store provide_id as str
  char proTypestr[5];
  int rows; // number of tuples returned from sql query
  int upID;
  int val, *params[1], length[1], formats[1];   //PQexecParams
  int pafActiveID, pafID, pafSortCode, pafTypeID;  // field values
  char pafAccountNo[30], pafRef[30];
  int cf; // confirm save to DB
  int newRec = 'y';
  int cfUpdate = 'y';

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
      proAccountUpdateWin = newwin(parow+20,pacol+10,1,120);

      proPanel = new_panel(proListWin);
      proTypePanel = new_panel(proTypeWin);
      proAccountUpdatePanel = new_panel(proAccountUpdateWin);
      mainPanel = new_panel(proAcctWin);
      update_panels();
      doupdate();
      
      keypad(proAcctWin, TRUE);
      keypad(proListWin, TRUE);
      keypad(proTypeWin, TRUE);
      keypad(proAccountUpdateWin, TRUE);

      set_form_win(proAcctForm,proAcctWin);
      set_form_sub(proAcctForm, derwin(proAcctWin,parow,pacol,1,1));
      getmaxyx(proAcctWin, parow, pacol);
      getmaxyx(proListWin, nrow, ncol);
      getmaxyx(proTypeWin, ptrow, ptcol);
      getmaxyx(proAccountUpdateWin, urows, ucols);
      box(proAcctWin,0,0);
      box(proListWin,0,0);
      box(proTypeWin,0,0);
      box(proAccountUpdateWin,0,0);
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
      mvwprintw(proAcctWin, parow-28,pacol-64, "Provider ID(F2):");
      mvwprintw(proAcctWin, parow-26,pacol-64, "Account Number:");
      mvwprintw(proAcctWin, parow-24,pacol-64, "Sort Code:");
      mvwprintw(proAcctWin, parow-22,pacol-64, "Reference:");
      mvwprintw(proAcctWin, parow-20,pacol-64, "Provider Type ID(F3):");
      wmove(proAcctWin,parow-30,pacol-39);
      wrefresh(proAcctWin);
  
      while((ch = wgetch(proAcctWin)) != KEY_F(1))
	{
	  hide_panel(proPanel);
	  hide_panel(proTypePanel);
	  hide_panel(proAccountUpdatePanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();	  
	  keyNavigate(ch, proAcctForm);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 2;
	      wclear(proListWin);
	      box(proListWin,0,0);
	      waddstr(proListWin, "Provider List");
	      wmove(proListWin,1,1);
	      //wrefresh(proListWin);
	      show_panel(proPanel);
	      update_panels();
	      doupdate();
	  
	      res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1 ORDER BY provider_id");	  
	      rows = PQntuples(res);

	      wrefresh(proListWin);
	  
	      while((p = wgetch(proListWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proListWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(proListWin);
		    }
		  list = 2;      
		  // wclrtoeol(proListWin);  //clear current line to right of cursor
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
		  //wrefresh(proAcctWin);
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
	      show_panel(proTypePanel);
	      update_panels();
	      doupdate();
	      wrefresh(proTypeWin);

	      res = PQexec(conn,"SELECT * FROM provider_type ORDER BY provider_type_id");
	      rows = PQntuples(res);

	      wrefresh(proTypeWin);

	      while((p = wgetch(proTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proTypeWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(proTypeWin);
		    }
		  list = 2;      
		  //wclrtoeol(proTypeWin);  //clear current line to right of cursor
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
		  // wrefresh(proAcctWin);
		}
	      noecho();
	      PQclear(res);
	    }  // if (ch == f3)
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(proAccountUpdateWin);
	      box(proAccountUpdateWin,0,0);
	      waddstr(proAccountUpdateWin, "Provider Account");
	      wmove(proAccountUpdateWin,1,1);
	      show_panel(proAccountUpdatePanel);
	      update_panels();
	      doupdate();
	      //wrefresh(proAccountUpdateWin);

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexec(conn,"SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(proAccountUpdateWin);
	  
	      while((p = wgetch(proAccountUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */ 
		      mvwprintw(proAccountUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(proAccountUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(proAccountUpdateWin);  
		      mvwprintw(proAccountUpdateWin,10,1,"End of list");
		      box(proAccountUpdateWin,0,0);
		      mvwprintw(proAccountUpdateWin,0,0, "provider Account");
		      wmove(proAccountUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(proAccountUpdateWin,11,1,"Select Option: ");
	      mvwscanw(proAccountUpdateWin,11,25, "%d", &upID);

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_account WHERE provider_acct_id = $1 ORDER BY provider_acct_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  mvwprintw(proAccountUpdateWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  mvwprintw(proAccountUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(proAccountUpdateWin);
		  set_field_buffer(proAcctField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(proAcctField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(proAcctField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(proAcctField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(proAcctField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(proAcctField[5],0,PQgetvalue(res,0,6));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(proAccountUpdateWin,12,1,"Number invalied");
		  wrefresh(proAccountUpdateWin);		
		  //wrefresh(proAcctWin);
		}
	      noecho();
	      PQclear(res);
	    } //F9	  
	}//while F1
      hide_panel(proPanel);
      hide_panel(proTypePanel);
      hide_panel(proAccountUpdatePanel);
      update_panels();
      doupdate();

      /* code goes here for assign buffer value and validate prior to insert */
      form_driver(proAcctForm,REQ_VALIDATION);

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
	  mvwprintw(proAcctWin,parow-9,pacol-64,"(d = delete record)");
	  wmove(proAcctWin,parow-10,pacol-54);

	  while((cf = wgetch(proAcctWin)) != 'y')
	    {
	      wmove(proAcctWin,parow-10,pacol-53);
	      if (cf == 'n')
		{
		  mvwprintw(proAcctWin,parow-8,pacol-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  proAccountDelete(upID);
		  mvwprintw(proAcctWin,parow-8,pacol-64, "Record deleted");                
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  proAccountUpdate(upID,pafActiveID, pafID, pafAccountNo, pafSortCode, pafRef, pafTypeID); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(proAcctWin,19,5, "Data updated");
		  mvwprintw(proAcctWin,20,5, "cfUpdate %d,upID %d pafActiveID %d padID %d", cfUpdate,upID,pafActiveID, pafID);  //DEBUG
		}
	      else		
		{
		  proAccountInsert(pafActiveID, pafID, pafAccountNo, pafSortCode, pafRef, pafTypeID);
		  mvwprintw(proAcctWin,parow-8, pacol-64, "Data saved");
		}
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

      cfUpdate = 0;

      mvwprintw(proAcctWin,parow-6,pacol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proAcctWin)) != 'y')
	{
	  wmove(proAcctWin,parow-6,pacol-27);  //CHECK COL POSITION OK
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(proAcctWin);
    }

  PQfinish(conn); 
  endwin();

  return 0;
  
}



  
