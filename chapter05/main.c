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
    char *out = F1WAE_print(tree);

    FunList *funs = FunList_new();
    SubList *subs = SubList_new();
    
    FunList_addFun(
      funs,
      F1WAEFun_new("double", "x", F1WAE_parse("{+ x x}"))
    );

    printf(
      "%s\n%d\n",
      out,
      F1WAE_interp(tree, funs, subs)
    );

    F1WAE_free(tree);
    FunList_free(funs);
    SubList_free(subs);
    free(out);
  }
  return 0;
}
