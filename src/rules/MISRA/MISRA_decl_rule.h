/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_decl_rule.h
// ====================================================================
//
// implement Decl related rules for MISRA-C-2012
//

#include "scope_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

class MISRADeclRule : public DeclNullHandler {
public:
  ~MISRADeclRule() {}

private:

public:
  void Finalize() {
  }

}; // MISRADeclRule

