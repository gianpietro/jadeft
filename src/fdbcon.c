/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "fdbcon.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "../inc/fdbcon.h"


/* exec sql begin declare section */
   

#line 9 "fdbcon.pgc"
 char dbname [ 1024 ] ;
/* exec sql end declare section */
#line 10 "fdbcon.pgc"


void print_sqlca()
{
  fprintf(stderr, "sqlerrm.sqlerrmc: %s\n", sqlca.sqlerrm.sqlerrmc);
}

void connectToDB(){

  /* exec sql whenever sqlerror  stop ; */
#line 19 "fdbcon.pgc"

  
  { ECPGconnect(__LINE__, 0, "jadebld" , "gianpietro" , NULL , NULL, 0); 
#line 21 "fdbcon.pgc"

if (sqlca.sqlcode < 0) exit (1);}
#line 21 "fdbcon.pgc"

     /* exec sql whenever sqlerror  continue ; */
#line 22 "fdbcon.pgc"
  
  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "select current_database ( )", ECPGt_EOIT, 
	ECPGt_char,(dbname),(long)1024,(long)1,(1024)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);}
#line 23 "fdbcon.pgc"
    

  //printf("current=%s\n", dbname);
    
  //printf("connection successful\n");

}
