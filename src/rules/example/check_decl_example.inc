/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// check_decl_example.inc
// ====================================================================
//
// example to describe how to write a checker to check clang Decl.
//
#include <clang/AST/Decl.h>
class CheckDeclExample : public DeclNullHandler {
public:
  ~CheckDeclExample() {
    TRACE0();
  }

public:
  void VisitLabel(const clang::LabelDecl *decl) {
    printf("Hit a LabelDecl.\n");
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    scope_mgr->CurrentScope()->AddIdentifier<clang::LabelDecl>(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    printf("Hit a EnumDecl.\n");
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    for (const auto &it : decl->enumerators()) {
      if (clang::dyn_cast<clang::ValueDecl>(it)) {
        scope_mgr->CurrentScope()->
          AddIdentifier<clang::ValueDecl>(clang::dyn_cast<clang::ValueDecl>(it));
      } else {
        printf("Cast failed\n");
      }
    }
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    printf("Hit a Function Decl.\n");
    auto scope_mgr = XcalCheckerManager::GetScopeManager();

    /* Add FuncParamDecl to the lexical scope. */
    for (const auto &it : decl->parameters()) {
      if (clang::dyn_cast<clang::VarDecl>(it)) {
        scope_mgr->CurrentScope()->
            AddIdentifier<clang::VarDecl>(clang::dyn_cast<clang::VarDecl>(it));
      }
    }
  }

  void VisitVar(const clang::VarDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();

    /* Add VarDecl to the lexical scope. */
    scope_mgr->CurrentScope()->AddIdentifier<clang::VarDecl>(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    printf("Hit a CXXRecord.\n");
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    printf("Hit a Record.\n");
  }


};  // CheckDeclExample
