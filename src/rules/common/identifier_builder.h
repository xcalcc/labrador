/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// identifier_builder.h
// ====================================================================
//
// check decls to collect identifier information
//
#include "scope_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

namespace xsca {
namespace rule {

class IdentifierBuilder : public DeclNullHandler {
public:
  ~IdentifierBuilder() {}

public:
  void VisitLabel(const clang::LabelDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    scope_mgr->CurrentScope()->AddIdentifier<clang::LabelDecl>(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    for (const auto &it : decl->enumerators()) {
      if (clang::dyn_cast<clang::ValueDecl>(it)) {
        scope_mgr->CurrentScope()->AddIdentifier<clang::ValueDecl>(
            clang::dyn_cast<clang::ValueDecl>(it));
      }
    }
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    /* Add FunctionDecl to global scope */
    scope_mgr->GlobalScope()->AddIdentifier<clang::FunctionDecl>(decl);

    /* Add FuncParamDecl to the lexical scope. */
    for (const auto &it : decl->parameters()) {
      if (clang::dyn_cast<clang::VarDecl>(it)) {
        scope_mgr->CurrentScope()->AddIdentifier<clang::VarDecl>(
            clang::dyn_cast<clang::VarDecl>(it));
      }
    }
  }

  void VisitVar(const clang::VarDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();

    /* Add VarDecl to the lexical scope. */
    scope_mgr->CurrentScope()->AddIdentifier<clang::VarDecl>(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    // Add CXXRecord to current lexical scope.
    scope_mgr->CurrentScope()->AddIdentifier<clang::TypeDecl>(
        clang::dyn_cast<clang::TypeDecl>(decl));

    // Add field to current lexical scope.
    for (const auto &it : decl->fields()) {
      scope_mgr->CurrentScope()->AddIdentifier<clang::FieldDecl>(it);
    }
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    scope_mgr->CurrentScope()->AddIdentifier<clang::TypedefDecl>(decl);
  }

}; // IdentifierBuilder
}
}