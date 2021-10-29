#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libpq-fe.h>
#include "../inc/jadlib.h"
#include "../inc/stmuplibf.h"
#include "../inc/stmuplf.h"

void upLoadStatement()
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
  int p = 0;
  int q = 0;
  int x = 0;
  int l = 0;
  int g = 0;                    /* count for free up of memory */ 
  int qcount;                   /* count of quotation marks seperating columns in file */
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
  int aliasPos;                 /* position where alias is found in statement description */ 
  int resRow = 0;               /* number of rows returned from statment_link table */

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

  /*  assign dynamic memory to the data pointers */
  tmpDate = malloc(kr * sizeof(int *));
  transDate = malloc (kr * sizeof(int *));
  transType = malloc (kr * sizeof(int *));
  transDescription = malloc (kr * sizeof(int *));
  transValue = malloc (kr * sizeof(int *));
  accountNumber = malloc (kr * sizeof(int *));
  transAlias = malloc (kr * sizeof(int *));
  for (h = 0; h < kr; h++)
    {
      tmpDate[h] = (char *)malloc(TDATE * sizeof(char));
      transDate[h] = (char *)malloc(TDATE * sizeof(char));
      transType[h] = (char *)malloc(TTYPE * sizeof(char));
      transDescription[h] = (char *)malloc(TDESC * sizeof(char));
      transValue[h] = (char *)malloc(TVALUE * sizeof(char));
      accountNumber[h] = (char *)malloc(ANUM * sizeof(char));
      transAlias[h] = (char *)malloc(ALIAS * sizeof(char));
      } 

  /* move cursor to the first quotation mark in the file and read this
     then move one character back with fseek */
  while (c!= QM)
    {
      c = fgetc(cp);
      n = fgetc(np);
    }
  fseek(cp, -1, SEEK_CUR);
  fseek(np, -1, SEEK_CUR);

  while (c != EOF)
    {
      c = fgetc(cp);
      n = fgetc(np);
      if (c == QM)
	{
	  qcount++;	  
	  n = fgetc(np);  /*if qm check to see if next char is also a qm which means no value in column */
	  if (n == QM)
	    c = 'Z';  /* if next char is a qm then apply Z as a value to column */
	  fseek(np, -1, SEEK_CUR);  /* move cursor back one character */
	} 
      if (c != QM && c != SP && c != CM && c != FS && c != AP)
	{
	  if (qcount > 0 && qcount < 2)  /* first column of statment file which is the date */
	    {
	      tmpDate[i][j] = c;
	      j++;
	    }
	  if (qcount == 3)
	    j = 0;
	  if (qcount >= 3 && qcount < 4) /* second column of statment file which is the type */
	    {
	      transType[i][j2] = c;
	      j2++;
	    }
	  if (qcount == 5)
	    j2 = 0;
	  if (qcount >= 5 && qcount < 6) /* third column of statement file which is the description */
	    {	
	      transDescription[i][j3] = c;	     
	      j3++;	      
	    }
	  if (qcount == 6)  /* if qcount is 6 the next column, value, does not have qm around the number so add 1 to qcount to indicate start of value column */
	    qcount++;
	  if (qcount == 7)	 
	    j3 = 0;
	  if (qcount >= 7 && qcount < 8)  /* fourth column of statement file which is the value */
	    {
	      transValue[i][j4] = c;
	      j4++;	      
	    }
	  if (qcount == 8)  /* once value column has been assigned add 1 to qcount to indicate end of value column */
	    {
	      j4 = 0;
	      qcount++;
	    }
	  if (qcount >= 13 && qcount < 14)
	    {
	      accountNumber[i][j5] = c;
	      j5++;		
	    }
	  if (qcount == 14)  /* last qm in the current row */
	    {
	      j5 = 0;
	      qcount = 0;
	    }
	  if (c == '\n')  /* end of row  */
	    i++;
	}
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

   resRow = resultRows();  /* return number of rows in the statment_link table */
   printf("result rows %d\n", resRow);  // DEBUG
   stmtAliasRtn = addAlias();   /* string array of alias column in the statement_link table */
  
   printf("value of i %d\n", i); // DEBUD
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
  
  printStatement(start);

  fclose(cp);
  fclose(np);

  /* free linked list memory allocation */
  freeStatement(start); 

  /* free dynamic memory allocation for pointers to pointers */
  for (g = 0; g < i; g++)
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
}


char ** addAlias()
{
  PGconn *conn = fdbcon();
  PGresult *res;
  int rows;
  int j;
  int k;
  int h;
  int d;
  struct statement *ptr;
  char **stmtAlias;

  res = PQexec(conn, "SELECT * FROM statement_link ORDER BY id");
  rows = PQntuples(res);
  d = rows;

  //  for (j=0; j<rows; j++)
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
