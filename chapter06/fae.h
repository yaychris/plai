#ifndef FAE_H
#define FAE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FAENUM(x)  ((FAENum *) (x))
#define FAEID(x)   ((FAEId *) (x))
#define FAEADD(x)  ((FAEAdd *) (x))
#define FAEAPP(x)  ((FAEApp *) (x))
#define FAEFUN(x)  ((FAEFun *) (x))

typedef unsigned long Expr;
typedef struct FunList FunList;
typedef struct SubList SubList;

// node types
typedef enum {
  FAE_NUM,
  FAE_ADD,
  FAE_ID,
  FAE_WITH,
  FAE_APP,
  FAE_FUN
} FAEType;

// base node
typedef struct {
  FAEType type;
} FAE;

// number
typedef struct {
  FAEType type;
  int     val;
} FAENum;

// + operation
typedef struct {
  FAEType type;
  FAE     *lhs;
  FAE     *rhs;
} FAEAdd;

// id
typedef struct {
  FAEType type;
  char    *name;
} FAEId;

// application
typedef struct {
  FAEType type;
  FAE     *funexpr;
  FAE     *argexpr;
} FAEApp;

// function
typedef struct {
  FAEType type;
  FAEId   *arg;
  FAE     *body;
} FAEFun;

inline FAENum *FAENum_new(int);
inline FAEAdd *FAEAdd_new(FAE*, FAE*);
inline FAEId  *FAEId_new(char*);
inline FAEApp *FAEApp_new(FAE*, FAE*);
inline FAEFun *FAEFun_new(FAEId*, FAE*);

FAE  *FAE_parse(char*);
int  FAE_interp(FAE*, FunList*, SubList*);
char *FAE_print(FAE*);
void FAE_free(FAE*);


////
// deferred substitution
typedef struct SubListNode SubListNode;

struct SubList {
  SubListNode *head;
};

struct SubListNode {
  char        *name;
  int         val;
  SubListNode *next;
};

SubList *SubList_new();
SubList *SubList_unshift(SubList*, char*, int);
int     SubList_lookup(SubList*, char*);
void    SubList_free(SubList*);


////
// function definitions linked list
typedef struct FunListNode FunListNode;
struct FunList {
  FunListNode *head;
};
struct FunListNode {
  FAEFun      *fun;
  FunListNode *next;
};

FunList *FunList_new();
FunList *FunList_push(FunList*, FAEFun*);
FAEFun  *FunList_lookup(FunList*, char*);
void    FunList_free(FunList*);

#endif
