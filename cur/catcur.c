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
  int crow = 0, ccol = 0, curow = 0, cucol = 0;
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

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  // keypad(stdscr, TRUE);

  while (newRec == 'y')
    {
      categoryField[0] = new_field(2,25,2,24,0,0);  /* category */
      categoryField[1] = NULL;

      set_field_type(categoryField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      
      categoryForm = new_form(categoryField);
      scale_form(categoryForm, &crow, &ccol);      

      categoryWin = newwin(crow+20,ccol+10,1,1);
      categoryUpdateWin = newwin(20,50,1,120);
      categoryPanel = new_panel(categoryWin);
      categoryUpdatePanel = new_panel(categoryUpdateWin);
      update_panels();
      doupdate();
      
      keypad(categoryWin, TRUE);
      keypad(categoryUpdateWin, TRUE);

      set_form_win(categoryForm, categoryWin);
      set_form_sub(categoryForm, derwin(categoryWin, crow, ccol, 1, 1));
      getmaxyx(categoryWin,crow,ccol);
      getmaxyx(categoryUpdateWin, curow, cucol);
      box(categoryWin,0,0);      
      box(categoryUpdateWin,0,0);
      waddstr(categoryWin,"Category Type Form");
      waddstr(categoryUpdateWin,"Category Type");

      if (categoryWin == NULL || categoryUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      post_form(categoryForm);
      wrefresh(categoryWin);

      mvwprintw(categoryWin,3,5, "Category:");
      mvwprintw(categoryWin,21,5,"Press F1 when form complete (F9 for Update)");
      wmove(categoryWin,3,25);    
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
	      list = 2;
	      wclear(categoryUpdateWin);
	      box(categoryUpdateWin,0,0);
	      waddstr(categoryUpdateWin, "Category Type");
	      wmove(categoryUpdateWin,1,1);	     
	      show_panel(categoryUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM category_type ORDER BY id");	  
	      rows = PQntuples(res);

	      wrefresh(categoryUpdateWin);
	  
	      while((p = wgetch(categoryUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(categoryUpdateWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(categoryUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(categoryUpdateWin);  
		      mvwprintw(categoryUpdateWin,10,1,"End of list");
		      box(categoryUpdateWin,0,0);
		      mvwprintw(categoryUpdateWin,0,0, "Category");
		      wmove(categoryUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(categoryUpdateWin,11,1,"Select Option: ");
	      mvwscanw(categoryUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(categoryUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(categoryUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(categoryField[0],0,PQgetvalue(res,0,1));
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(categoryUpdateWin,12,1,"Number invalied");
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
	  mvwprintw(categoryWin,14,5,"Save y/n: ");
	  mvwprintw(categoryWin,15,5,"(d = delete record)");
	  wmove(categoryWin,14,16);

	  while((cf = wgetch(categoryWin)) != 'y')
	    {
	      wmove(categoryWin,14,16);
	      if (cf == 'n')
		{
		  mvwprintw(categoryWin,16,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  catDelete(upID);
		  mvwprintw(categoryWin,16,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  catUpdate(upID, catDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(categoryWin,16,5, "Data updated");
		  mvwprintw(categoryWin,17,5, "cfUpdate %d, upID %d, catDesc %s", cfUpdate,upID,catDesc);  //DEBUG
		}
      	      else
		{
		  catInsert(catDesc);
		  mvwprintw(categoryWin,17,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(categoryWin,17,5, "Data invalid");	
	}
      noecho();

      unpost_form(categoryForm);
      free_form(categoryForm);
      free_field(categoryField[0]);

      cfUpdate = 0;

      mvwprintw(categoryWin,19,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(categoryWin)) != 'y')
	{
	  wmove(categoryWin,19,44);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      hide_panel(categoryPanel);
      //del_panel(categoryPanel); //+ 2/11/21
      update_panels(); 
      doupdate();

      // wclear(categoryWin);
      //wclear(categoryUpdateWin);
      //del_panel(categoryUpdatePanel); //new code 30/10/21 was comment out + 2/11/21
      //update_panels();   // + 2/11/21
      //doupdate();        // + 2/11/21
      delwin(categoryWin); 
      delwin(categoryUpdateWin);
      //categoryPanel = NULL;
      //categoryWin = NULL;
      //categoryUpdateWin = NULL;
      //categoryForm = NULL;
      //categoryField[0] = NULL;
    } //while newRec
  PQfinish(conn);
  free(catDesc);
  catDesc = NULL;
  endwin(); 
  //touchwin(stdscr);
  //free(catDesc);
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

  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while (newRec == 'y')
    {
      stmtLinkField[0] = new_field(2,25,2,33,0,0);  /* alias       */
      stmtLinkField[1] = new_field(1,8,4,33,0,0);   /* category_id */
      stmtLinkField[2] = new_field(2,25,6,33,0,0);  /* category    */
      stmtLinkField[3] = NULL;

      set_field_type(stmtLinkField[0],TYPE_REGEXP,"^[A-Za-z0-9 -&]+$");
      set_field_type(stmtLinkField[1],TYPE_INTEGER,0,1,99999);
      set_field_type(stmtLinkField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      field_opts_off(stmtLinkField[2], O_ACTIVE); /* field is inactive cursor skips over */

      stmtLinkForm = new_form(stmtLinkField);
      scale_form(stmtLinkForm, &stlrow, &stlcol);
      //stmtLinkWin = newwin(stlrow+20,stlcol+10,1,1);
      stmtLinkWin = newwin(28,68,1,1);
      catTypeWin = newwin(20,50,1,120);
      stmtLinkUpdateWin = newwin(20,50,1,120);

      stmtLinkPanel = new_panel(stmtLinkWin);
      stmtLinkUpdatePanel = new_panel(stmtLinkUpdateWin);
      catTypePanel = new_panel(catTypeWin);
      update_panels();
      doupdate();
      
      keypad(stmtLinkWin, TRUE);
      keypad(catTypeWin, TRUE);
      keypad(stmtLinkUpdateWin, TRUE);

      set_form_win(stmtLinkForm, stmtLinkWin);
      set_form_sub(stmtLinkForm, derwin(stmtLinkWin, stlrow, stlcol, 1, 1));
      getmaxyx(stmtLinkWin,stlrow,stlcol);
      getmaxyx(catTypeWin,ctrow,ctcol);
      getmaxyx(stmtLinkUpdateWin, slurow, slucol);
      box(stmtLinkWin,0,0);
      box(catTypeWin,0,0);
      box(stmtLinkUpdateWin,0,0);
      waddstr(stmtLinkWin,"Statement Link Entry Form");
      waddstr(catTypeWin,"Category Type");
      waddstr(stmtLinkUpdateWin,"Statement Link");

      if (stmtLinkWin == NULL || catTypeWin == NULL || stmtLinkUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      post_form(stmtLinkForm);
      wrefresh(stmtLinkWin);

      mvwprintw(stmtLinkWin,3,5, "Alias:");
      mvwprintw(stmtLinkWin,5,5, "Category_ID(F2):");
      mvwprintw(stmtLinkWin,7,5, "Category:");
      mvwprintw(stmtLinkWin,stlrow-2,5,"Press F1 when form complete (F9 for update)");
      wmove(stmtLinkWin,3,34);
      wrefresh(stmtLinkWin);
      
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
	      list = 2;
	      wclear(catTypeWin);
	      box(catTypeWin,0,0);
	      waddstr(catTypeWin, "Category Type");
	      wmove(catTypeWin,1,1);
	      //wrefresh(catTypeWin);
	      show_panel(catTypePanel);
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM category_type ORDER BY id");	  
	      rows = PQntuples(res);

	      //wrefresh(catTypeWin);
	  
	      while((p = wgetch(catTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(catTypeWin,list,1,"%s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(catTypeWin);  
		    }
		  list = 2;      
		  //wclrtoeol(catTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(catTypeWin);  
		      mvwprintw(catTypeWin,10,1,"End of list");
		      box(catTypeWin,0,0);
		      mvwprintw(catTypeWin,0,0, "Category Type");
		      wmove(catTypeWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(catTypeWin,11,1,"Select Option: ");
	      mvwscanw(catTypeWin,11,25, "%5s", &stlIDStr);
	      set_field_buffer(stmtLinkField[1],0, stlIDStr);

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
		  mvwprintw(catTypeWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(catTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[2],0,PQgetvalue(res,0,1));	 //Set category to field 
		  wrefresh(catTypeWin);
		}
	      else
		{
		  mvwprintw(catTypeWin,12,1,"Number invalid");
		  wrefresh(catTypeWin);		
		  //wrefresh(stmtLinkWin);
		}
	      noecho();
	      PQclear(res);
	    } // F2
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 2;
	      wclear(stmtLinkUpdateWin);
	      box(stmtLinkUpdateWin,0,0);
	      waddstr(stmtLinkUpdateWin, "Statement Link List");
	      wmove(stmtLinkUpdateWin,1,1);
	      //wrefresh(stmtLinkUpdateWin);
	      show_panel(stmtLinkUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM statement_link ORDER BY id");	  
	      rows = PQntuples(res);

	      wrefresh(stmtLinkUpdateWin);
	  
	      while((p = wgetch(stmtLinkUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(stmtLinkUpdateWin,list,1,"%s %s %s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(stmtLinkUpdateWin);
		    }
		  list = 2;      
		  if  (i == rows)
		    {
		      wclrtobot(stmtLinkUpdateWin);  
		      mvwprintw(stmtLinkUpdateWin,10,1,"End of list");
		      box(stmtLinkUpdateWin,0,0);
		      mvwprintw(stmtLinkUpdateWin,0,0, "Statement Link List");
		      wmove(stmtLinkUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();  
	      mvwprintw(stmtLinkUpdateWin,11,1,"Select Option: ");
	      mvwscanw(stmtLinkUpdateWin,11,25, "%d", &upID);

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
		  mvwprintw(stmtLinkUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  mvwprintw(stmtLinkUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(stmtLinkField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(stmtLinkField[2],0,PQgetvalue(res,0,3));			 
		  cfUpdate = 1;
		}
	      else
		{
		  mvwprintw(stmtLinkUpdateWin,12,1,"Number invalied");
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
	  mvwprintw(stmtLinkWin,15,5,"Save y/n: ");
	  mvwprintw(stmtLinkWin,16,5,"(d = delete record)");
	  wmove(stmtLinkWin,15,16);

	  while((cf = wgetch(stmtLinkWin)) != 'y')
	    {
	      wmove(stmtLinkWin,15,16);
	      if (cf == 'n')
		{
		  mvwprintw(stmtLinkWin,17,5, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  stmtLinkDelete(upID);
		  mvwprintw(stmtLinkWin,17,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  stmtLinkUpdate(upID, slfAlias, slfCatID, slfCategory);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(stmtLinkWin,17,5, "Data updated");
		  mvwprintw(stmtLinkWin,18,5, "cfUpdate %d,upID %d, slfAlias %s", cfUpdate,upID, slfAlias);  //DEBUG
		}
      	      else
		{
		  stmtLinkInsert(slfAlias, slfCatID, slfCategory);
		  mvwprintw(stmtLinkWin,17,5, "Data saved");
		}
	    }
	}
      else
	{
	  mvwprintw(stmtLinkWin,17,5, "Data invalid");	
	}
      noecho();

      unpost_form(stmtLinkForm);
      free_form(stmtLinkForm);
      free_field(stmtLinkField[0]);
      free_field(stmtLinkField[1]);
      free_field(stmtLinkField[2]);
   
      cfUpdate = 0;

      mvwprintw(stmtLinkWin,23,5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(stmtLinkWin)) != 'y')
	{
	  wmove(stmtLinkWin,23,44);
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
