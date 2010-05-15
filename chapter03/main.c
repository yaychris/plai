#include "wae.h"


int main() {
  WAE* tree = WAE_parse();

  if (tree != NULL) {
    char* out = WAE_print(tree);

    //printf("%d\n", WAE_calc(tree));
    printf("%s\n", out);

    WAE_free(tree);
    free(out);
  }
  return 0;
}
