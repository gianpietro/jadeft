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
  WINDOW *mainWin,*subOneWin, *subTwoWin, *updateWin;
  FORM *mainForm;
  FIELD *inputField[7];
  PANEL *mainPanel, *subOnePanel, *subTwoPanel, *updatePanel;
  int list = 2, i = 0, j = 0;
  //int RANGE = 5;
  char p;
  int ch, mrow, mcol, s1row, s1col, s2row, s2col, urow, ucol;
  int subOneID, subTwoID;
  char subOneIDstr[5];  
  char subTwoIDstr[5];  
  int rows; 
  int upID;
  int val, *params[1], length[1], formats[1];   //PQexecParams 
  int fv0, fv1, fv3, fv5;  // field values
  char fv2[30], fv4[30];  
  int cf; // confirm save to DB
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
      inputField[0] = new_field(1,1,4,28,0,0);    
      inputField[1] = new_field(1,5,6,28,0,0);    
      inputField[2] = new_field(1,30,8,28,0,0);   
      inputField[3] = new_field(1,9,10,28,0,0);   
      inputField[4] = new_field(1,30,12,28,0,0);  
      inputField[5] = new_field(1,5,14,28,0,0);   
      inputField[6] = NULL;

      
      set_field_fore(inputField[0], COLOR_PAIR(9));
      set_field_back(inputField[0], COLOR_PAIR(9));
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
      

      set_field_type(inputField[0],TYPE_INTEGER,1,1,2);
      set_field_type(inputField[1],TYPE_INTEGER,0,1,99999);
      set_field_type(inputField[2],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(inputField[3],TYPE_INTEGER,0,1,999999999);
      set_field_type(inputField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(inputField[5],TYPE_INTEGER,0,1,99999);

      mainForm = new_form(inputField);
      scale_form(mainForm, &mrow, &mcol);   

      mainWin = newwin(LINES-15, COLS/3,LINES-(LINES-4),COLS/15);
      subOneWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      subTwoWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      updateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      subOnePanel = new_panel(subOneWin);      
      subTwoPanel = new_panel(subTwoWin);
      updatePanel = new_panel(updateWin);
      mainPanel = new_panel(mainWin);
      wbkgd(mainWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();
      
      keypad(mainWin, TRUE);
      keypad(subOneWin, TRUE);
      keypad(subTwoWin, TRUE);
      keypad(updateWin, TRUE);

      set_form_win(mainForm,mainWin);
      set_form_sub(mainForm, derwin(mainWin,mrow,mcol,2,2));      
      getmaxyx(mainWin, mrow, mcol);
      getmaxyx(subOneWin, s1row, s1col);
      getmaxyx(subTwoWin, s2row, s2col);
      getmaxyx(updateWin, urow, ucol);
      box(mainWin,0,0);
      box(subOneWin,0,0);
      box(subTwoWin,0,0);
      box(updateWin,0,0);
      wattron(mainWin,A_BOLD | COLOR_PAIR(1));     
      mvwprintw(mainWin,1,(mcol-lenOne)/2,titleOne);  
      wattroff(mainWin,A_BOLD | COLOR_PAIR(1));   

      if(mainWin == NULL || subOneWin == NULL || subTwoWin == NULL || updateWin == NULL)
	{
	  endwin();
	  puts("Unable to create window");
	  getch();
	}

      post_form(mainForm);  
      wrefresh(mainWin);

      mvwprintw(mainWin, mrow-(mrow-6),mcol-(mcol-5), "Active Ind:");
      mvwprintw(mainWin, mrow-(mrow-8),mcol-(mcol-5), "Provider ID(F2):");
      mvwprintw(mainWin, mrow-(mrow-10),mcol-(mcol-5), "Account Number:");
      mvwprintw(mainWin, mrow-(mrow-12),mcol-(mcol-5), "Sort Code:");
      mvwprintw(mainWin, mrow-(mrow-14),mcol-(mcol-5), "Reference:");
      mvwprintw(mainWin, mrow-(mrow-16),mcol-(mcol-5), "Provider Type ID(F3):");
      mvwprintw(mainWin, mrow-2,mcol-(mcol-5),"Press F1 when form complete (F9 for Update)");
      wmove(mainWin, mrow-(mrow-6),mcol-(mcol-30));     
      wrefresh(mainWin);
  
      while((ch = wgetch(mainWin)) != KEY_F(1))
	{
	  hide_panel(subOnePanel);
	  hide_panel(subTwoPanel);
	  hide_panel(updatePanel);
	  show_panel(mainPanel);
	  update_panels();
	  doupdate();	  
	  keyNavigate(ch, mainForm);
	  if(ch == KEY_F(2))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(subOneWin);
	      box(subOneWin,0,0);
	      wattron(subOneWin,A_BOLD | COLOR_PAIR(1));    
	      mvwprintw(subOneWin,1,(s1col-lenTwo)/2, titleTwo); 
	      wattroff(subOneWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(subOneWin,1,1);	   
	      show_panel(subOnePanel);
	      wbkgd(subOneWin, COLOR_PAIR(1));          
	      update_panels();
	      doupdate();
	  
	      res = PQexec(conn,"SELECT * FROM provider WHERE active_ind = 1 ORDER BY provider_id");	  
	      rows = PQntuples(res);

	      wrefresh(subOneWin);
	      wattron(subOneWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(subOneWin, 4, 1, "ID    Name"); 
	      wattroff(subOneWin,A_BOLD | COLOR_PAIR(1)); 
	      while((p = wgetch(subOneWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(subOneWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(subOneWin);
		      box(subOneWin,0,0);   
		    }
		  list = 6;      	
		  if  (i == rows)
		    {
		      wclrtobot(subOneWin);  
		      mvwprintw(subOneWin,13,1,"End of list");
		      box(subOneWin,0,0);
		      wattron(subOneWin,A_BOLD | COLOR_PAIR(1));       
		      mvwprintw(subOneWin,1,(s1col-lenTwo)/2, titleTwo);
		      wattroff(subOneWin,A_BOLD | COLOR_PAIR(1)); 
		      wmove(subOneWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(subOneWin,A_BOLD | COLOR_PAIR(1));          
	      mvwprintw(subOneWin,s1row-7,1,"Select Option: ");
	      mvwscanw(subOneWin,s1row-7,s1col-45, "%5s", &subOneIDstr);
	      set_field_buffer(inputField[1],0, subOneIDstr);
              wattroff(subOneWin,A_BOLD | COLOR_PAIR(1));         
	      subOneID = atoi(field_buffer(inputField[1],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)subOneID);
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
		  set_field_buffer(inputField[1],0,PQgetvalue(res,0,0));
		  wrefresh(subOneWin);
		}
	      else
		{
		  set_field_buffer(inputField[1],0, "");
		  wattron(subOneWin,A_BOLD | COLOR_PAIR(1));   
		  mvwprintw(subOneWin,s1row-6,1,"Number invalid");
		  wattroff(subOneWin,A_BOLD | COLOR_PAIR(1));  
		  wrefresh(subOneWin);		
		}
	      noecho();
	      PQclear(res);
	    }
	  if(ch == KEY_F(3))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(subTwoWin);
	      box(subTwoWin,0,0);
	      wattron(subTwoWin,A_BOLD | COLOR_PAIR(1));   
	      mvwprintw(subTwoWin,1,(s1col-lenThree)/2, titleThree); 
	      wattroff(subTwoWin,A_BOLD | COLOR_PAIR(1));   
	      wmove(subTwoWin,1,1);
	      show_panel(subTwoPanel);
	      wbkgd(subTwoWin, COLOR_PAIR(1));         
	      update_panels();
	      doupdate();
	      wrefresh(subTwoWin);

	      res = PQexec(conn,"SELECT * FROM provider_type ORDER BY provider_type_id");
	      rows = PQntuples(res);

	      wrefresh(subTwoWin);
	      wattron(subTwoWin,A_BOLD | COLOR_PAIR(1));
	      mvwprintw(subTwoWin, 4, 1, "ID    Description"); 
	      wattroff(subTwoWin,A_BOLD | COLOR_PAIR(1));   

	      while((p = wgetch(subTwoWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {	 
		      mvwprintw(subTwoWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(subTwoWin);
		      box(subTwoWin,0,0);    
		    }
		  list = 6;      

		  if  (i == rows)
		    {
		      wclrtobot(subTwoWin);  
		      mvwprintw(subTwoWin,13,1,"End of list");
		      box(subTwoWin,0,0);		     
		      wattron(subTwoWin,A_BOLD | COLOR_PAIR(1)); 
		      mvwprintw(subTwoWin,1,(s1col-lenThree)/2, titleThree);   
		      wattroff(subTwoWin,A_BOLD | COLOR_PAIR(1));  
		      wmove(subTwoWin,10,1);
		      break;
		    }
		}
	      echo();
	      wattron(subTwoWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(subTwoWin,s2row-7,1,"Select Provider: ");
	      mvwscanw(subTwoWin,s2row-7,s2col-45, "%5s", &subTwoIDstr);
	      set_field_buffer(inputField[5],0, subTwoIDstr);
	      wattroff(subTwoWin,A_BOLD | COLOR_PAIR(1));          

	      subTwoID = atoi(field_buffer(inputField[5],0));
	      PQclear(res);

	      val = htonl((uint32_t)subTwoID);
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
		  set_field_buffer(inputField[5],0,PQgetvalue(res,0,0));
		  wrefresh(subTwoWin);
		}
	      else
		{
		  set_field_buffer(inputField[5],0,"");
		  wattron(subTwoWin,A_BOLD | COLOR_PAIR(1));    
		  mvwprintw(subTwoWin,s2row-6,1,"Number invalid");
		  wattroff(subTwoWin,A_BOLD | COLOR_PAIR(1));   
		  wrefresh(subTwoWin);		
		}
	      noecho();
	      PQclear(res);
	    }  // if (ch == f3)
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(updateWin);
	      box(updateWin,0,0);
	      wattron(updateWin,A_BOLD | COLOR_PAIR(1));   
	      mvwprintw(updateWin,1,(s1col-lenFour)/2, titleFour);   
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
		      mvwprintw(updateWin,13,1,"End of list");
		      box(updateWin,0,0);
		      wattron(updateWin,A_BOLD | COLOR_PAIR(1));  
	              mvwprintw(updateWin,1,(s1col-lenFour)/2, titleFour);  
	              wattroff(updateWin,A_BOLD | COLOR_PAIR(1));   
		      wmove(updateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(updateWin,A_BOLD | COLOR_PAIR(1));           
	      mvwprintw(updateWin,urow-7,1,"Select Option: ");
	      mvwscanw(updateWin,urow-7, ucol-45, "%d", &upID);
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
		  set_field_buffer(inputField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(inputField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(inputField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(inputField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(inputField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(inputField[5],0,PQgetvalue(res,0,6));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(updateWin,A_BOLD | COLOR_PAIR(1));       
		  mvwprintw(updateWin,urow-6,1,"Number invalid");
		  wrefresh(updateWin);
		  wattroff(updateWin,A_BOLD | COLOR_PAIR(1));      
		}
	      noecho();
	      PQclear(res);
	    } //F9	  
	}//while F1
      hide_panel(subOnePanel);
      hide_panel(subTwoPanel);
      hide_panel(updatePanel);
      update_panels();
      doupdate();

      /* code goes here for assign buffer value and validate prior to insert */
      form_driver(mainForm,REQ_VALIDATION);

      fv0 = atoi(field_buffer(inputField[0],0));
      fv1 = atoi(field_buffer(inputField[1],0));
      strcpy(fv2,trimWS(field_buffer(inputField[2],0)));
      fv3 = atoi(field_buffer(inputField[3],0));
      strcpy(fv4, trimWS(field_buffer(inputField[4],0)));
      fv5 = atoi(field_buffer(inputField[5],0));

      if ((form_driver(mainForm,REQ_VALIDATION) == E_OK) && (fv0 >= 1))
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
	  wmove(mainWin,mrow-6,mcol-27);  //CHECK COL POSITION OK
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
