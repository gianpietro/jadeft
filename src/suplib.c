/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "suplib.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/suplib.h"
#include "../inc/fdbcon.h"


void supplierInsert(int activeInd,char supName[])
{
   /* exec sql begin declare section */
       
       
   
#line 13 "suplib.pgc"
 int v_activeInd ;
 
#line 14 "suplib.pgc"
 char v_supName [ 30 ] ;
/* exec sql end declare section */
#line 15 "suplib.pgc"

   
   connectToDB();
     
   v_activeInd = activeInd;
   strcpy(v_supName,supName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier ( active_ind , supplier_name ) values ( $1  , $2  )", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_supName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 23 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 24 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 26 "suplib.pgc"

}

void supplierUpdate(int upID, int activeInd,char supName[])
{
   /* exec sql begin declare section */
       
       
       
   
#line 32 "suplib.pgc"
 int v_upID ;
 
#line 33 "suplib.pgc"
 int v_activeInd ;
 
#line 34 "suplib.pgc"
 char v_supName [ 30 ] ;
/* exec sql end declare section */
#line 35 "suplib.pgc"

   
   connectToDB();

   v_upID = upID;
   v_activeInd = activeInd;
   strcpy(v_supName,supName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update supplier set active_ind = $1  , supplier_name = $2  where supplier_id = $3 ", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_supName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 44 "suplib.pgc"
  
   { ECPGtrans(__LINE__, NULL, "commit");}
#line 45 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 47 "suplib.pgc"

}

void supplierDelete(int upID)
{
   /* exec sql begin declare section */
       
   
#line 53 "suplib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 54 "suplib.pgc"

   
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier where supplier_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 61 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 62 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 64 "suplib.pgc"

}

void supTypeInsert(char supTypeDesc[])
{
  /* exec sql begin declare section */
      
   
#line 70 "suplib.pgc"
 char v_supTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 71 "suplib.pgc"

  
   connectToDB();

   strcpy(v_supTypeDesc, supTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier_type ( description ) values ( $1  )", 
	ECPGt_char,(v_supTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 78 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 79 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 81 "suplib.pgc"

}

void supTypeUpdate(int upID, char supTypeDesc[])
{
   /* exec sql begin declare section */
      
      
   
#line 87 "suplib.pgc"
 int v_upID ;
 
#line 88 "suplib.pgc"
 char v_supTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 89 "suplib.pgc"

  
   connectToDB();

   v_upID = upID;
   strcpy(v_supTypeDesc, supTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update supplier_type set description = $1  where supplier_type_id = $2 ", 
	ECPGt_char,(v_supTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 97 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 98 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 100 "suplib.pgc"

}

void supTypeDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 106 "suplib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 107 "suplib.pgc"

  
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier_type where supplier_type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 114 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 115 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 117 "suplib.pgc"

}


void payPeriodInsert(char payPeriod[])
{
  /* exec sql begin declare section */
      
   
#line 124 "suplib.pgc"
 char v_payPeriod [ 30 ] ;
/* exec sql end declare section */
#line 125 "suplib.pgc"

  
   connectToDB();

   strcpy(v_payPeriod, payPeriod);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into payment_period ( period ) values ( $1  )", 
	ECPGt_char,(v_payPeriod),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 132 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 133 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 135 "suplib.pgc"

}

void payPerUpdate(int upID, char payPeriod[])
{
   /* exec sql begin declare section */
       
       
   
#line 141 "suplib.pgc"
 int v_upID ;
 
#line 142 "suplib.pgc"
 char v_payPeriod [ 30 ] ;
/* exec sql end declare section */
#line 143 "suplib.pgc"

   
   connectToDB();

   v_upID = upID;
   strcpy(v_payPeriod,payPeriod);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update payment_period set period = $1  where payment_period_id = $2 ", 
	ECPGt_char,(v_payPeriod),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 151 "suplib.pgc"
  
   { ECPGtrans(__LINE__, NULL, "commit");}
#line 152 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 154 "suplib.pgc"

}

void payPerDelete(int upID)
{
   /* exec sql begin declare section */
       
   
#line 160 "suplib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 161 "suplib.pgc"

   
   connectToDB();

   v_upID = upID;
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from payment_period where payment_period_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 168 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 169 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 171 "suplib.pgc"

}


void prtInsert(int actInd, char prtPostCode[], char prtAddress[], char prtCity[])
{
   /* exec sql begin declare section */
      
      
      
      
   
#line 178 "suplib.pgc"
 int v_actInd ;
 
#line 179 "suplib.pgc"
 char v_prtPostCode [ 10 ] ;
 
#line 180 "suplib.pgc"
 char v_prtAddress [ 30 ] ;
 
#line 181 "suplib.pgc"
 char v_prtCity [ 30 ] ;
/* exec sql end declare section */
#line 182 "suplib.pgc"

  
   connectToDB();

   v_actInd = actInd;
   strcpy(v_prtPostCode, prtPostCode);
   strcpy(v_prtAddress, prtAddress);
   strcpy(v_prtCity,prtCity);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into property ( active_ind , post_code , address , city ) values ( $1  , $2  , $3  , $4  )", 
	ECPGt_int,&(v_actInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtPostCode),(long)10,(long)1,(10)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtAddress),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtCity),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 193 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 194 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 196 "suplib.pgc"

}

void prtUpdate(int upID, int actInd, char prtPostCode[], char prtAddress[], char prtCity[])
{
   /* exec sql begin declare section */
      
      
      
      
      
   
#line 202 "suplib.pgc"
 int v_upID ;
 
#line 203 "suplib.pgc"
 int v_actInd ;
 
#line 204 "suplib.pgc"
 char v_prtPostCode [ 10 ] ;
 
#line 205 "suplib.pgc"
 char v_prtAddress [ 30 ] ;
 
#line 206 "suplib.pgc"
 char v_prtCity [ 30 ] ;
/* exec sql end declare section */
#line 207 "suplib.pgc"

  
   connectToDB();

   v_upID = upID;
   v_actInd = actInd;
   strcpy(v_prtPostCode, prtPostCode);
   strcpy(v_prtAddress, prtAddress);
   strcpy(v_prtCity,prtCity);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update property set active_ind = $1  , post_code = $2  , address = $3  , city = $4  where property_id = $5 ", 
	ECPGt_int,&(v_actInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtPostCode),(long)10,(long)1,(10)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtAddress),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_prtCity),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 219 "suplib.pgc"
  		 		 
   { ECPGtrans(__LINE__, NULL, "commit");}
#line 220 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 222 "suplib.pgc"

}

void prtDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 228 "suplib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 229 "suplib.pgc"

  
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from property where property_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 236 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 237 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 239 "suplib.pgc"

}

