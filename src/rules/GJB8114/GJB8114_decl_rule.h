/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_decl_rule.h
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include "scope_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

class GJB8114DeclRule : public DeclNullHandler {
public:
  ~GJB8114DeclRule() {}

private:

public:
  void Finalize() {
  }

}; // GJB8114DeclRule

