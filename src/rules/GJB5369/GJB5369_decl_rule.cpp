/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for GJB5369
//

#include "GJB5369_enum.inc"
#include "GJB5369_decl_rule.h"
#include <clang/AST/ASTContext.h>

namespace xsca {
namespace rule {

std::string GJB5369DeclRule::GetTypeString(clang::QualType type) {
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

bool GJB5369DeclRule::IsExplicitSign(std::string type_name) {
  if (type_name.find("unsigned") != std::string::npos) {
    return false;
  } else {
    if (type_name.find("signed") != std::string::npos) {
      return false;
    }
  }
  return true;
};

/* Check if this typedef declared a builtin type
 * Used by 4.1.2.1 -> CheckTypedefBasicType
 */
bool GJB5369DeclRule::IsTypedefBasicType(clang::QualType &decl_type) {
  if (decl_type->isBuiltinType()) {
    if (decl_type->getTypeClass() != clang::Type::Typedef) {
      return true;
    }
  }
  return false;
};

/* Get function prototype tokens
 * Used by 4.2.1.10 ->  CheckMainFunctionDefine
 */
void GJB5369DeclRule::GetFunctionTokens(const clang::FunctionDecl *decl,
                                        std::vector<std::string> &tokens) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto func_loc = decl->getLocation();
  auto func_raw_chars = src_mgr->getCharacterData(func_loc);

  std::string token;

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

    if (!token.empty()) {
      tokens.push_back(token);
      token = "";
    }

  } while (*func_raw_chars != ')');
}

/* Check if the parameter list is empty
 * Used by 4.1.1.10 -> CheckParameterTypeDecl
 */
bool GJB5369DeclRule::IsEmptyParamList(const clang::FunctionDecl *decl,
                                       std::vector<std::string> &tokens) {
  if (decl->param_empty()) {
    if ((tokens.size() == 1) && (tokens[0] == "void")) {
      return false;
    } else if (tokens.empty()) {
      return true;
    } else {
      //DBG_ASSERT(0, "Unknown fault.");
    }
  }
  return false;
}

/*
 * Check the pointer nested levels
 * Used by 4.4.1.2 -> CheckPointerNestedLevel
 */
bool GJB5369DeclRule::IsPointerNestedMoreThanTwoLevel(clang::QualType decl_type) {
  if (decl_type->isPointerType()) {
    int nested_level = 0;
    auto pointee_type = decl_type->getPointeeType();
    if (pointee_type->isPointerType()) {
      auto nested_type = pointee_type->getPointeeType();
      if (nested_type->isPointerType()) {
        return true;
      }
    }
  }
  return false;
}

/* Check if the parameter declaration without type
 * Used in 4.1.1.5 -> CheckParameterTypeDecl
 */
bool GJB5369DeclRule::DoesParamHasNotTypeDecl(const clang::FunctionDecl *decl) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  for (const auto &it : decl->parameters()) {
    auto param_decl = clang::dyn_cast<clang::ParmVarDecl>(it);
//    param_decl->has
    if (param_decl->getType()->isIntegerType()) {
      auto start_loc = param_decl->getLocation();
      auto param_data = src_mgr->getCharacterData(start_loc);
      --param_data;
      if (*param_data == ',' || *param_data == '(') {
        return true;
      }
    }
  }
  return false;
}

/*
 * GJB5369: 4.1.1.1
 * procedure name reused as other purpose is forbidden
 */
void GJB5369DeclRule::CheckFunctionNameReuse() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();


  top_scope->TraverseAll<IdentifierManager::IdentifierKind::NON_FUNC,
      const std::function<void(const std::string &, IdentifierManager *)>>(
      [&top_scope, &issue, &report](
          const std::string &func_name, IdentifierManager *id_mgr) -> void {
        if (top_scope->HasFunctionName(func_name)) {

          /* Find all functions which named func_name */
          auto function_decl_range = top_scope->GetFunctionDecls(func_name);
          auto begin = function_decl_range.first;
          auto end = function_decl_range.second;
          for (; begin != end; begin++) {
            // not concern constructor
            if (begin->second->getDeclKind() == clang::Decl::Kind::CXXConstructor) return;

            if (issue == nullptr) {
              issue = report->ReportIssue(GJB5369, G4_1_1_1, begin->second);
              std::string ref_msg = "Procedure name reused as other purpose is forbidden: ";
              ref_msg += begin->second->getNameAsString();
              issue->SetRefMsg(ref_msg);
            }
            issue->AddDecl(begin->second);
          }
        }
      });
}