void supAccountInsert(int safActiveID ,char safSupAcctRef[],int safSupID, int safPrtID,
                      int safSupTypeID, int safStartDt, int safEndDt, int safPayID, double safAmount,
		      char safComment[], char safAlias[], int safProAcctID)
{
   /* exec sql begin declare section */
      
      
      
      
      
      
      
      
      
      
      
      
   
#line 247 "suplib.pgc"
 int v_safActiveID ;
 
#line 248 "suplib.pgc"
 char v_safSupAcctRef [ 30 ] ;
 
#line 249 "suplib.pgc"
 int v_safSupID ;
 
#line 250 "suplib.pgc"
 int v_safPrtID ;
 
#line 251 "suplib.pgc"
 int v_safSupTypeID ;
 
#line 252 "suplib.pgc"
 int v_safStartDt ;
 
#line 253 "suplib.pgc"
 int v_safEndDt ;
 
#line 254 "suplib.pgc"
 int v_safPayID ;
 
#line 255 "suplib.pgc"
 double v_safAmount ;
 
#line 256 "suplib.pgc"
 char v_safComment [ 30 ] ;
 
#line 257 "suplib.pgc"
 char v_safAlias [ 10 ] ;
 
#line 258 "suplib.pgc"
 int v_safProAcctID ;
/* exec sql end declare section */
#line 259 "suplib.pgc"

  
   connectToDB();

   v_safActiveID = safActiveID;
   strcpy(v_safSupAcctRef, safSupAcctRef);
   v_safSupID = safSupID;
   v_safPrtID = safPrtID;
   v_safSupTypeID = safSupTypeID;
   v_safStartDt = safStartDt;
   v_safEndDt = safEndDt;
   v_safPayID = safPayID;
   v_safAmount = safAmount;
   strcpy(v_safComment, safComment);
   strcpy(v_safAlias, safAlias);
   v_safProAcctID = safProAcctID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier_account ( active_ind , supplier_acct_ref , supplier_id , property_id , supplier_type_id , start_date , end_date , payment_period_id , amount , comment , alias , provider_acct_id ) values ( $1  , $2  , $3  , $4  , $5  , $6  , $7  , $8  , $9  , $10  , $11  , $12  )", 
	ECPGt_int,&(v_safActiveID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safSupAcctRef),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safSupID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safPrtID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safSupTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safPayID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_safAmount),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safComment),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safAlias),(long)10,(long)1,(10)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safProAcctID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 280 "suplib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 281 "suplib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 283 "suplib.pgc"

}

