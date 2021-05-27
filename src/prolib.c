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


void providerInsert(int activeInd, char proName[])
{
   /* exec sql begin declare section */
       
       
   
#line 12 "prolib.pcg"
 int v_activeInd ;
 
#line 13 "prolib.pcg"
 char v_proName [ 30 ] ;
/* exec sql end declare section */
#line 14 "prolib.pcg"

   
   connectToDB();
     
   v_activeInd = activeInd;
   strcpy(v_proName,proName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider ( active_ind , provider_name ) values ( $1  , $2  )", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_proName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 22 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 23 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 25 "prolib.pcg"

}

void providerUpdate(int upID, int activeInd, char proName[])
{
   /* exec sql begin declare section */
       
       
       
   
#line 31 "prolib.pcg"
 int v_upID ;
 
#line 32 "prolib.pcg"
 int v_activeInd ;
 
#line 33 "prolib.pcg"
 char v_proName [ 30 ] ;
/* exec sql end declare section */
#line 34 "prolib.pcg"

   
   connectToDB();

   v_upID = upID;
   v_activeInd = activeInd;
   strcpy(v_proName,proName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update provider set active_ind = $1  , provider_name = $2  where provider_id = $3 ", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_proName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 43 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 44 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 46 "prolib.pcg"

}

void providerDelete(int upID)
{
   /* exec sql begin declare section */
             
   
#line 52 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 53 "prolib.pcg"

   
   connectToDB();

   v_upID = upID;
       
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider where provider_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 60 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 61 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 63 "prolib.pcg"

}

void proTypeInsert(char proTypeDesc[])
{
   /* exec sql begin declare section */
      
   
#line 69 "prolib.pcg"
 char v_proTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 70 "prolib.pcg"

  
   connectToDB();

   strcpy(v_proTypeDesc, proTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider_type ( description ) values ( $1  )", 
	ECPGt_char,(v_proTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 77 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 78 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 80 "prolib.pcg"

}

void proTypeUpdate(int upID, char proTypeDesc[])
{
   /* exec sql begin declare section */
      
      
   
#line 86 "prolib.pcg"
 int v_upID ;
 
#line 87 "prolib.pcg"
 char v_proTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 88 "prolib.pcg"

  
   connectToDB();

   v_upID = upID;
   strcpy(v_proTypeDesc, proTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update provider_type set description = $1  where provider_type_id = $2 ", 
	ECPGt_char,(v_proTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 96 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 97 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 99 "prolib.pcg"

}

void proTypeDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 105 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 106 "prolib.pcg"

  
   connectToDB();

   v_upID = upID;
   
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider_type where provider_type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 113 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 114 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 116 "prolib.pcg"

}

void proAccountInsert(int pafActiveID, int pafID, char pafAccountNo[], int pafSortCode, char pafRef[], int pafTypeID)
{
  /* exec sql begin declare section */
     
     
     
     
     
     
  
#line 122 "prolib.pcg"
 int v_pafActiveID ;
 
#line 123 "prolib.pcg"
 int v_pafID ;
 
#line 124 "prolib.pcg"
 char v_pafAccountNo [ 30 ] ;
 
#line 125 "prolib.pcg"
 int v_pafSortCode ;
 
#line 126 "prolib.pcg"
 char v_pafRef [ 30 ] ;
 
#line 127 "prolib.pcg"
 int v_pafTypeID ;
/* exec sql end declare section */
#line 128 "prolib.pcg"


  connectToDB();

  v_pafActiveID = pafActiveID;
  v_pafID = pafID;
  strcpy(v_pafAccountNo, pafAccountNo);
  v_pafSortCode = pafSortCode;
  strcpy(v_pafRef, pafRef);
  v_pafTypeID = pafTypeID;

 { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider_account ( active_ind , provider_id , provider_acct_no , sort_code , preference , provider_type_id ) values ( $1  , $2  , $3  , $4  , $5  , $6  )", 
	ECPGt_int,&(v_pafActiveID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_pafAccountNo),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafSortCode),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_pafRef),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 141 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 142 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 144 "prolib.pcg"

}

void proAccountUpdate(int upID, int pafActiveID, int pafID, char pafAccountNo[], int pafSortCode, char pafRef[], int pafTypeID)
{
  /* exec sql begin declare section */
     
     
     
     
     
     
     
  
#line 150 "prolib.pcg"
 int v_upID ;
 
#line 151 "prolib.pcg"
 int v_pafActiveID ;
 
#line 152 "prolib.pcg"
 int v_pafID ;
 
#line 153 "prolib.pcg"
 char v_pafAccountNo [ 30 ] ;
 
#line 154 "prolib.pcg"
 int v_pafSortCode ;
 
#line 155 "prolib.pcg"
 char v_pafRef [ 30 ] ;
 
#line 156 "prolib.pcg"
 int v_pafTypeID ;
/* exec sql end declare section */
#line 157 "prolib.pcg"


  connectToDB();

  v_upID = upID;
  v_pafActiveID = pafActiveID;
  v_pafID = pafID;
  strcpy(v_pafAccountNo, pafAccountNo);
  v_pafSortCode = pafSortCode;
  strcpy(v_pafRef, pafRef);
  v_pafTypeID = pafTypeID;

 { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update provider_account set active_ind = $1  , provider_id = $2  , provider_acct_no = $3  , sort_code = $4  , preference = $5  , provider_type_id = $6  where provider_acct_id = $7 ", 
	ECPGt_int,&(v_pafActiveID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_pafAccountNo),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafSortCode),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_pafRef),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_pafTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 170 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 171 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 173 "prolib.pcg"

}

void proAccountDelete(int upID)
{
  /* exec sql begin declare section */
     
  
#line 179 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 180 "prolib.pcg"


  connectToDB();

  v_upID = upID;

 { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider_account where provider - acct_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 187 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 188 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 190 "prolib.pcg"

}

 	       