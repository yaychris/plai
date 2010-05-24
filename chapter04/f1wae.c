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

F1WAE *F1WAE_clone(F1WAE *self) {
  switch (self->type) {
    case F1WAE_NUM:
      return (F1WAE *) F1WAENum_new(F1WAENUM(self)->val);

    case F1WAE_ADD:
      return (F1WAE *) F1WAEAdd_new(
        F1WAE_clone(F1WAEADD(self)->lhs),
        F1WAE_clone(F1WAEADD(self)->rhs)
      );

    case F1WAE_WITH:
      return (F1WAE *) F1WAEWith_new(
        (F1WAEId *) F1WAE_clone((F1WAE *) F1WAEWITH(self)->id),
        F1WAE_clone(F1WAEWITH(self)->expr),
        F1WAE_clone(F1WAEWITH(self)->body)
      );

    case F1WAE_APP:
      return (F1WAE *) F1WAEApp_new(
        (F1WAEId *) F1WAE_clone((F1WAE *) F1WAEAPP(self)->id),
        F1WAE_clone(F1WAEAPP(self)->expr)
      );

    case F1WAE_ID:
      return (F1WAE *) F1WAEId_new(F1WAEID(self)->name);

    case F1WAE_FUN:
      return (F1WAE *) F1WAEFun_new(
        F1WAEFUN(self)->name,
        F1WAEFUN(self)->arg,
        F1WAE_clone(F1WAEFUN(self)->body)
      );

    default:
      fprintf(stderr, "error: F1WAE_clone: unknown type\n");
      exit(1);
  }
  return self;
}

F1WAE *F1WAE_subst(F1WAE *expr, char *name, F1WAENum *num) {
  F1WAEAdd  *add;
  F1WAEWith *with;
  F1WAEId   *id;
  F1WAEApp  *app;

  switch (expr->type) {
    case F1WAE_NUM:
      return expr;

    case F1WAE_ADD:
      add = F1WAEADD(expr);
      add->lhs = F1WAE_subst(add->lhs, name, num);
      add->rhs = F1WAE_subst(add->rhs, name, num);
      return (F1WAE *) add;

    case F1WAE_WITH:
      with = F1WAEWITH(expr);
      with->expr = F1WAE_subst(with->expr, name, num);

      if (strcmp(with->id->name, name) == 0) {
        // new binding instance for this name, don't substitute
        return expr;
      } else {
        // enter new scope and substitute
        with->body = F1WAE_subst(with->body, name, num);
        return (F1WAE *) with;
      }

    case F1WAE_APP:
      app = F1WAEAPP(expr);
      app->expr = F1WAE_subst(app->expr, name, num);
      return (F1WAE *) app;

    case F1WAE_ID:
      id = F1WAEID(expr);
      if (strcmp(name, id->name) == 0) {
        // matching identifier, substitute with new num
        F1WAE_free((F1WAE *) id);
        return (F1WAE *) F1WAENum_new(num->val);
      } else {
        // no match
        return expr;
      }

    default:
      fprintf(stderr, "error: F1WAE_subst: unknown type\n");
      exit(1);
  }
}

int F1WAE_interp(F1WAE *self, FunList *funs) {
  F1WAE     *body;
  F1WAENum  *num;
  F1WAEAdd  *add;
  F1WAEWith *with;
  F1WAEApp  *app;
  F1WAEFun  *fun;
  int       result;

  switch (self->type) {
    case F1WAE_NUM:
      result = F1WAENUM(self)->val;
      break;

    case F1WAE_ADD:
      add = F1WAEADD(self);
      result = F1WAE_interp(add->lhs, funs) + F1WAE_interp(add->rhs, funs);
      break;

    case F1WAE_WITH:
      with = F1WAEWITH(self);
      num  = F1WAENum_new(F1WAE_interp(with->expr, funs));
      with->body = F1WAE_subst(with->body, with->id->name, num);

      result = F1WAE_interp(with->body, funs);

      F1WAE_free((F1WAE *) num);
      break;

    case F1WAE_APP:
      app = F1WAEAPP(self);
      fun = FunList_lookup(funs, app->id->name);
      body = (F1WAE *) F1WAE_clone(fun->body);
      num = F1WAENum_new(F1WAE_interp(app->expr, funs));

      result = F1WAE_interp(
        F1WAE_subst(body, fun->arg, num),
        funs
      );

      F1WAE_free(body);
      F1WAE_free((F1WAE *) num);
      break;

    case F1WAE_ID:
      fprintf(stderr, "error: F1WAE_interp: free identifier '%s'\n", F1WAEID(self)->name);
      exit(1);

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
