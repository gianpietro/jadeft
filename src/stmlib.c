/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "stmlib.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/stmlib.h"
#include "../inc/stmuplibf.h"
#include "../inc/fdbcon.h"

void stmtInsert(int stmtTranDate, char stmtTranType[], char stmtTranDesc[], double stmtValue,
                char stmtAcctNo[], char stmtTranAlias[])
{
  /* exec sql begin declare section */
    
    
    
    
    
    
  
#line 14 "stmlib.pgc"
 int v_stmtTranDate ;
 
#line 15 "stmlib.pgc"
 char v_stmtTranType [ TTYPE ] ;
 
#line 16 "stmlib.pgc"
 char v_stmtTranDesc [ TDESC ] ;
 
#line 17 "stmlib.pgc"
 double v_stmtValue ;
 
#line 18 "stmlib.pgc"
 char v_stmtAcctNo [ ANUM ] ;
 
#line 19 "stmlib.pgc"
 char v_stmtTranAlias [ ALIAS ] ;
/* exec sql end declare section */
#line 20 "stmlib.pgc"

  
  connectToDB();

  v_stmtTranDate = stmtTranDate;
  strcpy(v_stmtTranType, stmtTranType);
  strcpy(v_stmtTranDesc, stmtTranDesc);
  v_stmtValue = stmtValue;
  strcpy(v_stmtAcctNo, stmtAcctNo);
  strcpy(v_stmtTranAlias, stmtTranAlias);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into statement ( date , type , description , value , account , alias ) values ( $1  , $2  , $3  , $4  , $5  , $6  )", 
	ECPGt_int,&(v_stmtTranDate),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_stmtTranType),(long)TTYPE,(long)1,(TTYPE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_stmtTranDesc),(long)TDESC,(long)1,(TDESC)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_stmtValue),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_stmtAcctNo),(long)ANUM,(long)1,(ANUM)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_stmtTranAlias),(long)ALIAS,(long)1,(ALIAS)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 33 "stmlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 34 "stmlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 36 "stmlib.pgc"

}

void recordUpdate(int stID, int fv2, char fv3[], char fv4[], double fv5, char fv6[], char fv7[])
{
  /* exec sql begin declare section */
    
    
    
    
    
    
    
  
#line 42 "stmlib.pgc"
 int v_stID ;
 
#line 43 "stmlib.pgc"
 int v_fv2 ;
 
#line 44 "stmlib.pgc"
 char v_fv3 [ TTYPE ] ;
 
#line 45 "stmlib.pgc"
 char v_fv4 [ TDESC ] ;
 
#line 46 "stmlib.pgc"
 double v_fv5 ;
 
#line 47 "stmlib.pgc"
 char v_fv6 [ ANUM ] ;
 
#line 48 "stmlib.pgc"
 char v_fv7 [ ALIAS ] ;
/* exec sql end declare section */
#line 49 "stmlib.pgc"


  connectToDB();

  v_stID = stID;
  v_fv2 = fv2;
  strcpy(v_fv3, fv3);
  strcpy(v_fv4, fv4);
  v_fv5 = fv5;
  strcpy(v_fv6, fv6);
  strcpy(v_fv7, fv7);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update statement set date = $1  , type = $2  , description = $3  , value = $4  , account = $5  , alias = $6  where statement_id = $7 ", 
	ECPGt_int,&(v_fv2),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_fv3),(long)TTYPE,(long)1,(TTYPE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_fv4),(long)TDESC,(long)1,(TDESC)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_fv5),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_fv6),(long)ANUM,(long)1,(ANUM)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_fv7),(long)ALIAS,(long)1,(ALIAS)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_stID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 63 "stmlib.pgc"


  { ECPGtrans(__LINE__, NULL, "commit");}
#line 65 "stmlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 67 "stmlib.pgc"
 
}

void recordDelete(int stID)
{
 /* exec sql begin declare section */
      
  
#line 73 "stmlib.pgc"
 int v_stID ;
/* exec sql end declare section */
#line 74 "stmlib.pgc"


  connectToDB();

  v_stID = stID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from statement where statement_id = $1 ", 
	ECPGt_int,&(v_stID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 81 "stmlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 82 "stmlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 84 "stmlib.pgc"

}


