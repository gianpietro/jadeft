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
  int cf;                                                                       /* confirm save data      */
  int actInd;                                                                   /* active_Ind             */
  char pname[30];                                                               /* provider_name          */ 
  int newRec= 'y';                                                              /* Add another new record */
  int rows, cols;
  int cfUpdate = 0;
  int list = 3, i = 0, j = 0; 
  char p;
  int urows, ucols;
  int trows, val, upID, *params[1], length[1], formats[1];
  const char *formTitle = "Provider Entry Form";
  int titleLen = strlen(formTitle);
  const char *provTitle = "Provider";
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
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(3,COLOR_YELLOW,COLOR_MAGENTA);
  init_pair(4,COLOR_WHITE,COLOR_CYAN);  
  init_pair(6,COLOR_BLACK,COLOR_YELLOW);
  init_pair(8,COLOR_BLACK,COLOR_WHITE); 
  init_pair(9,COLOR_WHITE,COLOR_BLACK);
  init_pair(10,COLOR_BLACK,COLOR_CYAN);
  

    while (newRec == 'y')                                                       /* Start loop to allow option to add subsequent records to form */
    {       
      providerField[0] = new_field(1, 1, 4, 22, 0, 0);
      providerField[1] = new_field(1, 30, 6, 22, 0, 0);      
      providerField[2] = NULL;

      set_field_fore(providerField[0], COLOR_PAIR(9));
      set_field_back(providerField[0], COLOR_PAIR(9));
      set_field_fore(providerField[1], COLOR_PAIR(9));
      set_field_back(providerField[1], COLOR_PAIR(9));

      set_field_type(providerField[0],TYPE_INTEGER,1,1,2);     
      set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");   
 
      providerForm = new_form(providerField);
      scale_form(providerForm, &rows, &cols); 

      /* Add window which will be associated to form */
      proWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);           
      proUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);     

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
      mvwprintw(proWin,1,(cols-titleLen)/2, formTitle);     
      wattroff(proWin,A_BOLD | COLOR_PAIR(1));

      post_form(providerForm); 
      wrefresh(proWin);

      mvwprintw(proWin,rows-(rows-6),cols-(cols-5), "Active Ind:");                           
      mvwprintw(proWin,rows-(rows-8),cols-(cols-5), "Provider Name:");
      mvwprintw(proWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(proWin, rows-(rows-6),cols-(cols-24));                              /* move cursor */

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
	      list = 6;    
	      wclear(proUpdateWin);
	      box(proUpdateWin,0,0);
	      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(proUpdateWin,1,(cols-provLen)/2, provTitle);     
	      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      wmove(proUpdateWin,1,1);
	      show_panel(proPanel);
	      wbkgd(proUpdateWin, COLOR_PAIR(1));
	      update_panels();
	      doupdate();

	      /* assign the required select statement */
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
		      /* change number of pqgetvalue return items as required */ 
		      mvwprintw(proUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;		     
		      wclrtoeol(proUpdateWin);
		      box(proUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(proUpdateWin);  
		      mvwprintw(proUpdateWin,13,1,"End of list");  
		      box(proUpdateWin,0,0);
		      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		      mvwprintw(proUpdateWin,1,(cols-provLen)/2, provTitle);     
		      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		      wmove(proUpdateWin,urows-8,1); 
		      break;
		    }
		}	  
	      echo();
	      wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(proUpdateWin,urows-7,1,"Select Option: "); 
	      mvwscanw(proUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* assign the required select statement */
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
		  set_field_buffer(providerField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(providerField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		  mvwprintw(proUpdateWin,urows-6,1,"Number invalid");
		  wattroff(proUpdateWin,A_BOLD | COLOR_PAIR(1));
		  wrefresh(proUpdateWin);		
		}
	      noecho();
	      PQclear(res);
	    } /* F9 */
	} /* while F1 */
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
	  wattron(proWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(proWin,rows-8,cols-65,"Save: y/n: ");
	  wattroff(proWin,A_BOLD | COLOR_PAIR(1));
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
		  providerUpdate(upID,actInd,pname); 
		  wattron(proWin, A_BOLD | A_BLINK);
		  mvwprintw(proWin,rows-6, cols-65, "Data updated");
		  wattroff(proWin, A_BOLD | A_BLINK);		  
		}
	      else
		{
		  providerInsert(actInd,pname);   
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
  const char *titleOne = "Provider Type Form";
  const char *titleTwo = "Provider Type";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  
  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')  
    {
      proTypeField[0] = new_field(1,30,6,22,0,0);
      proTypeField[1] = NULL;
      set_field_type(proTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      set_field_fore(proTypeField[0], COLOR_PAIR(9));                           /* set_field_foreground */
      set_field_back(proTypeField[0], COLOR_PAIR(9));                           /* set_field_background */

      proTypeForm = new_form(proTypeField);
      scale_form(proTypeForm, &rows, &cols);
      proTypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4); 
      proTypeUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4); 

      proTypePanel = new_panel(proTypeUpdateWin);
      mainPanel = new_panel(proTypeWin);
      wbkgd(proTypeWin, COLOR_PAIR(1));                                         /* main_window_background_color */    
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
      
      wattron(proTypeWin,A_BOLD | COLOR_PAIR(1));                               /* atton_main_win_title */
      mvwprintw(proTypeWin,1,(cols-lenOne)/2,titleOne);                         /* set_main_wind_title */
      wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));                              /* attoff_main_win_title */
      post_form(proTypeForm); 
      wrefresh(proTypeWin);
      mvwprintw(proTypeWin,rows-(rows-8),cols-(cols-5), "Description:");
      mvwprintw(proTypeWin,rows-2,cols-(cols-5),"Press F1 when form complete (F9 for Update)");
      wmove(proTypeWin,rows-(rows-8),cols-(cols-24));    

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
	      list = 6;
	      wclear(proTypeUpdateWin);
	      box(proTypeUpdateWin,0,0);
	      wattron(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));   
	      mvwprintw(proTypeUpdateWin,1,(cols-lenTwo)/2, titleTwo);  
	      wattroff(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(proTypeUpdateWin,1,1);
	      show_panel(proTypePanel);
	      wbkgd(proTypeUpdateWin, COLOR_PAIR(1));          
	      update_panels();
	      doupdate();	  
	      wrefresh(proTypeUpdateWin);

	      /* assign the required select statement */
	      res = PQexec(conn,"SELECT * FROM provider_type ORDER BY provider_type_id");	  
	      trows = PQntuples(res);

	      wrefresh(proTypeUpdateWin);
	      wattron(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));     
	      mvwprintw(proTypeUpdateWin, 4, 1, "ID    Type");  
	      wattroff(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));   
	      while((p = wgetch(proTypeUpdateWin)) == '\n')
		{
		  if ( j + RANGE < trows)
		    j = j + RANGE;	
		  else
		    j = j + (trows - j);
		  for (i; i < j; i++)
		    {
		      /* change number of pqgetvalue return items as required */ 
		      mvwprintw(proTypeUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(proTypeUpdateWin);
		      box(proTypeUpdateWin,0,0);    
		    }
		  list = 6;      
		  if  (i == trows)
		    {
		      wclrtobot(proTypeUpdateWin);  
		      mvwprintw(proTypeUpdateWin,13,1,"End of list");
		      box(proTypeUpdateWin,0,0);
      		      wattron(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1)); 
		      mvwprintw(proTypeUpdateWin,1,(cols-lenTwo)/2, titleTwo); 
		      wattroff(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));  
		      wmove(proTypeUpdateWin,urows-8,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(proTypeUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(proTypeUpdateWin,urows-7,ucols-45, "%d", &upID);
	      wattroff(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));          

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* assign the required select statement */
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
		  set_field_buffer(proTypeField[0],0,PQgetvalue(res,0,1));		 
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));       
		  mvwprintw(proTypeUpdateWin,urows-6,1,"Number invalid");
		  wattroff(proTypeUpdateWin,A_BOLD | COLOR_PAIR(1));      
		  wrefresh(proTypeUpdateWin);		
		}
	      noecho();
	      PQclear(res);
	    } /* F9 */
	} /* while F1 */
      hide_panel(proTypePanel);
      update_panels();
      doupdate();	  
      
      form_driver(proTypeForm,REQ_VALIDATION);

      strcpy(pdesc,field_buffer(proTypeField[0],0));
     
      if ((form_driver(proTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*pdesc)))
	{
	  strcpy(pdesc, trimWS(pdesc));
	  echo();
	  wattron(proTypeWin,A_BOLD | COLOR_PAIR(1));   
	  mvwprintw(proTypeWin,rows-8,cols-65, "Save y/n: ");
	  wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));  
	  mvwprintw(proTypeWin,rows-7,cols-65,"(d = delete record)");
	  wmove(proTypeWin,rows-8,cols-54);
	  while((cf = wgetch(proTypeWin)) != 'y')
	    {
	      wmove(proTypeWin,rows-8,cols-54);
	      if (cf == 'n')
		{
		  mvwprintw(proTypeWin,rows-6,cols-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  proTypeDelete(upID);
		  wattron(proTypeWin, A_BOLD | A_BLINK);       
		  mvwprintw(proTypeWin,rows-6,cols-65, "Record deleted");
		  wattroff(proTypeWin, A_BOLD | A_BLINK);      
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  proTypeUpdate(upID,pdesc); 
		  wattron(proTypeWin, A_BOLD | A_BLINK);    
		  mvwprintw(proTypeWin,rows-6, cols-65, "Data updated");
		  wattroff(proTypeWin, A_BOLD | A_BLINK);
		}
	      else
		{
		  proTypeInsert(pdesc);
		  wattron(proTypeWin, A_BOLD | A_BLINK);     
		  mvwprintw(proTypeWin,rows-6,cols-65, "Data saved");
		  wattroff(proTypeWin, A_BOLD | A_BLINK);   
		}
	    }
	}
	  else
	    {
	      mvwprintw(proTypeWin,rows-6,cols-65, "Data invalid");
	    }
	  noecho();

      unpost_form(proTypeForm);
      free_form(proTypeForm);
      free_field(proTypeField[0]);
      free_field(proTypeField[1]);

      cfUpdate = 0;

      mvwprintw(proTypeWin,rows-4,cols-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proTypeWin)) != 'y')
	{
	  wmove(proTypeWin,rows-4,cols-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(proTypeWin);
    }
  PQfinish(conn);
  endwin();
}


