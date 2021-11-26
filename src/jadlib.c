#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <form.h>
#include <unistd.h>      /* access() used to check file exists */
#include <libpq-fe.h>
#include "../inc/jadlib.h"

/* API database connection method */
PGconn * fdbcon()
{  
  PGconn *connection = PQconnectdb("user=gianpietro dbname=jadebld");
  if(PQstatus(connection) == CONNECTION_BAD)
    {
      PQfinish(connection);
      exit(1);
    }  
  return connection;
}

/* Form navigation keys */
void keyNavigate(int ch, FORM * f)
{ 
  switch(ch)
    {
    case KEY_DOWN:
      form_driver(f, REQ_NEXT_FIELD);
      form_driver(f, REQ_END_LINE);
      break;
    case KEY_UP:
      form_driver(f, REQ_PREV_FIELD);
      form_driver(f, REQ_END_LINE);
      break;
    case KEY_BACKSPACE:
      form_driver(f, REQ_CLR_FIELD);	  
      break;
    case 10:                              /* ASCII value for carriage return */
      //form_driver(f, REQ_VALIDATION);
      form_driver(f, REQ_NEXT_FIELD);
      break;
    default:
      form_driver(f, ch);
      break;
    }
}

/* Trim trailing whitespace from the string entered in form field */
char * trimWS(char *s)
{
  int i = 0;
  int index = -1;
  				
  while (s[i] != '\0')
    {
      if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
	{
	  index = i;
	}
      i++;
    }
  s[index + 1] = '\0';

  return s;
}

int checkFileExists(char fName[])
{  
  if(access(fName, F_OK) == 0)
    return 1;                 /* file exists */
  else
    return 2;     
}
