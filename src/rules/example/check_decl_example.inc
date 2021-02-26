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
#include "scope_manager.h"
#include <clang/AST/Decl.h>
class CheckDeclExample : public DeclNullHandler {
public:
  ~CheckDeclExample() { TRACE0(); }

private:
  void CheckFunctionNameReuse() {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    top_scope->TraverseAll<IdentifierManager::IdentifierKind::NON_FUNC>(
        [&top_scope](const std::string &x, IdentifierManager *id_mgr) -> void {
          if (top_scope->HasFunctionName(x)) {
            printf("Function name reused: %s\n", x.c_str());
          }
        });
  }

  void CheckVariableNameReuse() {
    using IdentifierKind = IdentifierManager::IdentifierKind;
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    constexpr uint32_t kind =
        IdentifierKind::VALUE | IdentifierKind ::LABEL | IdentifierKind::FIELD;
    for (const auto &it : top_scope->Children()) {
      if (it->GetScopeKind() == SK_FUNCTION) {
        it->TraverseAll<kind>(
            [&it](const std::string &x, IdentifierManager *id_mgr) -> void {
              if (it->HasVariableName(x)) {
                printf("Variable name reused: %s\n", x.c_str());
              }
            });
      }
    }
  }

  /* GJB5369: 4.1.1.9
   * redefining the keywords of C/C++ is forbidden
   */
  void CheckKeywordRedefine() {
    using IdentifierKind = IdentifierManager::IdentifierKind;
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    top_scope->TraverseAll<IdentifierKind::VAR>(
        [](const std::string &x, IdentifierManager *id_mgr) -> void {
          if (id_mgr->IsKeyword(x)) {
            printf("Redefining the keywords of C/C++ is forbidden: %s\n",
                   x.c_str());
          }
        });
  }

  /* GJB5369: 4.1.1.7
   * Only type but no identifiers in function prototype.
   */
  void CheckFunctionParameter(const clang::FunctionDecl *decl) {
    for (const auto &it : decl->parameters()) {
      if (it->getNameAsString() == "") {
        clang::SourceManager *src_mgr = XcalCheckerManager::GetSourceManager();
        printf("Only type but no identifiers in function %s, loc: %s\n",
               decl->getNameAsString().c_str(),
               it->getLocation().printToString(*src_mgr).c_str());
      }
    }
  }

public:
  void Finalize() {
    CheckFunctionNameReuse();
    CheckVariableNameReuse();
    CheckKeywordRedefine();
  }

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
        scope_mgr->CurrentScope()->AddIdentifier<clang::ValueDecl>(
            clang::dyn_cast<clang::ValueDecl>(it));
      } else {
        printf("Cast failed\n");
      }
    }
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    printf("Hit a Function Decl.\n");
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

    CheckFunctionParameter(decl);
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
        clang::cast<clang::TypeDecl>(decl));

    // Add field to current lexical scope.
    for (const auto &it : decl->fields()) {
      scope_mgr->CurrentScope()->AddIdentifier<clang::FieldDecl>(it);
    }
  }

  void VisitRecord(const clang::RecordDecl *decl) { printf("Hit a Record.\n"); }

}; // CheckDeclExample
