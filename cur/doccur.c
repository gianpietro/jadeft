#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#include <arpa/inet.h>
#include "../inc/doccur.h"
#include "../inc/doclib.h"
#include "../inc/jadlib.h"


void documentTypeInsert()
{
  FIELD *doctypeField[2];
  FORM *doctypeForm;
  WINDOW *doctypeWin, *doctypeUpdateWin;
  PANEL *doctypePanel, *doctypeUpdatePanel;
  int newRec = 'y';
  int drow, dcol, urow, ucol;
  int list = 2, i = 0, j = 0;
  //int range = 5;
  char p;
  int ch;
  int rows;
  int val, upID, *params[1], length[1],  formats[1];
  char dtDesc[30];
  int cfUpdate = 0;
  int cf;
  char *titleOne = "Document Type Form";
  char *titleTwo = "Document Type";
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
      doctypeField[0] = new_field(1,30,6,22,0,0);  /* description */
      doctypeField[1] = NULL;

      set_field_fore(doctypeField[0], COLOR_PAIR(9));    /* SET_FIELD_FOREGROUND */
      set_field_back(doctypeField[0], COLOR_PAIR(9));    /* SET_FIELD_BACKGROUND */

      set_field_type(doctypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      doctypeForm = new_form(doctypeField);
      scale_form(doctypeForm, &drow, &dcol);

      doctypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/4);
      doctypeUpdateWin = newwin((LINES-10)/2, COLS/3,(LINES-10)/2+5, COLS/4);

      doctypePanel = new_panel(doctypeWin);
      doctypeUpdatePanel = new_panel(doctypeUpdateWin);
      wbkgd(doctypeWin, COLOR_PAIR(1));        /* MAIN_WINDOW_BACKGROUND_COLOR */    
      update_panels();
      doupdate();
      
      keypad(doctypeWin, TRUE);
      keypad(doctypeUpdateWin, TRUE);

      set_form_win(doctypeForm, doctypeWin);
      set_form_sub(doctypeForm, derwin(doctypeWin, drow, dcol, 2, 2));
      getmaxyx(doctypeWin,drow,dcol);
      getmaxyx(doctypeUpdateWin, urow, ucol);
      box(doctypeWin,0,0);
      box(doctypeUpdateWin,0,0);
      //waddstr(doctypeWin,"Document Type Form");
      //waddstr(doctypeUpdateWin,"Document Type");

      if (doctypeWin == NULL || doctypeUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}

      wattron(doctypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(doctypeWin,1,(dcol-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
      wattroff(doctypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */

      post_form(doctypeForm);
      wrefresh(doctypeWin);

      mvwprintw(doctypeWin,drow-(drow-8),dcol-(dcol-5), "Description:");
      mvwprintw(doctypeWin,drow-2,dcol-(dcol-5),"Press F1 when form complete (F9 for Update)");
      wmove(doctypeWin,drow-(drow-8),dcol-(dcol-24));
      //mvwprintw(doctypeWin,8,5,"rows %d cols %d", drow, dcol);
      //wrefresh(doctypeWin);

      while((ch = wgetch(doctypeWin)) != KEY_F(1))
	{
	  hide_panel(doctypeUpdatePanel);
	  show_panel(doctypePanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, doctypeForm);   
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(doctypeUpdateWin);
	      box(doctypeUpdateWin,0,0);
	      //waddstr(doctypeUpdateWin, "Document Type");
	      wattron(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(doctypeUpdateWin,1,(dcol-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(doctypeUpdateWin,1,1);
	      //wrefresh(doctypeUpdateWin);
	      show_panel(doctypeUpdatePanel);
	      wbkgd(doctypeUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM document_type ORDER BY type_id");	  
	      rows = PQntuples(res);

	      wrefresh(doctypeUpdateWin);
	      wattron(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(doctypeUpdateWin, 4, 1, "ID    Decscription");  //+3
	      wattroff(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS*/
	  
	      while((p = wgetch(doctypeUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(doctypeUpdateWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(doctypeUpdateWin);
		      box(doctypeUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(doctypeUpdateWin);  
		      mvwprintw(doctypeUpdateWin,13,1,"End of list");
		      box(doctypeUpdateWin,0,0);
		      //mvwprintw(doctypeUpdateWin,0,0, "Document Type");
		      wattron(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(doctypeUpdateWin,1,(dcol-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */ 
		      //mvwprintw(proUpdateWin,0,0, "Provider");
		      wattroff(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(doctypeUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(doctypeUpdateWin,drow-7,1,"Select Option: ");
	      mvwscanw(doctypeUpdateWin,drow-7,dcol-45, "%d", &upID);
	      wattroff(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM document_type WHERE type_id = $1 ORDER BY type_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(doctypeUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(doctypeUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  //wrefresh(doctypeUpdateWin);
		  set_field_buffer(doctypeField[0],0,PQgetvalue(res,0,1));
		  //set_field_buffer(doctypeField[1],0,PQgetvalue(res,0,2));
		  cfUpdate = 1;
		}
	      else
		{
		  wattron(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(doctypeUpdateWin,drow-6,1,"Number invalid");
		  wattroff(doctypeUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(doctypeUpdateWin);		
		  //wrefresh(PARENT_WIN);
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} // while F1
      hide_panel(doctypeUpdatePanel);
      update_panels();
      doupdate();

      form_driver(doctypeForm,REQ_VALIDATION);

      strcpy(dtDesc, field_buffer(doctypeField[0],0));

      if ((form_driver(doctypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*dtDesc)))
	{
	  strcpy(dtDesc, trimWS(dtDesc));
	  echo();
	  wattron(doctypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SAVE_YN */
	  mvwprintw(doctypeWin,drow-8,dcol-65,"Save y/n: ");
	  wattroff(doctypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTOFF_SAVE_YN */
	  mvwprintw(doctypeWin,drow-7,dcol-65,"(d = delete record)");	  
	  wmove(doctypeWin,drow-8,dcol-54);

	  while((cf = wgetch(doctypeWin)) != 'y')
	    {
	      wmove(doctypeWin,drow-8,dcol-54);
	      if (cf == 'n')
		{
		  mvwprintw(doctypeWin,drow-6,dcol-65, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{  
		  docTypeDelete(upID);
		  wattron(doctypeWin, A_BOLD | A_BLINK);             /* ATTON_DELETED */
		  mvwprintw(doctypeWin,drow-6,dcol-65, "Record deleted");
		  wattroff(doctypeWin, A_BOLD | A_BLINK);            /* ATTOFF_DELETED */
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{
		  docTypeUpdate(upID, dtDesc);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  wattron(doctypeWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID 
		  mvwprintw(doctypeWin,drow-6,dcol-65, "Data updated");
		  wattroff(doctypeWin, A_BOLD | A_BLINK);
		  //mvwprintw(doctypeWin,17,5, "cfUpdate %d,upID %d, dtDesc %s", cfUpdate,upID, dtDesc);  //DEBUG
		}
      	      else
		{
		  docTypeInsert(dtDesc);
		  wattron(doctypeWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		  mvwprintw(doctypeWin,drow-6,dcol-65, "Data saved");
		  wattroff(doctypeWin, A_BOLD | A_BLINK);      /* ATTON_SAVED */
		}
	    }
	}
      else
	{
	  mvwprintw(doctypeWin,drow-6,dcol-65, "Data invalid");	
	}
      noecho();

      unpost_form(doctypeForm);
      free_form(doctypeForm);
      free_field(doctypeField[0]);

      cfUpdate = 0;

      mvwprintw(doctypeWin,drow-4,dcol-65,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(doctypeWin)) != 'y')
	{
	  wmove(doctypeWin,drow-4,dcol-28);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(doctypePanel);
      update_panels();
      doupdate();
      delwin(doctypeWin);	
    } //while newRec
  PQfinish(conn);
  endwin(); 
}

void documentInsert()
{
  PANEL *docPanel, *proAcctPanel, *supAcctPanel, *supInvPanel, *docTypePanel, *docUpdatePanel;
  WINDOW *docWin, *proAcctWin, *supAcctWin, *supInvWin, *docTypeWin, *docUpdateWin;
  FORM *docForm;
  FIELD *docField[11];
  int newRec = 'y';
  int docrow, doccol, parow, pacol, sarow, sacol, invrow, invcol, trow, tcol, urow, ucol;
  int ch;
  int cf;
  int cfUpdate = 0;
  int i = 0, j = 0;
  int list = 6;
  int rows;
  char p;
  int docProAcctID, supAcctID, supInvID, docTypeID;
  char docProAcctIDstr[5], supAcctIDStr[5], supInvIDStr[5], docTypeIDStr[5];
  int val, *params[1], length[1], formats[1];
  char parentType[3][9] = {"PROVIDER", "SUPPLIER", "INVOICE"};
  char parentSelected[9];
  //char parentSelected[1][9];
  int docfParentID, docfOid, docfTypeID, docfStartDt, docfEndDt;
  char docfFileName[30], docfRef[50], docfTitle[100], docfDesc[150], docfCat[30];
  int fExist;
  int upID;
  char updateCatalog[30];                     /* variable to store catalog for update */
  char f[5];// = "/tmp/";
  char e[strlen(docfFileName)];
  char fn[30];                                /* variable to store filename for update */
  char *titleOne = "Document Import Form";
  char *titleTwo = "Provider Account";
  char *titleThree = "Supplier Account";
  char *titleFour = "Invoice";
  char *titleFive = "Document Type";
  char *titleNine = "Document";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  int lenFour = strlen(titleFour);
  int lenFive = strlen(titleFive);
  int lenNine = strlen(titleNine);
  int fldColor;
  
  PGconn *conn = fdbcon();
  PGresult *res;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(9,COLOR_WHITE,COLOR_BLACK);  

  while ( newRec == 'y')
    {
      docField[0] = new_field(1,5,2,28,0,0);    /* parent_id   */
      docField[1] = new_field(1,30,4,28,0,0);   /* file_name   */
      docField[2] = new_field(1,5,6,28,0,0);    /* oid_value   */
      docField[3] = new_field(1,5,8,28,0,0);    /* type_id     */
      docField[4] = new_field(2,25,10,28,0,0);  /* reference   */
      docField[5] = new_field(4,25,13,28,0,0);  /* title       */
      docField[6] = new_field(5,30,18,28,0,0);  /* description */
      docField[7] = new_field(1,8,24,28,0,0);   /* start_date  */
      docField[8] = new_field(1,8,26,28,0,0);   /* end_date    */
      docField[9] = new_field(1,30,28,28,0,0);  /* catalog     */
      docField[10] = NULL;

      for (fldColor = 0; fldColor < 9; fldColor++)
	{
	  set_field_fore(docField[fldColor], COLOR_PAIR(9));
	  set_field_back(docField[fldColor], COLOR_PAIR(9));
	}
      
      set_field_type(docField[0],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[1],TYPE_REGEXP,"^[A-Za-z0-9 -./]+$");
      set_field_type(docField[2],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[3],TYPE_INTEGER,0,1,99999);
      set_field_type(docField[4],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[5],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[6],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
      set_field_type(docField[7],TYPE_INTEGER,0,1,99999999);
      set_field_type(docField[8],TYPE_INTEGER,0,1,99999999);
      set_field_type(docField[9],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      field_opts_off(docField[2], O_ACTIVE);   /* field is inactive cursor skips over */
      field_opts_off(docField[9], O_ACTIVE);
      
      docForm = new_form(docField);
      scale_form(docForm, &docrow, &doccol);

      docWin = newwin(LINES-10, COLS/3,LINES-(LINES-4),COLS/15);
      proAcctWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      supAcctWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      supInvWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      docTypeWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);
      docUpdateWin = newwin((LINES-10)/2, COLS/3,LINES-(LINES-4),COLS/2);

      docPanel = new_panel(docWin);
      proAcctPanel = new_panel(proAcctWin);
      supAcctPanel = new_panel(supAcctWin);
      supInvPanel = new_panel(supInvWin);
      docTypePanel = new_panel(docTypeWin);
      docUpdatePanel = new_panel(docUpdateWin);
      wbkgd(docWin, COLOR_PAIR(1));     
      update_panels();
      doupdate();

      keypad(docWin, TRUE);
      keypad(proAcctWin, TRUE);
      keypad(supAcctWin, TRUE);
      keypad(supInvWin, TRUE);
      keypad(docTypeWin, TRUE);
      keypad(docUpdateWin, TRUE);

      set_form_win(docForm, docWin);
      set_form_sub(docForm, derwin(docWin, docrow, doccol, 2, 2));
      getmaxyx(docWin, docrow, doccol);
      getmaxyx(proAcctWin, parow, pacol);
      getmaxyx(supAcctWin, sarow, sacol);
      getmaxyx(supInvWin, invrow, invcol);
      getmaxyx(docTypeWin, trow, tcol);
      getmaxyx(docUpdateWin, urow, ucol);
      box(docWin,0,0);
      box(proAcctWin,0,0);
      box(supAcctWin,0,0);
      box(supInvWin,0,0);
      box(docTypeWin,0,0);
      box(docUpdateWin,0,0);
      wattron(docWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(docWin,1,(doccol-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */      
      wattroff(docWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      /* waddstr(docWin, "Document Import Form");
      waddstr(proAcctWin, "Provider Account");
      waddstr(supAcctWin, "Suppler Account");
      waddstr(supInvWin, "Invoice");
      waddstr(docTypeWin, "Document Type");
      waddstr(docUpdateWin, "Document");*/      

      if(docWin == NULL || proAcctWin == NULL  || supAcctWin == NULL || supInvWin == NULL || docTypeWin == NULL  || docUpdateWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();
	}
      
      post_form(docForm);
      wrefresh(docWin);

      wattron(docWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(docWin, docrow-(docrow-4),doccol-(doccol-5),  "ParentID*:");
      wattroff(docWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      mvwprintw(docWin, docrow-(docrow-6),doccol-(doccol-5),  "File Name:");
      mvwprintw(docWin, docrow-(docrow-8),doccol-(doccol-5),  "OID:");
      mvwprintw(docWin, docrow-(docrow-10),doccol-(doccol-5),  "Type(F5):");
      mvwprintw(docWin, docrow-(docrow-12),doccol-(doccol-5), "Reference:");
      mvwprintw(docWin, docrow-(docrow-15),doccol-(doccol-5), "Title:");
      mvwprintw(docWin, docrow-(docrow-20),doccol-(doccol-5), "Description:");
      mvwprintw(docWin, docrow-(docrow-26),doccol-(doccol-5), "Start Date:");
      mvwprintw(docWin, docrow-(docrow-28),doccol-(doccol-5), "End Date:");
      mvwprintw(docWin, docrow-(docrow-30),doccol-(doccol-5), "Catalog:");
      wattron(docWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_MAIN_WIN_TITLE */
      mvwprintw(docWin, docrow-4, doccol-(doccol-5), "*ParentID: F2-Provider F3-Supplier F4-Invoice");
      wattroff(docWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_MAIN_WIN_TITLE */
      mvwprintw(docWin, docrow-2, doccol-(doccol-5), "Press F1 when form complete (F9 for Update)");
      wmove(docWin,docrow-(docrow-4),doccol-(doccol-30));
      wrefresh(docWin);     

      strcpy(f,"");
      
      while((ch = wgetch(docWin)) != KEY_F(1))
	{
	  hide_panel(proAcctPanel);
	  hide_panel(supAcctPanel);
	  hide_panel(supInvPanel);
	  hide_panel(docTypePanel);
	  hide_panel(docUpdatePanel);
	  show_panel(docPanel);
	  update_panels();
	  doupdate();
	  keyNavigate(ch, docForm);
	  if(ch == KEY_F(2))
	    {
	      strcpy(parentSelected, parentType[0]);	     /* assign PROVIDER to parentSelected to be used for catalog field */
	      i = j = rows = 0;
	      list = 6;
	      wclear(proAcctWin);
	      box(proAcctWin,0,0);
	      //waddstr(proAcctWin, "Provider Account");
	      wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(proAcctWin,1,(pacol-lenTwo)/2, titleTwo);     /*SET_SUB_WIM_TITLE */
	      wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(proAcctWin,1,1);
	      show_panel(proAcctPanel);
	      wbkgd(proAcctWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      res = PQexec(conn, "SELECT * FROM provider_account WHERE active_ind = 1 ORDER BY provider_acct_id");
	      rows = PQntuples(res);

	      wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(proAcctWin, 4, 1, "ID    Provider Account");  //+3
	      wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */

	      while((p = wgetch(proAcctWin)) == '\n')
		{
		  if(j + RANGE < rows)
		    j = j + RANGE;
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      mvwprintw(proAcctWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,3));
		      list++;
		      wclrtoeol(proAcctWin);
		      box(proAcctWin,0,0);
		    }
		  list = 6;
		  if (i == rows)
		    {
		      wclrtobot(proAcctWin);  
		      mvwprintw(proAcctWin,13,1,"End of list");
		      box(proAcctWin,0,0);
		      //mvwprintw(proAcctWin,0,0, "Provider Account");
		      wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(proAcctWin,1,(pacol-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(proAcctWin,10,1);
		      break;		      
		    }		  
		}
	      echo();
	      wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(proAcctWin,parow-7,1,"Select Option: ");
	      mvwscanw(proAcctWin,parow-7, pacol-45, "%5s", &docProAcctIDstr);
	      wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      set_field_buffer(docField[0],0, docProAcctIDstr);

	      /* CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES */
	      docProAcctID = atoi(field_buffer(docField[0],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)docProAcctID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      /* ASSIGN THE REQUIRED SELECT STATEMENT */
	      res = PQexecParams(conn, "SELECT * FROM provider_account WHERE active_ind = 1 and provider_acct_id = $1 ORDER BY provider_acct_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      //mvwprintw(supWin,11,1,"rows %d", rows);
	      if (rows == 1)
		{
		  set_field_buffer(docField[0],0,PQgetvalue(res,0,0));
		  //mvwprintw(proAcctWin,13,1, "no or rows %d ",rows);
		  /* CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED */
		  //mvwprintw(proAcctWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,3));
		  wrefresh(proAcctWin);
		}
	      else
		{
		  set_field_buffer(docField[0],0,"");
		  wattron(proAcctWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(proAcctWin,parow-6,1,"Number invalid");
		  wattroff(proAcctWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  // strcpy(docfCat, parentSelected[0]);     //DEBUG
		  //mvwprintw(proAcctWin,3,1,"parent type %s",docfCat); // DEBUG CODE
		  wrefresh(proAcctWin);		
		}
	      noecho();
	      PQclear(res);	      
	    } //F2
	  if(ch == KEY_F(3))
	    {
	      strcpy(parentSelected, parentType[1]);
	      i = j = rows = 0;
	      list = 6;
	      wclear(supAcctWin);
	      box(supAcctWin,0,0);
	      //waddstr(supAcctWin, "Supplier Account");
	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supAcctWin,1,(sacol-lenThree)/2, titleThree);     /*SET_SUB_WIM_TITLE */
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supAcctWin,1,1);
	      //wrefresh(supAcctWin);
	      show_panel(supAcctPanel);
	      wbkgd(supAcctWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_account WHERE active_ind = 1 ORDER BY supplier_acct_id");	  
	      rows = PQntuples(res);
	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supAcctWin, 4, 1, "ID    Supplier Account");  //+3
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */

	      //wrefresh(supAcctWin);
	  
	      while((p = wgetch(supAcctWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(supAcctWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2));
		      list++;
		      wclrtoeol(supAcctWin);
		      box(supAcctWin,0,0);
		    }
		  list = 6;      
		  //wclrtoeol(supAcctWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supAcctWin);  
		      mvwprintw(supAcctWin,13,1,"End of list");
		      box(supAcctWin,0,0);
		      //mvwprintw(supAcctWin,0,0, "Supplier Account");
		      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supAcctWin,1,(sacol-lenTwo)/2, titleTwo);    /* SET_SUB_WIN_TITLE */
		      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supAcctWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */
	      mvwprintw(supAcctWin,sarow-7,1,"Select Option: ");
	      mvwscanw(supAcctWin,sarow-7,sacol-45, "%5s", &supAcctIDStr);
	      wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      set_field_buffer(docField[0],0, supAcctIDStr);

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      supAcctID = atoi(field_buffer(docField[0],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supAcctID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM supplier_account WHERE active_ind = 1 AND supplier_acct_id = $1 ORDER BY supplier_acct_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(docField[0],0,PQgetvalue(res,0,0));
		  //mvwprintw(supAcctWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(supAcctWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supAcctWin);
		}
	      else
		{
		  set_field_buffer(docField[0],0,"");
		  wattron(supAcctWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supAcctWin,sarow-6,1,"Number invalid");
		  wattroff(supAcctWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(supAcctWin);		
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F3
	  if(ch == KEY_F(4))
	    {
	      strcpy(parentSelected, parentType[2]);
	      i = j = rows = 0;
	      list = 6;
	      wclear(supInvWin);
	      box(supInvWin,0,0);
	      //waddstr(supInvWin, "Invoice");
	      wattron(supInvWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(supInvWin,1,(invcol-lenFour)/2, titleFour);     /*SET_SUB_WIM_TITLE */
	      wattroff(supInvWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(supInvWin,1,1);
	      //wrefresh(supInvWin);
	      show_panel(supInvPanel);
	      wbkgd(supInvWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM supplier_invoice ORDER BY supplier_invoice_id");	  
	      rows = PQntuples(res);

	      //wrefresh(supInvWin);
	      wattron(supInvWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(supInvWin, 4, 1, "ID    Invoice number");  //+3
	      wattroff(supInvWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(supInvWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(supInvWin,list,1,"%-5s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(supInvWin);
		      box(supInvWin,0,0);     /* REAPPLY_BOX */
		    }
		  list = 6;      
		  //wclrtoeol(supInvWin);  
		  if  (i == rows)
		    {
		      wclrtobot(supInvWin);  
		      mvwprintw(supInvWin,13,1,"End of list");
		      box(supInvWin,0,0);
		      //mvwprintw(supInvWin,0,0, "Invoice");
		      wattron(supInvWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(supInvWin,1,(invcol-lenFour)/2, titleFour);    /* SET_SUB_WIN_TITLE */
		      wattroff(supInvWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(supInvWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(supInvWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(supInvWin,invrow-7,1,"Select Option: ");
	      mvwscanw(supInvWin,invrow-7,invcol-45, "%5s", &supInvIDStr);
	      set_field_buffer(docField[0],0, supInvIDStr);
	      wattroff(supInvWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      supInvID = atoi(field_buffer(docField[0],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)supInvID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM supplier_invoice WHERE supplier_invoice_id = $1 ORDER BY supplier_invoice_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(docField[0],0,PQgetvalue(res,0,0));
		  //mvwprintw(supInvWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(supInvWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  wrefresh(supInvWin);
		}
	      else
		{
		  set_field_buffer(docField[4],0,"");
		  wattron(supInvWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(supInvWin,invrow-6,1,"Number invalid");
		  wattroff(supInvWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  
		  wrefresh(supInvWin);		
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F4
	  if(ch == KEY_F(5))
	    {
	      i = j = rows = 0;
	      list = 6;
	      wclear(docTypeWin);
	      box(docTypeWin,0,0);
	      //waddstr(docTypeWin, "Document Type");
	      wattron(docTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(docTypeWin,1,(tcol-lenFive)/2, titleFive);     /*SET_SUB_WIM_TITLE */
	      wattroff(docTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(docTypeWin,1,1);
	      //wrefresh(docTypeWin);
	      show_panel(docTypePanel);
	      wbkgd(docTypeWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();	

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM document_type ORDER BY type_id");	  
	      rows = PQntuples(res);

	      //wrefresh(docTypeWin);
	      wattron(docTypeWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(docTypeWin, 4, 1, "ID    Description");  //+3
	      wattroff(docTypeWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */	  
	      while((p = wgetch(docTypeWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(docTypeWin,list,1,"%-5s %-25s ", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
		      list++;
		      wclrtoeol(docTypeWin);
		      box(docTypeWin,0,0);
		    }
		  list = 6;      
		  //wclrtoeol(docTypeWin);  
		  if  (i == rows)
		    {
		      wclrtobot(docTypeWin);  
		      mvwprintw(docTypeWin,13,1,"End of list");
		      box(docTypeWin,0,0);
		      //mvwprintw(docTypeWin,0,0, "Document Type");
		      wattron(docTypeWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(docTypeWin,1,(tcol-lenFive)/2, titleFive);    /* SET_SUB_WIN_TITLE */
		      wattroff(docTypeWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(docTypeWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(docTypeWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(docTypeWin,trow-7,1,"Select Option: ");
	      mvwscanw(docTypeWin,trow-7,tcol-45, "%5s", &docTypeIDStr);
	      wattroff(docTypeWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */
	      set_field_buffer(docField[3],0, docTypeIDStr);

	      // CODE TO ASSIGN VARIABLES TO FIELD_BUFFER VALUES 
	      docTypeID = atoi(field_buffer(docField[3],0));
	      PQclear(res);
	  
	      val = htonl((uint32_t)docTypeID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM document_type WHERE type_id = $1 ORDER BY type_id;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  set_field_buffer(docField[3],0,PQgetvalue(res,0,0));
		  //mvwprintw(docTypeWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(docTypeWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,1));
		  wrefresh(docTypeWin);
		}
	      else
		{
		  set_field_buffer(docField[3],0,"");
		  wattron(docTypeWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(docTypeWin,trow-6,1,"Number invalid");
		  wattroff(docTypeWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */
		  wrefresh(docTypeWin);		
		  //wrefresh(MAIN_WIN);
		}
	      noecho();
	      PQclear(res);
	    } // F5
	  if(ch == KEY_F(9))
	    {
	      i = j = rows = 0, cfUpdate = 0;
	      list = 6;
	      wclear(docUpdateWin);
	      box(docUpdateWin,0,0);
	      //waddstr(docUpdateWin, "Document");
	      wattron(docUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTON_SUB_WIN */
	      mvwprintw(docUpdateWin,1,(ucol-lenNine)/2, titleNine);     /*SET_SUB_WIM_TITLE */
	      wattroff(docUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SUB_WIN */
	      wmove(docUpdateWin,1,1);
	      //wrefresh(docUpdateWin);
	      show_panel(docUpdatePanel);
	      wbkgd(docUpdateWin, COLOR_PAIR(1));           /* SUB_WIN_BACKGROUND_COLOR */
	      update_panels();
	      doupdate();

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexec(conn,"SELECT * FROM documents ORDER BY document_id");	  
	      rows = PQntuples(res);

	      wrefresh(docUpdateWin);
	      wattron(docUpdateWin,A_BOLD | COLOR_PAIR(1));     /* ATTON_SEARCH_ITEM_HEADERS */
	      mvwprintw(docUpdateWin, 4, 1, "ID    File Name       Title");  //+3
	      wattroff(docUpdateWin,A_BOLD | COLOR_PAIR(1));    /* ATTOFF_SEARCH_ITEM_HEADERS */
	  
	      while((p = wgetch(docUpdateWin)) == '\n')
		{
		  if ( j + RANGE < rows)
		    j = j + RANGE;	
		  else
		    j = j + (rows - j);
		  for (i; i < j; i++)
		    {
		      // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		      mvwprintw(docUpdateWin,list,1,"%-5s %-15s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,2),PQgetvalue(res,i,6));
		      list++;
		      wclrtoeol(docUpdateWin);
		      box(docUpdateWin,0,0);
		    }
		  list = 6;      
		  if  (i == rows)
		    {
		      wclrtobot(docUpdateWin);  
		      mvwprintw(docUpdateWin,13,1,"End of list");
		      box(docUpdateWin,0,0);
		      //mvwprintw(docUpdateWin,0,0, "Document");
		      wattron(docUpdateWin,A_BOLD | COLOR_PAIR(1));        /* ATTON_SUB_WIN_TITLE */
		      mvwprintw(docUpdateWin,1,(ucol-lenNine)/2, titleNine);    /* SET_SUB_WIN_TITLE */
		      wattroff(docUpdateWin,A_BOLD | COLOR_PAIR(1));   /* ATTOFF_SUB_WIN_TITLE */
		      wmove(docUpdateWin,10,1);
		      break;
		    }
		}	  
	      echo();
	      wattron(docUpdateWin,A_BOLD | COLOR_PAIR(1));             /* ATTON_SELECT_OPTION */		
	      mvwprintw(docUpdateWin,urow-7,1,"Select Option: ");
	      mvwscanw(docUpdateWin,urow-7,ucol-45, "%d", &upID);
	      wattroff(docUpdateWin,A_BOLD | COLOR_PAIR(1));           /* ATTOFF_SELECT_OPTION */

	      PQclear(res);
	  
	      val = htonl((uint32_t)upID);
	      params[0] = (int *)&val;
	      length[0] = sizeof(val);
	      formats[0] = 1;

	      // ASSIGN THE REQUIRED SELECT STATEMENT 
	      res = PQexecParams(conn, "SELECT * FROM documents WHERE document_id = $1;"
				 ,1
				 ,NULL
				 ,(const char *const *)params
				 ,length
				 ,formats
				 ,0);
	  
	      rows = PQntuples(res);
	      if (rows == 1)
		{
		  //mvwprintw(docUpdateWin,13,1, "no or rows %d ",rows);
		  // CHANGE NUMBER OF PQgetvalue RETURN ITEMS AS REQUIRED 
		  //mvwprintw(docUpdateWin,12,1,"Value selected %s %s", PQgetvalue(res,0,0), PQgetvalue(res,0,2));
		  //wrefresh(docUpdateWin);
		  set_field_buffer(docField[0],0,PQgetvalue(res,0,1));
		  set_field_buffer(docField[1],0,PQgetvalue(res,0,2));
		  set_field_buffer(docField[2],0,PQgetvalue(res,0,3));
		  set_field_buffer(docField[3],0,PQgetvalue(res,0,4));
		  set_field_buffer(docField[4],0,PQgetvalue(res,0,5));
		  set_field_buffer(docField[5],0,PQgetvalue(res,0,6));
		  set_field_buffer(docField[6],0,PQgetvalue(res,0,7));
		  set_field_buffer(docField[7],0,PQgetvalue(res,0,8));
		  set_field_buffer(docField[8],0,PQgetvalue(res,0,9));
		  set_field_buffer(docField[9],0,PQgetvalue(res,0,10));
		  cfUpdate = 1;
		  //wrefresh(docUpdateWin);
		}
	      else
		{
		  wattron(docUpdateWin,A_BOLD | COLOR_PAIR(1));            /* ATTON_NUMBER_INVALID */
		  mvwprintw(docUpdateWin,urow-6,1,"Number invalid");
		  wattroff(docUpdateWin,A_BOLD | COLOR_PAIR(1));          /* ATTOFF_NUMBER_INVALID */                   
		  wrefresh(docUpdateWin);		
		  //wrefresh(PARENT_WIN);
		}
	      noecho();
	      PQclear(res);
	    } //F9	 	   
	} //while not F1
      hide_panel(proAcctPanel);
      hide_panel(supAcctPanel);
      hide_panel(supInvPanel);
      hide_panel(docTypePanel);
      hide_panel(docUpdatePanel);
      update_panels();
      doupdate();

      form_driver(docForm, REQ_VALIDATION);

      docfParentID = atoi(field_buffer(docField[0],0));
      strcpy(docfFileName, trimWS(field_buffer(docField[1],0)));
      //docfOid = atoi(field_buffer(docField[2],0));
      docfTypeID = atoi(field_buffer(docField[3],0));
      strcpy(docfRef, trimWS(field_buffer(docField[4],0)));
      strcpy(docfTitle, trimWS(field_buffer(docField[5],0)));
      strcpy(docfDesc, trimWS(field_buffer(docField[6],0)));
      docfStartDt = atoi(field_buffer(docField[7],0));
      docfEndDt = atoi(field_buffer(docField[8],0));
      strcpy(docfCat, parentSelected);
      //set_field_buffer(docField[9],0,docfCat);

      if(cfUpdate == 1)
	{
	  set_field_buffer(docField[9],0,docfCat);
	  docfOid = atoi(field_buffer(docField[2],0));	  
	  strcpy(updateCatalog,field_buffer(docField[9],0));
	  strcpy(docfCat,updateCatalog);
	}
      else
	{
	  strcpy(docfCat, parentSelected);
	  set_field_buffer(docField[9],0,docfCat);
	}
			 
      if((form_driver(docForm,REQ_VALIDATION) == E_OK) && docfParentID >=1 && docfTypeID >=1)
	{
	  /* check to see if the file name entered exists */
	  if (cfUpdate == 0)
	    {
	      strcpy(f,"/tmp/");
	      strcpy(e,docfFileName);
	      strcat(f,e);
	      fExist = checkFileExists(f);
	      if (fExist == 2)
		mvwprintw(docWin,docrow-8,doccol-64, "Error no file %s",f);
	    }	      
	  else
	    {
	      strcpy(fn, trimWS(field_buffer(docField[1],0)));
	      strcpy(e,fn);
	    }  
	  	    
	  echo();
	  wattron(docWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(docWin,docrow-10,doccol-64, "Save y/n: ");
	  wattroff(docWin,A_BOLD | COLOR_PAIR(1));
	  mvwprintw(docWin,docrow-9,doccol-64, "(d = delete record)");
	  wmove(docWin,docrow-10,doccol-53);

	  while((cf = wgetch(docWin)) != 'y')
	    {
	      wmove(docWin,docrow-10,doccol-53);
	      if (cf == 'n')
		{
		  mvwprintw(docWin,docrow-8,doccol-64, "Data not saved");
		  break;
		}
	      if (cf == 'd')
		{
		  wattron(docWin,A_BOLD | A_BLINK);
		  documentImportDelete(upID, docfOid);
		  mvwprintw(docWin,docrow-8,doccol-64,"Record deleted");
		  wattroff(docWin,A_BOLD | A_BLINK);
		  break;
		}	      
	    }
	  if (cf == 'y')
	    {
	      if (cfUpdate == 1)
		{		  
		  /* mvwprintw(docWin, 3,65,  "%d", docfParentID);
		  mvwprintw(docWin, 5,65,  "%s", e);
		  mvwprintw(docWin, 7,65,  "%d", docfOid);
		  mvwprintw(docWin, 9,65,  "%d", docfTypeID);
		  mvwprintw(docWin, 11,65, "%s", docfRef);
		  mvwprintw(docWin, 14,65, "%s", docfTitle);
		  mvwprintw(docWin, 19,65, "%s", docfDesc);
		  mvwprintw(docWin, 25,65, "%d", docfStartDt);
		  mvwprintw(docWin, 27,65, "%d", docfEndDt);
		  mvwprintw(docWin, 29,65, "%s", docfCat); */
		  docImportUpdate(upID, docfParentID, e, docfOid, docfTypeID, docfRef, docfTitle,docfDesc, docfStartDt, docfEndDt,docfCat);  //REPLACE WITH NAME AND PARAMENTS OF FUNCTION
		  //THE UPDATE FUNCTION WILL HAVE SAME PARAMETERS AS INSERT FUNCTION PLUS upID
		  wattron(docWin,A_BOLD | A_BLINK);
		  mvwprintw(docWin,docrow-8,doccol-64, "Data updated");
		  wattroff(docWin,A_BOLD | A_BLINK);
		  //mvwprintw(docWin,35,5, "cfUpdate %d,upID %d, dtDesc %s", cfUpdate,upID, dtDesc);  //DEBUG
		}
      	      else if (fExist != 2)
		{
		  documentImport(docfParentID, f, e, docfTypeID, docfRef, docfTitle, docfDesc, docfStartDt, docfEndDt, parentSelected);
		  wattron(docWin,A_BOLD | A_BLINK);
		  mvwprintw(docWin,docrow-8,doccol-64, "Data saved");
		  wattroff(docWin,A_BOLD | A_BLINK);
		}
	    }
	}
      else
	{
	  mvwprintw(docWin,docrow-8,doccol-64, "Data invalid");
	}
      noecho();

      unpost_form(docForm);
      free_form(docForm);
      free_field(docField[0]);
      free_field(docField[1]);
      free_field(docField[2]);
      free_field(docField[3]);
      free_field(docField[4]);
      free_field(docField[5]);
      free_field(docField[6]);
      free_field(docField[7]);
      free_field(docField[8]);
      free_field(docField[9]);

      cfUpdate = 0;

      mvwprintw(docWin,docrow-6,doccol-64, "Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(docWin)) != 'y')
	{
	  wmove(docWin,docrow-6,doccol-27);
	  if(newRec == 'n')
	    break;
	}
      noecho();
      hide_panel(docPanel);
      update_panels();
      doupdate();
      delwin(docWin);      
    } //while newRec == 1

  PQfinish(conn);
  endwin();

} //end

/* Import the document and obtain the OID. 
   Update the document table with all the required fields
   including OID and catalog fields which are not entered by the user */
void documentImport(int dParentID, char f[], char e[], int dTypeID, char dRef[], char dTitle[],char dDesc[], int dStartDt, int dEndDt, char dCatalog[])
{
  Oid objImportID;
  
  PGconn *conn = fdbcon();
  PGresult *res;

/*File which is located in tmp directory is imported
  into pg_largeobject table and Oid number returned.*/
  res = PQexec(conn, "BEGIN");
  PQclear(res);
  objImportID = lo_import(conn, f);  
  res = PQexec(conn, "END");
  PQclear(res);

  if (objImportID > 0)
    {
      docImportInsert(dParentID, e, objImportID, dTypeID, dRef, dTitle, dDesc, dStartDt, dEndDt, dCatalog);
    }
  
  PQfinish(conn);
}

void documentImportDelete(int upID, int objImportID)
{
  PGconn *conn = fdbcon();
  PGresult *res;

  /*This will delete the file from the pg_largeobject and
    pg_largeobject_metadata tables. The binary document
    will no longer be available. The entry in the
    documents table then needs to be deleted. */
  res = PQexec(conn, "BEGIN");
  PQclear(res);
  res = PQexec(conn, "SELECT");
  lo_unlink(conn, objImportID);
  res = PQexec(conn, "END");
  PQclear(res);

  /* Delete corresponding row from documents table */
  docImportDelete(upID);      
  
  PQfinish(conn);  
}

