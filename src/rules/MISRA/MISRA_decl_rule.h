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

  /* MISRA
   * Rule: 2.6
   * A function should not contain unused label declarations
   */
  void CheckUnusedLabelInFunction();

  /* MISRA
   * Rule: 2.7
   * There should be no unused parameters in functions
   */
  void CheckUnusedParameters(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 5.1
   * External identifiers shall be distinct
   */
  void CheckUndistinctExternalIdent();

public:
  void Finalize() {
    CheckUnusedTypedef();
    CheckUnusedLabelInFunction();
    CheckUndistinctExternalIdent();
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckUnusedTypedef(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckUnusedTypedef(decl);
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckUnusedParameters(decl);
  }

}; // MISRADeclRule
} // rule
} // xsca

