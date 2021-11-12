#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
//#include <stdarg.h>
//#include <ncurses.h>
#include <panel.h>
#include <libpq-fe.h>
#include "../inc/jadlib.h"
#include "../inc/stmuplibf.h"
#include "../inc/stmuplf.h"
#include "../inc/stmlib.h"
//#include "../inc/catcur.h"

void upLoadStatement()
{
  WINDOW *upLoadStmtWindow;
  PANEL *upLoadStmtPanel;
  struct statement *start, *newStmtPtr, *end, *ptr;
  FILE *cp,*np;                 /* pointers to file */
  int i = 0;                    /* count of rows in the statement file download */
  int j = 0;                    /* count of characters in the date column */
  int j2 = 0;                   /* count of characters in the type column */ 
  int j3 = 0;                   /* count of characters in the description column */ 
  int j4 = 0;                   /* count of character in the value column */
  int j5 = 0;                   /* count of characters in the account number column */
  int c = 0, n = 0;             /* character selected from file */
  int h = 0;                    /* various counters for loops and array */
  int p = 0;
  int q = 0;
  int x = 0;
  int l = 0;
  int g = 0;                    /* count for free up of memory */ 
  int qcount = 0;                   /* count of quotation marks seperating columns in file */
  int kr= 0, kc = 0;            /* count of rows kr and characters kc in the file */
  char **tmpDate;               /* temporary store for date */
  char **transDate;             /* transaction date on statement formatted yyyymmdd */
  char **transType;             /* Type column contains up to 3 characters */
  char **transDescription;      /* Description column on statement which can have a large number of characters */
  char **transValue;            /* Value column on statement */
  char **accountNumber;         /* Account Number on statement */
  char **transAlias;            /* Alias value matched from statement_link table */
  char **stmtAliasRtn;          /* alias value returned from statement_link table */  
  int w = 0;
  int aliasPos = 0;                 /* position where alias is found in statement description */ 
  int resRow = 0;               /* number of rows returned from statment_link table */
  int cr, cc;
  char *path = "/tmp/";
  int ft = 0; //DEBUB

  int tr = 0; //number or lines before first QM, top rows
  int rs = 0; //number of statement rows excluding top rows before data.
  long int pos = 0;
  long int posn = 0;
  //char upfStmt;
  //setlocale(LC_ALL,"");
  //  setlocale(LC_CTYPE,"");  
  initscr();
  cbreak();
  //nonl();
  noecho();
  keypad(stdscr, TRUE);

  upLoadStmtWindow = newwin(40, 200, 1, 1); 
  upLoadStmtPanel = new_panel(upLoadStmtWindow);  
  update_panels();
  doupdate();  
  keypad(upLoadStmtWindow, TRUE);
  scrollok(upLoadStmtWindow, TRUE);
  box(upLoadStmtWindow, 0,0);
  //meta(upLoadStmtWindow, TRUE);
  waddstr(upLoadStmtWindow, "Statement Up Load");
  wrefresh(upLoadStmtWindow);
  

  if (upLoadStmtWindow == NULL)
    {
      addstr("Unable to create window");
      refresh();
      getch();
    }

  cp = fopen("/tmp/bankstmtFull2.csv", "r");
  np = fopen("/tmp/bankstmtFull2.csv", "r");

  /* count how many rows there are in the statement file download 
     will also count number of characters although not really required */
  while (c != EOF)
    {
      kc++;  /* number of characters in the file */       
      c = fgetc(cp);      
      if (c == '\n')	
	kr++; /* number of rows in the file */
    }

  rewind(cp);   /* return cursor to beginning of file */

  while (c!= QM)
    {
      c = fgetc(cp);  
      n = fgetc(np);
      if (c == '\n')
	tr++;	
    }
  scroll(upLoadStmtWindow);
  wrefresh(upLoadStmtWindow);
  fseek(cp, -1, SEEK_CUR);
  fseek(np, -1, SEEK_CUR);
  wprintw(upLoadStmtWindow, "c_cur %c %d   d_cur %c %d\n", c,c,n,n);
   
  // number of valid rows in statement
  rs = kr - tr;

  tmpDate = (char **) malloc(rs * sizeof(char *));
  transDate = (char **) malloc (rs * sizeof(char *));
  transType = (char **) malloc (rs * sizeof(char *));
  transDescription = (char **) malloc (rs * sizeof(char *));
  transValue = (char **) malloc (rs * sizeof(char *));
  accountNumber = (char **) malloc (rs * sizeof(char *));
  transAlias = (char **) malloc (rs * sizeof(char *));
  for (h = 0; h < rs; h++)
    {
      tmpDate[h] = (char *)malloc(TDATE * sizeof(char));
      transDate[h] = (char *)malloc(TDATE * sizeof(char));
      transType[h] = (char *)malloc(TTYPE * sizeof(char));
      transDescription[h] = (char *)malloc(TDESC * sizeof(char));
      transValue[h] = (char *)malloc(TVALUE * sizeof(char));
      accountNumber[h] = (char *)malloc(ANUM * sizeof(char));
      transAlias[h] = (char *)malloc(ALIAS * sizeof(char));
    }

  wprintw(upLoadStmtWindow, "rows in statement %d\n", rs);
  wrefresh(upLoadStmtWindow);

  int charCount = 0;
  
  while (c != EOF)
    {
      c = fgetc(cp);
      //wprintw(upLoadStmtWindow, "c at start %c\n", c);
      pos = ftell(cp);      
      // wprintw(upLoadStmtWindow, "c_cur %c %d pos %d\n", c, c, pos);
      //wgetch(upLoadStmtWindow);
      wprintw(upLoadStmtWindow, "value j %d\n", j);
      if (c == '\n')
	{
	  /* tmpDate[i][j+1] = '\0';
	  transType[i][j2+1] = '\0';
	  transDescription[i][j3+1] = '\0';
	  transValue[i][j4+1] = '\0';
	  accountNumber[i][j5+1] = '\0';*/
	  j = 0;
	  j2 = 0;
	  j3 = 0;
	  j4 = 0;
	  j5 = 0;
	  ++i;
	  qcount = 0;
	}

      if ( c == QM || qcount == 6 || qcount == 8)
	{
	  // wprintw(upLoadStmtWindow, "c: %c pos: %d n: %c qcount: %d\n",c,pos,n,qcount);
	  // wgetch(upLoadStmtWindow);
	  // if (qcount != 6)
	  qcount++;
	  //wprintw(upLoadStmtWindow, "qcount_top %d  char c %c %d\n", qcount,c,c);
	  fseek(np, pos, SEEK_SET);
	  do{
	    n = fgetc(np);
	    // wprintw(upLoadStmtWindow, "n at start %c\n",n);
	    posn = ftell(np);
	    //wprintw(upLoadStmtWindow, "value n %c  posn %d\n", n,posn);
	    charCount++;
	    //   wprintw(upLoadStmtWindow, "cur_n %c %d posn %d\n", n,n,posn);
	    // wrefresh(upLoadStmtWindow);
	    //wgetch(upLoadStmtWindow);
	    if (n != SP && n != CM && n != FS && n != AP)
	      {
		if (qcount == 1 && n != QM)
		  {
		    //wprintw(upLoadStmtWindow, "qcount at 1 %d\n", qcount);
		    tmpDate[i][j] = n;
		    //wprintw(upLoadStmtWindow, "tmpDate %c\n", tmpDate[i][j]);
		    //wprintw(upLoadStmtWindow, "value j %d n  %c\n", j, n);
		    ++j;
		    //posn = ftell(np);
		    // wprintw(upLoadStmtWindow, "posn %d\n", posn);
		  }
		if (qcount == 3)
		  {
		    if (charCount == 1 && n == QM)
		      {
			transType[i][j2] = 'Z';
			//		wprintw(upLoadStmtWindow, "value_Z j2 %d n  %c\n", j2, n);
			//++j2;
		      }
		    else if (n != QM)
		      {
			transType[i][j2] = n;
			//	wprintw(upLoadStmtWindow, "value j2 %d n  %c\n", j2, n);
			//++j2;
		      }
		    ++j2;		      
		  }
		if (qcount == 5 && n != QM)
		  {
		    transDescription[i][j3] = n;
		    // wprintw(upLoadStmtWindow, "value j3 %d n  %c\n", j3, n);
		    ++j3;
		  }
		if (qcount == 7 && n != QM)
		  {
		    transValue[i][j4] = n;
		    // wprintw(upLoadStmtWindow, "value j4 %d n  %c\n", j4, n);
		    ++j4;
		  }
		if (qcount == 13 && n != QM)
		  {
		    accountNumber[i][j5] = n;
		    // wprintw(upLoadStmtWindow, "qcount at acctno %d\n", qcount);
		    // wprintw(upLoadStmtWindow, "value j5 %d n  %c\n", j5, n);
		    ++j5;
		  }
		
	      }	    //end of if
	  }while (n != QM);	  
	  fseek(cp, posn, SEEK_SET);
	  //wprintw(upLoadStmtWindow, "posn_end %d\n", posn);
	  qcount++;
	  // wprintw(upLoadStmtWindow, "qcount_low %d\n", qcount);
	  pos = ftell(cp);
	  //wgetch(upLoadStmtWindow);
	  // wprintw(upLoadStmtWindow, "c_cur %c %d pos %d\n", c, c, pos);
	}
      // wprintw(upLoadStmtWindow, "char count %d\n", charCount);
      charCount = 0;
      // j = 0;
    }
  /*
  wprintw(upLoadStmtWindow, "i %d j %d", i,j);

  int a1 = 0;
  int b1 = 0;
  
  for (a1=0; a1 < i; a1++)
    {
      for (b1=0; b1<12; b1++)
	{
	  wprintw(upLoadStmtWindow, "tmpdate %c", tmpDate[a1][b1]);
	}
    }
  */

  int r = 0;
  /*  for (r=0; r<i; r++)
    {
      wprintw(upLoadStmtWindow, "tmpdate %s\n", tmpDate[r]);
      wprintw(upLoadStmtWindow, "transType %s\n", transType[r]);
      wprintw(upLoadStmtWindow, "transDesc %s\n", transDescription[r]);
      wprintw(upLoadStmtWindow, "transValue %s\n", transValue[r]);
      wprintw(upLoadStmtWindow, "accountNumber %s\n", accountNumber[r]);
      wgetch(upLoadStmtWindow);
      }*/

 
  
  /*int r = 0;
    for (r = 0 ; r < j; r++)
    {
    wprintw(upLoadStmtWindow, "%d", tmpDate[0][r]);
    }*/
 
  //  wgetch(upLoadStmtWindow);
  scroll(upLoadStmtWindow);
  //  wrefresh(upLoadStmtWindow);
  wgetch(upLoadStmtWindow);

  /* arrange date format to yyyymmdd */
  for (q = 0; q < i; q++)
    {
      transDate[q][0] = tmpDate[q][4];
      transDate[q][1] = tmpDate[q][5];
      transDate[q][2] = tmpDate[q][6];
      transDate[q][3] = tmpDate[q][7];
      transDate[q][4] = tmpDate[q][2];
      transDate[q][5] = tmpDate[q][3];
      transDate[q][6] = tmpDate[q][0];
      transDate[q][7] = tmpDate[q][1];
    }

 
  for (r=0; r<i; r++)
    {
     wprintw(upLoadStmtWindow, "%s  %s %s  %s  %s\n", transDate[r], transType[r], transDescription[r], transValue[r], accountNumber[r]);
      wgetch(upLoadStmtWindow);
      }
 
  resRow = resultRows();  /* return number of rows in the statment_link table */
  // mvwprintw(upLoadStmtWindow, 4,1, "result rows %d\n", resRow);  // DEBUG
  //refresh();
  stmtAliasRtn = addAlias();   /* string array of alias column in the statement_link table */
      
  // mvwprintw(upLoadStmtWindow,5,1, "value of i %d\n", i); // DEBUD
  //refresh();   
  //wrefresh(upLoadStmtWindow);
   
  for (x = 0; x < i; x++)
    { 
      for (w = 0; w < resRow; w++)
	{
	  aliasPos = aliasMatch(transDescription[x], stmtAliasRtn[w]);
	  if (aliasPos != -1)
	    {
	      strcpy(transAlias[x], stmtAliasRtn[w]);	  
	      break;
	    }
	  else	     
	    strcpy(transAlias[x],"NA");	     
	}
    }

  /* assign value to the linked list */
  for (x = 0; x < i; x++)
    {
      if (l == 0)
	{
	  start = importStmt(transDate[x], transType[x], transDescription[x], transValue[x], accountNumber[x], transAlias[x]);
	  end = start;
	}
      else
	{
	  newStmtPtr = importStmt(transDate[x], transType[x], transDescription[x], transValue[x], accountNumber[x], transAlias[x]);
	  end = append(end, newStmtPtr);
	}
      l++;
    }

  //wmove(upLoadStmtWindow,12,1);
  
  printStatement(start, upLoadStmtWindow);
  wgetch(upLoadStmtWindow);

   for (r=0; r<i; r++)
    {
     wprintw(upLoadStmtWindow, "%s  %s %s  %s  %s\n", transDate[r], transType[r], transDescription[r], transValue[r], accountNumber[r]);
      wgetch(upLoadStmtWindow);
      }

   printStatement_new(start);
   wgetch(upLoadStmtWindow);
  freeStatement(start);

  
  /* tmpDate = (char **) realloc(tmpDate, rs);
  transDate = (char **) realloc (transDate, rs);
  transType = (char **) realloc (transType, rs);
  transDescription = (char **) realloc (transDescription, rs);
  transValue = (char **) realloc (transValue, rs);
  accountNumber = (char **) realloc (accountNumber, rs);
  transAlias = (char **) realloc (transAlias, rs);
  for (h = 0; h < rs; h++)
    {
      tmpDate[h] = (char *)realloc(tmpDate,TDATE);
      transDate[h] = (char *)realloc(transDate,TDATE);
      transType[h] = (char *)realloc(transType, TTYPE);
      transDescription[h] = (char *)realloc(transDescription, TDESC);
      transValue[h] = (char *)realloc(transValue, TVALUE);
      accountNumber[h] = (char *)realloc(accountNumber, ANUM);
      transAlias[h] = (char *)realloc(transAlias, ALIAS);
      }*/
  
  
 for (g = 0; g < rs; g++)
   {
     /* tmpDate[g] = NULL;
   transDate[g] = NULL;
   transType[g] = NULL;
   transDescription[g] = NULL;
   transValue[g] = NULL;
   accountNumber[g] = NULL;
   transAlias[g] = NULL;*/
     /*  tmpDate[g] = '\0';
     transDate[g] = '\0';
     transType[g] = '\0';
   transDescription[g] = '\0';
   transValue[g] = '\0';
   accountNumber[g] = '\0';
   transAlias[g] = '\0';*/
   free(tmpDate[g]);
     free(transDate[g]);
     free(transType[g]);
      free(transDescription[g]);
      free(transValue[g]);
      free(accountNumber[g]);
      free(transAlias[g]);
      }
  free(tmpDate);
  free(transDate);
  free(transType);
  free(transDescription);  
  free(transValue);
  free(accountNumber);
  free(transAlias);

  /*    for (r=0; r<i; r++)
    {
      wprintw(upLoadStmtWindow, "%s  %s %s  %s  %s\n", transDate[r], transType[r], transDescription[r], transValue[r], accountNumber[r]);
      wgetch(upLoadStmtWindow);
      }*/


  fclose(cp);
  fclose(np);
  
  hide_panel(upLoadStmtPanel);
  update_panels();
  doupdate();
  delwin(upLoadStmtWindow);
  endwin(); 
}


