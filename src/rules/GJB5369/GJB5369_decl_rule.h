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
#include <set>

class GJB5369DeclRule : public DeclNullHandler {
public:
  ~GJB5369DeclRule() {}

private:
  std::string GetTypeString(clang::QualType type) {
    std::string type_name;
    if (type->getTypeClass() == clang::Type::Typedef) {
      auto underlying_tp =
          clang::dyn_cast<clang::TypedefType>(type)->getDecl()->getUnderlyingType();
      type_name = underlying_tp.getAsString();
    } else {
      type_name = type.getAsString();
    }
    return type_name;
  }

  bool IsExplicitSign(std::string type_name) {
    if (type_name.find("unsigned") != std::string::npos) {
      return false;
    } else {
      if (type_name.find("signed") != std::string::npos) {
        return false;
      }
    }
    return true;
  };

  bool IsTypedefBasicType(clang::QualType &decl_type) {
    if (decl_type->isBuiltinType()) {
      if (decl_type->getTypeClass() != clang::Type::Typedef) {
        return true;
      }
    }
    return false;
  };

  void GetFunctionTokens(const clang::FunctionDecl *decl,
                         std::vector<std::string> &tokens) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    auto func_loc = decl->getLocation();
    auto func_raw_chars = src_mgr->getCharacterData(func_loc);

    std::string token = "";

    // eat function name
    while (*func_raw_chars != '(')
      func_raw_chars++;

