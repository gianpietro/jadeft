#ifndef _STMUPLIBF_H
#define _STMUPLIBF_H

#define QM 34     /* quotation mark */
#define SP 32     /* space */
#define CM 44     /* comma */
#define FS 47     /* forward slash */
#define AP 39     /* apostrophe */
#define TDATE 9   /* transaction date */
#define TTYPE 4   /* transaction type */
#define TDESC 150 /* transaction description */
#define TVALUE 10 /* transaction value */
#define ANUM 16   /* account number */

struct statement{
  char tDate[TDATE];
  char tType[TTYPE];
  char tDescription[TDESC];
  char tValue[TVALUE];
  char actNumber[ANUM];
  struct statement *next;
};

typedef struct statement statement;

struct statement *importStmt(char *, char *, char *, char *, char *);
struct statement *append(struct statement *, struct statement *);
void printStatement(struct statement *);
void freeStatement(struct statement *);
int aliasMatch(char *, char *);

#endif
