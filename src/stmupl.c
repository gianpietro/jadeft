#define _GNU_SOURCE
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

/* Function to read the downloaded bank statement file, selects required
   columns for data import and uploads these to a linked list */
//struct statement *upLoadStatement()
void upLoadStatement()
{
  WINDOW *stmtWin;
  PANEL *stmtPanel;
  struct statement *start, *newStmtPtr, *end, *ptr;
  FILE *cp,*np;  /* pointers to file */
  int row, col;
  int i = 0;                                                             /* count of rows in the statement file download */
  int j = 0;                                                             /* count of characters in the date column */
  int j2 = 0;                                                            /* count of characters in the type column */ 
  int j3 = 0;                                                            /* count of characters in the description column */ 
  int j4 = 0;                                                            /* count of character in the value column */
  int j5 = 0;                                                            /* count of characters in the account number column */
  int c = 0, n = 0;                                                      /* character selected from file */
  int h = 0;                                                             /* various counters for loops and array */
  int q = 0;
  int x = 0;
  int l = 0;
  int g = 0;                                                             /* count for free up of memory */ 
  int qcount = 0;                                                        /* count of quotation marks seperating columns in file */
  int kr= 0, kc = 0;                                                     /* count of rows kr and characters kc in the file */
  char **tmpDate;                                                        /* temporary store for date */
  char **transDate;                                                      /* transaction date on statement formatted yyyymmdd */
  char **transType;                                                      /* Type column contains up to 3 characters */
  char **transDescription;                                               /* Description column on statement */
  char **transValue;                                                     /* Value column on statement */
  char **accountNumber;                                                  /* Account Number on statement */
  char **transAlias;                                                     /* Alias value matched from statement_link table */
  char **stmtAliasRtn;                                                   /* alias value returned from statement_link table */  
  int w = 0;
  int aliasPos = 0;                                                      /* position where alias is found in description */ 
  int resRow = 0;                                                        /* number of rows returned from statment_link table */
  int cr, cc;  
  int tr = 0;                                                            /* number or lines before first QM, top rows */
  int rs = 0;                                                            /* number of statement rows excluding top rows before data. */
  long int pos = 0;                                                      /* position of file cursor c */
  long int posn = 0;                                                     /* position of file cursor n */  
  int charCount = 0;                                                     /* number of characters in each data column */

  char * upf = fStmtName();                                              /* function to obtain filename to load and if exists */
 
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  
  stmtWin = newwin(LINES*0.75, COLS*0.75, LINES-(LINES-4), COLS*0.1);
  stmtPanel = new_panel(stmtWin);
  //touchwin(stmtWin);
  //touchwin(stdscr);
  update_panels();
  doupdate();

  getmaxyx(stmtWin, row, col);
  
  if(upf != NULL)
    {
      cp = fopen(upf, "r");
      np = fopen(upf, "r");
  
      while (c != EOF) 
	{
	  kc++;                                                          /* number of characters in the file */       
	  c = fgetc(cp);      
	  if (c == '\n')	
	    kr++;                                                        /* number of rows in the file */
	}

      rewind(cp);                                                        /* return cursor to beginning of file */

      while (c!= QM)
	{
	  c = fgetc(cp);  
	  n = fgetc(np);
	  if (c == '\n')
	    tr++;	                                                 /* number of rows before first qm */
	}

      fseek(cp, -1, SEEK_CUR);
      fseek(np, -1, SEEK_CUR);
       
      rs = kr - tr;                                                      /* number of valid rows in statement excludes top empty rows */

      tmpDate = (char **) malloc(rs * sizeof(char *));
      transDate = (char **) malloc (rs * sizeof(char *));
      transType = (char **) malloc (rs * sizeof(char *));
      transDescription = (char **) malloc (rs * sizeof(char *));
      transValue = (char **) malloc (rs * sizeof(char *));
      accountNumber = (char **) malloc (rs * sizeof(char *));
      transAlias = (char **) malloc (rs * sizeof(char *));
      for (h = 0; h < rs; h++)
	{
	  tmpDate[h] = (char *)malloc(TDATE+1 * sizeof(char));
	  transDate[h] = (char *)malloc(TDATE+1 * sizeof(char));
	  transType[h] = (char *)malloc(TTYPE+1 * sizeof(char));
	  transDescription[h] = (char *)malloc(TDESC+1 * sizeof(char));
	  transValue[h] = (char *)malloc(TVALUE+1 * sizeof(char));
	  accountNumber[h] = (char *)malloc(ANUM+1 * sizeof(char));
	  transAlias[h] = (char *)malloc(ALIAS+1 * sizeof(char));
	}

      if (tmpDate == NULL ||
	  transDate == NULL ||
	  transType == NULL ||
	  transDescription == NULL ||
	  transValue == NULL ||
	  accountNumber == NULL ||
	  transAlias == NULL)
	  printf ("Memory not available");
	  
  
      while (c != EOF)
	{
	  c = fgetc(cp);
	  pos = ftell(cp);      
	  if (c == '\n')
	    {
	      tmpDate[i][j+1] = '\0';                                    /* add end of string char when reach end of row */
	      transType[i][j2+1] = '\0';
	      transDescription[i][j3+1] = '\0';
	      transValue[i][j4+1] = '\0';
	      accountNumber[i][j5+1] = '\0';
	      j = j2 = j3 = j4 = j5 = 0;
	      ++i;
	      qcount = 0;
	    }

	  if ( c == QM || qcount == 6 || qcount == 8)                    /* 6 and 8 required for data Value with no qm */
	    {
	      qcount++;
	      fseek(np, pos, SEEK_SET);                                  /* cursor np at first qm */
	      do{                            
		n = fgetc(np);                                           /* move one char until next qm */ 
		posn = ftell(np);
		charCount++;                                             /* count no. of characters between qms */
		if (n != SP && n != CM && n != FS && n != AP)            /* not space, comma, forward slash, apostrophe */
		  {
		    if (qcount == 1 && n != QM)                          /* Date column */ 
		      {
			tmpDate[i][j] = n;
			++j;
		      }
		    if (qcount == 3)                                     /* Type column */
		      {
			if (charCount == 1 && n == QM)                   /* If Type blank */
			  {
			    transType[i][j2] = 'Z';
			  }
			else if (n != QM)
			  {
			    transType[i][j2] = n;
			  }
			++j2;		      
		      }
		    if (qcount == 5 && n != QM)                          /* Description column */
		      {
			transDescription[i][j3] = n;
			++j3;
		      }
		    if (qcount == 7 && n != QM)                          /* Value column */ 
		      {
			transValue[i][j4] = n;
			++j4;
		      }
		    if (qcount == 13 && n != QM)                         /* Account number column */
		      {
			accountNumber[i][j5] = n;
			++j5;
		      }		
		  }	                                    
	      }while (n != QM);	  
	      fseek(cp, posn, SEEK_SET);                                 /* move cursor cp to current np pos */
	      qcount++;
	      pos = ftell(cp);
	    }
	  charCount = 0;
	}

      for (q = 0; q < i; q++)                                            /* arrange date format to yyyymmdd */
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
 
      resRow = resultRows();                                             /* return number of rows in the statement_link table */
      stmtAliasRtn = addAlias();                                         /* string array of alias column in the statement_link table */

      /* match the string patter in the Decription to the alias in the
	 statement_link table */
      for (x = 0; x < i; x++)
	{ 
	  for (w = 0; w < resRow; w++)
	    {
	      aliasPos = aliasMatch(transDescription[x], stmtAliasRtn[w]);
	      if (aliasPos != -1)
		{
		  strcpy(transAlias[x], stmtAliasRtn[w]);	         /* assign alias to array */
		  break;
		}
	      else	     
		strcpy(transAlias[x],"NA");	                         /* if no alias match found assign NA */    
	    }
	}

      for (x = 0; x < i; x++)                                            /* assign statement data to the linked list */
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

      printStmtFile(start);
      statementInsert(start);
  
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

      free(upf);
      free(stmtAliasRtn);
      freeStatement(start);

      fclose(cp);
      fclose(np);
      
      //return start;
    }
  hide_panel(stmtPanel);
  update_panels();
  doupdate();
  delwin(stmtWin);
  endwin();
}

