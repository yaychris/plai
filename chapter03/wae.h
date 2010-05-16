#ifndef WAE_H
#define WAE_H

#include <stdlib.h>
#include <stdio.h>

#define WAEOP(x)   ((WAEOp *) (x))
#define WAENUM(x)  ((WAENum *) (x))
#define WAEID(x)   ((WAEId *) (x))
#define WAEWITH(x) ((WAEWith *) (x))

typedef unsigned long Expr;

// node types
typedef enum {
  WAE_NUM,
  WAE_ADD,
  WAE_SUB,
  WAE_ID,
  WAE_WITH
} WAEType;

// base node
typedef struct {
  WAEType type;
} WAE;

// number
typedef struct {
  WAEType type;
  int     val;
} WAENum;

// +/- operation
typedef struct {
  WAEType type;
  WAE    *lhs;
  WAE    *rhs;
} WAEOp;

// id
typedef struct {
  WAEType type;
  char   *name;
} WAEId;

// with operation
typedef struct {
  WAEType type;
  WAEId  *id;
  WAE    *expr;
  WAE    *body;
} WAEWith;


inline WAENum  *WAENum_new(int val);
inline WAEOp   *WAEOp_new(unsigned short type, WAE *l, WAE *r);
inline WAEId   *WAEId_new(char* name);
inline WAEWith *WAEWith_new(WAEId *id, WAE *expr, WAE *body);

WAE  *WAE_parse();
WAE  *WAE_subst(WAE *expr, char *name, WAENum *val);
int   WAE_calc(WAE *self);
char *WAE_print(WAE *self);
void  WAE_free(WAE *self);

#endif
