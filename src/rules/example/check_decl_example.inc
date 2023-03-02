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
// check_decl_example.inc
// ====================================================================
//
// example to describe how to write a checker to check clang Decl.
//
#include "scope_manager.h"
#include <clang/AST/Decl.h>
#include <vector>
class CheckDeclExample : public DeclNullHandler {
public:
  ~CheckDeclExample() {}

private:
  void CheckFunctionNameReuse() {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    top_scope->TraverseAll<IdentifierManager::IdentifierKind::NON_FUNC>(
        [&top_scope](const std::string &x, IdentifierManager *id_mgr) -> void {
          if (top_scope->HasFunctionName(x)) {
            printf("GJB5396:4.1.1.1: Function name reused: %s\n", x.c_str());
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
        it->TraverseAll<kind>([&it](const std::string &x,
                                    IdentifierManager *id_mgr) -> void {
          if (it->HasVariableName<false>(x)) {
            printf("GJB5396:4.1.1.2: Variable name reused: %s\n", x.c_str());
          }
        });
      }
    }
  }

  /** GJB5396
   * 4.1.1.3 struct with empty field is forbidden
   */
  void CheckStructEmptyField(const clang::RecordDecl *decl) {
    for (const auto &it : decl->fields()) {
      if (it->isAnonymousStructOrUnion()) {
        printf("GJB5396:4.1.1.3: Struct with empty field is forbidden: struct: "
               "%s\n",
               decl->getNameAsString().c_str());
      }
    }
  }

  /* GJB5396
   * TODO: 4.1.1.5 declaring the type of parameters is a must
   * TODO: 4.1.1.6 without the parameter declarations
   * in function declaration is forbidden
   * 4.1.1.8 ellipsis in the function parameter list is forbidden
   * 4.1.1.10 the empty function parameter list is forbidden
   * */
  void CheckParameterTypeDecl(const clang::FunctionDecl *decl) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    auto func_loc = decl->getLocation();
    auto func_raw_chars = src_mgr->getCharacterData(func_loc);

    std::string token = "";
    std::vector<std::string> tokens;

    // eat function name
    while (*func_raw_chars != '(')
      func_raw_chars++;

    /* Maybe there are some APIs for getting tokens of parameter decl.
     * Such as clang::SourceRange and clang::Lexer.
     * TODO: Refine the tokenize methods with clang APIs
     */
    do {
      *func_raw_chars++; // eat '(' or  ',' or ' '
      while ((*func_raw_chars != ',') && (*func_raw_chars != ' ') &&
             (*func_raw_chars != ')')) {
        token += *func_raw_chars;
        func_raw_chars++;
      }

      if (token != "") {
        tokens.push_back(token);
        token = "";
      }

    } while (*func_raw_chars != ')');

    /* 4.1.1.10
     * The empty function parameter list is forbidden
     */
    if (decl->param_empty()) {
      if ((tokens.size() == 1) && (tokens[0] == "void")) {
        return;
      } else if (token.size() == 0) {
        printf("GJB5396:4.1.1.10: The empty function parameter list is "
               "forbidden: %s\n",
               decl->getNameAsString().c_str());
      } else {
        DBG_ASSERT(0, "Unknown fault.");
      }
    }

    /* 4.1.1.8
     * ellipsis in the function parameter list is forbidden
     */
    for (const auto &it : tokens) {
      if (it == "...") {
        printf("GJB5396:4.1.1.8: \"...\" in the funtion's parameter list is "
               "forbidden: %s\n",
               decl->getNameAsString().c_str());
      }
    }

    //    for (const auto &it : tokens) {
    //      printf("Token: %s\n", it.c_str());
    //    }
  }

  /* GJB5369: 4.1.1.7
   * Only type but no identifiers in function prototype.
   */
  void CheckParameterNoIdentifier(const clang::FunctionDecl *decl) {
    for (const auto &it : decl->parameters()) {
      if (it->getNameAsString() == "") {
        clang::SourceManager *src_mgr = XcalCheckerManager::GetSourceManager();
        printf("GJB5396:4.1.1.7: Only type but no identifiers in function %s, "
               "loc: %s\n",
               decl->getNameAsString().c_str(),
               it->getLocation().printToString(*src_mgr).c_str());
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
            printf("GJB5396:4.1.1.9: Redefining the keywords of C/C++ is "
                   "forbidden: %s\n",
                   x.c_str());
          }
        });
  }

public:
  void Finalize() {
    CheckFunctionNameReuse();
    CheckVariableNameReuse();
    CheckKeywordRedefine();
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
      } else {
        printf("Cast failed\n");
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

    CheckParameterNoIdentifier(decl);
    CheckParameterTypeDecl(decl);
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

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckStructEmptyField(decl);
  }

}; // CheckDeclExample