/*
 * GJB5369: 4.1.1.2
 * identifier name reused as other purpose is forbidden
 */
void GJB5369DeclRule::CheckVariableNameReuse() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind =
      IdentifierKind::VALUE | IdentifierKind::LABEL | IdentifierKind::FIELD;


  for (const auto &it : top_scope->Children()) {
    if (it->GetScopeKind() == SK_FUNCTION) {
      it->TraverseAll<kind,
          const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
          [&it, &issue, &report](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
            if (it->HasVariableName<false>(x)) {
              if (issue == nullptr) {
                issue = report->ReportIssue(GJB5369, G4_1_1_2, decl);
                std::string ref_msg = "Variable name reused: ";
                if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
                  ref_msg += var_decl->getNameAsString();
                } else if (auto label_decl = clang::dyn_cast<clang::LabelDecl>(decl)) {
                  ref_msg += label_decl->getNameAsString();
                } else if (auto field_decl = clang::dyn_cast<clang::FieldDecl>(decl)) {
                  ref_msg += field_decl->getNameAsString();
                }
                issue->SetRefMsg(ref_msg);
              }
              issue->AddDecl(decl);
            }
          }, true);
    }
  }
}

/* GJB5396
 * 4.1.1.3 struct with empty field is forbidden
 */
void GJB5369DeclRule::CheckStructEmptyField(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->fields()) {
    if (it->isAnonymousStructOrUnion()) {
      if (issue == nullptr) {
        // create issue for decl
        issue = report->ReportIssue(GJB5369, G4_1_1_3, decl);
        std::string ref_msg = "Struct with anonymous field is forbidden: struct: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      // append field for path info
      issue->AddDecl(&(*it));
    }
  }
}

/* GJB5396
 * 4.1.1.5 declaring the type of parameters is a must
 * 4.1.1.6 without the parameter declarations in function declaration is forbidden
 * 4.1.1.8 ellipsis in the function parameter list is forbidden
 * 4.1.1.10 the empty function parameter list is forbidden
 */
void GJB5369DeclRule::CheckParameterTypeDecl(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::vector<std::string> tokens;
  GetFunctionTokens(decl, tokens);

  if (!decl->doesThisDeclarationHaveABody()) {
    if (tokens.empty()) {
      issue = report->ReportIssue(GJB5369, G4_1_1_6, decl);
      std::string ref_msg = "Without the parameter declarations in function declaration is forbidden: ";
      ref_msg += decl->getNameAsString();
      issue->SetRefMsg(ref_msg);
      return;
    }
  }

  /* 4.1.1.10
   * The empty function parameter list is forbidden
   */
  if (IsEmptyParamList(decl, tokens)) {
    issue = report->ReportIssue(GJB5369, G4_1_1_10, decl);
    std::string ref_msg = " The empty function parameter list is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
    return;
  }

  /* 4.1.1.5
   * declaring the type of parameters is a must
   */
  if (DoesParamHasNotTypeDecl(decl)) {
    issue = report->ReportIssue(GJB5369, G4_1_1_5, decl);
    std::string ref_msg = "Declaring the type of parameters is a must: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }

  /* 4.1.1.8
   * ellipsis in the function parameter list is forbidden
   */
  for (const auto &it : tokens) {
    if (it == "...") {
      issue = report->ReportIssue(GJB5369, G4_1_1_8, decl);
      std::string ref_msg = "\"...\" in the funtion's parameter list is forbidden: ";
      ref_msg += decl->getNameAsString();
      issue->SetRefMsg(ref_msg);
    }
  }

  //    for (const auto &it : tokens) {
  //      REPORT("Token: %s\n", it.c_str());
  //    }
}

/* GJB5369: 4.1.1.7
 * Only type but no identifiers in function prototype.
 */
void GJB5369DeclRule::CheckParameterNoIdentifier(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->parameters()) {
    if (it->getNameAsString().empty()) {
      issue = report->ReportIssue(GJB5369, G4_1_1_7, decl);
      std::string ref_msg = "Only type but no identifiers in function: ";
      ref_msg += decl->getNameAsString();
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* GJB5369: 4.1.1.9
 * redefining the keywords of C/C++ is forbidden
 */
void GJB5369DeclRule::CheckKeywordRedefine() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  top_scope->TraverseAll<IdentifierKind::VAR,
      std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        if (!var_name.empty() && id_mgr->IsKeyword(var_name)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB5369, G4_1_1_9, decl);
            std::string ref_msg = "Redefining the keywords of C/C++ is forbidden: ";
            ref_msg += clang::dyn_cast<clang::VarDecl>(decl)->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
          issue->AddDecl(decl);
        }
      }, true);
}

