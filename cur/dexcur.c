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
  const char *titleFour = "Supplier Account List";
  const char *titleFive = "Document For Supplier Account";
  const char *titleSix = "Supplier Invoice List";
  const char *titleSeven = "Document For Supplier Invoice";
  const char *titleEight = "Document Type List";
  const char *titleNine = "Documents For Type Selected";
  int lenOne = strlen(titleOne);
  int lenTwo = strlen(titleTwo);
  int lenThree = strlen(titleThree);
  int lenFour = strlen(titleFour);
  int lenFive = strlen(titleFive);
  int lenSix = strlen(titleSix);
  int lenSeven = strlen(titleSeven);
  int lenEight = strlen(titleEight);
  int lenNine = strlen(titleNine);
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

  selectWin = newwin(LINES*0.4, COLS*0.4,LINES*0.07,COLS/4);
  selectProWin = newwin(LINES*0.5, COLS*0.5,LINES*0.07,COLS/5);
  docWin = newwin(LINES*0.4, COLS*0.7, LINES/2, COLS/5);
  selectPanel = new_panel(selectWin);
  selectProPanel = new_panel(selectProWin);
  docPanel = new_panel(docWin);
  
  wbkgd(selectWin,COLOR_PAIR(3));  
  
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

  wattron(selectWin,A_BOLD | COLOR_PAIR(3));   
  mvwprintw(selectWin, 1, (col-lenOne)/2, titleOne);

  mvwprintw(selectWin,3,1, "1 - Provider");
  mvwprintw(selectWin,4,1, "2 - Supplier\n");
  mvwprintw(selectWin,5,1, "3 - Supplier Invoice\n");
  mvwprintw(selectWin,6,1, "4 - Document Type\n");  
  mvwprintw(selectWin,8,1, "Enter option:");

  echo();
  mvwscanw(selectWin,8,18,"%d",&selectOption);
  noecho();
  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
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
	  wattron(selectWin,A_BOLD | COLOR_PAIR(8));  
	  mvwprintw(selectWin,10,1,"AcctID: %s      Name: %s      ProID: %s      AcctNo: %s  ",proAcctID,proName,providerID,proAcctNo);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(8)); 
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,12,1,"Enter start date(optional): ");
	  mvwscanw(selectWin,12,col*0.35, "%s", sDate);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	  sDate[9] = '\0';
	  strcpy(startDate, sDate);
	  PQclear(res);
	  
	  /******** Start of section  to select document ********/
	  wbkgd(docWin,COLOR_PAIR(4));
          wattron(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin, 1, (dcol-lenThree)/2, titleThree);
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
                                       AND d.catalog = 'PROVIDER' \
                                       ORDER BY d.document_id;"
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
                                       AND d.catalog = 'PROVIDER' \
                                       ORDER BY d.document_id;" \
       				 ,1
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }

	  rRow = PQntuples(res);

	  mvwprintw(docWin,4,1, "DocumentID      Title                     Start_Date           OID             File");
	  wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin,6,1, "Press Enter to continue");
	  wmove(docWin,6,pcol*0.25);
	  
	  while((p = wgetch(docWin)) == '\n')
	    {
	      if (j + RANGE < rRow)
		j = j + RANGE;
	      else
		j = j + (rRow - j);
	      for (i; i < j; i++)
		{
		  mvwprintw(docWin,list,1,"%-15s %-25s %-20s %-15s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),PQgetvalue(res,i,3),PQgetvalue(res,i,4));
		  list++;
		  wclrtobot(docWin);
		  box(docWin,0,0);		    
		}
	      list = 6;
	      if (i == rRow)
		{
		  wclrtobot(docWin);
		  mvwprintw(docWin, drow-9,1, "End of list");
		  box(docWin,0,0);
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
             3. pass these values to an export fnction to create the document
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
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));   
	      mvwprintw(selectWin,15,1, "Document ID to export %d", docExportID);
	      mvwprintw(selectWin,16,1, "Export document y/n: ");
	      wmove(selectWin,16,25);
	
	      echo();
	      while((ConfirmExport = wgetch(selectWin)) != 'y')
		{
		  wmove(selectWin,16,25);
		  if(ConfirmExport == 'n')
		    break;
		}
	      noecho();
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
         
	      if (ConfirmExport == 'y')
		{
		  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
		  expFName = exportDocument(oidValue, docFileName);
		  mvwprintw(selectWin,17,1,"exported file %s", expFName);
		  wgetch(selectWin);
		  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
		}
	    }
	  else
	    {
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	      mvwprintw(selectWin,15,1,"Number invalid - press Enter to continue");
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	    }
	
	  PQclear(res);
	  rRow = 0;	  
	}  
      else
	{
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,10,1,"Number invalid - press Enter to continue");
	  wgetch(selectWin);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));
	}
    }
  else if (selectOption == 2)
    {
      hide_panel(selectPanel);
      hide_panel(docPanel);
      wbkgd(selectProWin,COLOR_PAIR(4));
      wattron(selectProWin, A_BOLD | COLOR_PAIR(4));
      mvwprintw(selectProWin, 1, (pcol-lenFour)/2, titleFour);
      wmove(selectProWin,1,1);
      show_panel(selectProPanel);
      update_panels();
      doupdate();

      i = j = rRow = 0;
      list = 6;

      res = PQexec(conn, "SELECT sa.supplier_acct_id, s.supplier_name,s.supplier_id, sa.supplier_acct_ref, p.property_id, p.post_code \
                          FROM supplier_account sa \
                          INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                          INNER JOIN property p ON (p.property_id = sa.property_id) \
                          ORDER BY sa.supplier_acct_id");
      
      rRow = PQntuples(res);
      
      mvwprintw(selectProWin,4,1, "AccountID       Name                      SupplierID           AccountRef           Post_Code");
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
	      mvwprintw(selectProWin,list,1,"%-15s %-25s %-20s %-20s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),PQgetvalue(res,i,3),PQgetvalue(res,i,5));
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

      res = PQexecParams(conn,"SELECT sa.supplier_acct_id, s.supplier_name,s.supplier_id, sa.supplier_acct_ref \
                               FROM supplier_account sa \
                               INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                               WHERE sa.supplier_acct_id = $1;" \
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
	  wattron(selectWin,A_BOLD | COLOR_PAIR(8)); 

	  mvwprintw(selectWin,10,1,"AcctID: %s      Name: %s      SupID: %s      AcctRef: %s  ",proAcctID,proName,providerID,proAcctNo);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(8));
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	  mvwprintw(selectWin,12,1,"Enter start date(optional): ");
	  mvwscanw(selectWin,12,col*0.35, "%s", sDate);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));
	  sDate[9] = '\0';
	  strcpy(startDate, sDate);
	  PQclear(res);
	  
	  /******** Start of section  to select document ********/
	  wbkgd(docWin,COLOR_PAIR(4));
          wattron(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin, 1, (dcol-lenFive)/2, titleFive);
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
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, s.supplier_name, s.supplier_id, sa.supplier_acct_id, sa.supplier_acct_ref \
                                       FROM supplier_account sa \
                                       INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                                       INNER JOIN documents d ON (sa.supplier_acct_id = d.parent_id) \
                                       WHERE sa.supplier_acct_id = $1 \
                                       AND d.start_date >= $2 \
                                       AND d.catalog IN ('SUPPLIER') \
                                       ORDER BY d.document_id;"
				 ,2
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }
	  if(ckdate == 0)
	    {
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, s.supplier_name, s.supplier_id, sa.supplier_acct_id, sa.supplier_acct_ref \
                                       FROM supplier_account sa \
                                       INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                                       INNER JOIN documents d ON (sa.supplier_acct_id = d.parent_id) \
                                       WHERE sa.supplier_acct_id = $1 \
                                       AND d.catalog IN ('SUPPLIER') \
                                       ORDER BY d.document_id;" \
       				 ,1
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }

	  rRow = PQntuples(res);
	  mvwprintw(docWin,4,1, "DocumentID      Title                     Start_Date           OID             File");
	  wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin,6,1, "Press Enter to continue");
	  wmove(docWin,6,pcol*0.25);
	  
	  while((p = wgetch(docWin)) == '\n')
	    {
	      if (j + RANGE < rRow)
		j = j + RANGE;
	      else
		j = j + (rRow - j);
	      for (i; i < j; i++)
		{
		  mvwprintw(docWin,list,1,"%-15s %-25s %-20s %-15s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),PQgetvalue(res,i,3),PQgetvalue(res,i,4));
		  list++;
		  wclrtobot(docWin);
		  box(docWin,0,0);
		}
	      list = 6;
	      if (i == rRow)
		{
		  wclrtobot(docWin);
		  mvwprintw(docWin, drow-9,1, "End of list");
		  box(docWin,0,0);
		  break;
		}
	    }
	  echo();
	  mvwprintw(docWin, drow-8,1, "Select Option: ");
	  mvwscanw(docWin, drow-8, dcol*0.17, "%d", &docExportID);
          PQclear(res);

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
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	      mvwprintw(selectWin,15,1, "Document ID to export %d", docExportID);
	      mvwprintw(selectWin,16,1, "Export document y/n: ");
	      wmove(selectWin,16,25);

	      echo();
	      while((ConfirmExport = wgetch(selectWin)) != 'y')
		{
		  wmove(selectWin,16,25);
		  if(ConfirmExport == 'n')
		    break;
		}
	      noecho();
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
         
	      if (ConfirmExport == 'y')
		{
		  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
		  expFName = exportDocument(oidValue, docFileName);
		  mvwprintw(selectWin,17,1,"exported file %s", expFName);
		  wgetch(selectWin);
		  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
		}
	    }
	  else
	    {
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	      mvwprintw(selectWin,15,1,"Number invalid - press Enter to continue");
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
	    }

	  PQclear(res);
	  rRow = 0;	  
	}  //if row == 1
      else
	{
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,10,1,"Number invalid - press Enter to continue");
	  wgetch(selectWin);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
	}      
    }
  else if (selectOption == 3)
    {
      hide_panel(selectPanel);
      hide_panel(docPanel);
      wbkgd(selectProWin,COLOR_PAIR(4));
      wattron(selectProWin, A_BOLD | COLOR_PAIR(4));
      mvwprintw(selectProWin, 1, (pcol-lenSix)/2, titleSix);
      wmove(selectProWin,1,1);
      show_panel(selectProPanel);
      update_panels();
      doupdate();

      i = j = rRow = 0;
      list = 6;

      res = PQexec(conn, "SELECT sa.supplier_acct_id, s.supplier_name,s.supplier_id, sa.supplier_acct_ref, p.property_id, p.post_code  \
                          FROM supplier_account sa \
                          INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                          INNER JOIN property p ON (p.property_id = sa.property_id) \
                          ORDER BY sa.supplier_acct_id;");
      
      rRow = PQntuples(res);
      
      mvwprintw(selectProWin,4,1, "AccountID       Name                      SupplierID           AccountRef           Post_Code");
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
	      mvwprintw(selectProWin,list,1,"%-15s %-25s %-20s %-20s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),
			PQgetvalue(res,i,3),PQgetvalue(res,i,5));
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

      res = PQexecParams(conn,"SELECT sa.supplier_acct_id, s.supplier_name,s.supplier_id, sa.supplier_acct_ref \
                               FROM supplier_account sa \
                               INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                               WHERE sa.supplier_acct_id = $1;" \
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
	  wattron(selectWin,A_BOLD | COLOR_PAIR(8)); 

	  mvwprintw(selectWin,10,1,"AcctID: %s      Name: %s      SupID: %s      AcctRef: %s  ",proAcctID,proName,providerID,proAcctNo);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(8)); 
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,12,1,"Enter start date(optional): ");
	  mvwscanw(selectWin,12,col*0.35, "%s", sDate);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	  sDate[9] = '\0';
	  strcpy(startDate, sDate);
	  PQclear(res);
	  
	  /******** Start of section  to select document ********/
	  wbkgd(docWin,COLOR_PAIR(4));
          wattron(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin, 1, (dcol-lenSeven)/2, titleSeven);

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
	      res = PQexecParams(conn,"SELECT d.document_id, d.title,si.supplier_invoice_id, d.start_date, d.oid_value, \
                                       d.file_name, s.supplier_name, s.supplier_id, sa.supplier_acct_id, sa.supplier_acct_ref \
                                       FROM supplier_account sa \
                                       INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                                       INNER JOIN supplier_invoice si ON (sa.supplier_acct_id = si.supplier_acct_id) \
                                       INNER JOIN documents d ON (si.supplier_invoice_id = d.parent_id) \
                                       WHERE sa.supplier_acct_id = $1  \
                                       AND d.start_date >= $2 \
                                       AND d.catalog = 'INVOICE' \
                                       ORDER BY d.document_id;"
				 ,2
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }
	  if(ckdate == 0)
	    {
	      res = PQexecParams(conn, "SELECT d.document_id, d.title,si.supplier_invoice_id, d.start_date, d.oid_value, \
                                       d.file_name, s.supplier_name, s.supplier_id, sa.supplier_acct_id, sa.supplier_acct_ref \
                                       FROM supplier_account sa \
                                       INNER JOIN supplier s ON (sa.supplier_id = s.supplier_id) \
                                       INNER JOIN supplier_invoice si ON (sa.supplier_acct_id = si.supplier_acct_id) \
                                       INNER JOIN documents d ON (si.supplier_invoice_id = d.parent_id) \
                                       WHERE sa.supplier_acct_id = $1 \
                                       AND d.catalog = 'INVOICE' \
                                       ORDER BY d.document_id;" 
       				 ,1
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }

	  rRow = PQntuples(res);

	  mvwprintw(docWin,4,1, "DocumentID      Title                     Invoice         Start_Date           OID             File");
	  wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin,6,1, "Press Enter to continue");
	  wmove(docWin,6,pcol*0.25);
	  
	  while((p = wgetch(docWin)) == '\n')
	    {
	      if (j + RANGE < rRow)
		j = j + RANGE;
	      else
		j = j + (rRow - j);
	      for (i; i < j; i++)
		{
		  mvwprintw(docWin,list,1,"%-15s %-25s %-15s %-20s %-15s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),
			    PQgetvalue(res,i,3),PQgetvalue(res,i,4),PQgetvalue(res,i,5));
		  list++;
		  wclrtobot(docWin);
		  box(docWin,0,0);
		}
	      list = 6;
	      if (i == rRow)
		{
		  wclrtobot(docWin);
		  mvwprintw(docWin, drow-9,1, "End of list");
		  box(docWin,0,0);
		  break;
		}
	    }
	  echo();
	  mvwprintw(docWin, drow-8,1, "Select Option: ");
	  mvwscanw(docWin, drow-8, dcol*0.17, "%d", &docExportID);
          PQclear(res);

	  /**** start of section to select document details for export ****/
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
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	      mvwprintw(selectWin,15,1, "Document ID to export %d", docExportID);
	      mvwprintw(selectWin,16,1, "Export document y/n: ");
	      wmove(selectWin,16,25);

	      echo();
	      while((ConfirmExport = wgetch(selectWin)) != 'y')
		{
		  wmove(selectWin,16,25);
		  if(ConfirmExport == 'n')
		    break;
		}
	      noecho();
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
         
	      if (ConfirmExport == 'y')
		{
		  wattron(selectWin,A_BOLD | COLOR_PAIR(3));
		  expFName = exportDocument(oidValue, docFileName);
		  mvwprintw(selectWin,17,1,"exported file %s", expFName);
		  wgetch(selectWin);
		  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
		}
	    }
	  else
	    {
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	      mvwprintw(selectWin,15,1,"Number invalid - press Enter to continue");
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));   
	    }

	  PQclear(res);
	  rRow = 0;	  
	}
      else
	{
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	  mvwprintw(selectWin,10,1,"Number invalid - press Enter to continue");
	  wgetch(selectWin);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	}      
    }
   else if (selectOption == 4)
    {
      hide_panel(selectPanel);
      hide_panel(docPanel);
      wbkgd(selectProWin,COLOR_PAIR(4));
      wattron(selectProWin, A_BOLD | COLOR_PAIR(4));
      mvwprintw(selectProWin, 1, (pcol-lenEight)/2, titleEight);
      wmove(selectProWin,1,1);
      show_panel(selectProPanel);
      update_panels();
      doupdate();

      i = j = rRow = 0;
      list = 6;

      res = PQexec(conn, "SELECT d.type_id, d.description FROM document_type d \
                          ORDER BY d.description");
      
      rRow = PQntuples(res);
      
      mvwprintw(selectProWin,4,1, "TypeID          DocumentType");
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
	      mvwprintw(selectProWin,list,1,"%-15s %-25s", PQgetvalue(res,i,0),PQgetvalue(res,i,1));
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

      res = PQexecParams(conn,"SELECT d.type_id, d.description FROM document_type d \
                               WHERE d.type_id = $1 \
                               ORDER BY d.description;"
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
	  strcpy(sDate, "NONE");
	  wattron(selectWin,A_BOLD | COLOR_PAIR(8)); 

	  mvwprintw(selectWin,10,1,"TypeID: %s         DocumentType: %s", proAcctID, proName);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(8)); 
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,12,1,"Enter start date(optional): ");
	  mvwscanw(selectWin,12,col*0.35, "%s", sDate);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	  sDate[9] = '\0';
	  strcpy(startDate, sDate);
	  PQclear(res);
	  
	  /******** Start of section  to select document ********/
	  wbkgd(docWin,COLOR_PAIR(4));
          wattron(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin, 1, (dcol-lenNine)/2, titleNine);

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
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, d.type_id, dt.description, p.post_code \
                                       FROM documents d \
                                       LEFT OUTER JOIN	supplier_invoice si ON (si.supplier_invoice_id = d.parent_id AND d.catalog = 'INVOICE') \
                                       LEFT OUTER JOIN supplier_account sa ON (sa.supplier_acct_id = si.supplier_acct_id) \
                                       LEFT OUTER JOIN supplier_account sa2 ON (sa2.supplier_acct_id = d.parent_id AND d.catalog = 'SUPPLIER') \
                                       LEFT OUTER JOIN property p ON (sa.property_id = p.property_id OR sa2.property_id = p.property_id) \
                                       LEFT OUTER JOIN document_type dt ON (dt.type_id = d.type_id) \
                                       WHERE d.type_id = $1 \
                                       AND d.start_date >= $2 \
                                       ORDER BY d.document_id;"
				 ,2
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }
	  if(ckdate == 0)
	    {
	      res = PQexecParams(conn,"SELECT d.document_id, d.title, d.start_date, d.oid_value, d.file_name, d.type_id, dt.description, p.post_code\
                                       FROM documents d \
                                       LEFT OUTER JOIN	supplier_invoice si ON (si.supplier_invoice_id = d.parent_id AND d.catalog = 'INVOICE') \
                                       LEFT OUTER JOIN supplier_account sa ON (sa.supplier_acct_id = si.supplier_acct_id) \
                                       LEFT OUTER JOIN supplier_account sa2 ON (sa2.supplier_acct_id = d.parent_id AND d.catalog = 'SUPPLIER') \
                                       LEFT OUTER JOIN property p ON (sa.property_id = p.property_id OR sa2.property_id = p.property_id) \
                                       LEFT OUTER JOIN document_type dt ON (dt.type_id = d.type_id) \
                                       WHERE d.type_id = $1 \
                                       ORDER BY d.document_id;" 
       				 ,1
				 ,NULL
				 ,(const char *const *)paramsDoc
				 ,NULL				     
				 ,NULL
				 ,0);
	    }

	  rRow = PQntuples(res);

	  mvwprintw(docWin,4,1, "DocumentID      Title                               Start_Date           OID             file  \
                    Description                   Post_Code");
	  wattroff(docWin, A_BOLD | COLOR_PAIR(4));
	  mvwprintw(docWin,6,1, "Press Enter to continue");
	  wmove(docWin,6,pcol*0.25);
	  	  
	  while((p = wgetch(docWin)) == '\n')
	    {
	      if (j + RANGE < rRow)
		j = j + RANGE;
	      else
		j = j + (rRow - j);
	      for (i; i < j; i++)
		{
		  mvwprintw(docWin,list,1,"%-15s %-35s %-20s %-15s %-25s %-30s %-5s", PQgetvalue(res,i,0),PQgetvalue(res,i,1),PQgetvalue(res,i,2),
			    PQgetvalue(res,i,3),PQgetvalue(res,i,4),PQgetvalue(res,i,6),PQgetvalue(res,i,7));
		  list++;
		  wclrtobot(docWin);
		  box(docWin,0,0);
		}
	      list = 6;
	      if (i == rRow)
		{
		  wclrtobot(docWin);
		  mvwprintw(docWin, drow-9,1, "End of list");
		  box(docWin,0,0);
		  break;
		}
	    }
	  echo();
	  mvwprintw(docWin, drow-8,1, "Select Option: ");
	  mvwscanw(docWin, drow-8, dcol*0.17, "%d", &docExportID);
          PQclear(res);

	  /**** start of section to select document details for export ****/
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
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	      mvwprintw(selectWin,15,1, "Document ID to export %d", docExportID);
	      mvwprintw(selectWin,16,1, "Export document y/n: ");
	      wmove(selectWin,16,25);

	      echo();
	      while((ConfirmExport = wgetch(selectWin)) != 'y')
		{
		  wmove(selectWin,16,25);
		  if(ConfirmExport == 'n')
		    break;
		}
	      noecho();
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));  
         
	      if (ConfirmExport == 'y')
		{
		  wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
		  expFName = exportDocument(oidValue, docFileName);
		  mvwprintw(selectWin,17,1,"exported file %s", expFName);
		  wgetch(selectWin);
		  wattroff(selectWin,A_BOLD | COLOR_PAIR(3));
		}
	    }
	  else
	    {
	      wattron(selectWin,A_BOLD | COLOR_PAIR(3)); 
	      mvwprintw(selectWin,15,1,"Number invalid - press Enter to continue");
	      wattroff(selectWin,A_BOLD | COLOR_PAIR(3));
	    }

	  PQclear(res);
	  rRow = 0;	  
	} 
      else
	{
	  wattron(selectWin,A_BOLD | COLOR_PAIR(3));  
	  mvwprintw(selectWin,10,1,"Number invalid - press Enter to continue");
	  wgetch(selectWin);
	  wattroff(selectWin,A_BOLD | COLOR_PAIR(3)); 
	} 
    }
  else
    {
      hide_panel(selectProPanel);
      hide_panel(docPanel);
      update_panels();
      doupdate();
      mvwprintw(selectWin,10,1,"No Option");
      wgetch(selectWin);
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


/* Function to export the document as file to tmp folder */
char * exportDocument(int oid, char fn[])
{
  char *fullFileName = (char *)malloc(FNAMEEXP*sizeof(char)); 
  char *expFileName = (char *)malloc(FNAMEEXP*sizeof(char));

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