/* Select values in the statement_link table and assign values to 
   char pointer */
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

  PQclear(res);
  PQfinish(conn); 

  return stmtAlias;
}

/* select values from the statement_link table and count
   the number of rows returned. To use for string match loop */
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

/* Insert function which will insert the linked list statement
   data into the statement table of database using ecpg function */
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
  noecho();

  stmtInsertWin = newwin(10, 110, 1, 1);
  stmtInsertPanel = new_panel(stmtInsertWin);
  show_panel(stmtInsertPanel);
  touchwin(stmtInsertWin);
  touchwin(stdscr);
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
 
  mvwprintw(stmtInsertWin, 3, 2, "Confirm insert statement (y/n): ");
  wmove(stmtInsertWin, 3, 35);
  echo();
  while ((ch = wgetch(stmtInsertWin)) != 'y')
    {
      wmove(stmtInsertWin, 3, 35);
      if (ch == 'n')
	{
	  mvwprintw(stmtInsertWin, 4, 2, "Statement not saved");
	  break;
	}
    }
  if (ch == 'y')
    {
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
      mvwprintw(stmtInsertWin, 4, 2, "Statement saved");
    }
  
  wgetch(stmtInsertWin);
  hide_panel(stmtInsertPanel);
  update_panels();
  doupdate();
  endwin();
}

