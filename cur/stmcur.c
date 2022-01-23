#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <arpa/inet.h>
#include "../inc/stmcur.h"
//#include "../inc/prolib.h"
#include "../inc/jadlib.h"


/* Form linked to statement table to insert, update
   and delete values */
void stmtDataAudit()
{
  WINDOW *mainWin, *updateWin;
  FORM *mainForm;
  FIELD *inputField[8];
  PANEL *mainPanel,*updatePanel;
  int list = 2, i = 0, j = 0;
  char p;
  int ch, mrow, mcol, urow, ucol;
  int rows; 
  int upID;
  int val, *params[1], length[1], formats[1];   //PQexecParams 
  int fv1;  // field values
  char fv2[4], fv3[150], fv5[50], fv6[30];
  float fv4;
  int cf; // confirm save to DB
  int newRec = 'y';
  int cfUpdate = 0;
  const char *titleOne = "Statement Form";
  const char *titleFour = "Provider Account";  
  int lenOne = strlen(titleOne);
  int lenFour = strlen(titleFour);
  int fldColor = 0;
  int df= 0; //date from
  int dt = 0; //date to

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
      inputField[0] = new_field(1,6,4,28,0,0);        /* statement_id */
      inputField[1] = new_field(1,8,6,28,0,0);        /* date */
      inputField[2] = new_field(1,3,8,28,0,0);        /* type */
      inputField[3] = new_field(5,30,10,28,0,0);      /* description */
      inputField[4] = new_field(1,10,16,28,0,0);      /* value */
      inputField[5] = new_field(1,30,18,28,0,0);      /* account */
      inputField[6] = new_field(1,30,20,28,0,0);      /* alias */
      inputField[7] = NULL;

      set_field_fore(inputField[0], COLOR_PAIR(1));   
      set_field_back(inputField[0], COLOR_PAIR(1));
      set_field_fore(inputField[1], COLOR_PAIR(9));   
      set_field_back(inputField[1], COLOR_PAIR(9));
      set_field_fore(inputField[2], COLOR_PAIR(9));   
      set_field_back(inputField[2], COLOR_PAIR(9));
      set_field_fore(inputField[3], COLOR_PAIR(9));   
      set_field_back(inputField[3], COLOR_PAIR(9));
      set_field_fore(inputField[4], COLOR_PAIR(9));   
      set_field_back(inputField[4], COLOR_PAIR(9));
      set_field_fore(inputField[5], COLOR_PAIR(9));   
      set_field_back(inputField[5], COLOR_PAIR(9));
      set_field_fore(inputField[6], COLOR_PAIR(9));   
      set_field_back(inputField[6], COLOR_PAIR(9));   

      field_opts_off(inputField[0], O_ACTIVE);
      //set_field_type(inputField[0],TYPE_INTEGER,1,1,2);
      set_field_type(inputField[1],TYPE_INTEGER,0,1,99999999);
      set_field_type(inputField[2],TYPE_REGEXP,"^[A-Z ]+$");
      //set_field_type(inputField[3],TYPE_);
      //set_field_type(inputField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(inputField[5],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(inputField[6],TYPE_REGEXP,"^[A-Za-z0-9- &]+$");

      mainForm = new_form(inputField);
      scale_form(mainForm, &mrow, &mcol);   

      mainWin = newwin(LINES-15, COLS/3,LINES-(LINES-4),COLS/15);
      //subOneWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      //subTwoWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      updateWin = newwin(LINES*0.5, COLS/3,LINES-(LINES-4),COLS/2);

      //subOnePanel = new_panel(subOneWin);      
      //subTwoPanel = new_panel(subTwoWin);
      updatePanel = new_panel(updateWin);
      mainPanel = new_panel(mainWin);
      wbkgd(mainWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();
      
      keypad(mainWin, TRUE);
      //keypad(subOneWin, TRUE);
      //keypad(subTwoWin, TRUE);
      keypad(updateWin, TRUE);

      set_form_win(mainForm,mainWin);
      set_form_sub(mainForm, derwin(mainWin,mrow,mcol,2,2));      
      getmaxyx(mainWin, mrow, mcol);
      //getmaxyx(subOneWin, s1row, s1col);
      //getmaxyx(subTwoWin, s2row, s2col);
      getmaxyx(updateWin, urow, ucol);
      box(mainWin,0,0);
      //box(subOneWin,0,0);
      //box(subTwoWin,0,0);
      box(updateWin,0,0);
      wattron(mainWin,A_BOLD | COLOR_PAIR(1));     
      mvwprintw(mainWin,1,(mcol-lenOne)/2,titleOne);  
      wattroff(mainWin,A_BOLD | COLOR_PAIR(1));   

      if(mainWin == NULL || updateWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  getch();
	}

      post_form(mainForm);  
      wrefresh(mainWin);

      mvwprintw(mainWin, mrow-(mrow-6),mcol-(mcol-5), "Statement_ID:");
      mvwprintw(mainWin, mrow-(mrow-8),mcol-(mcol-5), "Date:");
      mvwprintw(mainWin, mrow-(mrow-10),mcol-(mcol-5), "Type:");
      mvwprintw(mainWin, mrow-(mrow-12),mcol-(mcol-5), "Description:");
      mvwprintw(mainWin, mrow-(mrow-18),mcol-(mcol-5), "Value:");
      mvwprintw(mainWin, mrow-(mrow-20),mcol-(mcol-5), "Account:");
      mvwprintw(mainWin, mrow-(mrow-22),mcol-(mcol-5), "Alias:");
      mvwprintw(mainWin, mrow-2,mcol-(mcol-5),"Press F1 when form complete (F9 for Update)");
      wmove(mainWin, mrow-(mrow-8),mcol-(mcol-30));     
      wrefresh(mainWin);
  
      while((ch = wgetch(mainWin)) != KEY_F(1))
	{
	  //hide_panel(subOnePanel);
	  //hide_panel(subTwoPanel);
	  hide_panel(updatePanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();	  
	  keyNavigate(ch, mainForm);
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(updateWin);
	      box(updateWin,0,0);
	      wattron(updateWin,A_BOLD | COLOR_PAIR(1));   
	      mvwprintw(updateWin,1,(ucol-lenFour)/2, titleFour);   
	      wattroff(updateWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(updateWin,1,1);
	      show_panel(updatePanel);
	      wbkgd(updateWin, COLOR_PAIR(1));          
	      update_panels();
	      doupdate();

	      res = PQexec(conn,"SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");	  
	      rows = PQntuples(res);

	      wrefresh(updateWin);
	      wattron(updateWin,A_BOLD | COLOR_PAIR(1));    
	      mvwprintw(updateWin, 4, 1, "ID    Provider_ID     Account Number");  //+3
	      wattroff(updateWin,A_BOLD | COLOR_PAIR(1));   
	  
	      while((p = wgetch(updateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      mvwprintw(updateWin,list,1,"%-5s %-15s %-15s", PQgetvalue(res,i,0),PQgetvalue(res,i,2),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(updateWin);
		      box(updateWin,0,0);  
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(updateWin);  
		      mvwprintw(updateWin,urow-15,1,"End of list");
		      box(updateWin,0,0);
		      wattron(updateWin,A_BOLD | COLOR_PAIR(1));  
	              mvwprintw(updateWin,1,(ucol-lenFour)/2, titleFour);  
	              wattroff(updateWin,A_BOLD | COLOR_PAIR(1));   
		      wmove(updateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(updateWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(updateWin,urow-14,1,"Select Option: ");  //urow-7
	      mvwscanw(updateWin,urow-14, ucol-45, "%d", &upID);
	      wattroff(updateWin,A_BOLD | COLOR_PAIR(1));           

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

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
		  /*set_field_buffer(inputField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(inputField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(inputField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(inputField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(inputField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(inputField[5],0,PQgetvalue(res,0,6));*/
		  strcpy(fv5, trimWS(field_buffer(inputField[5],0)));

		  mvwprintw(updateWin, urow-11,1,"Date From:");
		  mvwscanw(updateWin,urow-11, ucol-45, "%d", &df);
		  mvwprintw(updateWin, urow-10,1,"Date To:");
		  mvwscanw(updateWin,urow-10, ucol-45, "%d", &df);
		  cfUpdate = 1;
		  hide_panel(updatePanel);
		  update_panels();
		  doupdate();
		}
	      else
		{
		  wattron(updateWin,A_BOLD | COLOR_PAIR(1));       
		  mvwprintw(updateWin,urow-13,1,"Number invalid");
		  wrefresh(updateWin);
		  wattroff(updateWin,A_BOLD | COLOR_PAIR(1));      
		}
	      noecho();
	      PQclear(res);
	    } //F9	  
	}//while F1
      //hide_panel(subOnePanel);
      //hide_panel(subTwoPanel);
      hide_panel(updatePanel);
      update_panels();
      doupdate();

      /* code goes here for assign buffer value and validate prior to insert */
      form_driver(mainForm,REQ_VALIDATION);

      /*fv0 = atoi(field_buffer(inputField[0],0));
      fv1 = atoi(field_buffer(inputField[1],0));
      strcpy(fv2,trimWS(field_buffer(inputField[2],0)));
      fv3 = atoi(field_buffer(inputField[3],0));
      strcpy(fv4, trimWS(field_buffer(inputField[4],0)));
      strcpy(fv5, trimWS(field_buffer(inputField[5],0)));*/

      if ((form_driver(mainForm,REQ_VALIDATION) == E_OK) && (fv5 >= 0))
	{
	  echo();
	  wattron(mainWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(mainWin,mrow-10,mcol-64,"Save: y/n: ");
	  wattroff(mainWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(mainWin,mrow-9,mcol-64,"(d = delete record)");
	  wmove(mainWin,mrow-10,mcol-53);

	  while((cf = wgetch(mainWin)) != 'y')
	    {
	      wmove(mainWin,mrow-10,mcol-53);
	      if (cf == 'n')
		{
		  mvwprintw(mainWin,mrow-8,mcol-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  //recordDelete(upID);
		  wattron(mainWin,A_BOLD | A_BLINK);
		  mvwprintw(mainWin,mrow-8,mcol-64, "Record deleted");
		  wattroff(mainWin,A_BOLD | A_BLINK);
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  //recordUpdate(upID,fv0, fv1, fv2, fv3, fv4, fv5); // REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(mainWin,A_BOLD | A_BLINK);
		  mvwprintw(mainWin,mrow-8, mcol-64, "Data updated");
		  wattroff(mainWin,A_BOLD | A_BLINK);
		}
	      else		
		{
		  //recordInsert(fv0, fv1, fv2, fv3, fv4, fv5);
		  wattron(mainWin,A_BOLD | A_BLINK);
		  mvwprintw(mainWin,mrow-8, mcol-64, "Data saved");
		  wattroff(mainWin,A_BOLD | A_BLINK);
		}
	    }
	}  
      else
	{
	  mvwprintw(mainWin,mrow-8,mcol-64, "Data invalid");	
	}
      noecho();
  
      unpost_form(mainForm);
      free_form(mainForm);
      free_field(inputField[0]);
      free_field(inputField[1]);
      free_field(inputField[2]);
      free_field(inputField[3]);
      free_field(inputField[4]);
      free_field(inputField[5]);
      free_field(inputField[6]);

      cfUpdate = 0;

      mvwprintw(mainWin,mrow-6,mcol-64,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(mainWin)) != 'y')
	{
	  wmove(mainWin,mrow-6,mcol-27);  
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(mainPanel);
      update_panels();
      doupdate();
      delwin(mainWin);
    }

  PQfinish(conn); 
  endwin();
  
}