/*
 * GJB5369: 4.1.1.15
 * the sign of the char type should be explicit
 */
void GJB5369DeclRule::CheckExplicitCharType(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::string type_name;

  // Check parameters and return type
  for (const auto &it : decl->parameters()) {
    if (!it->getType()->isCharType()) { continue; }
    if (IsExplicitSign(GetTypeString(it->getType()))) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_1_15, decl);
        std::string ref_msg = "The sign of the char type should be explicit: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }

    auto ret_type = decl->getReturnType();
    if ((ret_type->isVoidType()) || (!ret_type->isCharType())) return;

    if (IsExplicitSign(GetTypeString(ret_type))) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_1_15, decl);
        std::string ref_msg = "The sign of the char type should be explicit: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

void GJB5369DeclRule::checkExplicitCharType(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : clang::dyn_cast<clang::RecordDecl>(decl)->fields()) {
    if (!it->getType()->isCharType()) { continue; }
    if (IsExplicitSign(GetTypeString(it->getType()))) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_1_15, decl);
        std::string ref_msg = "The sign of the char type should be explicit: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

void GJB5369DeclRule::CheckExplicitCharType(const clang::VarDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto decl_type = decl->getType();

  // return if decl is not char type
  if (!decl_type->isCharType()) {
    return;
  }

  if (IsExplicitSign(GetTypeString(decl_type))) {
    issue = report->ReportIssue(GJB5369, G4_1_1_15, decl);
    std::string ref_msg = "The sign of the char type should be explicit: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }

}

/*
 * GJB5369: 4.1.1.17
 * self-defined types(typedef) redefined as other types is forbidden
 * Algorithm: Check if name of typedef has been defined when the TypedefDecl is hit
 */
void GJB5369DeclRule::CheckTypedefRedefine() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  std::set<std::string> user_types;
  top_scope->TraverseAll<IdentifierKind::TYPEDEF,
      std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&user_types, &issue, &report](const std::string &typedef_name, const clang::Decl *decl,
                                     IdentifierManager *id_mgr) -> void {
        auto res = user_types.find(typedef_name);
        if (res == user_types.end()) {
          user_types.insert(typedef_name);
        } else {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB5369, G4_1_1_17, decl);
            std::string ref_msg = "Self-defined types(typedef) redefined as other types is forbidden: ";
            ref_msg += clang::dyn_cast<clang::TypedefDecl>(decl)->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
          issue->AddDecl(decl);
        }
      }, true);
}

/*
 * GJB5369: 4.1.1.19
 * arrays without boundary limitation is forbidden
 */
