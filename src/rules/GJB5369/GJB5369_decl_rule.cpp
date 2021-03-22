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

#include "GJB5369_decl_rule.h"

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

bool GJB5369DeclRule::IsTypedefBasicType(clang::QualType &decl_type) {
  if (decl_type->isBuiltinType()) {
    if (decl_type->getTypeClass() != clang::Type::Typedef) {
      return true;
    }
  }
  return false;
};

void GJB5369DeclRule::GetFunctionTokens(const clang::FunctionDecl *decl,
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

bool GJB5369DeclRule::IsEmptyParamList(const clang::FunctionDecl *decl,
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

void GJB5369DeclRule::CheckFunctionNameReuse() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  top_scope->TraverseAll<IdentifierManager::IdentifierKind::NON_FUNC>(
      [&top_scope](const std::string &x, IdentifierManager *id_mgr) -> void {
        if (top_scope->HasFunctionName(x)) {
          REPORT("GJB5396:4.1.1.1: Function name reused: %s\n", x.c_str());
        }
      });
}

void GJB5369DeclRule::CheckVariableNameReuse() {
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

}
}