char ** addAlias()
{
  PGconn *conn = fdbcon();
  PGresult *res;
  int rows = 0;
  //int j;
  //int k;
  int h = 0;
  int d = 0;
  //struct statement *ptr;
  char **stmtAlias;

  res = PQexec(conn, "SELECT * FROM statement_link ORDER BY id");
  rows = PQntuples(res);
  d = rows;

  //for (j=0; j<rows; j++)
  // printf("%s %s %s %s\n", PQgetvalue(res,j,0), PQgetvalue(res,j,1), PQgetvalue(res,j,2), PQgetvalue(res,j,3));

  stmtAlias = malloc(rows * sizeof(int *));
  for (h=0; h<rows; h++)
    {
      stmtAlias[h] = (char *)(malloc)(ALIAS * sizeof(char));
      stmtAlias[h] = PQgetvalue(res, h, 1);
    }

  // for (h=0; h<rows; h++)
  // printf("alias: %s\n", stmtAlias[h]);

   return stmtAlias;
   
  PQclear(res);
  PQfinish(conn);  

  for (h=0; h<d; h++)
    free(stmtAlias[h]);
  free (stmtAlias);  
}


int resultRows()
{
  PGconn *conn = fdbcon();
  PGresult *res;
  int rows;

  res = PQexec(conn, "SELECT * FROM statement_link ORDER BY id");
  rows = PQntuples(res);
  
  PQclear(res);
  PQfinish(conn);

  return rows;
  
}


