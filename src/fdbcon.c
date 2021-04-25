/* Processed by ecpg (12.4) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "fdbcon.pcg"
#include <stdio.h>
#include "../inc/fdbcon.h"


/* exec sql begin declare section */
   

#line 6 "fdbcon.pcg"
 char dbname [ 1024 ] ;
/* exec sql end declare section */
#line 7 "fdbcon.pcg"


void print_sqlca(){
  fprintf(stderr, "sqlerrm.sqlerrmc: %s\n", sqlca.sqlerrm.sqlerrmc);
}

void connectToDB(){
  { ECPGconnect(__LINE__, 0, "jadedev" , "gianpietro" , NULL , NULL, 0); }
#line 14 "fdbcon.pcg"

    
  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "select current_database ( )", ECPGt_EOIT, 
	ECPGt_char,(dbname),(long)1024,(long)1,(1024)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);}
#line 16 "fdbcon.pcg"
    

  printf("current=%s\n", dbname);

  /* exec sql whenever sqlerror  call print_sqlca ( ) ; */
#line 20 "fdbcon.pcg"


  if(dbname == NULL)
    print_sqlca();
  else
    printf("connection successful\n");

//  EXEC SQL DISCONNECT;
}
