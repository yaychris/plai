#include "wae.h"

WAENum *WAENum_new(int val) {
  WAENum *num = malloc(sizeof(WAENum));
  num->type = WAE_NUM;
  num->val  = val;
  return num;
}

WAEOp *WAEOp_new(unsigned short type, WAE* l, WAE* r) {
  WAEOp *op = malloc(sizeof(WAEOp));
  op->type = type;
  op->lhs  = l;
  op->rhs  = r;
  return op;
}

WAEId *WAEId_new(char *name) {
  WAEId *id = malloc(sizeof(WAEId));
  id->type = WAE_ID;
  id->name = name;
  return id;
}

WAEWith *WAEWith_new(WAEId *id, WAE *expr, WAE *body) {
  WAEWith *with = malloc(sizeof(WAEWith));
  with->type = WAE_WITH;
  with->id   = id;
  with->expr = expr;
  with->body = body;
  return with;
}

WAE *WAE_subst(WAE *expr, char *name, WAENum *num) {
  WAEOp   *op;
  WAEWith *with;
  WAEId   *id;

  switch (expr->type) {
    case WAE_NUM:
      return expr;
    case WAE_ADD:
    case WAE_SUB:
      op = WAEOP(expr);
      op->lhs = WAE_subst(op->lhs, name, num);
      op->rhs = WAE_subst(op->rhs, name, num);
      return (WAE *) op;
    case WAE_WITH:
      with = WAEWITH(expr);
      if (strcmp(with->id->name, name) == 0) {
        // new binding instance for this name, don't substitute
        return expr;
      } else {
        // enter new scope and substitute
        with->body = WAE_subst(with->body, name, num);
        return (WAE *) with;
      }
    case WAE_ID:
      id = WAEID(expr);
      if (strcmp(name, id->name) == 0) {
        // matching identifier, substitute with new num
        free(id);
        return (WAE *) WAENum_new(num->val);
      } else {
        // no match
        return expr;
      }
    default:
      fprintf(stderr, "error: WAE_subst: unknown type\n");
      exit(1);
  }
}

int WAE_calc(WAE *self) {
  WAENum  *num;
  WAEOp   *op;
  WAEWith *with;

  switch (self->type) {
    case WAE_NUM:
      return WAENUM(self)->val;
    case WAE_ADD:
      op = WAEOP(self);
      return WAE_calc(op->lhs) + WAE_calc(op->rhs);
    case WAE_SUB:
      op = WAEOP(self);
      return WAE_calc(op->lhs) - WAE_calc(op->rhs);
    case WAE_WITH:
      with = WAEWITH(self);
      num  = WAENum_new(WAE_calc(with->expr));

      with->body = WAE_subst(with->body, with->id->name, num);
      return WAE_calc(with->body);
    case WAE_ID:
      fprintf(stderr, "error: WAE_calc: free identifier '%s'\n", WAEID(self)->name);
      exit(1);
    default:
      fprintf(stderr, "error: WAE_calc: unknown type\n");
      exit(1);
  }
}

char *WAE_print(WAE *self) {
  char *buf = malloc(sizeof(char) * 512);

  switch (self->type) {
    case WAE_NUM:
      sprintf(buf, "(num %d)", WAENUM(self)->val);
      break;
    case WAE_ID:
      sprintf(buf, "(id %s)", WAEID(self)->name);
      break;
    case WAE_WITH:
      sprintf(buf, "(with %s %s %s)",
        WAE_print((WAE *) WAEWITH(self)->id),
        WAE_print(WAEWITH(self)->expr),
        WAE_print(WAEWITH(self)->body)
      );
      break;
    case WAE_ADD:
    case WAE_SUB:
      sprintf(buf, "(%s %s %s)",
        self->type == WAE_ADD ? "add" : "sub",
        WAE_print(WAEOP(self)->lhs),
        WAE_print(WAEOP(self)->rhs)
      );
      break;
  }
  return buf;
}

void WAE_free(WAE *self) {
  switch (self->type) {
    case WAE_NUM:
      break;
    case WAE_ID:
      free(WAEID(self)->name);
      break;
    case WAE_WITH:
      free(WAEWITH(self)->id);
      free(WAEWITH(self)->expr);
      free(WAEWITH(self)->body);
      break;
    case WAE_ADD:
    case WAE_SUB:
      WAE_free(WAEOP(self)->lhs);
      WAE_free(WAEOP(self)->rhs);
      break;
  }
  free(self);
}