void GJB5369DeclRule::CheckArrayBoundary(const clang::VarDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto decl_type = decl->getType().getAsString();

  if (!decl->getType()->isArrayType()) return;
  auto array_type = clang::dyn_cast<clang::ConstantArrayType>(decl->getType());
  if (!array_type) {
    issue = report->ReportIssue(GJB5369, G4_1_1_19, decl);
    std::string ref_msg = "Arrays without boundary limitation is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.1.1.21
 * the incomplete declaration of struct is forbidden
 */
void GJB5369DeclRule::CheckIncompleteStruct(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (decl->getDefinition() == nullptr) {
    issue = report->ReportIssue(GJB5369, G4_1_1_21, decl);
    std::string ref_msg = "The incomplete declaration of struct is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.1.1.22
 * the forms of the parameter declarations in the parameter list
 * should keep in line
 */
void GJB5369DeclRule::CheckDifferentParamForms(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (decl->param_empty()) return;
  bool with_name = !(decl->getParamDecl(0)->getNameAsString().empty());
  bool tmp;

  for (const auto &it : decl->parameters()) {
    tmp = !(it->getNameAsString().empty());
    if (tmp ^ with_name) {
      issue = report->ReportIssue(GJB5369, G4_1_1_22, decl);
      std::string ref_msg = "The forms of the parameter declarations"
                            " in the parameter list should keep in line: ";
      ref_msg += decl->getNameAsString();
      issue->SetRefMsg(ref_msg);
      break;
    }
  }
}

/*
 * GJB5369: 4.1.2.1
 * Use typedef redefine the basic type
 */
void GJB5369DeclRule::CheckTypedefBasicType(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  // check parameters
  for (const auto &it : decl->parameters()) {

    auto param_type = it->getType();
    if (IsTypedefBasicType(param_type)) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_2_1, decl);
        std::string ref_msg = "Use typedef redefine the basic type Function: ";
        ref_msg += decl->getNameAsString();
        ref_msg += " -> Param: " + it->getNameAsString();
        issue->SetRefMsg(ref_msg);
      } else {
        issue->AddDecl(&(*it));
      }
    }
  }

  // check return type
  auto ret_type = decl->getReturnType();
  if (ret_type->isVoidType()) return;
  if (IsTypedefBasicType(ret_type)) {
    if (issue == nullptr) {
      issue = report->ReportIssue(GJB5369, G4_1_2_1, decl);
      std::string ref_msg = "Use typedef redefine the basic type Function: ";
      ref_msg += decl->getNameAsString();
      ref_msg += " -> return: " + ret_type.getAsString();
      issue->SetRefMsg(ref_msg);
    } else {
      issue->AddDecl(decl);
    }

  }
}

void GJB5369DeclRule::CheckTypedefBasicType(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->fields()) {
    auto field_type = it->getType();
    if (IsTypedefBasicType(field_type)) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_2_1, decl);
        std::string ref_msg = "Use typedef redefine the basic type Struct: ";
        ref_msg += decl->getNameAsString();
        ref_msg += " -> Field: " + it->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

void GJB5369DeclRule::CheckTypedefBasicType(const clang::VarDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto decl_type = decl->getType();
  if (IsTypedefBasicType(decl_type)) {
    issue = report->ReportIssue(GJB5369, G4_1_2_1, decl);
    std::string ref_msg = "Use typedef redefine the basic type variable: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(decl);
  }
}

/*
 * GJB5369: 4.1.2.2
 * avoid using the function as parameter
 */
void GJB5369DeclRule::CheckFunctionAsParameter(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->parameters()) {
    if (it->getType()->isFunctionPointerType()) {

      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_1_2_2, decl);
        std::string ref_msg = "Avoid using the function as parameter: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      } else {
        issue->AddDecl(&(*it));
      }
    }
  }
}

/*
 * GJB5369: 4.1.2.3
 * using too much parameters(more than 20) is forbidden
 */
