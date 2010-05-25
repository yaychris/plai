#include "f1wae.h"

////
// F1WAE functions

F1WAENum *F1WAENum_new(int val) {
  F1WAENum *num = malloc(sizeof(F1WAENum));
  num->type = F1WAE_NUM;
  num->val  = val;
  return num;
}

F1WAEAdd *F1WAEAdd_new(F1WAE* l, F1WAE* r) {
  F1WAEAdd *add = malloc(sizeof(F1WAEAdd));
  add->type = F1WAE_ADD;
  add->lhs  = l;
  add->rhs  = r;
  return add;
}

F1WAEId *F1WAEId_new(char *name) {
  F1WAEId *id = malloc(sizeof(F1WAEId));
  id->type = F1WAE_ID;
  id->name = strdup(name);
  return id;
}

F1WAEWith *F1WAEWith_new(F1WAEId *id, F1WAE *expr, F1WAE *body) {
  F1WAEWith *with = malloc(sizeof(F1WAEWith));
  with->type = F1WAE_WITH;
  with->id   = id;
  with->expr = expr;
  with->body = body;
  return with;
}

F1WAEApp *F1WAEApp_new(F1WAEId *id, F1WAE *expr) {
  F1WAEApp *app = malloc(sizeof(F1WAEApp));
  app->type = F1WAE_APP;
  app->id   = id;
  app->expr = expr;
  return app;
}

F1WAEFun *F1WAEFun_new(char *name, char *arg, F1WAE *body) {
  F1WAEFun *fun = malloc(sizeof(F1WAEFun));
  fun->type = F1WAE_FUN;
  fun->name = strdup(name);
  fun->arg  = strdup(arg);
  fun->body = body;
  return fun;
}

int F1WAE_interp(F1WAE *self, FunList *funs, SubList *subs) {
  F1WAEAdd  *add;
  F1WAEWith *with;
  F1WAEApp  *app;
  F1WAEFun  *fun;
  SubList   *app_sub;
  int       result;

  switch (self->type) {
    case F1WAE_NUM:
      result = F1WAENUM(self)->val;
      break;

    case F1WAE_ID:
      result = SubList_lookup(subs, F1WAEID(self)->name);
      break;

    case F1WAE_ADD:
      add = F1WAEADD(self);
      result = F1WAE_interp(add->lhs, funs, subs) + F1WAE_interp(add->rhs, funs, subs);
      break;

    case F1WAE_WITH:
      with = F1WAEWITH(self);
      subs = SubList_unshift(
        subs,
        with->id->name,
        F1WAE_interp(with->expr, funs, subs)
      );

      result = F1WAE_interp(with->body, funs, subs);
      break;

    case F1WAE_APP:
      app = F1WAEAPP(self);
      fun = FunList_lookup(funs, app->id->name);

      app_sub = SubList_new();
      app_sub = SubList_unshift(
        app_sub,
        fun->arg,
        F1WAE_interp(app->expr, funs, subs)
      );

      result = F1WAE_interp(fun->body, funs, app_sub);

      SubList_free(app_sub);
      break;

    default:
      fprintf(stderr, "error: F1WAE_interp: unknown type\n");
      exit(1);
  }
  return result;
}

char *F1WAE_print(F1WAE *self) {
  char *buf = malloc(sizeof(char) * 512);
  char *temp[3];

  switch (self->type) {
    case F1WAE_NUM:
      sprintf(buf, "(num %d)", F1WAENUM(self)->val);
      break;

    case F1WAE_ID:
      sprintf(buf, "(id %s)", F1WAEID(self)->name);
      break;

    case F1WAE_WITH:
      temp[0] = F1WAE_print((F1WAE *) F1WAEWITH(self)->id);
      temp[1] = F1WAE_print(F1WAEWITH(self)->expr);
      temp[2] = F1WAE_print(F1WAEWITH(self)->body);
      sprintf(buf, "(with %s %s %s)", temp[0], temp[1], temp[2]);
      free(temp[0]);
      free(temp[1]);
      free(temp[2]);
      break;

    case F1WAE_ADD:
      temp[0] = F1WAE_print(F1WAEADD(self)->lhs);
      temp[1] = F1WAE_print(F1WAEADD(self)->rhs);
      sprintf(buf, "(add %s %s)", temp[0], temp[1]);
      free(temp[0]);
      free(temp[1]);
      break;

    case F1WAE_APP:
      temp[0] = F1WAE_print((F1WAE *) F1WAEAPP(self)->id);
      temp[1] = F1WAE_print(F1WAEAPP(self)->expr);
      sprintf(buf, "(app %s %s)", temp[0], temp[1]);
      free(temp[0]);
      free(temp[1]);
      break;
  }
  return buf;
}

