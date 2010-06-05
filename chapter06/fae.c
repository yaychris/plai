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

int FAE_interp(FAE *self, SubList **subs) {
  FAEAdd  *add;
  FAEApp  *app;
  FAEFun  *fun;
  int     result;

  switch (self->type) {
    case FAE_NUM:
      result = FAENUM(self)->val;
      break;

    case FAE_ID:
      result = SubList_lookup(*subs, FAEID(self)->name);
      break;

    case FAE_ADD:
      add = FAEADD(self);
      result = FAE_interp(add->lhs, subs) + FAE_interp(add->rhs, subs);
      break;

    case FAE_APP:
      app = FAEAPP(self);
      fun = FAEFUN(app->funexpr);

      SubList_unshift(
        subs,
        fun->arg->name,
        FAE_interp(app->argexpr, subs)
      );

      result = FAE_interp(fun->body, subs);

      break;

    default:
      fprintf(stderr, "error: FAE_interp: unknown type\n");
      exit(1);
  }
  return result;
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
      FAE_free((FAE *) FAEFUN(self)->arg);
      FAE_free(FAEFUN(self)->body);
      break;
    default:
      fprintf(stderr, "error: FAE_free: unknown type");
  }
  free(self);
}


////
// deferred substitution

SubList *SubList_new() {
  SubList *self = malloc(sizeof(SubList));
  self->name = NULL;
  self->val  = 0;
  self->next = NULL;
  return self;
}

void SubList_unshift(SubList **self, char *name, int val) {
  SubList *node = malloc(sizeof(SubList));
  node->name = strdup(name);
  node->val = val;
  node->next = *self;
  *self = node;
}

int SubList_lookup(SubList *self, char *name) {
  SubList *iter;

  iter = self;
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
  SubList *iter;

  for (iter = self; iter != NULL; iter = self) {
    if (iter->name != NULL) {
      free(iter->name);
    }
    self = iter->next;
    free(iter);
  }
}
