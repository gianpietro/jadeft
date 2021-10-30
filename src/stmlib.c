/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "stmlib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/stmlib.h"
#include "../inc/stmuplibf.h"
#include "../inc/fdbcon.h"

void stmtInsert(int stmtTranDate, char stmtTranType[], char stmtTranDesc[], double stmtValue, char stmtAcctNo[], char stmtTranAlias[])
{
  /* exec sql begin declare section */
    
    
    
    
    
    
  
#line 12 "stmlib.pcg"
 int v_stmtTranDate ;
 
#line 13 "stmlib.pcg"
 char v_stmtTranType [ TTYPE ] ;
 
#line 14 "stmlib.pcg"
 char v_stmtTranDesc [ TDESC ] ;
 
#line 15 "stmlib.pcg"
 double v_stmtValue ;
 
#line 16 "stmlib.pcg"
 char v_stmtAcctNo [ ANUM ] ;
 
#line 17 "stmlib.pcg"
 char v_stmtTranAlias [ ALIAS ] ;
/* exec sql end declare section */
#line 18 "stmlib.pcg"

  
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
#line 30 "stmlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 31 "stmlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 33 "stmlib.pcg"

}

  

