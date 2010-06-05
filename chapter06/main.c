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
    SubList *subs = SubList_new();

    char *out = FAE_print(tree);
    printf("%s\n", out);

    //printf("%d\n", FAE_interp(tree, &subs));

    FAE_free(tree);
    SubList_free(subs);
    free(out);
  }
  return 0;
}
