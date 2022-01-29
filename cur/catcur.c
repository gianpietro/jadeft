#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/catcur.h"
#include "../inc/catlib.h"
#include "../inc/jadlib.h"
#include "../inc/jaddef.h"

void categoryInsert()
{
  FIELD *categoryField[2];
  FORM *categoryForm;
  WINDOW *categoryWin, *categoryUpdateWin;
  PANEL *categoryPanel, *categoryUpdatePanel;
  int newRec = 'y';
  int row = 0, col = 0, urow = 0, ucol = 0;
  int list = 2, i = 0, j = 0;
  char p = 0;
  int ch = 0;
  int rows = 0;
  int val = 0, upID = 0, *params[1], length[1], formats[1];
  //char catIDStr[5];
  int catID = 0;
  //char catDesc[50];  
  char *catDesc;  // using a pinter instead of array
  int cfUpdate = 0;
  int cf = 0;
  const char *titleOne = "Category Type Form";
  const char *titleTwo = "Category Type";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while (newRec == 'y')
    {
      categoryField[0] = new_field(1,30,6,22,0,0);  /* category */
      categoryField[1] = NULL;

      set_field_fore(categoryField[0], COLOR_PAIR(9));    /* SET_FIELD_FOREGROUND */
      set_field_back(categoryField[0], COLOR_PAIR(9));    /* SET_FIELD_BACKGROUND */

      set_field_type(categoryField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      
      categoryForm = new_form(categoryField);
      scale_form(categoryForm, &row, &col);
      categoryWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      categoryUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);
      
      categoryPanel = new_panel(categoryWin);
      categoryUpdatePanel = new_panel(categoryUpdateWin);
      wbkgd(categoryWin, COLOR_PAIR(1));        /* MAIN_WINDOW_BACKGROUND_COLOR */    
      update_panels();
      doupdate();
      
      keypad(categoryWin, TRUE);
      keypad(categoryUpdateWin, TRUE);

      set_form_win(categoryForm, categoryWin);
      set_form_sub(categoryForm, derwin(categoryWin, row, col, 2, 2));
      getmaxyx(categoryWin,row,col);
      getmaxyx(categoryUpdateWin, urow, ucol);
      box(categoryWin,0,0);      
      box(categoryUpdateWin,0,0);
      //waddstr(categoryWin,"Category Type Form");
      //waddstr(categoryUpdateWin,"Category Type");

      if (categoryWin == NULL || categoryUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      wattron(categoryWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(categoryWin,1,(col-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
      wattroff(categoryWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */

      post_form(categoryForm);
      wrefresh(categoryWin);

      mvwprintw(categoryWin,row-(row-8),col-(col-5), "Category:");
      mvwprintw(categoryWin,row-2,col-(col-5),"Press F1 when form complete (F9 for Update)");
      wmove(categoryWin,row-(row-8),col-(col-24));    
      //wrefresh(categoryWin);
      
      while((ch = wgetch(categoryWin)) != KEY_F(1))
	{
	  hide_panel(categoryUpdatePanel);
	  show_panel(categoryPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, categoryForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(categoryUpdateWin);
	      box(categoryUpdateWin,0,0);
	      //waddstr(categoryUpdateWin, "Category Type");
	      wattron(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(categoryUpdateWin,1,(col-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(categoryUpdateWin,1,1);	     
	      show_panel(categoryUpdatePanel);
	      wbkgd(categoryUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM category_type ORDER BY id");	  
	      rows = PQntuples(res);

	      wrefresh(categoryUpdateWin);
	      wattron(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(categoryUpdateWin, 4, 1, "ID    Category");  //+3
	      wattroff(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(categoryUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(categoryUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(categoryUpdateWin);
		      box(categoryUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(categoryUpdateWin);  
		      mvwprintw(categoryUpdateWin,13,1,"End of list");
		      box(categoryUpdateWin,0,0);
		      //mvwprintw(categoryUpdateWin,0,0, "Category");
		      wattron(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(categoryUpdateWin,1,(col-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */ 
		      //mvwprintw(proUpdateWin,0,0, "Provider");
		      wattroff(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(categoryUpdateWin,urow-8,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(categoryUpdateWin,row-7,1,"Select Option: ");
	      mvwscanw(categoryUpdateWin,row-7,col-45, "%d", &upID);
	      wattroff(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM category_type WHERE id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  // mvwprintw(categoryUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  // mvwprintw(categoryUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(categoryField[0],0,PQgetvalue(res,0,1));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(categoryUpdateWin,row-6,1,"Number invalid");
		  wattroff(categoryUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(categoryUpdateWin);		
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} // while F1
      hide_panel(categoryUpdatePanel);
      update_panels();
      doupdate();

      form_driver(categoryForm,REQ_VALIDATION);
      
      catDesc = (char*)malloc(50 * sizeof(char)); //NEW CODE 14NOV21
      strcpy(catDesc,field_buffer(categoryField[0],0));

      if ((form_driver(categoryForm,REQ_VALIDATION) == E_OK) && (!isspace(*catDesc)))
	{
	  strcpy(catDesc, trimWS(catDesc));
	  echo();
	  wattron(categoryWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(categoryWin,row-8,col-65,"Save y/n: ");
	  wattroff(categoryWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
	  mvwprintw(categoryWin,row-7,col-65,"(d = delete record)");
	  wmove(categoryWin,row-8,col-54);

	  while((cf = wgetch(categoryWin)) != 'y')
	    {
	      wmove(categoryWin,row-8,col-54);
	      if (cf == 'n')
		{
		  mvwprintw(categoryWin,row-6,col-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  catDelete(upID);
		  wattron(categoryWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		  mvwprintw(categoryWin,row-6,col-65, "Record deleted");
		  wattroff(categoryWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  catUpdate(upID, catDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  wattron(categoryWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(categoryWin,row-6,col-65, "Data updated");
		  wattroff(categoryWin, A_BOLD | A_BLINK);
		  //mvwprintw(categoryWin,17,5, "cfUpdate %d, upID %d, catDesc %s", cfUpdate,upID,catDesc);  //DEBUG
		}
      	      else
		{
		  catInsert(catDesc);
		  wattron(categoryWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(categoryWin,row-6,col-65, "Data saved");
		  wattroff(categoryWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(categoryWin,row-6,col-65, "Data invalid");	
	}
      noecho();

      unpost_form(categoryForm);
      free_form(categoryForm);
      free_field(categoryField[0]);

      cfUpdate = 0;

      mvwprintw(categoryWin,row-4,col-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(categoryWin)) != 'y')
	{
	  wmove(categoryWin,row-4,col-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      hide_panel(categoryPanel);     
      update_panels(); 
      doupdate();
      delwin(categoryWin); 
      
    } //while newRec
  PQfinish(conn);
  free(catDesc);
  catDesc = NULL;
  endwin();   
}


void statementLinkInsert()
{
  FIELD *stmtLinkField[4];
  FORM *stmtLinkForm;
  WINDOW *stmtLinkWin, *catTypeWin, *stmtLinkUpdateWin;
  PANEL *stmtLinkPanel, *stmtLinkUpdatePanel, *catTypePanel;
  int newRec = 'y';
  int stlrow, stlcol, ctrow, ctcol, slurow, slucol;
  int list = 2, i = 0, j = 0;
  char p;
  int ch;
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  char stlIDStr[5];
  int stlID;
  int slfCatID; 
  char slfAlias[30], slfCategory[50];
  int cfUpdate = 0;
  int cf;
  const char *titleOne = "Statement Link Entry Form";
  const char *titleTwo = "Category Type";
  const char *titleThree = "Statement Link List";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);        /* INIT_PAIR_FOR_WIN_COLOR */
  init_pair(9,COLOR_WHITE,COLOR_BLACK);      /* INIT_PAIR_FOR_FIELD_COLOR */

  while (newRec == 'y')
    {
      stmtLinkField[0] = new_field(1,30,4,22,0,0);  /* alias       */
      stmtLinkField[1] = new_field(1,8,6,22,0,0);   /* category_id */
      stmtLinkField[2] = new_field(2,25,8,22,0,0);  /* category    */
      stmtLinkField[3] = NULL;

      set_field_fore(stmtLinkField[0], COLOR_PAIR(9));    /* SET_FIELD_FOREGROUND */
      set_field_back(stmtLinkField[0], COLOR_PAIR(9));    /* SET_FIELD_BACKGROUND */
      set_field_fore(stmtLinkField[1], COLOR_PAIR(9));
      set_field_back(stmtLinkField[1], COLOR_PAIR(9));
      set_field_fore(stmtLinkField[2], COLOR_PAIR(9));
      set_field_back(stmtLinkField[2], COLOR_PAIR(9));

      set_field_type(stmtLinkField[0],TYPE_REGEXP,"^[A-Za-z0-9- &]+$");
      set_field_type(stmtLinkField[1],TYPE_INTEGER,0,1,99999);
      set_field_type(stmtLinkField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      field_opts_off(stmtLinkField[2], O_ACTIVE); /* field is inactive cursor skips over */

      stmtLinkForm = new_form(stmtLinkField);
      scale_form(stmtLinkForm, &stlrow, &stlcol);
      //stmtLinkWin = newwin(stlrow+20,stlcol+10,1,1);
      stmtLinkWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      catTypeWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);
      stmtLinkUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);

      stmtLinkPanel = new_panel(stmtLinkWin);
      stmtLinkUpdatePanel = new_panel(stmtLinkUpdateWin);
      wbkgd(stmtLinkWin, COLOR_PAIR(1));        /* MAIN_WINDOW_BACKGROUND_COLOR */    
      catTypePanel = new_panel(catTypeWin);
      update_panels();
      doupdate();
      
      keypad(stmtLinkWin, TRUE);
      keypad(catTypeWin, TRUE);
      keypad(stmtLinkUpdateWin, TRUE);

      set_form_win(stmtLinkForm, stmtLinkWin);
      set_form_sub(stmtLinkForm, derwin(stmtLinkWin, stlrow, stlcol, 2, 2));
      getmaxyx(stmtLinkWin,stlrow,stlcol);
      getmaxyx(catTypeWin,ctrow,ctcol);
      getmaxyx(stmtLinkUpdateWin, slurow, slucol);
      box(stmtLinkWin,0,0);
      box(catTypeWin,0,0);
      box(stmtLinkUpdateWin,0,0);
      //waddstr(stmtLinkWin,"Statement Link Entry Form");
      //waddstr(catTypeWin,"Category Type");
      //waddstr(stmtLinkUpdateWin,"Statement Link");

      if (stmtLinkWin == NULL || catTypeWin == NULL || stmtLinkUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      wattron(stmtLinkWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(stmtLinkWin,1,(stlcol-lenOne)/2, titleOne);   /* SET_MAIN_WIND_TITLE */  
      wattroff(stmtLinkWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */

      post_form(stmtLinkForm);
      wrefresh(stmtLinkWin);

      mvwprintw(stmtLinkWin,stlrow-(stlrow-6),stlcol-(stlcol-5), "Alias:");
      mvwprintw(stmtLinkWin,stlrow-(stlrow-8),stlcol-(stlcol-5), "Category_ID(F2):");
      mvwprintw(stmtLinkWin,stlrow-(stlrow-10),stlcol-(stlcol-5), "Category:");
      mvwprintw(stmtLinkWin,stlrow-2,5,"Press F1 when form complete (F9 for Update)");
      wmove(stmtLinkWin,stlrow-(stlrow-6),stlcol-(stlcol-24));
      //wrefresh(stmtLinkWin);
      
      while((ch = wgetch(stmtLinkWin)) != KEY_F(1))
	{
	  hide_panel(catTypePanel);
	  hide_panel(stmtLinkUpdatePanel);
	  show_panel(stmtLinkPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, stmtLinkForm);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(catTypeWin);
	      box(catTypeWin,0,0);
	      //waddstr(catTypeWin, "Category Type");
	      wattron(catTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(catTypeWin,1,(ctcol-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(catTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(catTypeWin,1,1);
	      //wrefresh(catTypeWin);
	      show_panel(catTypePanel);
	      wbkgd(catTypeWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM category_type ORDER BY id");	  
	      rows = PQntuples(res);

	      wrefresh(catTypeWin);
	      wattron(catTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(catTypeWin, 4, 1, "ID    Category");  //+3
	      wattroff(catTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */	  
	      while((p = wgetch(catTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(catTypeWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(catTypeWin);
		      box(catTypeWin,0,0);
		    }
		  list = 6;      
		  //wclrtoeol(catTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(catTypeWin);  
		      mvwprintw(catTypeWin,ctrow-8,1,"End of list");
		      box(catTypeWin,0,0);
		      //mvwprintw(catTypeWin,0,0, "Category Type");
		      wattron(catTypeWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(catTypeWin,1,(ctcol-lenTwo)/2,titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(catTypeWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(catTypeWin,ctrow-8,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(catTypeWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(catTypeWin,ctrow-7,1,"Select Option: ");
	      mvwscanw(catTypeWin,ctrow-7,ctcol-45, "%5s", &stlIDStr);
	      set_field_buffer(stmtLinkField[1],0, stlIDStr);
	      wattroff(catTypeWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      stlID = atoi(field_buffer(stmtLinkField[1],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)stlID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM category_type WHERE id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(catTypeWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(catTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[1],0,PQgetvalue(res,0,0));
		  set_field_buffer(stmtLinkField[2],0,PQgetvalue(res,0,1));	 //Set category to field 
		  wrefresh(catTypeWin);
		}
	      else
		{
		  set_field_buffer(stmtLinkField[1],0,"");
		  set_field_buffer(stmtLinkField[2],0,"");	
		  wattron(catTypeWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(catTypeWin,ctrow-6,1,"Number invalid");
		  wattroff(catTypeWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(catTypeWin);		
		  //wrefresh(stmtLinkWin);
		}
	      noecho();
	      PQclear(res);
	    } // F2
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(stmtLinkUpdateWin);
	      box(stmtLinkUpdateWin,0,0);
	      //waddstr(stmtLinkUpdateWin, "Statement Link List");
	      wattron(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(stmtLinkUpdateWin,1,(slucol-lenThree)/2, titleThree);     /*SET_SUB_WIM_TITLE */
	      wattroff(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(stmtLinkUpdateWin,1,1);
	      //wrefresh(stmtLinkUpdateWin);
	      show_panel(stmtLinkUpdatePanel);
	      wbkgd(stmtLinkUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM statement_link ORDER BY id");	  
	      rows = PQntuples(res);

	      wrefresh(stmtLinkUpdateWin);
	      wattron(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(stmtLinkUpdateWin, 4, 1, "ID    Alias                     Category");  //+3
	      wattroff(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(stmtLinkUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(stmtLinkUpdateWin,list,1,"%-5s %-25s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(stmtLinkUpdateWin);
		      box(stmtLinkUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(stmtLinkUpdateWin);  
		      mvwprintw(stmtLinkUpdateWin,13,1,"End of list");
		      box(stmtLinkUpdateWin,0,0);
		      //mvwprintw(stmtLinkUpdateWin,0,0, "Statement Link List");
		      wattron(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	              mvwprintw(stmtLinkUpdateWin,1,(slucol-lenThree)/2, titleThree);     /*SET_SUB_WIM_TITLE */
	              wattroff(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
		      wmove(stmtLinkUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(stmtLinkUpdateWin,slurow-7,1,"Select Option: ");
	      mvwscanw(stmtLinkUpdateWin,slurow-7,slucol-45, "%d", &upID);
	      wattroff(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM statement_link WHERE id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(stmtLinkUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(stmtLinkUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(stmtLinkField[2],0,PQgetvalue(res,0,3));			 
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(stmtLinkUpdateWin,slurow-6,1,"Number invalid");
		  wattroff(stmtLinkUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(stmtLinkUpdateWin);		
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} // while F1
      hide_panel(catTypePanel);
      hide_panel(stmtLinkUpdatePanel);
      update_panels();
      doupdate();

      form_driver(stmtLinkForm,REQ_VALIDATION);

      strcpy(slfAlias, field_buffer(stmtLinkField[0],0));
      slfCatID = atoi(field_buffer(stmtLinkField[1],0));     
      strcpy(slfCategory, trimWS(field_buffer(stmtLinkField[2],0)));

      if ((form_driver(stmtLinkForm,REQ_VALIDATION) == E_OK) && (!isspace(*slfAlias)))
	{
	  strcpy(slfAlias, trimWS(slfAlias));
	  echo();
	  wattron(stmtLinkWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(stmtLinkWin,stlrow-8,stlcol-65,"Save y/n: ");
	  wattroff(stmtLinkWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
	  mvwprintw(stmtLinkWin,stlrow-7,stlcol-65,"(d = delete record)");
	  wmove(stmtLinkWin,stlrow-8,stlcol-54);

	  while((cf = wgetch(stmtLinkWin)) != 'y')
	    {
	      wmove(stmtLinkWin,stlrow-8,stlcol-54);
	      if (cf == 'n')
		{
		  mvwprintw(stmtLinkWin,stlrow-6,stlcol-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  stmtLinkDelete(upID);
		  wattron(stmtLinkWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		  mvwprintw(stmtLinkWin,stlrow-6,stlcol-65, "Record deleted");
		  wattroff(stmtLinkWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  stmtLinkUpdate(upID, slfAlias, slfCatID, slfCategory);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  wattron(stmtLinkWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(stmtLinkWin,stlrow-6,stlcol-65, "Data updated");
		  wattroff(stmtLinkWin, A_BOLD | A_BLINK);
		  // mvwprintw(stmtLinkWin,18,5, "cfUpdate %d,upID %d, slfAlias %s", cfUpdate,upID, slfAlias);  //DEBUG
		}
      	      else
		{
		  stmtLinkInsert(slfAlias, slfCatID, slfCategory);
		  wattron(stmtLinkWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(stmtLinkWin,stlrow-6,stlcol-65, "Data saved");
		  wattroff(stmtLinkWin, A_BOLD | A_BLINK);    /* ATTOFF_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(stmtLinkWin,stlrow-6,stlcol-65, "Data invalid");	
	}
      noecho();

      unpost_form(stmtLinkForm);
      free_form(stmtLinkForm);
      free_field(stmtLinkField[0]);
      free_field(stmtLinkField[1]);
      free_field(stmtLinkField[2]);
   
      cfUpdate = 0;

      mvwprintw(stmtLinkWin,stlrow-4,stlcol-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(stmtLinkWin)) != 'y')
	{
	  wmove(stmtLinkWin,stlrow-4,stlcol-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(stmtLinkPanel);
      update_panels();
      doupdate();      
      //del_panel(stmtLinkUpdatePanel);  // del_panel (3) added 30/10/21
      //del_panel(catTypePanel);
      //del_panel(stmtLinkPanel);  
      delwin(stmtLinkWin);
    } //while newRec
  PQfinish(conn);    
    endwin();
}
