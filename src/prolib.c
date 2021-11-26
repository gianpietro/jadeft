/* Processed by ecpg (13.3) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "prolib.pcg"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/prolib.h"
#include "../inc/fdbcon.h"


void providerInsert(int activeInd, char proName[])
{
   /* exec sql begin declare section */
       
       
   
#line 13 "prolib.pcg"
 int v_activeInd ;
 
#line 14 "prolib.pcg"
 char v_proName [ 30 ] ;
/* exec sql end declare section */
#line 15 "prolib.pcg"

   
   connectToDB();
     
   v_activeInd = activeInd;
   strcpy(v_proName,proName);
    
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider ( active_ind , provider_name ) values ( $1  , $2  )", 
	ECPGt_int,&(v_activeInd),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_proName),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 23 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 24 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 26 "prolib.pcg"

}

void providerUpdate(int upID, int activeInd, char proName[])
{
   /* exec sql begin declare section */
       
       
       
   
#line 32 "prolib.pcg"
 int v_upID ;
 
#line 33 "prolib.pcg"
 int v_activeInd ;
 
#line 34 "prolib.pcg"
 char v_proName [ 30 ] ;
/* exec sql end declare section */
#line 35 "prolib.pcg"

   
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
#line 44 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 45 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 47 "prolib.pcg"

}

void providerDelete(int upID)
{
   /* exec sql begin declare section */
             
   
#line 53 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 54 "prolib.pcg"

   
   connectToDB();

   v_upID = upID;
       
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider where provider_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 61 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 62 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 64 "prolib.pcg"

}

void proTypeInsert(char proTypeDesc[])
{
   /* exec sql begin declare section */
      
   
#line 70 "prolib.pcg"
 char v_proTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 71 "prolib.pcg"

  
   connectToDB();

   strcpy(v_proTypeDesc, proTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into provider_type ( description ) values ( $1  )", 
	ECPGt_char,(v_proTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 78 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 79 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 81 "prolib.pcg"

}

void proTypeUpdate(int upID, char proTypeDesc[])
{
   /* exec sql begin declare section */
      
      
   
#line 87 "prolib.pcg"
 int v_upID ;
 
#line 88 "prolib.pcg"
 char v_proTypeDesc [ 30 ] ;
/* exec sql end declare section */
#line 89 "prolib.pcg"

  
   connectToDB();

   v_upID = upID;
   strcpy(v_proTypeDesc, proTypeDesc);

   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update provider_type set description = $1  where provider_type_id = $2 ", 
	ECPGt_char,(v_proTypeDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 97 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 98 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 100 "prolib.pcg"

}

void proTypeDelete(int upID)
{
   /* exec sql begin declare section */
      
   
#line 106 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 107 "prolib.pcg"

  
   connectToDB();

   v_upID = upID;
   
   { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider_type where provider_type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 114 "prolib.pcg"

   { ECPGtrans(__LINE__, NULL, "commit");}
#line 115 "prolib.pcg"


   { ECPGdisconnect(__LINE__, "CURRENT");}
#line 117 "prolib.pcg"

}

void proAccountInsert(int pafActiveID, int pafID, char pafAccountNo[], int pafSortCode, char pafRef[], int pafTypeID)
{
  /* exec sql begin declare section */
     
     
     
     
     
     
  
#line 123 "prolib.pcg"
 int v_pafActiveID ;
 
#line 124 "prolib.pcg"
 int v_pafID ;
 
#line 125 "prolib.pcg"
 char v_pafAccountNo [ 30 ] ;
 
#line 126 "prolib.pcg"
 int v_pafSortCode ;
 
#line 127 "prolib.pcg"
 char v_pafRef [ 30 ] ;
 
#line 128 "prolib.pcg"
 int v_pafTypeID ;
/* exec sql end declare section */
#line 129 "prolib.pcg"


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
#line 142 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 143 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 145 "prolib.pcg"

}

void proAccountUpdate(int upID, int pafActiveID, int pafID, char pafAccountNo[], int pafSortCode, char pafRef[], int pafTypeID)
{
  /* exec sql begin declare section */
     
     
     
     
     
     
     
  
#line 151 "prolib.pcg"
 int v_upID ;
 
#line 152 "prolib.pcg"
 int v_pafActiveID ;
 
#line 153 "prolib.pcg"
 int v_pafID ;
 
#line 154 "prolib.pcg"
 char v_pafAccountNo [ 30 ] ;
 
#line 155 "prolib.pcg"
 int v_pafSortCode ;
 
#line 156 "prolib.pcg"
 char v_pafRef [ 30 ] ;
 
#line 157 "prolib.pcg"
 int v_pafTypeID ;
/* exec sql end declare section */
#line 158 "prolib.pcg"


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
#line 173 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 174 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 176 "prolib.pcg"

}

void proAccountDelete(int upID)
{
  /* exec sql begin declare section */
     
  
#line 182 "prolib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 183 "prolib.pcg"


  connectToDB();

  v_upID = upID;

 { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from provider_account where provider_acct_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 190 "prolib.pcg"

 { ECPGtrans(__LINE__, NULL, "commit");}
#line 191 "prolib.pcg"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 193 "prolib.pcg"

}

 	       