void F1WAE_free(F1WAE *self) {
  switch (self->type) {
    case F1WAE_NUM:
      break;

    case F1WAE_ID:
      free(F1WAEID(self)->name);
      break;

    case F1WAE_WITH:
      F1WAE_free((F1WAE *) F1WAEWITH(self)->id);
      F1WAE_free(F1WAEWITH(self)->expr);
      F1WAE_free(F1WAEWITH(self)->body);
      break;

    case F1WAE_ADD:
      F1WAE_free(F1WAEADD(self)->lhs);
      F1WAE_free(F1WAEADD(self)->rhs);
      break;

    case F1WAE_APP:
      F1WAE_free((F1WAE *) F1WAEAPP(self)->id);
      F1WAE_free(F1WAEAPP(self)->expr);
      break;

    case F1WAE_FUN:
      free(F1WAEFUN(self)->name);
      free(F1WAEFUN(self)->arg);
      F1WAE_free(F1WAEFUN(self)->body);
      break;
  }
  free(self);
}


////
// deferred substitution

SubList *SubList_new() {
  SubList *self = malloc(sizeof(SubList));
  self->name = NULL;
  self->val = 0;
  self->next = NULL;
  return self;
}

SubList *SubList_unshift(SubList *self, char *name, int val) {
  SubList *list;

  // non-empty list, prepend a next node
  if (self->name != NULL) {
    list = self;
    self = SubList_new();
    self->next = list;
  }

  self->name = strdup(name);
  self->val = val;
  return self;
}

int SubList_lookup(SubList *self, char *name) {
  SubList *iter;

  iter = self;
  while(iter != NULL) {
    if (iter->name != NULL && strcmp(iter->name, name) == 0) {
      return iter->val;
    }
    iter = iter->next;
  }

  fprintf(stderr, "error: SubList_lookup: identifier '%s' not found\n", name);
  exit(1);
}

void SubList_free(SubList *self) {
  SubList *next;

  while (self != NULL) {
    if (self->name != NULL) {
      free(self->name);
    }
    next = self->next;
    free(self);
    self = next;
  }
}


////
// function definitions list

FunList *FunList_new() {
  FunList *list = malloc(sizeof(FunList));
  list->fun = NULL;
  list->next = NULL;
  return list;
}

FunList *FunList_addFun(FunList *self, F1WAEFun *fun) {
  FunList *new_list, *iter;

  if (self == NULL) {
    return NULL;
  }

  if (self->fun == NULL) {
    self->fun = fun;
    return self;
  }

  iter = self;
  while (iter->next != NULL) {
    iter = iter->next;
  }
  new_list = FunList_new();
  new_list->fun = fun;
  iter->next = new_list;
  return self;
}

F1WAEFun *FunList_lookup(FunList *self, char *name) {
  FunList *iter;

  iter = self;
  while(iter != NULL) {
    if (strcmp(iter->fun->name, name) == 0) {
      return iter->fun;
    }
    iter = iter->next;
  }

  fprintf(stderr, "error: FunList_lookup: function '%s' not found\n", name);
  exit(1);
}

void FunList_free(FunList *self) {
  FunList *next;

  while (self != NULL) {
    if (self->fun != NULL) {
      F1WAE_free((F1WAE *) self->fun);
    }
    next = self->next;
    free(self);
    self = next;
  }
}
