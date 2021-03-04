/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// SJT11682_decl_rule.h
// ====================================================================
//
// implement Decl related rules for SJT11682
//

#include "scope_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

class SJT11682DeclRule : public DeclNullHandler {
public:
  ~SJT11682DeclRule() {}

private:

public:
  void Finalize() {
  }

}; // SJT11682DeclRule

