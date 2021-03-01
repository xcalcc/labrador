/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// check_type_example.inc
// ====================================================================
//
// example to describe how to write checker which checks clang Type
//
#include <clang/AST/Type.h>
class CheckTypeExample : public TypeNullHandler {
public:
  ~CheckTypeExample() {}

public:
  void VisitEnum(const clang::EnumType *type) {
    //    printf("Hit an enum type.\n");
  }

  void VisitRecord(const clang::RecordType *type) {
//    type->dump();
  }

}; // CheckTypeExample
