/* Processed by ecpg (13.1) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "catlib.pcg"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/catlib.h"
#include "../inc/fdbcon.h"

void catInsert(char catDesc[])
{
  /* exec sql begin declare section */
    
  
#line 11 "catlib.pcg"
 char v_catDesc [ 50 ] ;
/* exec sql end declare section */
#line 12 "catlib.pcg"


  connectToDB();

  strcpy(v_catDesc, catDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into category_type ( category ) values ( $1  )", 
	ECPGt_char,(v_catDesc),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 19 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 20 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 22 "catlib.pcg"

}

void catUpdate(int upID,char catDesc[])
{
  /* exec sql begin declare section */
    
    
  
#line 28 "catlib.pcg"
 int v_upID ;
 
#line 29 "catlib.pcg"
 char v_catDesc [ 50 ] ;
/* exec sql end declare section */
#line 30 "catlib.pcg"


  connectToDB();

  v_upID = upID;
  strcpy(v_catDesc, catDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update category_type set category = $1  where id = $2 ", 
	ECPGt_char,(v_catDesc),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 38 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 39 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 41 "catlib.pcg"

}

void catDelete(int upID)
{
 /* exec sql begin declare section */
      
  
#line 47 "catlib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 48 "catlib.pcg"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from category_type where id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 55 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 56 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 58 "catlib.pcg"

}

void stmtLinkInsert(char slfAlias[], int slfCatID, char slfCategory[])
{
  /* exec sql begin declare section */
    
    
    
  
#line 64 "catlib.pcg"
 char v_slfAlias [ 50 ] ;
 
#line 65 "catlib.pcg"
 int v_slfCatID ;
 
#line 66 "catlib.pcg"
 char v_slfCategory [ 50 ] ;
/* exec sql end declare section */
#line 67 "catlib.pcg"


  connectToDB();

  strcpy(v_slfAlias, slfAlias);
  v_slfCatID = slfCatID;
  strcpy(v_slfCategory, slfCategory);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into statement_link ( alias , category_id , category ) values ( $1  , $2  , $3  )", 
	ECPGt_char,(v_slfAlias),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_slfCatID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_slfCategory),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 76 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 77 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 79 "catlib.pcg"

}

void stmtLinkUpdate(int upID, char slfAlias[], int slfCatID, char slfCategory[])
{
   /* exec sql begin declare section */   
    
    
    
    
  
#line 85 "catlib.pcg"
 int v_upID ;
 
#line 86 "catlib.pcg"
 char v_slfAlias [ 50 ] ;
 
#line 87 "catlib.pcg"
 int v_slfCatID ;
 
#line 88 "catlib.pcg"
 char v_slfCategory [ 50 ] ;
/* exec sql end declare section */
#line 89 "catlib.pcg"


  connectToDB();

  v_upID = upID;
  strcpy(v_slfAlias, slfAlias);
  v_slfCatID = slfCatID;
  strcpy(v_slfCategory, slfCategory);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update statement_link set alias = $1  , category_id = $2  , category = $3  where id = $4 ", 
	ECPGt_char,(v_slfAlias),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_slfCatID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_slfCategory),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 99 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 100 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 102 "catlib.pcg"

}

void stmtLinkDelete(int upID)
{
  /* exec sql begin declare section */
      
  
#line 108 "catlib.pcg"
 int v_upID ;
/* exec sql end declare section */
#line 109 "catlib.pcg"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from statement_link where id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 116 "catlib.pcg"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 117 "catlib.pcg"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 119 "catlib.pcg"

}
