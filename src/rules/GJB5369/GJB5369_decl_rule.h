/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_decl_rule.h
// ====================================================================
//
// implement Decl related rules for GJB5369
//

#include "scope_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

class GJB5369DeclRule : public DeclNullHandler {
public:
  ~GJB5369DeclRule() {}

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

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckParameterNoIdentifier(decl);
    CheckParameterTypeDecl(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckStructEmptyField(decl);
  }

}; // GJB5369DeclRule

