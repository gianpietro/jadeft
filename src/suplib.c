/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "suplib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/suplib.h"
#include "../inc/fdbcon.h"


void supplierInsert(int activeInd,char supName[])
{
   /* exec sql begin declare section */
       
       
   
#line 12 "suplib.pcg"
 int v_activeInd ;
 
#line 13 "suplib.pcg"
 char v_supName [ 30 ] ;
/* exec sql end declare section */
#line 14 "suplib.pcg"

   
   connectToDB();
     
   v_activeInd = activeInd;
   strcpy(v_supName,supName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier ( active_ind , supplier_name ) values ( $1  , $2  )", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_supName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 22 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 23 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 25 "suplib.pcg"

}

void supplierUpdate(int upID, int activeInd,char supName[])
{
   /* exec sql begin declare section */
       
       
       
   
#line 31 "suplib.pcg"
 int v_upID ;
 
#line 32 "suplib.pcg"
 int v_activeInd ;
 
#line 33 "suplib.pcg"
 char v_supName [ 30 ] ;
/* exec sql end declare section */
#line 34 "suplib.pcg"

   
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
#line 43 "suplib.pcg"
  
   { ECPGtrans(__LINE__, NULL, "commit");}
#line 44 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 46 "suplib.pcg"

}

void supplierDelete(int upID)
{
   /* exec sql begin declare section */
       
   
#line 52 "suplib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 53 "suplib.pcg"

   
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier where supplier_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 60 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 61 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 63 "suplib.pcg"

}

void supTypeInsert(char supTypeDesc[])
{
  /* exec sql begin declare section */
      
   
#line 69 "suplib.pcg"
 char v_supTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 70 "suplib.pcg"

  
   connectToDB();

   strcpy(v_supTypeDesc, supTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into supplier_type ( description ) values ( $1  )", 
	ECPGt_char,(v_supTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 77 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 78 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 80 "suplib.pcg"

}

void supTypeUpdate(int upID, char supTypeDesc[])
{
   /* exec sql begin declare section */
      
      
   
#line 86 "suplib.pcg"
 int v_upID ;
 
#line 87 "suplib.pcg"
 char v_supTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 88 "suplib.pcg"

  
   connectToDB();

   v_upID = upID;
   strcpy(v_supTypeDesc, supTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update supplier_type set description = $1  where supplier_type_id = $2 ", 
	ECPGt_char,(v_supTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 96 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 97 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 99 "suplib.pcg"

}

void supTypeDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 105 "suplib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 106 "suplib.pcg"

  
   connectToDB();

   v_upID = upID;

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from supplier_type where supplier_type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 113 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 114 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 116 "suplib.pcg"

}


void payPeriodInsert(char payPeriod[])
{
  /* exec sql begin declare section */
      
   
#line 123 "suplib.pcg"
 char v_payPeriod [ 30 ] ;
/* exec sql end declare section */
#line 124 "suplib.pcg"

  
   connectToDB();

   strcpy(v_payPeriod, payPeriod);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into payment_period ( period ) values ( $1  )", 
	ECPGt_char,(v_payPeriod),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 131 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 132 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 134 "suplib.pcg"

}


void prtInsert(int actInd, char prtPostCode[], char prtAddress[], char prtCity[])
{
   /* exec sql begin declare section */
      
      
      
      
   
#line 141 "suplib.pcg"
 int v_actInd ;
 
#line 142 "suplib.pcg"
 char v_prtPostCode [ 10 ] ;
 
#line 143 "suplib.pcg"
 char v_prtAddress [ 30 ] ;
 
#line 144 "suplib.pcg"
 char v_prtCity [ 30 ] ;
/* exec sql end declare section */
#line 145 "suplib.pcg"

  
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
#line 156 "suplib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 157 "suplib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 159 "suplib.pcg"

}

void supAccountInsert(int safActiveID ,char safSupAcctRef[],int safSupID, int safPrtID,
                      int safSupTypeID, int safStartDt, int safEndDt, int safPayID, double safAmount,
		      char safComment[], char safAlias[], int safProAcctID)
{
   /* exec sql begin declare section */
      
      
      
      
      
      
      
      
      
      
      
      
   
#line 167 "suplib.pcg"
 int v_safActiveID ;
 
#line 168 "suplib.pcg"
 char v_safSupAcctRef [ 30 ] ;
 
#line 169 "suplib.pcg"
 int v_safSupID ;
 
#line 170 "suplib.pcg"
 int v_safPrtID ;
 
#line 171 "suplib.pcg"
 int v_safSupTypeID ;
 
#line 172 "suplib.pcg"
 int v_safStartDt ;
 
#line 173 "suplib.pcg"
 int v_safEndDt ;
 
#line 174 "suplib.pcg"
 int v_safPayID ;
 
#line 175 "suplib.pcg"
 double v_safAmount ;
 
#line 176 "suplib.pcg"
 char v_safComment [ 30 ] ;
 
#line 177 "suplib.pcg"
 char v_safAlias [ 10 ] ;
 
#line 178 "suplib.pcg"
 int v_safProAcctID ;
/* exec sql end declare section */
#line 179 "suplib.pcg"

  
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
#line 200 "suplib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 201 "suplib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 203 "suplib.pcg"

}


 


