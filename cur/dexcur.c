#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <form.h>
#include <panel.h>
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#include <arpa/inet.h>
#include "../inc/dexcur.h"
#include "../inc/jadlib.h"


void selectType()
{
  WINDOW *selectWin, *selectProWin, *docWin;
  PANEL *selectPanel, *selectProPanel, *docPanel;
  int row, col, prow, pcol, drow, dcol;
  int ch = 0;
  int selectOption = 0;
  const char *titleOne = "Document Export Select Source";
  const char *titleTwo = "Provider Account List";
  const char *titleThree = "Document For Provider Account";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  char sDate[9];
  char startDate[9];
  int ckdate = 0;
  int list = 2, i = 0, j = 0, rRow = 0;
  char p;  
  int proID = 0;
  int val, *params[1], length[1], formats[1];
  char proAcctID[5],proName[30], providerID[5], proAcctNo[30];
  int cont;
  char *paramsDoc[2];
  int docExportID;
  char docFileName[30];
  int oidValue;
  char *expFName;
  int ConfirmExport = 'n';
  
  PGconn *conn =  fdbcon();
  PGresult *res;  

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(3,COLOR_WHITE,COLOR_GREEN);
  init_pair(4,COLOR_WHITE,COLOR_MAGENTA);
  init_pair(8,COLOR_BLACK,COLOR_WHITE);
  //init_pair(4,COLOR_WHITE,COLOR_BLUE);
  

  selectWin = newwin(LINES*0.4, COLS*0.4,LINES*0.07,COLS/4);
  selectProWin = newwin(LINES*0.5, COLS*0.5,LINES*0.07,COLS/5);
  docWin = newwin(LINES*0.4, COLS*0.5, LINES/2, COLS/5);
  selectPanel = new_panel(selectWin);
  selectProPanel = new_panel(selectProWin);
  docPanel = new_panel(docWin);
  
  wbkgd(selectWin,COLOR_PAIR(3));  //COLORCHG
  
  update_panels();
  doupdate();

  keypad(selectWin, TRUE);
  keypad(selectProWin, TRUE);
  keypad(docWin, TRUE);
  
  getmaxyx(selectWin, row, col);
  getmaxyx(selectProWin, prow, pcol);
  getmaxyx(docWin, drow, dcol);
  box(selectWin,0,0);
  box(selectProWin,0,0);
  box(docWin,0,0);

  if(selectWin == NULL || selectProWin == NULL || docWin == NULL)
    {
      endwin();
      puts("Unable to create window");
      getch();
    }

  wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG
  mvwprintw(selectWin, 1, (col-lenOne)/2, titleOne);
  // wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  //COLORCHG

  mvwprintw(selectWin,3,1, "1 - Provider");
  mvwprintw(selectWin,4,1, "2 - Supplier\n");
  mvwprintw(selectWin,6,1, "Enter option:");

  echo();
  mvwscanw(selectWin,6,18,"%d",&selectOption);
  noecho();
  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  //COLORCHG new line
  if (selectOption == 1)
    {
      hide_panel(selectPanel);
      hide_panel(docPanel);
      wbkgd(selectProWin,COLOR_PAIR(4));
      wattron(selectProWin, A_BOLD | COLOR_PAIR(4));
      mvwprintw(selectProWin, 1, (pcol-lenTwo)/2, titleTwo);
      wmove(selectProWin,1,1);
      show_panel(selectProPanel);
      update_panels();
      doupdate();

      i = j = rRow = 0;
      list = 6;

      res = PQexec(conn, "SELECT pa.provider_acct_id, p.provider_name, p.provider_id, pa.provider_acct_no \
                          FROM provider_account pa \
                          INNER JOIN provider p ON (pa.provider_id = p.provider_id) \
                          ORDER BY pa.provider_acct_id");
      
      rRow = PQntuples(res);
      
      mvwprintw(selectProWin,4,1, "AccountID       Name                      ProviderID           AccountNo");
      wattroff(selectProWin, A_BOLD | COLOR_PAIR(4));
      mvwprintw(selectProWin,6,1, "Press Enter to continue");
      wmove(selectProWin,6,pcol*0.25);
      box(selectProWin,0,0);

      while((p = wgetch(selectProWin)) == '\n')
	{
	  if ( j + RANGE < rRow)
	    j = j + RANGE;	
	  else
	    j = j + (rRow - j);
	  for (i; i < j; i++)
	    {
	      mvwprintw(selectProWin,list,1,"%-15s %-25s %-20s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),PQgetvalue(res,i,3));
	      list++;
	      wclrtoeol(selectProWin);
	      box(selectProWin,0,0);
	    }
	  list = 6;
	  if (i == rRow)
	    {
	      wclrtobot(selectProWin);
	      mvwprintw(selectProWin,row-8,1,"End of List");
	      box(selectProWin,0,0);
	      break;
	    }
	}
      echo();
      mvwprintw(selectProWin,row-7,1, "Select Option: ");
      mvwscanw(selectProWin,row-7,col*0.3,"%d", &proID);
      hide_panel(selectProPanel);
      update_panels();
      doupdate();
      delwin(selectProWin);
      noecho();

      PQclear(res);
      rRow = 0;

      val = htonl((uint32_t)proID);
      params[0] = (int *)&val;
      length[0] = sizeof(val);
      formats[0] = 1;

      res = PQexecParams(conn,"SELECT pa.provider_acct_id, p.provider_name, p.provider_id, pa.provider_acct_no \
                               FROM provider_account pa \
                               INNER JOIN provider p ON (pa.provider_id = p.provider_id) \
                               WHERE pa.provider_acct_id = $1;"
			 ,1
			 ,NULL
			 ,(const char *const *)params
			 ,length
			 ,formats
			 ,0);

      rRow = PQntuples(res);
      
      if(rRow == 1)
	{
	  hide_panel(selectPanel);
	  update_panels();
	  doupdate();
	  echo();
	  strcpy(proAcctID, PQgetvalue(res,0,0));
	  strcpy(proName,PQgetvalue(res,0,1));
	  strcpy(providerID, PQgetvalue(res,0,2));
	  strcpy(proAcctNo,PQgetvalue(res,0,3));
	  strcpy(sDate, "NONE");
	  wattron(selectWin,A_BOLD | COLOR_PAIR(8));  //COLORCHG
	  //mvwprintw(selectWin,8,1,"** AcctID: %s ** Name: %s ** ProID: %s ** AcctNo: %s **",proAcctID,proName,providerID,proAcctNo);	  
	  mvwprintw(selectWin,8,1,"AcctID: %s      Name: %s      ProID: %s      AcctNo: %s  ",proAcctID,proName,providerID,proAcctNo);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(8));  //COLORCHG
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	  mvwprintw(selectWin,10,1,"Enter start date(optional): ");
	  mvwscanw(selectWin,10,col*0.35, "%s", sDate);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG
	  sDate[9] = '\0';
	  strcpy(startDate, sDate);
	  PQclear(res);
	  
	  /******** Start of section  to select document ********/
	  wbkgd(docWin,COLOR_PAIR(4));
          wattron(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin, 1, (dcol-lenThree)/2, titleThree);
	  //wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  show_panel(docPanel);
	  update_panels();
	  doupdate();
	  noecho();

	  i = j = rRow = 0;
	  list = 6;

	  paramsDoc[0] = proAcctID;
	  paramsDoc[1] = startDate;
	  
	  ckdate = strcmp(startDate, "NONE");
	  
	  if(ckdate < 0 || ckdate > 0)
	    {
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, p.provider_name, p.provider_id, pa.provider_acct_id, pa.provider_acct_no \
                                       FROM provider_account pa \
                                       INNER JOIN provider p ON (pa.provider_id = p.provider_id) \
                                       INNER JOIN documents d ON (pa.provider_acct_id = d.parent_id) \
                                       WHERE pa.provider_acct_id = $1 \
                                       AND d.start_date >= $2 \
                                       AND d.catalog = 'PROVIDER';"
				 ,2
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }
	  if(ckdate == 0)
	    {
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, p.provider_name, p.provider_id, pa.provider_acct_id, pa.provider_acct_no \
                                       FROM provider_account pa \
                                       INNER JOIN provider p ON (pa.provider_id = p.provider_id) \
                                       INNER JOIN documents d ON (pa.provider_acct_id = d.parent_id) \
                                       WHERE pa.provider_acct_id = $1 \
                                       AND d.catalog = 'PROVIDER';" \
       				 ,1
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }

	  rRow = PQntuples(res);
	  //mvwprintw(docWin,6,1, "Press Enter to continue");
	  //wmove(docWin,6,pcol*0.25);
	  mvwprintw(docWin,4,1, "DocumentID      Title                     Start_Date           OID             File");
	  wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin,6,1, "Press Enter to continue");
	  wmove(docWin,6,pcol*0.25);
	  //wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  //mvwprintw(docWin,3,1,"rRow %d ", rRow); //DEBUG
	  
	  while((p = wgetch(docWin)) == '\n')
	    {
	      if (j + RANGE < rRow)
		j + j + RANGE;
	      else
		j = j + (rRow - j);
	      for (i; i < j; i++)
		{
		  //mvwprintw(docWin,list,1,"%s %s", PQgetvalue(res,i,0), PQgetvalue(res,i,1));
		  mvwprintw(docWin,list,1,"%-15s %-25s %-20s %-15s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),PQgetvalue(res,i,3),PQgetvalue(res,i,4));
		  list++;		  
		}
	      list = 6;
	      if (i == rRow)
		{
		  mvwprintw(docWin, drow-9,1, "End of list");
		  break;
		}
	    }
	  echo();
	  mvwprintw(docWin, drow-8,1, "Select Option: ");
	  mvwscanw(docWin, drow-8, dcol*0.17, "%d", &docExportID);
          PQclear(res);

	  /**** start of section to select document details for export ****/
	  /* now that we have the docExportId we have to 
             1. run a query on the documents table filetered in docExportID
             2. if the query pulls back one row  
             2. assign the file_name and oid_value to variables
             3. pass these values to an export function to create the document
             4. if the query does not pull back one row
             5. then output message that number entered is invalid and end
	  */

          val = htonl((uint32_t)docExportID);
	  params[0] = (int *)&val;
	  length[0] = sizeof(val);
	  formats[0] = 1;

	  res = PQexecParams(conn,"SELECT * FROM documents WHERE document_id = $1;"
			     ,1
			     ,NULL
			     ,(const char *const *)params
			     ,length
			     ,formats
			     ,0);

	  rRow = PQntuples(res);

	  if (rRow == 1)
	    {
	      strcpy(docFileName, PQgetvalue(res,0,2));
	      oidValue = atoi(PQgetvalue(res,0,3));
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	      mvwprintw(selectWin,12,1, "Document ID to export %d", docExportID);
	      mvwprintw(selectWin, 14,1, "Export document y/n: ");
	      wmove(selectWin, 14,25);
	      //wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	      echo();
	      while((ConfirmExport = wgetch(selectWin)) != 'y')
		{
		  wmove(selectWin, 14,25);
		  if(ConfirmExport == 'n')
		    break;
		}
	      noecho();
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
         
	      if (ConfirmExport == 'y')
		{
		  wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
		  expFName = exportDocument(oidValue, docFileName);
		  mvwprintw(selectWin,14,1,"exported file %s", expFName);
		  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
		}
	    }
	  else
	    {
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	      mvwprintw(selectWin,12,1,"Number invalid - press Enter to continue");
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	    }
	  wgetch(selectWin);  //used for select export document y/n	  
	  PQclear(res);
	  rRow = 0;	  
	}  //if row == 1
      else
	{
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	  mvwprintw(selectWin,8,1,"Number invalid - press Enter to continue");
	  mvwscanw(selectWin,8,col*0.8, "%d", cont);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   //COLORCHG new line
	}
    }
  else if (selectOption == 2)
    {
      hide_panel(selectProPanel);
      hide_panel(docPanel);
      update_panels();
      doupdate();
      mvwprintw(selectWin,8,1,"option 2 Service - WIP");
      mvwscanw(selectWin,8,col*0.8, "%d", cont);	  
    }
  else
    {
      hide_panel(selectProPanel);
      hide_panel(docPanel);
      update_panels();
      doupdate();
      mvwprintw(selectWin,8,1,"No Option");
      mvwscanw(selectWin,8,col*0.8, "%d", cont);
    }

  hide_panel(selectPanel);
  hide_panel(docPanel);
  update_panels();
  doupdate();
  delwin(selectWin);
  
  noecho();
  endwin();

  free(expFName);
}


char * exportDocument(int oid, char fn[])
{
  char *fullFileName = (char *)malloc(35*sizeof(char)); 
  char *expFileName = (char *)malloc(35*sizeof(char));

  PGconn *conn = fdbcon();
  PGresult * res;
    
  strcpy(fullFileName, "/tmp/");
  strcpy(expFileName, fn);
  strcat(fullFileName, expFileName);
  
  res = PQexec(conn, "BEGIN");
  PQclear(res);
  lo_export(conn, oid, fullFileName);
  res = PQexec(conn, "END");
  PQclear(res);
  PQfinish(conn);

  free(expFileName);
  
  return fullFileName;   
}
