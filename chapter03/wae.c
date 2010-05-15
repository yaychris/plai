#include "wae.h"

WAENum* WAENum_new(int val) {
  WAENum* num = malloc(sizeof(WAENum));
  num->type = WAE_NUM;
  num->val  = val;
  return num;
}

WAEOp* WAEOp_new(unsigned short type, WAE* l, WAE* r) {
  WAEOp* op = malloc(sizeof(WAEOp));
  op->type = type;
  op->lhs  = l;
  op->rhs  = r;
  return op;
}

WAEId* WAEId_new(char* name) {
  WAEId* id = malloc(sizeof(WAEId));
  id->type = WAE_ID;
  id->name = name;
  return id;
}

WAEWith* WAEWith_new(WAEId* id, WAE* expr, WAE* body) {
  WAEWith* with = malloc(sizeof(WAEWith));
  with->type = WAE_WITH;
  with->id   = id;
  with->expr = expr;
  with->body = body;
  return with;
}

int WAE_calc(WAE* self) {
  switch (self->type) {
    case WAE_NUM:
      return WAENUM(self)->val;
    case WAE_ADD:
      return WAE_calc(WAEOP(self)->lhs) + WAE_calc(WAEOP(self)->rhs);
    case WAE_SUB:
      return WAE_calc(WAEOP(self)->lhs) - WAE_calc(WAEOP(self)->rhs);
  }
  return -1;
}

char* WAE_print(WAE* self) {
  char* buf = malloc(sizeof(char) * 512);

  switch (self->type) {
    case WAE_NUM:
      sprintf(buf, "(num %d)", WAENUM(self)->val);
      break;
    case WAE_ID:
      sprintf(buf, "(id %s)", WAEID(self)->name);
      break;
    case WAE_WITH:
      sprintf(buf, "(with %s %s %s)",
        WAE_print((WAE*) WAEWITH(self)->id),
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

void WAE_free(WAE* self) {
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
