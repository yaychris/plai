#include "fae.h"

////
// FAE functions

FAENum *FAENum_new(int val) {
  FAENum *num = malloc(sizeof(FAENum));
  num->type = FAE_NUM;
  num->val  = val;
  return num;
}

FAEAdd *FAEAdd_new(FAE* l, FAE* r) {
  FAEAdd *add = malloc(sizeof(FAEAdd));
  add->type = FAE_ADD;
  add->lhs  = l;
  add->rhs  = r;
  return add;
}

FAEId *FAEId_new(char *name) {
  FAEId *id = malloc(sizeof(FAEId));
  id->type = FAE_ID;
  id->name = strdup(name);
  return id;
}

FAEApp *FAEApp_new(FAE *funexpr, FAE *argexpr) {
  FAEApp *app = malloc(sizeof(FAEApp));
  app->type = FAE_APP;
  app->funexpr = funexpr;
  app->argexpr = argexpr;
  return app;
}

FAEFun *FAEFun_new(FAEId *arg, FAE *body) {
  FAEFun *fun = malloc(sizeof(FAEFun));
  fun->type = FAE_FUN;
  fun->arg = arg;
  fun->body = body;
  return fun;
}

int FAE_interp(FAE *self, FunList *funs, SubList *subs) {
  /*FAEAdd  *add;
  FAEApp  *app;
  FAEFun  *fun;
  SubList *app_sub;
  int     result;

  switch (self->type) {
    case FAE_NUM:
      result = FAENUM(self)->val;
      break;

    case FAE_ID:
      result = SubList_lookup(subs, FAEID(self)->name);
      break;

    case FAE_ADD:
      add = FAEADD(self);
      result = FAE_interp(add->lhs, funs, subs) + FAE_interp(add->rhs, funs, subs);
      break;

    case FAE_APP:
      app = FAEAPP(self);
      fun = FunList_lookup(funs, app->id->name);

      app_sub = SubList_new();
      app_sub = SubList_unshift(
        app_sub,
        fun->arg,
        FAE_interp(app->expr, funs, subs)
      );

      result = FAE_interp(fun->body, funs, app_sub);

      SubList_free(app_sub);
      break;

    default:
      fprintf(stderr, "error: FAE_interp: unknown type\n");
      exit(1);
  }
  return result;*/
}

char *FAE_print(FAE *self) {
  char *buf = malloc(sizeof(char) * 512);
  char *temp[3];

  switch (self->type) {
    case FAE_NUM:
      sprintf(buf, "(num %d)", FAENUM(self)->val);
      break;

    case FAE_ID:
      sprintf(buf, "(id %s)", FAEID(self)->name);
      break;

    case FAE_ADD:
      temp[0] = FAE_print(FAEADD(self)->lhs);
      temp[1] = FAE_print(FAEADD(self)->rhs);
      sprintf(buf, "(add %s %s)", temp[0], temp[1]);
      free(temp[0]);
      free(temp[1]);
      break;

    case FAE_APP:
      temp[0] = FAE_print(FAEAPP(self)->funexpr);
      temp[1] = FAE_print(FAEAPP(self)->argexpr);
      sprintf(buf, "(app %s %s)", temp[0], temp[1]);
      free(temp[0]);
      free(temp[1]);
      break;

    case FAE_FUN:
      temp[0] = FAE_print((FAE *) FAEFUN(self)->arg);
      temp[1] = FAE_print(FAEFUN(self)->body);
      sprintf(buf, "(fun %s %s)", temp[0], temp[1]);
      free(temp[0]);
      free(temp[1]);
      break;
  }
  return buf;
}

void FAE_free(FAE *self) {
  switch (self->type) {
    case FAE_NUM:
      break;

    case FAE_ID:
      free(FAEID(self)->name);
      break;

    case FAE_ADD:
      FAE_free(FAEADD(self)->lhs);
      FAE_free(FAEADD(self)->rhs);
      break;

    case FAE_APP:
      FAE_free(FAEAPP(self)->funexpr);
      FAE_free(FAEAPP(self)->argexpr);
      break;

    case FAE_FUN:
      FAE_free((FAEId *) FAEFUN(self)->arg);
      FAE_free(FAEFUN(self)->body);
      break;
  }
  free(self);
}


////
// deferred substitution

SubList *SubList_new() {
  SubList *self = malloc(sizeof(SubList));
  self->head = NULL;
  return self;
}

SubList *SubList_unshift(SubList *self, char *name, int val) {
  SubListNode *node = malloc(sizeof(SubListNode));
  node->name = strdup(name);
  node->val = val;
  node->next = self->head;
  self->head = node;

  return self;
}

int SubList_lookup(SubList *self, char *name) {
  SubListNode *iter;

  iter = self->head;
  while(iter != NULL) {
    if (strcmp(iter->name, name) == 0) {
      return iter->val;
    }
    iter = iter->next;
  }

  fprintf(stderr, "error: SubList_lookup: identifier '%s' not found\n", name);
  exit(1);
}

void SubList_free(SubList *self) {
  SubListNode *iter;

  iter = self->head;
  while (iter != NULL) {
    if (iter->name != NULL) {
      free(iter->name);
    }
    self->head = iter->next;
    free(iter);
    iter = self->head;
  }
  free(self);
}


////
// function definitions list

FunList *FunList_new() {
  FunList *list = malloc(sizeof(FunList));
  list->head = NULL;
  return list;
}

FunList *FunList_push(FunList *self, FAEFun *fun) {
  FunListNode *iter,
              *node = malloc(sizeof(FunListNode));
  node->fun = fun;
  node->next = NULL;

  if (self->head == NULL) {
    self->head = node;
    return self;
  }

  iter = self->head;
  while (iter->next != NULL) {
    iter = iter->next;
  }
  iter->next = node;
  return self;
}

FAEFun *FunList_lookup(FunList *self, char *name) {
  /*FunListNode *iter;

  iter = self->head;
  while(iter != NULL) {
    if (strcmp(iter->fun->name, name) == 0) {
      return iter->fun;
    }
    iter = iter->next;
  }

  fprintf(stderr, "error: FunList_lookup: function '%s' not found\n", name);
  exit(1);*/
}

void FunList_free(FunList *self) {
  FunListNode *iter;

  iter = self->head;
  while (iter != NULL) {
    if (iter->fun != NULL) {
      FAE_free((FAE *) iter->fun);
    }
    self->head = iter->next;
    free(iter);
    iter = self->head;
  }
  free(self);
}
