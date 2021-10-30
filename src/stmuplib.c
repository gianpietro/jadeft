#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/stmuplibf.h"

struct statement *importStmt(char transactionDate[], char transactionType[], char transactionDescription[], char transactionValue[], char accountNumber[], char transactionAlias[])
{
  struct statement *ptr;
  int i = 0;

  ptr =  (struct statement *)malloc(sizeof(struct statement));

  for (i=0; i<TDATE; i++)
    ptr->tDate[i] = transactionDate[i];
  for (i=0; i<TTYPE; i++)
    ptr->tType[i] = transactionType[i];
  for (i=0; i<TDESC; i++)
    ptr->tDescription[i] = transactionDescription[i];
  for (i=0; i<TVALUE; i++)
    ptr->tValue[i] = transactionValue[i];
  for (i=0; i<ANUM; i++)
    ptr->actNumber[i] = accountNumber[i];
  for (i=0; i<ALIAS; i++)
    ptr->tAlias[i] = transactionAlias[i];
 
  return ptr;    
}

struct statement *append(struct statement *end, struct statement *newpt)
{
  end->next = newpt;
  return (end->next);
}

void printStatement(struct statement *start)
{
  struct statement *ptr;
  ptr = start;
  //int count = 0;

  printf("Date, Type, Description, Value, Account Number\n");
  while(ptr != NULL)
    { 
      printf("%s %s %s %s %s %s\n", ptr->tDate, ptr->tType, ptr->tDescription, ptr->tValue, ptr->actNumber, ptr->tAlias);
      ptr = ptr->next;
      //count++;
    }
}

void freeStatement(struct statement *start)
{
  struct statement *ptr;
  struct statement *tmp;
  ptr = start;
  while (ptr !=  NULL)
    {
      tmp = ptr->next;
      free(ptr);
      ptr = tmp;
    }
}

int aliasMatch(char *a, char *b)
{
  int c;
  int position = 0;
  char *x, *y;
   
  x = a;
  y = b;
   
  while(*a)
    {
      while(*x==*y)
	{	
	  x++;
	  y++;
	  if(*x=='\0'||*y=='\0')
            break;        
	}  
      if(*y=='\0')
	break;

      a++;
      position++;
      x = a;
      y = b;
    }
  if(*a)
    return position;
  else  
    return -1;  
}