/* Print link list to terminal */
void printStmtFile(struct statement *start)
{
  int i= 0;

  WINDOW *upLoadStmtWindow, *disStmtWin;
  PANEL *upLoadStmtPanel;
  int srow = 0;
  int scol = 0;
  struct statement *ptr;
  const char *titleOne = "Statement Up-Load";
  int lenOne = strlen(titleOne);

  ptr = start;

  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  init_pair(1,COLOR_WHITE,COLOR_BLUE);
  init_pair(2,COLOR_BLUE,COLOR_WHITE);
  init_pair(3,COLOR_YELLOW,COLOR_MAGENTA);
  init_pair(4,COLOR_WHITE,COLOR_CYAN);  
  init_pair(6,COLOR_BLACK,COLOR_YELLOW);
  init_pair(8,COLOR_BLACK,COLOR_WHITE); 
  init_pair(9,COLOR_WHITE,COLOR_BLACK);
  init_pair(10,COLOR_BLACK,COLOR_CYAN);
  init_pair(11,COLOR_YELLOW,COLOR_BLUE);

  upLoadStmtWindow = newwin(LINES*0.75, COLS*0.75, LINES-(LINES-4), COLS*0.1);                          //(40, 170, 1, 1);
  upLoadStmtPanel = new_panel(upLoadStmtWindow);
  //wbkgd(upLoadStmtWindow, COLOR_PAIR(11));
  //touchwin(upLoadStmtWindow);
  //touchwin(stdscr);
  update_panels();
  doupdate();
  
  keypad(upLoadStmtWindow, TRUE);
  getmaxyx(upLoadStmtWindow, srow, scol);

  disStmtWin = derwin(upLoadStmtWindow, srow-2, scol-2, 1, 1);

  scrollok(upLoadStmtWindow, TRUE);
  scrollok(disStmtWin, TRUE);
  

  box(upLoadStmtWindow, 0, 0);
  //touchwin(upLoadStmtWindow);
  // waddstr(upLoadStmtWindow, "Statement Up Load");
  waddstr(disStmtWin, "Statement upload");
  if(upLoadStmtWindow == NULL) 
     {
      addstr("Unable to create window");
      refresh();
      getch();
      }

  hide_panel(upLoadStmtPanel);
  update_panels();
  doupdate();
  
  //wattron(disStmtWin,A_BOLD | COLOR_PAIR(11));     /* ATTON_MAIN_WIN_TITLE */
  //mvwprintw(disStmtWin,1,(scol-lenOne)/2,titleOne);   /* SET_MAIN_WIND_TITLE */
  
  //wattroff(disStmtWin,A_BOLD | COLOR_PAIR(2));    /* ATTOFF_MAIN_WIN_TITLE */
  wrefresh(disStmtWin);

  // wattron(disStmtWin,A_BOLD | COLOR_PAIR(11));     /* ATTON_MAIN_WIN_TITLE */
  mvwprintw(disStmtWin,6,2,"Date, Type, Description, Value, Account Number\n");
  while(ptr != NULL)
    {     
      i++;
      mvwprintw(disStmtWin, i+8, 2,"%-12s %-5s %-75s %15s %17s %-20s\n", ptr->tDate, ptr->tType, ptr->tDescription, ptr->tValue, ptr->actNumber, ptr->tAlias);
      if (i == 20)                                                       
	{
	  box(disStmtWin, 0, 0);
	  wgetch(disStmtWin);	                                 /* if 20 rows hit enter */
	  i = 0;                                                         /* if 20 rows set i to 0 */
	} 
      ptr = ptr->next;
      wclrtobot(disStmtWin);
      box(disStmtWin, 0, 0);
      // wrefresh(upLoadStmtWindow);
    }
 
  // wattroff(upLoadStmtWindow,A_BOLD | COLOR_PAIR(11));    /* ATTOFF_MAIN_WIN_TITLE */
  wgetch(upLoadStmtWindow);
  //hide_panel(upLoadStmtPanel);  
  //update_panels();
  //doupdate();
  delwin(upLoadStmtWindow);
  endwin();
}


