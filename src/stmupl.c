#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <panel.h>
#include <libpq-fe.h>
#include "../inc/jadlib.h"
#include "../inc/stmuplibf.h"
#include "../inc/stmuplf.h"
#include "../inc/stmlib.h"

struct statement *upLoadStatement()
{
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
  //int p = 0;
  int q = 0;
  int x = 0;
  int l = 0;
  int g = 0;                    /* count for free up of memory */ 
  int qcount = 0;               /* count of quotation marks seperating columns in file */
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
  int aliasPos = 0;             /* position where alias is found in statement description */ 
  int resRow = 0;               /* number of rows returned from statment_link table */
  int cr, cc;
  char *path = "/tmp/";
  int tr = 0; //number or lines before first QM, top rows
  int rs = 0; //number of statement rows excluding top rows before data.
  long int pos = 0;             /* position of file cursor c */
  long int posn = 0;            /* position of file cursor n */
  //int r = 0;
  //int cf = 0;                    
  int charCount = 0;            /* number of characters in each data column */

  char * upf = fStmtName();     /* function to obtain filename to load and if exists */

  if(upf != NULL)
    {
      cp = fopen(upf, "r");
      np = fopen(upf, "r");
  
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

      fseek(cp, -1, SEEK_CUR);
      fseek(np, -1, SEEK_CUR);
   
      /* number of valid rows in statement excludes top empty rows */
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
  
      while (c != EOF)
	{
	  c = fgetc(cp);
	  pos = ftell(cp);      
	  if (c == '\n')
	    {
	      tmpDate[i][j+1] = '\0';
	      transType[i][j2+1] = '\0';
	      transDescription[i][j3+1] = '\0';
	      transValue[i][j4+1] = '\0';
	      accountNumber[i][j5+1] = '\0';
	      j = j2 = j3 = j4 = j5 = 0;
	      ++i;
	      qcount = 0;
	    }

	  if ( c == QM || qcount == 6 || qcount == 8)
	    {
	      qcount++;
	      fseek(np, pos, SEEK_SET);
	      do{
		n = fgetc(np);
		posn = ftell(np);
		charCount++;
		if (n != SP && n != CM && n != FS && n != AP)
		  {
		    if (qcount == 1 && n != QM)
		      {
			tmpDate[i][j] = n;
			++j;
		      }
		    if (qcount == 3)
		      {
			if (charCount == 1 && n == QM)
			  {
			    transType[i][j2] = 'Z';
			  }
			else if (n != QM)
			  {
			    transType[i][j2] = n;
			  }
			++j2;		      
		      }
		    if (qcount == 5 && n != QM)
		      {
			transDescription[i][j3] = n;
			++j3;
		      }
		    if (qcount == 7 && n != QM)
		      {
			transValue[i][j4] = n;
			++j4;
		      }
		    if (qcount == 13 && n != QM)
		      {
			accountNumber[i][j5] = n;
			++j5;
		      }
		
		  }	    //end of if
	      }while (n != QM);	  
	      fseek(cp, posn, SEEK_SET);
	      qcount++;
	      pos = ftell(cp);
	    }
	  charCount = 0;
	}

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
 
      resRow = resultRows();       /* return number of rows in the statment_link table */
      stmtAliasRtn = addAlias();   /* string array of alias column in the statement_link table */
      
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
  
      for (g = 0; g < rs; g++)
	{
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
    
      fclose(cp);
      fclose(np);
  
      return start;

      free(upf);
    }  
}


char ** addAlias()
{
  PGconn *conn = fdbcon();
  PGresult *res;
  int rows = 0;
  int h = 0;
  int d = 0;
  char **stmtAlias;

  res = PQexec(conn, "SELECT * FROM statement_link ORDER BY id");
  rows = PQntuples(res);
  d = rows;

  stmtAlias = malloc(rows * sizeof(int *));
  for (h=0; h<rows; h++)
    {
      stmtAlias[h] = (char *)(malloc)(ALIAS * sizeof(char));
      stmtAlias[h] = PQgetvalue(res, h, 1);
    }

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


void statementInsert(struct statement *start)
{
  WINDOW *stmtInsertWin;
  PANEL *stmtInsertPanel;
  struct statement *ptr;
  int stmtTranDate;
  char stmtTranType[TTYPE];
  char stmtTranDesc[TDESC]; 
  float stmtValue;
  char stmtAcctNo[ANUM];
  char stmtTranAlias[ALIAS];
  int i = 0;
  int srow, scol;
  int ch;

  ptr = start;

  initscr();
  cbreak();

  stmtInsertWin = newwin(40, 200, 1, 1);
  stmtInsertPanel = new_panel(stmtInsertWin);
  update_panels();
  doupdate();

  keypad(stmtInsertWin, TRUE);
  getmaxyx(stmtInsertWin, srow, scol);
  box(stmtInsertWin, 0, 0);
  waddstr(stmtInsertWin, "Insert Statement To Database");

  if(stmtInsertWin == NULL)
    {
      addstr("Unable to create window");
      refresh();
      getch();
    }
 
  mvwprintw(stmtInsertWin, 3, 2, "Confirm insert statement: ");
  echo();
  while ((ch = wgetch(stmtInsertWin)) != 'y')
    {
      wmove(stmtInsertWin, 31, 2);
      if (ch == 'n')
	{
	  mvwprintw(stmtInsertWin, 4, 2, "Statement not saved");
	  break;
	}
    }
  if (ch == 'y')
    {
      mvwprintw(stmtInsertWin,5,2, "ch value %d\n", ch);
      while (ptr != NULL)
	{
	  stmtTranDate = atoi(ptr->tDate);
	  strcpy(stmtTranType, ptr->tType);
	  strcpy(stmtTranDesc, ptr->tDescription);
	  stmtValue = atof(ptr->tValue);
	  strcpy(stmtAcctNo, ptr->actNumber);
	  strcpy(stmtTranAlias, ptr->tAlias);
	  stmtInsert(stmtTranDate, stmtTranType, stmtTranDesc, stmtValue, stmtAcctNo, stmtTranAlias);
	  ptr = ptr->next;
	}
    }
  
  wgetch(stmtInsertWin);
  hide_panel(stmtInsertPanel);
  update_panels();
  doupdate();
  endwin();
}


void printStmtFile(struct statement *start)
{
  int i= 0;

  WINDOW *upLoadStmtWindow;
  PANEL *upLoadStmtPanel;
  int srow = 0;
  int scol = 0;
  struct statement *ptr; 
  ptr = start;

  initscr();
  cbreak();
  noecho();

  keypad(stdscr, TRUE);

  upLoadStmtWindow = newwin(40, 170, 1, 1);
  upLoadStmtPanel = new_panel(upLoadStmtWindow);
  update_panels();
  doupdate();
  
  keypad(upLoadStmtWindow, TRUE);
  getmaxyx(upLoadStmtWindow, srow, scol);

  scrollok(upLoadStmtWindow, TRUE);

  box(upLoadStmtWindow, 0, 0);
  waddstr(upLoadStmtWindow, "Statement Up Load");

  if(upLoadStmtWindow == NULL) // || prStmtWindow == NULL)
     {
      addstr("Unable to create window");
      refresh();
      getch();
      }

  wrefresh(upLoadStmtWindow);

  mvwprintw(upLoadStmtWindow,3,2,"Date, Type, Description, Value, Account Number\n");
  while(ptr != NULL)
    {
      i++;
      mvwprintw(upLoadStmtWindow, i+4, 2,"%-12s %-5s %-75s %15s %17s %-20s\n", ptr->tDate, ptr->tType, ptr->tDescription, ptr->tValue, ptr->actNumber, ptr->tAlias);
       if (i == 20)
	{	  
	  wgetch(upLoadStmtWindow);	  
	  i = 0;
	} 
      ptr = ptr->next;
      wclrtobot(upLoadStmtWindow);
      wrefresh(upLoadStmtWindow);
    } 
  wgetch(upLoadStmtWindow);
  del_panel(upLoadStmtPanel);
  update_panels();
  delwin(upLoadStmtWindow);
  endwin();
}


char * fStmtName()
{
  WINDOW *fStmtUpWindow;
  PANEL *fStmtUpPanel;
  int frow, fcol;
  char *str;
  char ch;
  int i = 0;
  char f[FNAME] = "/tmp/";
  char *e; 
  int fExist = 0;

  initscr();
  cbreak();

  fStmtUpWindow = newwin(10, 110, 1, 1);
  fStmtUpPanel = new_panel(fStmtUpWindow);
  update_panels();
  doupdate();

  keypad(fStmtUpWindow, TRUE);
  getmaxyx(fStmtUpWindow, frow, fcol);
  box(fStmtUpWindow, 0, 0);
  waddstr(fStmtUpWindow, "Upload File");

  if(fStmtUpWindow == NULL)
    {
      addstr("Unable to create window");
      refresh();
      getch();
    }

  mvwprintw(fStmtUpWindow, 3, 2, "Enter file name: ");

  str = (char*)malloc(100 * sizeof(char));
  e = (char*)malloc(100 * sizeof(char));  

  ch = wgetch(fStmtUpWindow); 

  while(ch != '\n')
    {
      str[i] = ch;
      i++;
      ch = wgetch(fStmtUpWindow); 
    }
  str[i] = '\0'; 
  strcat(f,str);
  mvwprintw(fStmtUpWindow, 5, 2, "file to upload %s\n", f);
  wrefresh(fStmtUpWindow);
  fExist = checkFileExists(f);

  if(fExist == 2 || i == 0)
    {
      mvwprintw(fStmtUpWindow, 6, 3, "Error no file\n");
      wgetch(fStmtUpWindow);
      wrefresh(fStmtUpWindow);
      return NULL;
    }
  else
    {
      strcpy(e,f);
      return e;
    }
 
  free(str);
  free(e);

  wclear(fStmtUpWindow);

  hide_panel(fStmtUpPanel);
  update_panels();
  doupdate();
  delwin(fStmtUpWindow);
  endwin();  
}