void statementInsert(struct statement *start, WINDOW *win)
{
  struct statement *ptr = start;
  int stmtTranDate;
  char stmtTranType[TTYPE];
  char stmtTranDesc[TDESC]; 
  float stmtValue;
  char stmtAcctNo[ANUM];
  char stmtTranAlias[ALIAS];
  int i = 0;

  scrollok(win,TRUE);
  //idlok(win, TRUE);

  //scrollok(stdscr,TRUE);
  
  while (ptr != NULL)
    {
      stmtTranDate = atoi(ptr->tDate);
      strcpy(stmtTranType, ptr->tType);
      strcpy(stmtTranDesc, ptr->tDescription);
      stmtValue = atof(ptr->tValue);
      strcpy(stmtAcctNo, ptr->actNumber);
      strcpy(stmtTranAlias, ptr->tAlias);
  
      //printw(win, "%d %s %s %.2f %s %s", stmtTranDate, stmtTranType, stmtTranDesc, stmtValue, stmtAcctNo, stmtTranAlias);
      //   mvwprintw(win,i+4,2, "%d %s %s %.2f %s %s", stmtTranDate, stmtTranType, stmtTranDesc, stmtValue, stmtAcctNo, stmtTranAlias);
         wprintw(win, "%d %s %s %.2f %s %s\n", stmtTranDate, stmtTranType, stmtTranDesc, stmtValue, stmtAcctNo, stmtTranAlias);
       
       //stmtInsert(stmtTranDate, stmtTranType, stmtTranDesc, stmtValue, stmtAcctNo, stmtTranAlias);
       //            if (i == 20)
       //	{
	  // getch();
       // i = 0;
	  //break;
       // } 
	 //napms(100);
      ptr = ptr->next;
      i++;
      //scroll(win);
      wrefresh(win);
      //  scroll(win);
      //wgetch(win);
      
          }
//wgetch(win);  
  scroll(win);
  wrefresh(win);
  //scroll(stdscr);
  //refresh();
  //wgetch(win);
  

}

