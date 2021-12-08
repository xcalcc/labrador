/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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

private:
  const clang::FunctionDecl *_current_function_decl;

public:
  IdentifierBuilder() {
    _enable = true;
  }

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
    // TODO: Is this useless? not so sure
    for (const auto &it : decl->parameters()) {
      if (clang::dyn_cast<clang::VarDecl>(it)) {
        scope_mgr->CurrentScope()->AddIdentifier<clang::VarDecl>(
            clang::dyn_cast<clang::VarDecl>(it));
      }
    }

    /* Collect exception-specifications of functions */
    auto func_type = decl->getType()->getAs<clang::FunctionProtoType>();
    if (func_type) {
      if (!func_type->hasExceptionSpec()) return;
      for (const auto &it : func_type->exceptions()) {
        scope_mgr->GlobalScope()->AddExceptionSpec(decl, it);
      }
    }
  }

  void VisitVar(const clang::VarDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();

    /* Add VarDecl to the lexical scope. */
    scope_mgr->CurrentScope()->AddIdentifier<clang::VarDecl>(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    // Add CXXRecord to current lexical scope.
    scope_mgr->CurrentScope()->AddIdentifier<clang::RecordDecl>(decl);

    // Add field to current lexical scope.
    for (const auto &it : decl->fields()) {
      scope_mgr->CurrentScope()->AddIdentifier<clang::FieldDecl>(it);
    }
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    // Add CXXRecord to current lexical scope.
    scope_mgr->CurrentScope()->AddIdentifier<clang::RecordDecl>(decl);

    // Add field to current lexical scope.
    for (const auto &it : decl->fields()) {
      scope_mgr->CurrentScope()->AddIdentifier<clang::FieldDecl>(it);
    }
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    scope_mgr->CurrentScope()->AddIdentifier<clang::TypedefDecl>(decl);
  }

public:
  void SetCurrentFunctionDecl(const clang::FunctionDecl *decl) {
    _current_function_decl = decl;
  }

}; // IdentifierBuilder
}
}