/* Function to enter file name and check that it exists */
char * fStmtName()
{
  WINDOW *fStmtUpWindow;
  PANEL *fStmtUpPanel;
  int frow, fcol;
  char *str;                                                             /* string array of entered characters */
  char ch;                                                               /* char entered at keyboard */
 
  int i = 0;
  char f[FNAME] = "/tmp/";                                               /* directory where file saved */
  char *e;                                                               /* full file name with path */
  int fExist = 0;
  const char *titleOne = "Upload File";
  int lenOne = strlen(titleOne);

  initscr();
  start_color();
  cbreak();
  noecho();
  
  init_pair(2,COLOR_BLUE,COLOR_WHITE);

  fStmtUpWindow = newwin(LINES/5, COLS/2, LINES-(LINES-4), COLS/4); //(10, 110, 1, 1);
  fStmtUpPanel = new_panel(fStmtUpWindow);
  wbkgd(fStmtUpWindow, COLOR_PAIR(2));
  touchwin(fStmtUpWindow);
  touchwin(stdscr);
  update_panels();
  doupdate();

  keypad(fStmtUpWindow, TRUE);
  getmaxyx(fStmtUpWindow, frow, fcol);
  box(fStmtUpWindow, 0, 0);
  //waddstr(fStmtUpWindow, "Upload File");
  wattron(fStmtUpWindow,A_BOLD | COLOR_PAIR(2));
  mvwprintw(fStmtUpWindow,1,(fcol-lenOne)/2, titleOne);
  wattroff(fStmtUpWindow,A_BOLD | COLOR_PAIR(2));

  if(fStmtUpWindow == NULL)
    {
      addstr("Unable to create window");
      refresh();      
     getch();
    }

  mvwprintw(fStmtUpWindow, 3, 2, "Enter file name: ");

  str = (char*)malloc(FNAME * sizeof(char));
  e = (char*)malloc(FNAME * sizeof(char));

  for (i=0; (ch = wgetch(fStmtUpWindow))!= '\n';)
    {
      if(ch != BACKSP)                                                    /* used showkey -a  ctrl G = 7 */
	{
	  str[i] = ch;
	  wprintw(fStmtUpWindow, "%c", ch);
	  i++;
	  if(i == FNAME)
	    {	     
	      i = 0;
	      wmove(fStmtUpWindow, 3,19);
	      wclrtoeol(fStmtUpWindow);
	    }
	}
      else
	{
	  i--;
	  if(i<0)
	    i++;
	  else
	    wprintw(fStmtUpWindow, "\b \b");
	}      
    }
  str[i] = '\0';                                                         /* if new line add the string terminate char */
  strcat(f,str);                                                         /* concantenate f and str */
  mvwprintw(fStmtUpWindow, 5, 2, "file to upload %s\n", f);
  wrefresh(fStmtUpWindow);
  fExist = checkFileExists(f);                                           /* use library function to check file exists */

  if(fExist == 2 || i == 0)                                              /* if no file */
    {
      mvwprintw(fStmtUpWindow, 6, 3, "Error no file\n");
      wgetch(fStmtUpWindow);
      hide_panel(fStmtUpPanel);
      wrefresh(fStmtUpWindow);
      free(str);
      free(e);
      endwin();
      return NULL;
    }
  else
    {
      strcpy(e,f);                                                       /* copy path and file name to char pointer */
      hide_panel(fStmtUpPanel);
      free(str);
      endwin();
      return e;      
    }
}