void printStatement_new(struct statement *start)
{
  int i= 0;

  WINDOW *upLoadStmtWindow_x;//, *prStmtWindow;
  PANEL *upLoadStmtPanel_x;//, *prStmtPanel;  
  struct statement *ptr; 
  ptr = start;

  initscr();
  cbreak();
  noecho();
  //scrollok(stdscr,TRUE);
  keypad(stdscr, TRUE);

  upLoadStmtWindow_x = newwin(40, 200, 1, 1);

  upLoadStmtPanel_x = new_panel(upLoadStmtWindow_x);
  update_panels();
  doupdate();
  
  keypad(upLoadStmtWindow_x, TRUE); 

  scrollok(upLoadStmtWindow_x, TRUE);

  box(upLoadStmtWindow_x, 0,0);
  waddstr(upLoadStmtWindow_x, "Statment Up Load");

  if(upLoadStmtWindow_x == NULL) // || prStmtWindow == NULL)
     {
      addstr("Unable to create window");
      refresh();
      getch();
      }

  wrefresh(upLoadStmtWindow_x);
 
  mvwprintw(upLoadStmtWindow_x,3,2,"Date, Type, Description, Value, Account Number\n");
  while(ptr != NULL)
    {
      i++;
      mvwprintw(upLoadStmtWindow_x, i+4, 2,"%s %s %s %s %s %s\n", ptr->tDate, ptr->tType, ptr->tDescription, ptr->tValue, ptr->actNumber, ptr->tAlias);
       if (i == 20)
	{
	  wgetch(upLoadStmtWindow_x);
	  i = 0;
	} 
      ptr = ptr->next;
      wclrtobot(upLoadStmtWindow_x);
      wrefresh(upLoadStmtWindow_x);
    }
  wgetch(upLoadStmtWindow_x);
  del_panel(upLoadStmtPanel_x);
  update_panels();
  delwin(upLoadStmtWindow_x);
  endwin();
}