    /* Maybe there are some APIs for getting tokens of parameter decl.
     * Such as clang::SourceRange and clang::Lexer.
     * TODO: Refine the tokenize methods with clang APIs
     */
    do {
      *func_raw_chars++; // eat '(' or  ',' or ' '
      while ((*func_raw_chars != ',') && (!std::isspace(*func_raw_chars)) &&
             (*func_raw_chars != ')')) {
        token += *func_raw_chars;
        func_raw_chars++;
      }

      if (token != "") {
        tokens.push_back(token);
        token = "";
      }

    } while (*func_raw_chars != ')');
  }

  bool IsEmptyParamList(const clang::FunctionDecl *decl,
                        std::vector<std::string> &tokens) {
    if (decl->param_empty()) {
      if ((tokens.size() == 1) && (tokens[0] == "void")) {
        return false;
      } else if (tokens.size() == 0) {
        return true;
      } else {
        DBG_ASSERT(0, "Unknown fault.");
      }
    }
    return false;
  }

  void CheckFunctionNameReuse() {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    top_scope->TraverseAll<IdentifierManager::IdentifierKind::NON_FUNC>(
        [&top_scope](const std::string &x, IdentifierManager *id_mgr) -> void {
          if (top_scope->HasFunctionName(x)) {
            REPORT("GJB5396:4.1.1.1: Function name reused: %s\n", x.c_str());
          }
        });
  }

  void CheckVariableNameReuse() {
    using IdentifierKind = IdentifierManager::IdentifierKind;
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    constexpr uint32_t kind =
        IdentifierKind::VALUE | IdentifierKind::LABEL | IdentifierKind::FIELD;
    for (const auto &it : top_scope->Children()) {
      if (it->GetScopeKind() == SK_FUNCTION) {
        it->TraverseAll<kind>([&it](const std::string &x,
                                    IdentifierManager *id_mgr) -> void {
          if (it->HasVariableName<false>(x)) {
            REPORT("GJB5396:4.1.1.2: Variable name reused: %s\n", x.c_str());
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
        REPORT("GJB5396:4.1.1.3: Struct with empty field is forbidden: struct: "
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
    std::vector<std::string> tokens;
    GetFunctionTokens(decl, tokens);

    /* 4.1.1.10
     * The empty function parameter list is forbidden
     */
    if (IsEmptyParamList(decl, tokens)) {
      REPORT("GJB5396:4.1.1.10: The empty function parameter list is "
             "forbidden: %s\n",
             decl->getNameAsString().c_str());
    }

    /* 4.1.1.8
     * ellipsis in the function parameter list is forbidden
     */
    for (const auto &it : tokens) {
      if (it == "...") {
        REPORT("GJB5396:4.1.1.8: \"...\" in the funtion's parameter list is "
               "forbidden: %s\n",
               decl->getNameAsString().c_str());
      }
    }

    //    for (const auto &it : tokens) {
    //      REPORT("Token: %s\n", it.c_str());
    //    }
  }

  /* GJB5369: 4.1.1.7
   * Only type but no identifiers in function prototype.
   */
  void CheckParameterNoIdentifier(const clang::FunctionDecl *decl) {
    for (const auto &it : decl->parameters()) {
      if (it->getNameAsString() == "") {
        clang::SourceManager *src_mgr = XcalCheckerManager::GetSourceManager();
        REPORT("GJB5396:4.1.1.7: Only type but no identifiers in function %s, "
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
            REPORT("GJB5396:4.1.1.9: Redefining the keywords of C/C++ is "
                   "forbidden: %s\n",
                   x.c_str());
          }
        });
  }

  /*
   * GJB5369: 4.1.1.15
   * the sign of the char type should be explicit
   */
  void CheckExplicitCharType(const clang::FunctionDecl *decl) {
    std::string type_name;

    // Check parameters and return type
    for (const auto &it : decl->parameters()) {
      if (!it->getType()->isCharType()) { continue; }
      if (IsExplicitSign(GetTypeString(it->getType()))) {
        REPORT("GJB5396:4.1.1.15: The sign of the char type should be explicit: "
               "Function :%s -> Param: %s\n",
               decl->getNameAsString().c_str(),
               it->getNameAsString().c_str());
      }

      auto ret_type = decl->getReturnType();
      if ((ret_type->isVoidType()) || (!ret_type->isCharType())) return;

      if (IsExplicitSign(GetTypeString(ret_type))) {
        REPORT("GJB5396:4.1.1.15: The sign of the char type should be explicit: "
               "Function :%s -> Return: %s\n",
               decl->getNameAsString().c_str(),
               ret_type.getAsString().c_str());
      }
    }
  }

  void checkExplicitCharType(const clang::RecordDecl *decl) {
    for (const auto &it : clang::dyn_cast<clang::RecordDecl>(decl)->fields()) {
      if (!it->getType()->isCharType()) { continue; }
      if (IsExplicitSign(GetTypeString(it->getType()))) {
        REPORT("GJB5396:4.1.1.15: The sign of the char type should be explicit: "
               "Struct: %s -> Field: %s\n",
               decl->getNameAsString().c_str(),
               it->getNameAsString().c_str());
      }
    }

  }

  void CheckExplicitCharType(const clang::VarDecl *decl) {
    auto decl_type = decl->getType();

    // return if decl is not char type
    if (!decl_type->isCharType()) {
      return;
    }

    if (IsExplicitSign(GetTypeString(decl_type))) {
      REPORT("GJB5396:4.1.1.15: The sign of the char type should be explicit: %s\n",
             decl->getNameAsString().c_str());
    }

  }

  /*
   * GJB5369: 4.1.1.17
   * self-defined types(typedef) redefined as other types is forbidden
   */
  void CheckTypedefRedefine() {
    using IdentifierKind = IdentifierManager::IdentifierKind;
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();

    std::set<std::string> user_types;
    top_scope->TraverseAll<IdentifierKind::TYPEDEF>(
        [&user_types](const std::string &x, IdentifierManager *id_mgr) -> void {
          auto res = user_types.find(x);
          if (res == user_types.end()) {
            user_types.insert(x);
          } else {
            REPORT("GJB5396:4.1.1.17: Self-defined types(typedef) redefined as "
                   "other types is forbidden: %s\n",
                   x.c_str());
          }
        }
    );
  }

  /*
   * GJB5369: 4.1.1.19
   * arrays without boundary limitation is forbidden
   */
  void CheckArrayBoundary(const clang::VarDecl *decl) {
    auto decl_type = decl->getType().getAsString();

    // return if not array type
    if (decl_type.find('[') == std::string::npos) {
      return;
    }

    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto start_loc = decl->getBeginLoc();
    auto end_loc = decl->getEndLoc();
    auto start = src_mgr->getCharacterData(start_loc);
    auto end = src_mgr->getCharacterData(end_loc);

    while (start != end) {
      if (*start == '[') {
        start++;  // eat '['

        // eat space
        while (std::isspace(*start)) {
          start++;
        }

        if (*start == ']') {
          REPORT("GJB5396:4.1.1.19: Arrays without boundary limitation "
                 "is forbidden %s\n",
                 decl->getNameAsString().c_str());
        }
      }
      ++start;
    }
  }

  /*
   * GJB5369: 4.1.1.21
   * the incomplete declaration of struct is forbidden
   */
  void CheckIncompleteStruct(const clang::RecordDecl *decl) {
    if (decl->getDefinition() == nullptr) {
      REPORT("GJB5396:4.1.1.21: The incomplete declaration of struct "
             "is forbidden: %s\n",
             decl->getNameAsString().c_str());

    }
  }

  /*
   * GJB5369: 4.1.1.22
   * the forms of the parameter declarations in the parameter list
   * should keep in line
   */
  void CheckDifferentParamForms(const clang::FunctionDecl *decl) {
    if (decl->param_empty()) return;
    bool with_name = (decl->getParamDecl(0)->getNameAsString() == "") ? false : true;
    bool tmp;

    for (const auto &it : decl->parameters()) {
      tmp = (it->getNameAsString() == "") ? false : true;
      if (tmp ^ with_name) {
        REPORT("GJB5396:4.1.1.22: The forms of the parameter declarations"
               " in the parameter list is forbidden: %s\n",
               decl->getNameAsString().c_str());
      }
    }
  }

  /*
   * GJB5369: 4.1.2.1
   * Use typedef redefine the basic type
   */
  void CheckTypedefBasicType(const clang::FunctionDecl *decl) {
    // check parameters
    for (const auto &it : decl->parameters()) {
      auto param_type = it->getType();
      if (IsTypedefBasicType(param_type)) {
        REPORT("GJB5396:4.1.2.1: Use typedef redefine the basic type :"
               " Function: %s -> Param: %s\n",
               decl->getNameAsString().c_str(),
               it->getNameAsString().c_str());
      }
    }

    // check return type
    auto ret_type = decl->getReturnType();
    if (ret_type->isVoidType()) return;
    if (IsTypedefBasicType(ret_type)) {
      REPORT("GJB5396:4.1.2.1: Use typedef redefine the basic type :"
             " Function: %s -> return : %s\n",
             decl->getNameAsString().c_str(),
             ret_type.getAsString().c_str());
    }
  }

  void CheckTypedefBasicType(const clang::RecordDecl *decl) {
    for (const auto &it : decl->fields()) {
      auto field_type = it->getType();
      if (IsTypedefBasicType(field_type)) {
        REPORT("GJB5396:4.1.2.1: Use typedef redefine the basic type :"
               " Struct: %s -> Field: %s\n",
               decl->getNameAsString().c_str(),
               it->getNameAsString().c_str());
      }
    }
  }

  void CheckTypedefBasicType(const clang::VarDecl *decl) {
    auto decl_type = decl->getType();
    if (IsTypedefBasicType(decl_type)) {
      REPORT("GJB5396:4.1.2.1: Use typedef redefine the basic type :"
             " variable: %s\n",
             decl->getNameAsString().c_str());
    }
  }

  /*
   * GJB5369: 4.1.2.2
   * avoid using the function as parameter
   */
  void CheckFunctionAsParameter(const clang::FunctionDecl *decl) {
    for (const auto &it : decl->parameters()) {
      if (it->getType()->isFunctionPointerType()) {
        REPORT("GJB5396:4.1.2.2: Avoid using the function as parameter: %s\n",
               it->getNameAsString().c_str());
      }
    }
  }

  /*
   * GJB5369: 4.1.2.3
   * using too much parameters(more than 20) is forbidden
   */
  void CheckPlethoraParameters(const clang::FunctionDecl *decl) {
    if (decl->param_size() > 20) {
      REPORT("GJB5396:4.1.2.3: Using too much parameters(more than 20)"
             " is forbidden: %s\n",
             decl->getNameAsString().c_str());
    }
  }

  /*
   * GJB5369: 4.1.2.4
   * using bit-field in struct should be carefully
   * GJB5369: 4.1.2.9
   * using non-named bit fields carefully
   */
  void CheckBitfieldInStruct(const clang::RecordDecl *decl) {
    std::string field_name;
    for (const auto &it : decl->fields()) {
      field_name = it->getNameAsString();
      if (it->isBitField()) {
        REPORT("GJB5396:4.1.2.4: Using bit-field in struct "
               "should be carefully: struct %s -> field %s\n",
               decl->getNameAsString().c_str(),
               (field_name == "") ? "NAMELESS" : field_name.c_str());
        if (field_name == "") {
          REPORT("GJB5396:4.1.2.9: Using non-named bit fields carefully : %s\n",
                 decl->getNameAsString().c_str());
        }
      }
    }
  }

  /*
   * GJB5369: 4.1.2.8
   * using "union" carefully
   */
  void CheckUnionDecl(const clang::RecordDecl *decl) {
    if (!decl->isUnion()) return;
    REPORT("GJB5396:4.1.2.8: Using \"union\" carefully: %s\n",
           decl->getNameAsString().c_str());
  }

  /*
   * GJB5369: 4.2.1.1
   * procedure must be enclosed in braces
   */
  void CheckProcedureWithBraces(const clang::FunctionDecl *decl) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto func_loc = decl->getLocation();
    auto end_loc = decl->getEndLoc();
    const char *start = src_mgr->getCharacterData(func_loc);
    const char *end = src_mgr->getCharacterData(end_loc);

    while ((start != end) && (*start != ')')) start++;  // eat prototype
    do { start++; } while ((start != end) && std::isspace(*start));  // eat space

    if (*start != '{') {
      REPORT("GJB5396:4.1.2.8: Procedure must be enclosed in braces:"
             " %s -> %s\n",
             decl->getNameAsString().c_str(),
             func_loc.printToString(*src_mgr).c_str());
    }
  }

  /*
   * GJB5369: 4.2.1.10
   * main function should be defined as:
   * 1. int main(void)
   * 2. int main(int, char*[])
   */
  void CheckMainFunctionDefine(const clang::FunctionDecl *decl) {
    if (decl->getNameAsString() != "main") return;
    std::vector<std::string> tokens;
    GetFunctionTokens(decl, tokens);

    // check parameter
    bool need_report = false;
    if (decl->param_empty()) {
      need_report = (tokens.size() == 0) ? true : false;
    } else {
      if (tokens.size() != 2) {
        need_report = true;
      } else {
        if (tokens[0] == "int" && tokens[1] == "char*[]") {
          need_report = false;
        } else {
          need_report = true;
        }
      }
    }

    // check return type
    if (decl->getReturnType().getAsString() != "int") {
      need_report = true;
    }

    if (need_report) {
      REPORT("GJB5396:4.1.2.8: main function should be defined as: "
             "int main(void) or int main(int, char*[])\n");
    }
  }

  /*
   * GJB5369: 4.2.2.2
   * the function length shouldn't exceed 200 lines
   */
  void CheckFunctionLength(const clang::FunctionDecl *decl) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto start_loc = decl->getBeginLoc();
    auto end_loc = decl->getEndLoc();

    int start_line = src_mgr->getSpellingLineNumber(start_loc);
    int end_line = src_mgr->getSpellingLineNumber(end_loc);
    int len = end_line - start_line;
    if (len > 200) {
      REPORT("GJB5396:4.2.2.1: the function length shouldn't exceed 200 lines: "
             "function: %s -> length: %d\n",
             decl->getNameAsString().c_str(), len);
    }
  }

public:
  void Finalize() {
    CheckFunctionNameReuse();
    CheckVariableNameReuse();
    CheckKeywordRedefine();
    CheckTypedefRedefine();
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckParameterNoIdentifier(decl);
    CheckParameterTypeDecl(decl);
    CheckDifferentParamForms(decl);
    CheckFunctionAsParameter(decl);
    CheckPlethoraParameters(decl);
    CheckTypedefBasicType(decl);
    CheckExplicitCharType(decl);
    CheckMainFunctionDefine(decl);
    CheckProcedureWithBraces(decl);
    CheckFunctionLength(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckStructEmptyField(decl);
    CheckIncompleteStruct(decl);
    CheckBitfieldInStruct(decl);
    CheckUnionDecl(decl);
    CheckTypedefBasicType(decl);
    checkExplicitCharType(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckStructEmptyField(decl);
    CheckIncompleteStruct(decl);
    CheckBitfieldInStruct(decl);
    CheckUnionDecl(decl);
    CheckTypedefBasicType(decl);
    checkExplicitCharType(decl);
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckExplicitCharType(decl);
    CheckArrayBoundary(decl);
    CheckTypedefBasicType(decl);
  }

}; // GJB5369DeclRule
