#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/catcur.h"
#include "../inc/jadlib.h"
#include "../inc/jaddef.h"

void categoryInsert()
{
  FIELD *categoryField[2];
  FORM *categoryForm;
  WINDOW *categoryWin, *categoryUpdateWin;
  PANEL *categoryPanel, *categoryUpdatePanel;
  int newRec = 'y';
  int crow, ccol, curow, cucol;
  int list = 2, i = 0, j = 0;
  char p;
  int ch;
  int rows;
  int val, upID, *params[1], length[1], formats[1];
  char catIDStr[5];
  int catID;
  char catDesc[50];
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
      waddstr(categoryWin,"Category Entry Form");
      waddstr(categoryUpdateWin,"Category");

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
	      waddstr(categoryUpdateWin, "Category");
	      wmove(categoryUpdateWin,1,1);	     
	      show_panel(categoryUpdatePanel);
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM category_type ORDER BY category_id");	  
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
	      res = PQexecParams(conn, "SELECT * FROM category_type WHERE category_id = $1;"
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
		  mvwprintw(categoryUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
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
		  //catDelete(upID);
		  mvwprintw(categoryWin,16,5, "Record deleted");                
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  //catUpdate(upID, catDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(categoryWin,16,5, "Data updated");
		  mvwprintw(categoryWin,17,5, "cfUpdate %d, upID %d, catDesc %s", cfUpdate,upID,catDesc);  //DEBUG
		}
      	      else
		{
		  //catInsert(catDesc);
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
      update_panels();
      doupdate();
      delwin(categoryWin);	
    } //while newRec
  PQfinish(conn);
  endwin();
}
