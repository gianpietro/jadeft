/* Processed by ecpg (15.3 (Debian 15.3-0+deb12u1)) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "doclib.pgc"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../inc/doclib.h"
#include "../inc/fdbcon.h"

void docTypeInsert(char dtDesc[])
{
  /* exec sql begin declare section */
    
  
#line 12 "doclib.pgc"
 char v_dtDesc [ 30 ] ;
/* exec sql end declare section */
#line 13 "doclib.pgc"


  connectToDB();

  strcpy(v_dtDesc, dtDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into document_type ( description ) values ( $1  )", 
	ECPGt_char,(v_dtDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 20 "doclib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 21 "doclib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 23 "doclib.pgc"

}

void docTypeUpdate(int upID,char dtDesc[])
{
  /* exec sql begin declare section */
    
    
  
#line 29 "doclib.pgc"
 int v_upID ;
 
#line 30 "doclib.pgc"
 char v_dtDesc [ 30 ] ;
/* exec sql end declare section */
#line 31 "doclib.pgc"


  connectToDB();

  v_upID = upID;
  strcpy(v_dtDesc, dtDesc);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update document_type set description = $1  where type_id = $2 ", 
	ECPGt_char,(v_dtDesc),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 39 "doclib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 40 "doclib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 42 "doclib.pgc"

}

void docTypeDelete(int upID)
{
 /* exec sql begin declare section */
      
  
#line 48 "doclib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 49 "doclib.pgc"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from document_type where type_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 56 "doclib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 57 "doclib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 59 "doclib.pgc"

}

void docImportInsert(int dParentID, char e[], int objImportID, int dTypeID, char dRef[], char dTitle[], char dDesc[], int dStartDt, int dEndDt, char dCatalog[])
{
 /* exec sql begin declare section */
    
    
    
    
    
    
    
    
    
    
 
#line 65 "doclib.pgc"
 int v_dParentID ;
 
#line 66 "doclib.pgc"
 char v_e [ 30 ] ;
 
#line 67 "doclib.pgc"
 int v_objImportID ;
 
#line 68 "doclib.pgc"
 int v_dTypeID ;
 
#line 69 "doclib.pgc"
 char v_dRef [ 50 ] ;
 
#line 70 "doclib.pgc"
 char v_dTitle [ 100 ] ;
 
#line 71 "doclib.pgc"
 char v_dDesc [ 150 ] ;
 
#line 72 "doclib.pgc"
 int v_dStartDt ;
 
#line 73 "doclib.pgc"
 int v_dEndDt ;
 
#line 74 "doclib.pgc"
 char v_dCatalog [ 30 ] ;
/* exec sql end declare section */
#line 75 "doclib.pgc"


 connectToDB();

  v_dParentID =	dParentID;
  strcpy(v_e, e);
  v_objImportID	= objImportID;
  v_dTypeID = dTypeID;
  strcpy(v_dRef, dRef);
  strcpy(v_dTitle, dTitle);
  strcpy(v_dDesc,dDesc);
  v_dStartDt = dStartDt;
  v_dEndDt = dEndDt;
  strcpy(v_dCatalog, dCatalog);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into documents ( parent_id , file_name , oid_value , type_id , document_ref , title , description , start_date , end_date , catalog ) values ( $1  , $2  , $3  , $4  , $5  , $6  , $7  , $8  , $9  , $10  )", 
	ECPGt_int,&(v_dParentID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_e),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_objImportID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dRef),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dTitle),(long)100,(long)1,(100)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dDesc),(long)150,(long)1,(150)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dCatalog),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 92 "doclib.pgc"


 { ECPGtrans(__LINE__, NULL, "commit");}
#line 94 "doclib.pgc"


 { ECPGdisconnect(__LINE__, "CURRENT");}
#line 96 "doclib.pgc"

}

void docImportUpdate(int upID, int dParentID, char e[], int objImportID, int dTypeID, char dRef[], char dTitle[], char dDesc[], int dStartDt, int dEndDt, char dCatalog[])
{
 /* exec sql begin declare section */
    
    
    
    
    
    
    
    
    
    
    
 
#line 102 "doclib.pgc"
 int v_upID ;
 
#line 103 "doclib.pgc"
 int v_dParentID ;
 
#line 104 "doclib.pgc"
 char v_e [ 30 ] ;
 
#line 105 "doclib.pgc"
 int v_objImportID ;
 
#line 106 "doclib.pgc"
 int v_dTypeID ;
 
#line 107 "doclib.pgc"
 char v_dRef [ 50 ] ;
 
#line 108 "doclib.pgc"
 char v_dTitle [ 100 ] ;
 
#line 109 "doclib.pgc"
 char v_dDesc [ 150 ] ;
 
#line 110 "doclib.pgc"
 int v_dStartDt ;
 
#line 111 "doclib.pgc"
 int v_dEndDt ;
 
#line 112 "doclib.pgc"
 char v_dCatalog [ 30 ] ;
/* exec sql end declare section */
#line 113 "doclib.pgc"


 connectToDB();
   v_upID = upID; 
   v_dParentID = dParentID;
   strcpy(v_e, e);
   v_objImportID = objImportID;
   v_dTypeID = dTypeID;
   strcpy(v_dRef, dRef);
   strcpy(v_dTitle, dTitle);
   strcpy(v_dDesc,dDesc);
   v_dStartDt = dStartDt;
   v_dEndDt = dEndDt;
   strcpy(v_dCatalog, dCatalog);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update documents set parent_id = $1  , file_name = $2  , oid_value = $3  , type_id = $4  , document_ref = $5  , title = $6  , description = $7  , start_date = $8  , end_date = $9  , catalog = $10  where document_id = $11 ", 
	ECPGt_int,&(v_dParentID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_e),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_objImportID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dTypeID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dRef),(long)50,(long)1,(50)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dTitle),(long)100,(long)1,(100)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dDesc),(long)150,(long)1,(150)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dStartDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_dEndDt),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(v_dCatalog),(long)30,(long)1,(30)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 129 "doclib.pgc"

 
  { ECPGtrans(__LINE__, NULL, "commit");}
#line 131 "doclib.pgc"

 
  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 133 "doclib.pgc"

}

void docImportDelete(int upID)
{
 /* exec sql begin declare section */
    
  
#line 139 "doclib.pgc"
 int v_upID ;
/* exec sql end declare section */
#line 140 "doclib.pgc"


  connectToDB();

  v_upID = upID;

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "delete from documents where document_id = $1 ", 
	ECPGt_int,&(v_upID),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 147 "doclib.pgc"

  { ECPGtrans(__LINE__, NULL, "commit");}
#line 148 "doclib.pgc"


  { ECPGdisconnect(__LINE__, "CURRENT");}
#line 150 "doclib.pgc"

}







 