#ifndef AE_H
#define AE_H

#define AEOP(x) ((AEOp*) x)
#define AENUM(x) ((AENum*) x)

typedef unsigned long Expr;

// node types
typedef enum {
  AE_NUM,
  AE_ADD,
  AE_SUB
} AEType;

// base node
typedef struct {
  AEType type;
} AE;

// number
typedef struct {
  AEType type;
  int    val;
} AENum;

// +/- operation
typedef struct {
  AEType type;
  AE*    lhs;
  AE*    rhs;
} AEOp;


AE* AE_parse();

int AE_calc(AE* node);

char* AE_print(AE* node);

void AE_free(AE* node);

#endif
