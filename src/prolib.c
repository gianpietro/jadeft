/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "prolib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/prolib.h"
#include "../inc/fdbcon.h"

//EXEC SQL INCLUDE sqlca;


void providerInsert(int activeInd, char proName[])
{
   /* exec sql begin declare section */
       
       
   
#line 14 "prolib.pcg"
 int v_activeInd ;
 
#line 15 "prolib.pcg"
 char v_proName [ 30 ] ;
/* exec sql end declare section */
#line 16 "prolib.pcg"

   
   connectToDB();
     
   v_activeInd = activeInd;
   strcpy(v_proName,proName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider ( active_ind , provider_name ) values ( $1  , $2  )", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_proName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 24 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 25 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 27 "prolib.pcg"

}

void proTypeInsert(char proTypeDesc[])
{
   /* exec sql begin declare section */
      
   
#line 33 "prolib.pcg"
 char v_proTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 34 "prolib.pcg"

  
   connectToDB();

   strcpy(v_proTypeDesc, proTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider_type ( description ) values ( $1  )", 
	ECPGt_char,(v_proTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 41 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 42 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 44 "prolib.pcg"

}
 