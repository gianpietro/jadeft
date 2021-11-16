#ifndef _STMUPLF_H
#define _STMUPLF_H

#include "../inc/stmuplibf.h"


struct statement * upLoadStatement();

char ** addAlias();

int resultRows();

void statementInsert(struct statement *);

char * fStmtName();

#endif
             
