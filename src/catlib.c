/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "catlib.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/catlib.h"
#include "../inc/fdbcon.h"

void catInsert(char catDesc[])
{
  /* exec sql begin declare section */
    
  
#line 12 "catlib.pgc"
 char v_catDesc [ 50 ] ;
/* exec sql end declare section */
#line 13 "catlib.pgc"


  connectToDB();

  strcpy(v_catDesc, catDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into category_type ( category ) values ( $1  )", 
	ECPGt_char,(v_catDesc),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 20 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 21 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 23 "catlib.pgc"

}

void catUpdate(int upID,char catDesc[])
{
  /* exec sql begin declare section */
    
    
  
#line 29 "catlib.pgc"
 int v_upID ;
 
#line 30 "catlib.pgc"
 char v_catDesc [ 50 ] ;
/* exec sql end declare section */
#line 31 "catlib.pgc"


  connectToDB();

  v_upID = upID;
  strcpy(v_catDesc, catDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update category_type set category = $1  where id = $2 ", 
	ECPGt_char,(v_catDesc),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 39 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 40 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 42 "catlib.pgc"

}

void catDelete(int upID)
{
 /* exec sql begin declare section */
      
  
#line 48 "catlib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 49 "catlib.pgc"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from category_type where id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 56 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 57 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 59 "catlib.pgc"

}

void stmtLinkInsert(char slfAlias[], int slfCatID, char slfCategory[])
{
  /* exec sql begin declare section */
    
    
    
  
#line 65 "catlib.pgc"
 char v_slfAlias [ 50 ] ;
 
#line 66 "catlib.pgc"
 int v_slfCatID ;
 
#line 67 "catlib.pgc"
 char v_slfCategory [ 50 ] ;
/* exec sql end declare section */
#line 68 "catlib.pgc"


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
#line 77 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 78 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 80 "catlib.pgc"

}

void stmtLinkUpdate(int upID, char slfAlias[], int slfCatID, char slfCategory[])
{
   /* exec sql begin declare section */   
    
    
    
    
  
#line 86 "catlib.pgc"
 int v_upID ;
 
#line 87 "catlib.pgc"
 char v_slfAlias [ 50 ] ;
 
#line 88 "catlib.pgc"
 int v_slfCatID ;
 
#line 89 "catlib.pgc"
 char v_slfCategory [ 50 ] ;
/* exec sql end declare section */
#line 90 "catlib.pgc"


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
#line 100 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 101 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 103 "catlib.pgc"

}

void stmtLinkDelete(int upID)
{
  /* exec sql begin declare section */
      
  
#line 109 "catlib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 110 "catlib.pgc"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from statement_link where id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 117 "catlib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 118 "catlib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 120 "catlib.pgc"

}
