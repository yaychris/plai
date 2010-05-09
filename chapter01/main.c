#include <stdio.h>

int main() {
  while (yyparse())
    ;

  return 0;
}
