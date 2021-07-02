/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "doclib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/doclib.h"
#include "../inc/fdbcon.h"

void docTypeInsert(char dtDesc[])
{
  /* exec sql begin declare section */
    
  
#line 11 "doclib.pcg"
 char v_dtDesc [ 30 ] ;
/* exec sql end declare section */
#line 12 "doclib.pcg"


  connectToDB();

  strcpy(v_dtDesc, dtDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into document_type ( description ) values ( $1  )", 
	ECPGt_char,(v_dtDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 19 "doclib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 20 "doclib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 22 "doclib.pcg"

}

void docTypeUpdate(int upID,char dtDesc[])
{
  /* exec sql begin declare section */
    
    
  
#line 28 "doclib.pcg"
 int v_upID ;
 
#line 29 "doclib.pcg"
 char v_dtDesc [ 30 ] ;
/* exec sql end declare section */
#line 30 "doclib.pcg"


  connectToDB();

  v_upID = upID;
  strcpy(v_dtDesc, dtDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update document_type set description = $1  where type_id = $2 ", 
	ECPGt_char,(v_dtDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 38 "doclib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 39 "doclib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 41 "doclib.pcg"

}

void docTypeDelete(int upID)
{
 /* exec sql begin declare section */
    
    
  
#line 47 "doclib.pcg"
 int v_upID ;
 
#line 48 "doclib.pcg"
 char v_dtDesc [ 30 ] ;
/* exec sql end declare section */
#line 49 "doclib.pcg"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from document_type where type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 56 "doclib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 57 "doclib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 59 "doclib.pcg"

}

 