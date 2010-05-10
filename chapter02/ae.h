#ifndef AE_H
#define AE_H

#define AEOP(x) ((AEOp*) x)
#define AENUM(x) ((AENum*) x)


typedef unsigned long Val;

// tree types
typedef enum AEType {
  AE_NUM,
  AE_ADD,
  AE_SUB
} AEType;

// base type
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

// global tree root
AE* tree;


#endif
