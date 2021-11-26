/* Processed by ecpg (13.3) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "fdbcon.pcg"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "../inc/fdbcon.h"


/* exec sql begin declare section */
   

#line 9 "fdbcon.pcg"
 char dbname [ 1024 ] ;
/* exec sql end declare section */
#line 10 "fdbcon.pcg"


void print_sqlca()
{
  fprintf(stderr, "sqlerrm.sqlerrmc: %s\n", sqlca.sqlerrm.sqlerrmc);
}

void connectToDB(){

  /* exec sql whenever sqlerror  stop ; */
#line 19 "fdbcon.pcg"

  
  { ECPGconnect(__LINE__, 0, "jadebld" , "gianpietro" , NULL , NULL, 0); 
#line 21 "fdbcon.pcg"

if (sqlca.sqlcode < 0) exit (1);}
#line 21 "fdbcon.pcg"

     /* exec sql whenever sqlerror  continue ; */
#line 22 "fdbcon.pcg"
  
  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "select current_database ( )", ECPGt_EOIT, 
	ECPGt_char,(dbname),(long)1024,(long)1,(1024)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);}
#line 23 "fdbcon.pcg"
    

  //printf("current=%s\n", dbname);
    
  //printf("connection successful\n");

}
