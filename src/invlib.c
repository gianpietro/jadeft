/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "invlib.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/invlib.h"
#include "../inc/fdbcon.h"

void invoiceInsert(char invfNo[], int invfStartDt, int invfEndDt, int invfSupAcctID, char invfDesc[], double invfAmount)
{
  /* exec sql begin declare section */
    
    
    
    
    
    
  
#line 12 "invlib.pgc"
 char v_invfNo [ 30 ] ;
 
#line 13 "invlib.pgc"
 int v_invfStartDt ;
 
#line 14 "invlib.pgc"
 int v_invfEndDt ;
 
#line 15 "invlib.pgc"
 int v_invfSupAcctID ;
 
#line 16 "invlib.pgc"
 char v_invfDesc [ 30 ] ;
 
#line 17 "invlib.pgc"
 double v_invfAmount ;
/* exec sql end declare section */
#line 18 "invlib.pgc"


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
#line 31 "invlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 32 "invlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 34 "invlib.pgc"

}


void invoiceUpdate(int upID, char invfNo[], int invfStartDt, int invfEndDt, int invfSupAcctID, char invfDesc[], double invfAmount)
{
  /* exec sql begin declare section */
    
    
    
    
    
    
    
  
#line 41 "invlib.pgc"
 int v_upID ;
 
#line 42 "invlib.pgc"
 char v_invfNo [ 30 ] ;
 
#line 43 "invlib.pgc"
 int v_invfStartDt ;
 
#line 44 "invlib.pgc"
 int v_invfEndDt ;
 
#line 45 "invlib.pgc"
 int v_invfSupAcctID ;
 
#line 46 "invlib.pgc"
 char v_invfDesc [ 30 ] ;
 
#line 47 "invlib.pgc"
 double v_invfAmount ;
/* exec sql end declare section */
#line 48 "invlib.pgc"


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
#line 63 "invlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 64 "invlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 66 "invlib.pgc"

}

void invoiceDelete(int upID)
{
  /* exec sql begin declare section */
    
  
#line 72 "invlib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 73 "invlib.pgc"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier_invoice where supplier_invoice_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 80 "invlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 81 "invlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 83 "invlib.pgc"

}


  
			       
			       
   
