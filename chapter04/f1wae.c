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

//F1WAE *F1WAE_clone(F1WAE *self) {
//}

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
        free(id);
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

int F1WAE_interp(F1WAE *self) {
  F1WAENum  *num;
  F1WAEAdd  *add;
  F1WAEWith *with;

  switch (self->type) {
    case F1WAE_NUM:
      return F1WAENUM(self)->val;

    case F1WAE_ADD:
      add = F1WAEADD(self);
      return F1WAE_interp(add->lhs) + F1WAE_interp(add->rhs);

    case F1WAE_WITH:
      with = F1WAEWITH(self);
      num  = F1WAENum_new(F1WAE_interp(with->expr));
      with->body = F1WAE_subst(with->body, with->id->name, num);
      return F1WAE_interp(with->body);

    case F1WAE_ID:
      fprintf(stderr, "error: F1WAE_interp: free identifier '%s'\n", F1WAEID(self)->name);
      exit(1);

    default:
      fprintf(stderr, "error: F1WAE_interp: unknown type\n");
      exit(1);
  }
}

char *F1WAE_print(F1WAE *self) {
  char *buf = malloc(sizeof(char) * 512);

  switch (self->type) {
    case F1WAE_NUM:
      sprintf(buf, "(num %d)", F1WAENUM(self)->val);
      break;

    case F1WAE_ID:
      sprintf(buf, "(id %s)", F1WAEID(self)->name);
      break;

    case F1WAE_WITH:
      sprintf(buf, "(with %s %s %s)",
        F1WAE_print((F1WAE *) F1WAEWITH(self)->id),
        F1WAE_print(F1WAEWITH(self)->expr),
        F1WAE_print(F1WAEWITH(self)->body)
      );
      break;

    case F1WAE_ADD:
      sprintf(buf, "(add %s %s)",
        F1WAE_print(F1WAEADD(self)->lhs),
        F1WAE_print(F1WAEADD(self)->rhs)
      );
      break;

    case F1WAE_APP:
      sprintf(buf, "(app %s %s)",
        F1WAE_print((F1WAE *) F1WAEAPP(self)->id),
        F1WAE_print(F1WAEAPP(self)->expr)
      );
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
      free(F1WAEWITH(self)->id);
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

void FunList_free(FunList *self) {
  FunList *next;

  while (self != NULL) {
    F1WAE_free((F1WAE *) self->fun);
    next = self->next;
    free(self);
    self = next;
  }
}
