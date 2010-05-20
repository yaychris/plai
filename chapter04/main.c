#include "f1wae.h"

static int usage(char* exe) {
  printf(
    "usage: %s [options] [file]\n"
    "options:\n"
    "  -e   eval code\n",
    exe
  );
  return 1;
}

int main(int argc, char* argv[]) {
  F1WAE *tree;

  if (argc == 2) {
    // parse file
    printf("file\n");
  } else if (argc == 3 && strcmp(argv[1], "-e") == 0) {
    // eval
    tree = F1WAE_parse(argv[2]);
  } else {
    return usage(argv[0]);
  }

  if (tree != NULL) {
    //char *out = F1WAE_print(tree);
    //char *out = malloc(512 * sizeof(char));
    //F1WAEFun *fun = F1WAEFun_new(strdup("double"), strdup("x"), tree);
    //FunList *list = FunList_new();

    //FunList_addFun(list, fun);

    printf(
      "%s\n",
      F1WAE_print(F1WAE_subst(tree, "x", F1WAENum_new(5)))
    );
    //printf("%s\n", out);
    //printf("%d\n", F1WAE_calc(tree));

    //FunList_free(list);
    //free(out);
  }
  return 0;
}
