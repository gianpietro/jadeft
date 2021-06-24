/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "invlib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/invlib.h"
#include "../inc/fdbcon.h"

void invoiceInsert(char invfNo[], int invfStartDt, int invfEndDt, int invfSupAcctID, char invfDesc[], double invfAmount)
{
  /* exec sql begin declare section */
    
    
    
    
    
    
  
#line 11 "invlib.pcg"
 char v_invfNo [ 30 ] ;
 
#line 12 "invlib.pcg"
 int v_invfStartDt ;
 
#line 13 "invlib.pcg"
 int v_invfEndDt ;
 
#line 14 "invlib.pcg"
 int v_invfSupAcctID ;
 
#line 15 "invlib.pcg"
 char v_invfDesc [ 30 ] ;
 
#line 16 "invlib.pcg"
 double v_invfAmount ;
/* exec sql end declare section */
#line 17 "invlib.pcg"


  connectToDB();

   strcpy(v_invfNo, invfNo);
   v_invfStartDt = invfStartDt;
   v_invfEndDt = invfEndDt;
   v_invfSupAcctID = invfSupAcctID;
   strcpy(v_invfDesc, invfDesc);
   v_invfAmount = invfAmount;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier_invoice ( invoice_no , start_date , end_date , supplier_acct_id , description , amount ) values ( $1  , $2  , $3  , $4  , $5  , $6  )", 
	ECPGt_char,(v_invfNo),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfSupAcctID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_invfDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_invfAmount),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 30 "invlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 31 "invlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 33 "invlib.pcg"

}


void invoiceUpdate(int upID, char invfNo[], int invfStartDt, int invfEndDt, int invfSupAcctID, char invfDesc[], double invfAmount)
{
  /* exec sql begin declare section */
    
    
    
    
    
    
    
  
#line 40 "invlib.pcg"
 int v_upID ;
 
#line 41 "invlib.pcg"
 char v_invfNo [ 30 ] ;
 
#line 42 "invlib.pcg"
 int v_invfStartDt ;
 
#line 43 "invlib.pcg"
 int v_invfEndDt ;
 
#line 44 "invlib.pcg"
 int v_invfSupAcctID ;
 
#line 45 "invlib.pcg"
 char v_invfDesc [ 30 ] ;
 
#line 46 "invlib.pcg"
 double v_invfAmount ;
/* exec sql end declare section */
#line 47 "invlib.pcg"


  connectToDB();

   v_upID = upID;
   strcpy(v_invfNo, invfNo);
   v_invfStartDt = invfStartDt;
   v_invfEndDt = invfEndDt;
   v_invfSupAcctID = invfSupAcctID;
   strcpy(v_invfDesc, invfDesc);
   v_invfAmount = invfAmount;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update supplier_invoice set invoice_no = $1  , start_date = $2  , end_date = $3  , supplier_acct_id = $4  , description = $5  , amount = $6  where supplier_invoice_id = $7 ", 
	ECPGt_char,(v_invfNo),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_invfSupAcctID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_invfDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_invfAmount),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 62 "invlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 63 "invlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 65 "invlib.pcg"

}

void invoiceDelete(int upID)
{
  /* exec sql begin declare section */
    
  
#line 71 "invlib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 72 "invlib.pcg"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier_invoice where supplier_invoice_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 79 "invlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 80 "invlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 82 "invlib.pcg"

}


  
			       
			       
   
