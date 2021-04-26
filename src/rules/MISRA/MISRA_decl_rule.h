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
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class MISRADeclRule : public DeclNullHandler {
public:
  ~MISRADeclRule() = default;

private:
  std::set<const clang::TypedefNameDecl *> _used_typedef;

  /* MISRA
   * Rule: 2.3
   * A project should not contain unused type declarations
   */
  void CheckUnusedTypedef(const clang::VarDecl *decl);

  void CheckUnusedTypedef(const clang::TypedefDecl *decl);

  void CheckUnusedTypedef();

public:
  void Finalize() {
    CheckUnusedTypedef();
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckUnusedTypedef(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckUnusedTypedef(decl);
  }

}; // MISRADeclRule
} // rule
} // xsca

