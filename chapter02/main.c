#include <stdlib.h>
#include <stdio.h>
#include "ae.h"

int calc(AE* node) {
  switch (node->type) {
    case AE_NUM:
      return AENUM(node)->val;
    case AE_ADD:
      return calc(AEOP(node)->lhs) + calc(AEOP(node)->rhs);
    case AE_SUB:
      return calc(AEOP(node)->lhs) - calc(AEOP(node)->rhs);
  }
  return -1;
}

void free_ae(AE* node) {
  switch (node->type) {
    case AE_NUM:
      free(node);
      break;
    case AE_ADD:
    case AE_SUB:
      free_ae(AEOP(node)->lhs);
      free_ae(AEOP(node)->rhs);
      free(node);
      break;
  }
}

char* ae_to_str(AE* node) {
  char* buf = malloc(sizeof(char) * 512);

  switch (node->type) {
    case AE_NUM:
      sprintf(buf, "(num %d)", AENUM(node)->val);
      break;
    case AE_ADD:
      sprintf(buf, "(add %s %s)", ae_to_str(AEOP(node)->lhs), ae_to_str(AEOP(node)->rhs));
      break;
    case AE_SUB:
      sprintf(buf, "(sub %s %s)", ae_to_str(AEOP(node)->lhs), ae_to_str(AEOP(node)->rhs));
      break;
  }
  return buf;
}

int main() {
  while (yyparse())
    ;

  printf("%d\n", calc(tree));
  printf("%s\n", ae_to_str(tree));
  free_ae(tree);

  return 0;
}
