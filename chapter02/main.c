#include <stdlib.h>
#include <stdio.h>
#include "ae.h"

int AE_calc(AE* node) {
  switch (node->type) {
    case AE_NUM:
      return AENUM(node)->val;
    case AE_ADD:
      return AE_calc(AEOP(node)->lhs) + AE_calc(AEOP(node)->rhs);
    case AE_SUB:
      return AE_calc(AEOP(node)->lhs) - AE_calc(AEOP(node)->rhs);
  }
  return -1;
}

void AE_free(AE* node) {
  switch (node->type) {
    case AE_NUM:
      free(node);
      break;
    case AE_ADD:
    case AE_SUB:
      AE_free(AEOP(node)->lhs);
      AE_free(AEOP(node)->rhs);
      free(node);
      break;
  }
}

char* AE_print(AE* node) {
  char* buf = malloc(sizeof(char) * 512);
  char* op;
  char* l;
  char* r;

  switch (node->type) {
    case AE_NUM:
      sprintf(buf, "(num %d)", AENUM(node)->val);
      break;
    default:
      op = node->type == AE_ADD ? "add" : "sub";
      l  = AE_print(AEOP(node)->lhs);
      r  = AE_print(AEOP(node)->rhs);
      sprintf(buf, "(%s %s %s)", op, l, r);
      free(l);
      free(r);
      break;
  }
  return buf;
}

AE* AE_parse();

int main() {
  AE* tree = AE_parse();
  char* out = AE_print(tree);

  printf("%d\n", AE_calc(tree));
  printf("%s\n", out);

  AE_free(tree);
  free(out);
  return 0;
}