/* Function to display Provider Account Form and retrieve DB records
   for provider and provider type to select for provider account */
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
  char proIDstr[5];                                                             /* store provide_id as str */
  char proTypestr[5];
  int rows;                                                                     /* number of tuples returned from sql query */
  int upID;
  int val, *params[1], length[1], formats[1];                                   /* PQexecParams */
  int pafActiveID, pafID, pafSortCode, pafTypeID;                               /* field values */
  char pafAccountNo[30], pafRef[30];
  int cf;                                                                       /* confirm save to DB */
  int newRec = 'y';
  int cfUpdate = 0; 
  const char *titleOne = "Provider Account Form";
  const char *titleTwo = "Provider List";
  const char *titleThree = "Provider Type";
  const char *titleFour = "Provider Account";  
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  int lenFour = strlen(titleFour); 

  PGconn *conn =  fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  
  
  while (newRec == 'y')
    {
      proAcctField[0] = new_field(1,1,4,28,0,0);                                 /* active_ind          */
      proAcctField[1] = new_field(1,5,6,28,0,0);                                 /* provider_id (fk)    */
      proAcctField[2] = new_field(1,30,8,28,0,0);                                /* provider_account_no */
      proAcctField[3] = new_field(1,9,10,28,0,0);                                /* sort_code           */
      proAcctField[4] = new_field(1,30,12,28,0,0);                               /* reference           */
      proAcctField[5] = new_field(1,5,14,28,0,0);                                /* provider_type_id    */
      proAcctField[6] = NULL;

      
      set_field_fore(proAcctField[0], COLOR_PAIR(9));
      set_field_back(proAcctField[0], COLOR_PAIR(9));
      set_field_fore(proAcctField[1], COLOR_PAIR(9));
      set_field_back(proAcctField[1], COLOR_PAIR(9));
      set_field_fore(proAcctField[2], COLOR_PAIR(9));
      set_field_back(proAcctField[2], COLOR_PAIR(9));
      set_field_fore(proAcctField[3], COLOR_PAIR(9));
      set_field_back(proAcctField[3], COLOR_PAIR(9));
      set_field_fore(proAcctField[4], COLOR_PAIR(9));
      set_field_back(proAcctField[4], COLOR_PAIR(9));
      set_field_fore(proAcctField[5], COLOR_PAIR(9));
      set_field_back(proAcctField[5], COLOR_PAIR(9));
      

      set_field_type(proAcctField[0],TYPE_INTEGER,1,1,2);
      set_field_type(proAcctField[1],TYPE_INTEGER,0,1,99999);
      set_field_type(proAcctField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(proAcctField[3],TYPE_INTEGER,0,1,999999999);
      set_field_type(proAcctField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(proAcctField[5],TYPE_INTEGER,0,1,99999);

      proAcctForm = new_form(proAcctField);
      scale_form(proAcctForm, &parow, &pacol);   

      proAcctWin = newwin(LINES-15, COLS/3,LINES-(LINES-4),COLS/15);
      proListWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      proTypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      proAccountUpdateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      proPanel = new_panel(proListWin);      
      proTypePanel = new_panel(proTypeWin);
      proAccountUpdatePanel = new_panel(proAccountUpdateWin);
      mainPanel = new_panel(proAcctWin);
      wbkgd(proAcctWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();
      
      keypad(proAcctWin, TRUE);
      keypad(proListWin, TRUE);
      keypad(proTypeWin, TRUE);
      keypad(proAccountUpdateWin, TRUE);

      set_form_win(proAcctForm,proAcctWin);
      set_form_sub(proAcctForm, derwin(proAcctWin,parow,pacol,2,2));
      getmaxyx(proAcctWin, parow, pacol);
      getmaxyx(proListWin, nrow, ncol);
      getmaxyx(proTypeWin, ptrow, ptcol);
      getmaxyx(proAccountUpdateWin, urows, ucols);
      box(proAcctWin,0,0);
      box(proListWin,0,0);
      box(proTypeWin,0,0);
      box(proAccountUpdateWin,0,0);
      wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));     
      mvwprintw(proAcctWin,1,(pacol-lenOne)/2,titleOne);  
      wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));   
      
      if(proListWin == NULL || proAcctWin == NULL || proTypeWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  return(1);
	}
      
      post_form(proAcctForm);  
      wrefresh(proAcctWin);

      mvwprintw(proAcctWin, parow-(parow-6),pacol-(pacol-5), "Active Ind:");
      mvwprintw(proAcctWin, parow-(parow-8),pacol-(pacol-5), "Provider ID(F2):");
      mvwprintw(proAcctWin, parow-(parow-10),pacol-(pacol-5), "Account Number:");
      mvwprintw(proAcctWin, parow-(parow-12),pacol-(pacol-5), "Sort Code:");
      mvwprintw(proAcctWin, parow-(parow-14),pacol-(pacol-5), "Reference:");
      mvwprintw(proAcctWin, parow-(parow-16),pacol-(pacol-5), "Provider Type ID(F3):");
      mvwprintw(proAcctWin, parow-2,pacol-(pacol-5),"Press F1 when form complete (F9 for Update)");
      wmove(proAcctWin, parow-(parow-6),pacol-(pacol-30));     
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
	      list = 6;
	      wclear(proListWin);
	      box(proListWin,0,0);
	      wattron(proListWin,A_BOLD | COLOR_PAIR(1));   
	      mvwprintw(proListWin,1,(ncol-lenTwo)/2, titleTwo); 
	      wattroff(proListWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(proListWin,1,1);
	      show_panel(proPanel);
	      wbkgd(proListWin, COLOR_PAIR(1));          
	      update_panels();
	      doupdate();
	  
	      res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1 ORDER BY provider_id");	  
	      rows = PQntuples(res);

	      wrefresh(proListWin);
	      wattron(proListWin,A_BOLD | COLOR_PAIR(1));  
	      mvwprintw(proListWin, 4, 1, "ID    Name");  
	      wattroff(proListWin,A_BOLD | COLOR_PAIR(1));
	      while((p = wgetch(proListWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proListWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(proListWin);
		      box(proListWin,0,0);   
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(proListWin);                                    /* clear current line right of cursor and all lines below */
		      mvwprintw(proListWin,13,1,"End of list");
		      box(proListWin,0,0);
		      wattron(proListWin,A_BOLD | COLOR_PAIR(1));    
		      mvwprintw(proListWin,1,(ncol-lenTwo)/2, titleTwo); 
		      wattroff(proListWin,A_BOLD | COLOR_PAIR(1));  
		      wmove(proListWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(proListWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(proListWin,nrow-7,1,"Select Option: ");
	      mvwscanw(proListWin,nrow-7,ncol-45, "%5s", &proIDstr);
	      set_field_buffer(proAcctField[1],0, proIDstr);
              wattroff(proListWin,A_BOLD | COLOR_PAIR(1));          
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
		  set_field_buffer(proAcctField[1],0,PQgetvalue(res,0,0));
		  wrefresh(proListWin);
		}
	      else
		{
		  set_field_buffer(proAcctField[1],0, "");
		  wattron(proListWin,A_BOLD | COLOR_PAIR(1));          
		  mvwprintw(proListWin,nrow-6,1,"Number invalid");
		  wattroff(proListWin,A_BOLD | COLOR_PAIR(1));         
		  wrefresh(proListWin);		
		}
	      noecho();
	      PQclear(res);
	    }
	  if(ch == KEY_F(3))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(proTypeWin);
	      box(proTypeWin,0,0);
	      wattron(proTypeWin,A_BOLD | COLOR_PAIR(1)); 
	      mvwprintw(proTypeWin,1,(ncol-lenThree)/2, titleThree);  
	      wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(proTypeWin,1,1);
	      show_panel(proTypePanel);
	      wbkgd(proTypeWin, COLOR_PAIR(1));         
	      update_panels();
	      doupdate();
	      wrefresh(proTypeWin);

	      res = PQexec(conn,"SELECT * FROM provider_type ORDER BY provider_type_id");
	      rows = PQntuples(res);

	      wrefresh(proTypeWin);
	      wattron(proTypeWin,A_BOLD | COLOR_PAIR(1)); 
	      mvwprintw(proTypeWin, 4, 1, "ID    Description");
	      wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));   

	      while((p = wgetch(proTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(proTypeWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(proTypeWin);
		      box(proTypeWin,0,0); 
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(proTypeWin);                                    /* clear current line right of cursor and all lines below */
		      mvwprintw(proTypeWin,13,1,"End of list");
		      box(proTypeWin,0,0);
		      wattron(proTypeWin,A_BOLD | COLOR_PAIR(1));       
		      mvwprintw(proTypeWin,1,(ncol-lenThree)/2, titleThree); 
		      wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));  
		      wmove(proTypeWin,10,1);
		      break;
		    }
		}
	      echo();
	      wattron(proTypeWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(proTypeWin,ptrow-7,1,"Select Provider: ");
	      mvwscanw(proTypeWin,ptrow-7,ptcol-45, "%5s", &proTypestr);
	      set_field_buffer(proAcctField[5],0, proTypestr);
	      wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));          

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
		  set_field_buffer(proAcctField[5],0,PQgetvalue(res,0,0));
		  wrefresh(proTypeWin);
		}
	      else
		{
		  set_field_buffer(proAcctField[5],0,"");
		  wattron(proTypeWin,A_BOLD | COLOR_PAIR(1)); 
		  mvwprintw(proTypeWin,ptrow-6,1,"Number invalid");
		  wattroff(proTypeWin,A_BOLD | COLOR_PAIR(1));
		  wrefresh(proTypeWin);		
		  // wrefresh(proAcctWin);
		}
	      noecho();
	      PQclear(res);
	    }  /* if (ch == f3) */
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(proAccountUpdateWin);
	      box(proAccountUpdateWin,0,0);
	      wattron(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));  
	      mvwprintw(proAccountUpdateWin,1,(ncol-lenFour)/2, titleFour); 
	      wattroff(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(proAccountUpdateWin,1,1);
	      show_panel(proAccountUpdatePanel);
	      wbkgd(proAccountUpdateWin, COLOR_PAIR(1));         
	      update_panels();
	      doupdate();

	      /* assign the required select statement */	      
	      res = PQexec(conn,"SELECT pa.provider_acct_id, pa.provider_id, pa.provider_acct_no, p.provider_name, pt.description \
                                 FROM provider_account pa \
                                 INNER JOIN provider p ON (p.provider_id = pa.provider_id) \
                                 INNER JOIN provider_type pt ON (pt.provider_type_id = pa.provider_type_id) \
                                 ORDER BY pa.provider_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(proAccountUpdateWin);
	      wattron(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));  
	      mvwprintw(proAccountUpdateWin, 4, 1, "ID    Acct_No         Name                      Description");  
	      wattroff(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));  
	  
	      while((p = wgetch(proAccountUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      /* change number of pqgetvalue return items as required */ 		      
		      mvwprintw(proAccountUpdateWin,list,1,"%-5s %-15s %-25s %-15s", PQgetvalue(res,i,0),PQgetvalue(res,i,2),
				PQgetvalue(res,i,3),PQgetvalue(res,i,4));
		      list++;
		      wclrtoeol(proAccountUpdateWin);
		      box(proAccountUpdateWin,0,0);    
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(proAccountUpdateWin);  
		      mvwprintw(proAccountUpdateWin,13,1,"End of list");
		      box(proAccountUpdateWin,0,0);
		      wattron(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));
	              mvwprintw(proAccountUpdateWin,1,(ncol-lenFour)/2, titleFour); 
	              wattroff(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));   
		      wmove(proAccountUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	       wattron(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(proAccountUpdateWin,urows-7,1,"Select Option: ");
	      mvwscanw(proAccountUpdateWin,urows-7, ucols-45, "%d", &upID);
	      wattroff(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));           

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* assign the required select statement */
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
		  wattron(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1)); 
		  mvwprintw(proAccountUpdateWin,urows-6,1,"Number invalid");
		  wrefresh(proAccountUpdateWin);
		  wattroff(proAccountUpdateWin,A_BOLD | COLOR_PAIR(1));   
		}
	      noecho();
	      PQclear(res);
	    } /* F9 */	  
	}/* while F1 */
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
	  wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(proAcctWin,parow-10,pacol-64,"Save: y/n: ");
	  wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(proAcctWin,parow-9,pacol-64,"(d = delete record)");
	  wmove(proAcctWin,parow-10,pacol-53);

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
		  wattron(proAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(proAcctWin,parow-8,pacol-64, "Record deleted");
		  wattroff(proAcctWin,A_BOLD | A_BLINK);
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  proAccountUpdate(upID,pafActiveID, pafID, pafAccountNo, pafSortCode, pafRef, pafTypeID); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  wattron(proAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(proAcctWin,parow-8, pacol-64, "Data updated");
		  wattroff(proAcctWin,A_BOLD | A_BLINK);
		}
	      else		
		{
		  proAccountInsert(pafActiveID, pafID, pafAccountNo, pafSortCode, pafRef, pafTypeID);
		  wattron(proAcctWin,A_BOLD | A_BLINK);
		  mvwprintw(proAcctWin,parow-8, pacol-64, "Data saved");
		  wattroff(proAcctWin,A_BOLD | A_BLINK);
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
	  wmove(proAcctWin,parow-6,pacol-27); 
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



  
