#include "fae.h"

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
  FAE *tree;

  if (argc == 2) {
    // parse file
    printf("file\n");
  } else if (argc == 3 && strcmp(argv[1], "-e") == 0) {
    // eval
    tree = FAE_parse(argv[2]);
  } else {
    return usage(argv[0]);
  }

  if (tree != NULL) {
    char *out = FAE_print(tree);

    /*FunList *funs = FunList_new();
    SubList *subs = SubList_new();
    
    FunList_push(
      funs,
      FAEFun_new("double", "x", FAE_parse("{+ x x}"))
    );
    FunList_push(
      funs,
      FAEFun_new("triple", "x", FAE_parse("{+ x {+ x x}}"))
    );*/

    printf("%s\n", out);

    FAE_free(tree);
    //FunList_free(funs);
    //SubList_free(subs);
    free(out);
  }
  return 0;
}