void GJB5369DeclRule::CheckPlethoraParameters(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (decl->param_size() > 20) {
    issue = report->ReportIssue(GJB5369, G4_1_2_3, decl);
    std::string ref_msg = "Using too much parameters(more than 20) is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.1.2.4
 * using bit-field in struct should be carefully
 * GJB5369: 4.1.2.9
 * using non-named bit fields carefully
 */
void GJB5369DeclRule::CheckBitfieldInStruct(const clang::RecordDecl *decl) {
  XcalIssue *issue1 = nullptr, *issue2 = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::string field_name;
  for (const auto &it : decl->fields()) {
    field_name = it->getNameAsString();
    if (it->isBitField()) {
      if (issue1 == nullptr) {
        issue1 = report->ReportIssue(GJB5369, G4_1_2_4, decl);
        std::string ref_msg = "single bit-field in struct should be carefully: "
                              "struct: ";
        ref_msg += decl->getNameAsString();
        ref_msg += " -> field: " + it->getNameAsString();
        issue1->SetRefMsg(ref_msg);
        issue1->AddDecl(&(*it));

      } else {
        issue1->AddDecl(&(*it));
      }

      if (field_name.empty()) {
        if (issue2 == nullptr) {
          issue2 = report->ReportIssue(GJB5369, G4_1_2_9, decl);
          std::string ref_msg = "Using non-named bit fields carefully: ";
          ref_msg += decl->getNameAsString();
          issue2->SetRefMsg(ref_msg);
        }
      }
    }
  }
}

/*
 * GJB5369: 4.1.2.8
 * using "union" carefully
 */
void GJB5369DeclRule::CheckUnionDecl(const clang::RecordDecl *decl) {
  if (!decl->isUnion()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(GJB5369, G4_1_2_8, decl);
  std::string ref_msg = "Using \"union\" carefully: ";
  ref_msg += decl->getNameAsString();
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB5369: 4.2.1.1
 * procedure must be enclosed in braces
 */
void GJB5369DeclRule::CheckProcedureWithBraces(const clang::FunctionDecl *decl) {
  if (decl->getDeclKind() == clang::Decl::Kind::CXXConstructor) return;

  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto func_loc = decl->getLocation();
  auto end_loc = decl->getEndLoc();
  const char *start = src_mgr->getCharacterData(func_loc);
  const char *end = src_mgr->getCharacterData(end_loc);

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  while ((start != end) && (*start != ')')) start++;  // eat prototype
  do { start++; } while ((start != end) && std::isspace(*start));  // eat space

  if (*start != '{' && *start != ';') {
    issue = report->ReportIssue(GJB5369, G4_2_1_1, decl);
    std::string ref_msg = "Procedure must be enclosed in braces: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.2.1.10
 * main function should be defined as:
 * 1. int main(void)
 * 2. int main(int, char*[])
 */
void GJB5369DeclRule::CheckMainFunctionDefine(const clang::FunctionDecl *decl) {
  if (decl->getNameAsString() != "main") return;
  std::vector<std::string> tokens;
  GetFunctionTokens(decl, tokens);

  // check parameter
  bool need_report = false;
  if (decl->param_empty()) {
    need_report = (tokens.empty());
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
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_2_1_10, decl);
    std::string ref_msg = "main function should be defined as: int main(void) or int main(int, char*[]): ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.2.2.2
 * the function length shouldn't exceed 200 lines
 */
void GJB5369DeclRule::CheckFunctionLength(const clang::FunctionDecl *decl) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto start_loc = decl->getBeginLoc();
  auto end_loc = decl->getEndLoc();

  int start_line = src_mgr->getSpellingLineNumber(start_loc);
  int end_line = src_mgr->getSpellingLineNumber(end_loc);
  int len = end_line - start_line;
  if (len > 200) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_2_2_2, decl);
    std::string ref_msg = "the function length shouldn't exceed 200 lines: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.4.1.2
 * pointer's pointer nested more than two levels is forbidden
 */
void GJB5369DeclRule::CheckPointerNestedLevel(const clang::VarDecl *decl) {
  auto decl_type = decl->getType();
  if (IsPointerNestedMoreThanTwoLevel(decl_type)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue("MISRA", G4_4_1_2, decl);
    std::string ref_msg = "Pointer's pointer nested more than two levels is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

void GJB5369DeclRule::CheckPointerNestedLevel(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->parameters()) {
    auto param_type = it->getType();
    if (IsPointerNestedMoreThanTwoLevel(param_type)) {
      if (issue == nullptr) {
        issue = report->ReportIssue("MISRA", G4_4_1_2, decl);
        std::string ref_msg = "Pointer's pointer nested more than two levels is forbidden: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }

  if (IsPointerNestedMoreThanTwoLevel(decl->getReturnType())) {
    if (issue == nullptr) {
      issue = report->ReportIssue("MISRA", G4_4_1_2, decl);
      std::string ref_msg = "Pointer's pointer(function return type) nested more than two levels is forbidden: ";
      ref_msg += decl->getNameAsString();
      issue->SetRefMsg(ref_msg);
    } else {
      issue->AddDecl(decl);
    }
  }
}

void GJB5369DeclRule::CheckPointerNestedLevel(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->fields()) {
    auto field_type = it->getType();
    if (IsPointerNestedMoreThanTwoLevel(field_type)) {
      if (issue == nullptr) {
        issue = report->ReportIssue("MISRA", G4_4_1_2, decl);
        std::string ref_msg = "Pointer's pointer nested more than two levels is forbidden: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

/*
 * GJB5369: 4.4.1.3
 * function pointer is forbidden
 */
void GJB5369DeclRule::CheckFunctionPointer(const clang::VarDecl *decl) {
  auto decl_type = decl->getType();
  if (decl_type->isFunctionPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_4_1_3, decl);
    std::string ref_msg = "Function pointer is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.1.6
 * signed-value must be longer than two bits
 */
void GJB5369DeclRule::CheckSingleBitSignedValue(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->fields()) {
    if (it->getType()->isSignedIntegerType() && it->isBitField()) {
      auto bit_width = it->getBitWidthValue(decl->getASTContext());
      if (bit_width < 2) {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB5369, G4_6_1_6, decl);
          std::string ref_msg = "Signed-value must be longer than two bits: ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(&(*it));
      }
    }
  }
}

/*
 * GJB5369: 4.6.1.7
 * bits can only be defined as signed/unsigned int type
 */
void GJB5369DeclRule::CheckBitsIfInteger(const clang::RecordDecl *decl) {
  using BuiltinType = clang::BuiltinType;
  for (const auto &it : decl->fields()) {
    if (it->isBitField()) {
      const auto *BT = clang::dyn_cast<clang::BuiltinType>(it->getType().getCanonicalType());
      if (BT == nullptr) return;
      auto builtin_type = BT->getKind();
      if (!((builtin_type >= BuiltinType::Short && builtin_type <= BuiltinType::Int128) ||
            (builtin_type >= BuiltinType::UShort && builtin_type <= BuiltinType::UInt128))) {

        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(GJB5369, G4_6_1_7, decl);
        std::string ref_msg = "Bits can only be defined as signed/unsigned int type: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
    }
  }
}

/*
 * GJB5369: 4.7.1.8
 * void type variable used as parameter is forbidden
 */
void GJB5369DeclRule::CheckVoidTypeParameters(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->parameters()) {
    if (it->getType()->isVoidPointerType()) {

      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_7_1_8, decl);
        std::string ref_msg = "Function return void used in statement is forbidden: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

/*
 * GJB5369: 4.7.2.1
 * parameters should be used in the function
 */
void GJB5369DeclRule::CheckUnusedParameters(const clang::FunctionDecl *decl) {
  if (!decl->hasBody()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->parameters()) {
    if (!it->isUsed()) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G4_7_2_1, decl);
        std::string ref_msg = "Parameters should be used in the function: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

/*
 * GJB5369: 4.8.1.1
 * avoid using "O" or "I" as variable names
 */
void GJB5369DeclRule::CheckIandOUsedAsVariable(const clang::VarDecl *decl) {
  auto var_name = decl->getNameAsString();
  if (var_name == "I" || var_name == "O") {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_8_1_1, decl);
    std::string ref_msg = R"(Avoid using "O" or "I" as variable names: )";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

void GJB5369DeclRule::CheckIandOUsedAsVariable(const clang::ParmVarDecl *decl) {
  auto var_name = decl->getNameAsString();
  if (var_name == "I" || var_name == "O") {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_8_1_1, decl);
    std::string ref_msg = R"(Avoid using "O" or "I" as variable names: )";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.8.2.7
 * using register variable carefully
 */
void GJB5369DeclRule::CheckRegisterVariable(const clang::VarDecl *decl) {
  if (decl->getStorageClass() == clang::SC_Register) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_8_2_7, decl);
    std::string ref_msg = "Using register variable carefully: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.13.1.3
 * nested structure should stay the same with the struct
 */
void GJB5369DeclRule::CheckNestedStruct(const clang::VarDecl *decl) {
  auto decl_type = decl->getType();
  if (!decl_type->isRecordType() || !decl->hasInit()) return;

  auto init_expr = decl->getInit();
  auto initListExpr = clang::dyn_cast<clang::InitListExpr>(init_expr);
  if (initListExpr == nullptr) return;
  auto record_decl = decl_type.getCanonicalType()->getAsRecordDecl();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto init_start = initListExpr->begin();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  for (const auto &it : record_decl->fields()) {
    if (!it->getType()->isRecordType()) {
      init_start++;
      continue;
    } else {
      auto init_loc = (*init_start)->getBeginLoc();
      auto data = src_mgr->getCharacterData(init_loc);

      if (*data != '{') {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB5369, G4_13_1_3, decl);
          std::string ref_msg = "Nested structure should stay the same with the struct: ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        issue->AddStmt(*init_start);
      }
      init_start++;
    }
  }
}

/*
 * GJB5369: 4.13.1.4
 * value used before init is forbidden
 * TODO: Need refine -> with false positive:
 * int x;
 * int y = 2;
 * int z;
 * x = 3;
 * z = x;
 */
void GJB5369DeclRule::CheckUsedBeforeInit(const clang::VarDecl *decl) {
  if (decl->hasInit()) return;
  if (decl->isUsed()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G4_13_1_4, decl);
    std::string ref_msg = "Value used before init is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.15.1.1
 * enum name should not be collide with global variable
 */
void GJB5369DeclRule::CheckEnumNameDuplicate() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  using IdentifierKind = IdentifierManager::IdentifierKind;

  top_scope->TraverseAll<IdentifierKind::VALUE,
      std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        if (auto enum_decl = clang::dyn_cast<clang::EnumConstantDecl>(decl)) {
          if (top_scope->HasVariableName<false>(var_name)) {
            if (issue == nullptr) {
              issue = report->ReportIssue(GJB5369, G4_15_1_1, decl);
              std::string ref_msg = "Enum name should not be collide with global variable: ";
              ref_msg += enum_decl->getNameAsString();
              issue->SetRefMsg(ref_msg);
            } else {
              issue->AddDecl(decl);
            }
          }
        }
      }, true);
}

/*
 * GJB5369: 4.15.1.2
 * local variable name should be different from the global variable
 * GJB5369: 4.15.1.3
 * formal parameter's name should be different from global variable
 */
void GJB5369DeclRule::CheckLocalVarCollideWithGlobal() {
  XcalIssue *var_issue = nullptr, *param_issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  constexpr uint32_t kind = IdentifierKind::VAR;

  for (const auto &it : top_scope->Children()) {
    if (it->GetScopeKind() != SK_FUNCTION) continue;
    it->TraverseAll<kind,
        const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
        [&](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) {
          if (top_scope->HasVariableName<false>(var_name)) {

            if (auto param_decl = clang::dyn_cast<clang::ParmVarDecl>(decl)) {
              if (param_issue == nullptr) {
                param_issue = report->ReportIssue(GJB5369, G4_15_1_3, decl);

                std::string ref_msg = "Formal parameter's name should be different from global variable: ";
                ref_msg += param_decl->getNameAsString();
                param_issue->SetRefMsg(ref_msg);
              } else {
                param_issue->AddDecl(param_decl);
              }
            } else {
              if (var_issue == nullptr) {
                var_issue = report->ReportIssue(GJB5369, G4_15_1_2, decl);
                std::string ref_msg = "Local variable name should be different from the global variable: ";
                ref_msg += clang::dyn_cast<clang::VarDecl>(decl)->getNameAsString();
                var_issue->SetRefMsg(ref_msg);
              } else {
                var_issue->AddDecl(decl);
              }
            }
          }

        },
        true);
  }
}

/*
 * GJB5369: 4.15.1.4
 * formal parameter's name should be different with typename or identifier
 */
void GJB5369DeclRule::CheckParamNameCollideWithTypeName() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  constexpr uint32_t kind = IdentifierKind::FUNCTION;

  std::vector<const clang::TypedefDecl *> typedefs;
  top_scope->GetTypeDefs<true>("", typedefs);
  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&](const std::string &func_name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto func_decl = clang::dyn_cast<clang::FunctionDecl>(decl);
        std::string param_name;
        for (const auto &param : func_decl->parameters()) {
          param_name = param->getNameAsString();
          for (const auto &type : typedefs) {
            if (param_name == type->getNameAsString()) {
              if (issue == nullptr) {
                issue = report->ReportIssue(GJB5369, G4_15_1_4, decl);
                std::string ref_msg = "Formal parameter's name should be different with typename or identifier: ";
                ref_msg += param_name;
                issue->SetRefMsg(ref_msg);
              }
              issue->AddDecl(&(*param));
            }
          }
        }
      }, true);

}

/*
 * GJB5369: 4.15.1.5
 * redefining the exist variable is forbidden
 */
void GJB5369DeclRule::CheckVariableRedefine() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  constexpr uint32_t kind = IdentifierKind::VAR;

  for (const auto &it : top_scope->Children()) {
    if (it->GetScopeKind() != SK_FUNCTION) continue;
    it->TraverseAll<kind,
        const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
        [&issue, &report, &it](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) {
          auto cur_scope = id_mgr->scope();
          if (cur_scope->Children().empty()) return;
          for (const auto &block : cur_scope->Children()) {
            if (block->HasVariableName<true>(var_name)) {
              if (issue == nullptr) {
                issue = report->ReportIssue(GJB5369, G4_15_1_5, decl);
                std::string ref_msg = "Redefining the exist variable is forbidden: ";
                ref_msg += var_name;
                issue->SetRefMsg(ref_msg);
              } else {
                issue->AddDecl(decl);
              }

            }
          }
        }, true);
  }
}

/*
 * GJB5369: 4.15.1.6
 * using volatile type in complex in statement is forbidden
 */
void GJB5369DeclRule::CheckVolatileTypeVar(const clang::VarDecl *decl) {
  if (decl->getType().isVolatileQualified()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G4_15_1_6, decl);
    std::string ref_msg = "Using volatile type in complex in statement is forbidden: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.15.2.1
 * using non-ANSI char carefully
 */

void GJB5369DeclRule::CheckNonANSIChar(const clang::VarDecl *decl) {
  if (!decl->getType()->isCharType() || !decl->hasInit()) return;
  bool need_report = false;
  auto init_stmt = decl->getInit()->IgnoreParenImpCasts();

  if (auto int_literal = clang::dyn_cast<clang::IntegerLiteral>(init_stmt)) {
    int value;
    clang::Expr::EvalResult eval_result;
    auto ctx = XcalCheckerManager::GetAstContext();

    // try to fold the const expr
    if (init_stmt->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = int_literal->getValue().getZExtValue();
    }
    if (value > 127) {
      need_report = true;
    }
  } else if (auto char_literal = clang::dyn_cast<clang::CharacterLiteral>(init_stmt)) {
    unsigned char value = char_literal->getValue();
    if (value > 127) {
      need_report = true;
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G4_15_2_1, decl);
    std::string ref_msg = "Using non-ANSI char carefully: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}


} // rule
} // xsca
