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
#include "decl_null_handler.h"
//#include <clang/AST/Decl.h>
//#include <vector>

namespace xsca {
namespace rule {
class SJT11682DeclRule : public DeclNullHandler {
public:
  ~SJT11682DeclRule() {}

private:

public:

  void VisitFunction(const clang::FunctionDecl *decl) {
    XcalCheckerManager::SetCurrentFunction(decl);
  }
  void Finalize() {
  }


}; // SJT11682DeclRule
}  // rule
}  // xsca