void supAccountUpdate(int upID,int safActiveID ,char safSupAcctRef[],int safSupID, int safPrtID,
                      int safSupTypeID, int safStartDt, int safEndDt, int safPayID, double safAmount,
		      char safComment[], char safAlias[], int safProAcctID)
{
   /* exec sql begin declare section */
      
      
      
      
      
      
      
      
      
      
      
      
      
   
#line 291 "suplib.pgc"
 int v_upID ;
 
#line 292 "suplib.pgc"
 int v_safActiveID ;
 
#line 293 "suplib.pgc"
 char v_safSupAcctRef [ 30 ] ;
 
#line 294 "suplib.pgc"
 int v_safSupID ;
 
#line 295 "suplib.pgc"
 int v_safPrtID ;
 
#line 296 "suplib.pgc"
 int v_safSupTypeID ;
 
#line 297 "suplib.pgc"
 int v_safStartDt ;
 
#line 298 "suplib.pgc"
 int v_safEndDt ;
 
#line 299 "suplib.pgc"
 int v_safPayID ;
 
#line 300 "suplib.pgc"
 double v_safAmount ;
 
#line 301 "suplib.pgc"
 char v_safComment [ 30 ] ;
 
#line 302 "suplib.pgc"
 char v_safAlias [ 10 ] ;
 
#line 303 "suplib.pgc"
 int v_safProAcctID ;
/* exec sql end declare section */
#line 304 "suplib.pgc"

  
   connectToDB();
   
   v_upID = upID;
   v_safActiveID = safActiveID;
   strcpy(v_safSupAcctRef, safSupAcctRef);
   v_safSupID = safSupID;
   v_safPrtID = safPrtID;
   v_safSupTypeID = safSupTypeID;
   v_safStartDt = safStartDt;
   v_safEndDt = safEndDt;
   v_safPayID = safPayID;
   v_safAmount = safAmount;
   strcpy(v_safComment, safComment);
   strcpy(v_safAlias, safAlias);
   v_safProAcctID = safProAcctID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update supplier_account set active_ind = $1  , supplier_acct_ref = $2  , supplier_id = $3  , property_id = $4  , supplier_type_id = $5  , start_date = $6  , end_date = $7  , payment_period_id = $8  , amount = $9  , comment = $10  , alias = $11  , provider_acct_id = $12  where supplier_acct_id = $13 ", 
	ECPGt_int,&(v_safActiveID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safSupAcctRef),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safSupID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safPrtID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safSupTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safPayID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_double,&(v_safAmount),(long)1,(long)1,sizeof(double), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safComment),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_safAlias),(long)10,(long)1,(10)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_safProAcctID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 327 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 328 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 330 "suplib.pgc"

}

void supAccountDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 336 "suplib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 337 "suplib.pgc"

  
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier_account where supplier_acct_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 344 "suplib.pgc"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 345 "suplib.pgc"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 347 "suplib.pgc"

}
 


