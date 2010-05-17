#include "f1wae.h"


int main() {
  F1WAE *tree = F1WAE_parse();

  if (tree != NULL) {
    char *out = F1WAE_print(tree);

    printf("%s\n", out);
    //printf("%d\n", F1WAE_calc(tree));

    F1WAE_free(tree);
    free(out);
  }
  return 0;
}
