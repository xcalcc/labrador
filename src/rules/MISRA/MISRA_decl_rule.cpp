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
// MISRA_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for MISRA
//

#include <unordered_set>
#include <clang/AST/Decl.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/FileManager.h>

#include "xsca_report.h"
#include "MISRA_decl_rule.h"

namespace xsca {
namespace rule {


std::string MISRADeclRule::GetTypeString(clang::QualType type) {
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

bool MISRADeclRule::IsExplicitSign(const std::string &type_name) {
  if (type_name.find("unsigned ") != std::string::npos) {
    return true;
  } else {
    if (type_name.find("signed ") != std::string::npos) {
      return true;
    }
  }
  return false;
};

const clang::CXXRecordDecl *MISRADeclRule::GetBaseDecl(const clang::CXXBaseSpecifier &BS) {
  auto record_type = clang::dyn_cast<clang::RecordType>(BS.getType());
  if (!record_type) return nullptr;
  auto record_decl = record_type->getAsCXXRecordDecl();
  if (!record_decl) return nullptr;

  if (!record_decl->hasDefinition()) return nullptr;

  return record_decl;
}

/*
 * check if a function has a single parameter whose type is template type
 */
bool MISRADeclRule::IsSingleTemplateTypeParamFunction(const clang::Decl *decl) {
  if (!decl->isTemplateDecl()) return false;
  if (auto func_tp = clang::dyn_cast<clang::FunctionTemplateDecl>(decl)) {
    auto func = func_tp->getTemplatedDecl();
    if (func->param_size() != 1) return false;
    auto param = func->getParamDecl(0);
    auto param_tp = param->getType();

    // check if it is reference type
    if (param_tp->isLValueReferenceType()) {
      auto lref = clang::dyn_cast<clang::LValueReferenceType>(param_tp);
      param_tp = lref->getPointeeType();
    }

    // check if it is template type
    if (param_tp->isTemplateTypeParmType()) return true;
  }
  return false;
}

/*
 * check if pointer nested more than tow levels
 */
bool MISRADeclRule::IsPointerNestedMoreThanTwoLevel(clang::QualType decl_type) {
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

// check if the expr is an IntegerLiteral expression
bool MISRADeclRule::IsIntegerLiteralExpr(const clang::Expr *expr, uint64_t *res) {
  clang::Expr::EvalResult eval_result;
  auto ctx = XcalCheckerManager::GetAstContext();
  bool state = expr->EvaluateAsInt(eval_result, *ctx);
  if (state && res) *res = eval_result.Val.getInt().getZExtValue();
  return state;
}

/* MISRA
 * Rule: 2.3
 * A project should not contain unused type declarations
 */
void MISRADeclRule::CheckUnusedTypedef(clang::QualType type) {
  if (auto typedefType = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedecl = typedefType->getDecl();
    _used_typedef.insert(typedecl);
  } else if (auto pt_tp = clang::dyn_cast<clang::PointerType>(type)) {
    CheckUnusedTypedef(pt_tp->getPointeeType());
  } else if (auto decayed_tp = clang::dyn_cast<clang::DecayedType>(type)) {
    CheckUnusedTypedef(decayed_tp->getOriginalType());
  } else {
    /* ... */
  }
}

void MISRADeclRule::CheckUnusedTypedef(const clang::FunctionDecl *decl) {
  // check parameters
  for (const auto &it : decl->parameters())
    CheckUnusedTypedef(it);


  // check return value
  auto type = decl->getReturnType();
  if (auto typedefType = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedecl = typedefType->getDecl();
    _used_typedef.insert(typedecl);
  }
}

void MISRADeclRule::CheckUnusedTypedef(const clang::TypedefDecl *decl) {
  auto type = decl->getTypeSourceInfo()->getType();
  if (auto typedef_type = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedef_decl = typedef_type->getDecl();
    _used_typedef.insert(typedef_decl);
  }
}

void MISRADeclRule::CheckUnusedTypedef() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::TYPEDEF;
  auto used_typedefs = &(this->_used_typedef);

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&used_typedefs, &issue, &report](const std::string &x, const clang::Decl *decl,
                                        IdentifierManager *id_mgr) -> void {
        auto src_mgr = XcalCheckerManager::GetSourceManager();
        auto typedef_decl = clang::dyn_cast<const clang::TypedefDecl>(decl);

        if (used_typedefs->find(reinterpret_cast<const clang::TypedefDecl *const>(typedef_decl)) ==
            used_typedefs->end()) {

          // check if this typedef is in a header file
          auto location = typedef_decl->getLocation();
          auto filename = src_mgr->getFilename(location);
          if (filename.endswith(".h") || decl->isUsed()) return;

          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_2_3, decl);
            std::string ref_msg = "A project should not contain unused type declarations: ";
            ref_msg += typedef_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 2.6
 * A function should not contain unused label declarations
 */
void MISRADeclRule::CheckUnusedLabelInFunction() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::LABEL;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        if (!decl->isUsed()) {
          issue = report->ReportIssue(MISRA, M_R_2_6, decl);
          std::string ref_msg = "A project should not contain unused type declarations: ";
          ref_msg += clang::dyn_cast<clang::LabelDecl>(decl)->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
      }, true);
}

/* MISRA
 * Rule: 2.7
 * There should be no unused parameters in functions
 */
void MISRADeclRule::CheckUnusedParameters(const clang::FunctionDecl *decl) {
  if (decl->getBody()) {
    if (decl->getNumParams() == 0) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    for (const auto &it : decl->parameters()) {
      if (!it->isUsed()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(MISRA, M_R_2_7, decl);
          std::string ref_msg = "There should be no unused parameters in functions ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(&(*it));
      }
    }
  }
}

/* MISRA
 * Rule: 5.1
 * External identifiers shall be distinct
 */
void MISRADeclRule::CheckUndistinctExternalIdent() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR;

  std::unordered_map<std::string, const clang::VarDecl *> vars;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&vars](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        const auto *var_decl = clang::dyn_cast<clang::VarDecl>(decl);
        if (var_decl) {
          auto name = var_decl->getNameAsString();

          if (name.length() > 31) {
            bool found = false;
            for (const auto &it : vars) {
              if (name.substr(0, 31) == it.first.substr(0, 31)) {
                found = true;
                issue = report->ReportIssue(MISRA, M_R_5_1, it.second);
                std::string ref_msg = "External identifiers shall be distinct: ";
                ref_msg += var_decl->getNameAsString();
                issue->SetRefMsg(ref_msg);
                issue->AddDecl(var_decl);
              }
            }

            if (!found) {
              vars.emplace(std::make_pair(name, var_decl));
            }

          }
        }
      }, true);

}

/* MISRA
 * Rule: 5.3
 * An identifier declared in an inner scope shall not hide an identifier declared in an outer scope
 */
void MISRADeclRule::CheckIdentifierNameConflict() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::TYPEDEF;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        std::vector<const clang::VarDecl *> vars;
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        if (var_name.empty()) return;
        if (auto var = clang::dyn_cast<clang::VarDecl>(decl)) {
          if (const_cast<clang::VarDecl *>(var)->isLocalExternDecl()) return;
        }
        id_mgr->GetOuterVariables(var_name, vars);

        if (!vars.empty()) {
          auto var_decl = clang::dyn_cast<clang::NamedDecl>(decl);
          if (var_decl == nullptr) return;
          issue = report->ReportIssue(MISRA, M_R_5_3, decl);
          std::string ref_msg = "External identifiers shall be distinct: ";
          ref_msg += var_decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
          for (const auto &it : vars) {
            issue->AddDecl(&(*it));
          }
        }
      }, true);
}

/* MISRA
 * Rule: 5.4
 * Macro identifiers shall be distinct
 */
void MISRADeclRule::CheckMacroIdentifierDistinct() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();

  auto it = scope_mgr->GetMacroMap().begin();
  auto end = scope_mgr->GetMacroMap().end();
  for (; it != end; it++) {
    for (auto next = std::next(it); next != end; next++) {
      if (strncmp(it->first.c_str(), next->first.c_str(), 31) == 0) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();
        issue = report->ReportIssue(MISRA, M_R_5_4, it->second->getLocation());
        std::string ref_msg = "Identifiers shall be distinct from macro names";
        issue->SetRefMsg(ref_msg);
        issue->AddLocation(next->second->getLocation());
      }
    }
  }
}

/* MISRA
 * Rule: 5.5
 * Identifiers shall be distinct from macro names
 */
void MISRADeclRule::CheckIdentifiedMacro() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::FUNCTION;

  for (const auto &it : scope_mgr->GetMacroMap()) {
    top_scope->TraverseAll<kind,
        const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
        [&it](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
          XcalIssue *issue = nullptr;
          XcalReport *report = XcalCheckerManager::GetReport();

          if (strncmp(name.c_str(), it.first.c_str(), 31) == 0) {
            issue = report->ReportIssue(MISRA, M_R_5_5, it.second->getLocation());
            std::string ref_msg = "Identifiers shall be distinct from macro names";
            issue->SetRefMsg(ref_msg);
          }
        }, true);
  }
}

/* MISRA
 * Rule: 5.6
 * A typedef name shall be a unique identifier
 */
void MISRADeclRule::CheckTypedefUnique() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::FIELD | IdentifierManager::TYPE |
                            IdentifierManager::FUNCTION | IdentifierManager::LABEL | IdentifierManager::VALUE;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        if (top_scope->HasTypeDef<true>(name)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_5_6, decl);
            std::string ref_msg = "A typedef name shall be a unique identifier";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 5.7
 * A tag name shall be a unique identifier
 */
void MISRADeclRule::CheckTageUnique() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::TYPE;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        if (top_scope->HasNRecordName<true>(name, 2)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_5_7, decl);
            std::string ref_msg = "A tag name shall be a unique identifier";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 5.9
 * Identifiers that define objects or functions with internal linkage shall be unique
 */
void MISRADeclRule::CheckInternalIdentifierUnique() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        std::vector<const clang::VarDecl *> vars;
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        if (var_name.empty()) return;
        id_mgr->GetOuterVariables(var_name, vars);
        bool need_report = false;

        if (!vars.empty()) {
          for (const auto &it : vars) {
            if ((&*it)->getStorageClass() == clang::StorageClass::SC_Static)
              need_report = true;
          }
          if (need_report) {
            auto var_decl = clang::dyn_cast<clang::NamedDecl>(decl);
            if (var_decl == nullptr) return;
            issue = report->ReportIssue(MISRA, M_R_5_9, decl);
            std::string ref_msg = "Identifiers that define objects or functions "
                                  "with internal linkage shall be unique: ";
            ref_msg += var_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
        }
      }, true);
}

/* MISRA
 * Rule 6.1
 * Bit-fields shall only be declared with an appropriate type
 * Note: This assumes that the "int" type is 32 bit
 */
void MISRADeclRule::CheckInappropriateBitField(const clang::RecordDecl *decl) {
  if (decl->field_empty()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  for (const auto &field : decl->fields()) {
    if (!field->isBitField()) continue;
    bool need_report = false;
    auto type = field->getType();
    if (!type->isIntegerType() || type->isBooleanType()) continue;

    auto start = src_mgr->getCharacterData(field->getBeginLoc());
    auto end = src_mgr->getCharacterData(field->getEndLoc());
    std::string token;
    while (start != end) {
      token += *start;
      start++;
    }

    if (!IsExplicitSign(GetTypeString(type)) && !IsExplicitSign(token))
      need_report = true;

    auto bt_type = clang::dyn_cast<clang::BuiltinType>(type);
    if (bt_type != nullptr) {
      auto kind = bt_type->getKind();
      if ((kind > clang::BuiltinType::Int && kind <= clang::BuiltinType::Int128) ||
          (kind > clang::BuiltinType::UInt && kind <= clang::BuiltinType::UInt128))
        need_report = true;
    }

    if (need_report) {
      if (issue == nullptr) {
        issue = report->ReportIssue(MISRA, M_R_6_1, decl);
        std::string ref_msg = "Bit-fields shall only be declared with an appropriate type: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(field);
    }
  }
}

/* MISRA
 * Rule: 6.2
 * A typedef name shall be a unique identifier
 */
void MISRADeclRule::CheckSingleBitSignedValue(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->fields()) {
    if (it->getType()->isSignedIntegerType() && it->isBitField()) {
      auto bit_width = it->getBitWidthValue(decl->getASTContext());
      if (bit_width < 2) {
        if (issue == nullptr) {
          issue = report->ReportIssue(MISRA, M_R_6_2, decl);
          std::string ref_msg = "Signed-value must be longer than two bits: ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(&(*it));
      }
    }
  }
}

/* MISRA
 * Rule: 7.4
 * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
 */
void MISRADeclRule::CheckStringLiteralToNonConstChar(const clang::VarDecl *decl) {
  auto decl_type = decl->getType();
  if (decl_type->isPointerType() && decl_type->getPointeeType()->isCharType() &&
      !decl_type->getPointeeType().isConstQualified()) {
    if (!decl->hasInit()) return;
    auto init = decl->getInit()->IgnoreParenImpCasts();
    if (init->getStmtClass() == clang::Stmt::StringLiteralClass) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_7_4, decl);
      std::string ref_msg = "A string literal shall not be assigned to an object unless the object’s type is \"pointer to const-qualified char\"";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8.2
 * Function types shall be in prototype form with named parameters
 */
void MISRADeclRule::CheckParameterNoIdentifier(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->parameters()) {
    if (it->getNameAsString().empty()) {
      issue = report->ReportIssue(MISRA, M_R_8_2, decl);
      std::string ref_msg = "Function types shall be in prototype form with named parameters";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8.8
 * The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage
 */
void MISRADeclRule::CheckStaticSpecifier(const clang::FunctionDecl *decl) {
  if (decl->isThisDeclarationADefinition()) {
    auto canonical_decl = decl->getCanonicalDecl();
    if (canonical_decl->isStatic() != decl->isStatic()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_8_8, decl);
      std::string ref_msg = "The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8.10
 * An inline function shall be declared with the static storage class
 */
void MISRADeclRule::CheckInlineFunctionWithExternalLinkage(const clang::FunctionDecl *decl) {
  if (decl->isInlineSpecified() && (decl->getStorageClass() == clang::StorageClass::SC_Extern)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_10, decl);
    std::string ref_msg = "An inline function shall be declared with the static storage class";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 8.11
 * When an array with external linkage is declared, its size should be explicitly specified
 */
void MISRADeclRule::CheckImplicitSizeWithExternalArray(const clang::VarDecl *decl) {
  if (!decl->getType()->isArrayType()) return;
  if (decl->getStorageClass() != clang::StorageClass::SC_Extern) return;

  auto array_type = clang::dyn_cast<clang::ArrayType>(decl->getType());
  if (array_type && !array_type->isConstantArrayType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_11, decl);
    std::string ref_msg = "When an array with external linkage is declared, its size should be explicitly specified";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 8.12
 * Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique
 */
void MISRADeclRule::CheckNonUniqueValueOfEnum(const clang::EnumDecl *decl) {
  std::unordered_map<uint64_t, const clang::EnumConstantDecl *> vals;
  for (const auto &it : decl->enumerators()) {
    auto val = it->getInitVal().getZExtValue();
    auto res = vals.find(val);
    if (res == vals.end()) vals.emplace(val, it);
    else {
      if (it->getInitExpr() && res->second->getInitExpr()) continue;
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_8_12, decl);
      std::string ref_msg = "Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique";
      issue->SetRefMsg(ref_msg);
      issue->AddDecl(it);
      issue->AddDecl(res->second);
      return;
    }
  }
}

/* MISRA
 * Rule: 9.3
 * Arrays shall not be partially initialized
 */
void MISRADeclRule::CheckArrayPartialInitialized(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  if (!decl->hasInit()) return;
  auto init = decl->getInit();

  auto const_array_type = clang::dyn_cast<clang::ConstantArrayType>(type);
  if (!const_array_type) return;
  auto size = const_array_type->getSize().getZExtValue();

  auto initList = clang::dyn_cast<clang::InitListExpr>(init);
  if (initList == nullptr) return;

  auto inits = initList->inits();

  if (inits.size() == size) return;

  if (inits.size() == 1) {
    auto head = inits[0]->IgnoreParenImpCasts();
    uint64_t val = 0;

    auto child0 = (head->children().empty()) ? nullptr : *(head->child_begin());
    clang::Expr *sub_expr = nullptr;
    if (child0 != nullptr) {
      sub_expr = clang::dyn_cast<clang::Expr>(child0);
    }
    if (IsIntegerLiteralExpr(head, &val) || (sub_expr && IsIntegerLiteralExpr(sub_expr, &val))) {
      if (val == 0) {
        return;
      }
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_9_3, decl);
  std::string ref_msg = "Arrays shall not be partially initialized";
  issue->SetRefMsg(ref_msg);

}

/* MISRA
 * Rule: 9.5
 * Where designated initializers are used to initialize an array object the size of the array shall be specified explicitly
 */
void MISRADeclRule::CheckDesignatedInitWithImplicitSizeArray(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  auto arr_type = clang::dyn_cast<clang::ArrayType>(type);
  if (arr_type && arr_type->getElementType()->isArrayType()) return;
  if (!decl->hasInit()) return;


  auto src_mgr = XcalCheckerManager::GetSourceManager();
  {
    auto var_loc = decl->getLocation();
    auto end_loc = decl->getEndLoc();
    auto var_pos = src_mgr->getCharacterData(var_loc);
    auto end_pos = src_mgr->getCharacterData(end_loc);
    if (var_pos == end_pos) return;
    while (*var_pos != '[') {
      var_pos++;
      if (var_pos == end_pos) break;
    }

    // eat '['
    var_pos++;
    // return if size is specified
    if (*var_pos != ']') return;
  }

  bool with_designated = false;
  auto inits = clang::dyn_cast<clang::InitListExpr>(decl->getInit());
  if (!inits) return;
  auto init_loc = inits->getBeginLoc();
  auto end_loc = inits->getEndLoc();
  auto init_pos = src_mgr->getCharacterData(init_loc);
  auto end_pos = src_mgr->getCharacterData(end_loc);
  // eat '{'
  if (*init_pos == '{') init_pos++;
  while (init_pos != end_pos) {
    bool need_eat = false;
    /* parse init expr, split with ',' */
    // eat space
    while (std::isspace(*init_pos)) {
      init_pos++;
      if (init_pos == end_pos) break;
    }
    if (init_pos == end_pos) break;
    if (*init_pos == '[') {
      with_designated = true;
      break;
    } else {
      while (*init_pos != ',') {
        init_pos++;
        if (init_pos == end_pos) {
          need_eat = false;
          break;
        }
        need_eat = true;
      }
      // eat ','
      if (need_eat) init_pos++;
    }
  }

  if (with_designated) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_9_5, decl);
    std::string ref_msg = "Where designated initializers are used to initialize an array object the size of the array "
                          "shall be specified explicitly";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 17.1
 * The features of <stdarg.h> shall not be used
 * Rule: 21.5
 * The standard header file <signal.h> shall not be used
 */
void MISRADeclRule::CheckForbiddenHeaderFile() {
  std::string res;
  std::string command = "python3 $XSCA_HOME/doti_parser.py -f ";
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  auto fid_file_list = conf_mgr->ForbidHeaderFileList();

  auto file = XcalCheckerManager::GetSourceManager()->fileinfo_begin()->first->tryGetRealPathName();
  if (!file.endswith(".i") && !file.endswith(".ii")) return;
  command += file.str() + " -l \"";
  for (const auto &it : fid_file_list) {
    command += it + ", ";
  }
  command += '"';

  int ret = XcalCheckerManager::RunCommand(command, res);
  if (ret != 0) return;

  std::vector<std::array<std::string, 3>> includeInfo;

  // [('/usr/include/signal.h', 'a.c', '1'), ('/usr/include/stdio.h', 'a.c', '2')]
  auto parseItem = [](std::string::iterator &it, std::string &incFile, std::string &sinkFile, std::string &lineNum) {
    if (*it == '(') {
      it += 2;  // eat ('
      while (*it != '\'') {
        incFile += *it;
        it++;
      }
      it += 4;  // eat ', '
      while (*it != '\'') {
        sinkFile += *it;
        it++;
      }
      it += 4;  // eat ', '
      while (*it != '\'') {
        lineNum += *it;
        it++;
      }
    }
  };

  for (auto it = res.begin(); it != res.end();) {
    std::string incFile, sinkFile, lineNum;
    if (*it != '(') {
      it++;
      continue;
    }
    parseItem(it, incFile, sinkFile, lineNum);
    if (!incFile.empty() && !sinkFile.empty() && !lineNum.empty())
      includeInfo.push_back({incFile, sinkFile, lineNum});
  }


  auto getShortIncFile = [](const std::string &name) -> std::string {
    std::string res;
    for (auto it = name.rbegin(); it != name.rend(); it++) {
      if (*it == '/') break;
      res = *it + res;
    }
    return res;
  };

  auto src_mgr = XcalCheckerManager::GetSourceManager();
  clang::FileManager &file_mgr = src_mgr->getFileManager();

  for (const auto &it : includeInfo) {
    std::string filename = it[0];
    if (conf_mgr->IsForbidHeaderFile(getShortIncFile(filename))) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      std::string i2c;
      for (auto i = 0; i < it[1].size(); i++) {
        if (it[1][i] != '.') {
          i2c += it[1][i];
          continue;
        }

        // convert .c to .i, .cpp to .ii
        if (it[1][i + 1] == 'c' && (i + 2 == it[1].size())) {
          i2c += ".i";
          break;
        } else if (it[1][i + 1] == 'c' && it[1][i + 2] == 'p') {
          i2c += ".ii";
          break;
        } else {
          i2c += it[1][i];
        }
      }

      auto file_ref = file_mgr.getOptionalFileRef(i2c);
      if (!file_ref.hasValue()) continue;
      const clang::FileEntry *file_entry_ptr = &file_ref.getValue().getFileEntry();
      int line = std::stoi(it[2]);
      auto loc = src_mgr->translateFileLineCol(file_entry_ptr, line, 0);
      if (loc.isInvalid()) continue;

      if (filename.find("stdarg.h") != std::string::npos)
        issue = report->ReportIssue(MISRA, M_R_17_1, loc);
      else if (filename.find("signal.h") != std::string::npos)
        issue = report->ReportIssue(MISRA, M_R_21_5, loc);
      else if (filename.find("tgmath.h") != std::string::npos)
        issue = report->ReportIssue(MISRA, M_R_21_11, loc);
      else if (filename.find("setjmp.h") != std::string::npos)
        issue = report->ReportIssue(MISRA, M_R_21_4, loc);
      else if (filename.find("time.h") != std::string::npos)
        issue = report->ReportIssue(MISRA, M_R_21_10, loc);

      std::string ref_msg = "The features of " + it[0] + " shall not be used";
      issue->SetRefMsg(ref_msg);
    }
  }

}

/* MISRA
 * Rule: 17.6
 * The declaration of an array parameter shall not contain the static keyword between the [ ]
 */
void MISRADeclRule::CheckStaticBetweenBracket(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &param : decl->parameters()) {
    auto param_type = param->getType();
    auto decay_type = clang::dyn_cast<clang::DecayedType>(param_type);
    if (decay_type == nullptr) continue;
    auto origin_type = decay_type->getOriginalType();
    auto array_type = clang::dyn_cast<clang::ArrayType>(origin_type);
    if (array_type == nullptr) continue;
    if (array_type->getSizeModifier() == clang::ArrayType::Static) {
      if (issue == nullptr) {
        issue = report->ReportIssue(MISRA, M_R_17_6, decl);
        std::string ref_msg = "The declaration of an array parameter shall not contain the static keyword between the [ ]";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(param);
    }
  }
}

/* MISRA
 * Rule: 18.5
 * Declarations should contain no more than two levels of pointer nesting
 */
XcalIssue *MISRADeclRule::ReportNestedTwoLevel(const clang::Decl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_18_5, decl);
  std::string ref_msg = "Declarations should contain no more than two levels of pointer nesting";
  issue->SetRefMsg(ref_msg);
  return issue;
}

void MISRADeclRule::CheckPointerNestedMoreThanTwoLevel(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  if (IsPointerNestedMoreThanTwoLevel(decl->getReturnType()))
    issue = ReportNestedTwoLevel(decl);

  for (const auto &param : decl->parameters()) {
    if (IsPointerNestedMoreThanTwoLevel(param->getType())) {
      if (issue == nullptr) issue = ReportNestedTwoLevel(decl);
      issue->AddDecl(param);
    }
  }
}


/* MISRA
 * Rule: 18.7
 * Flexible array members shall not be declared
 */
void MISRADeclRule::CheckFlexibleArray(const clang::RecordDecl *decl) {
  if (decl->hasFlexibleArrayMember()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_7, decl);
    std::string ref_msg = "Flexible array members shall not be declared";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 18.8
 * Variable-length array types shall not be used
 */
void MISRADeclRule::CheckVariableAsArrayLength(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  auto array_type = clang::dyn_cast<clang::ArrayType>(type);
  if (!array_type) return;
  if (array_type->hasSizedVLAType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_8, decl);
    std::string ref_msg = "Variable-length array types shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRADeclRule::CheckVariableAsArrayLength(const clang::FieldDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  auto array_type = clang::dyn_cast<clang::ArrayType>(type);
  if (!array_type) return;
  bool need_report = false;
  if (array_type->hasSizedVLAType()) {
    need_report = true;
  } else if (auto const_array_type = clang::dyn_cast<clang::ConstantArrayType>(array_type)) {
    auto size = const_array_type->getSize().getZExtValue();
    if (size == 0) need_report = true;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_8, decl);
    std::string ref_msg = "Variable-length array types shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 19.2
 * The union keyword should not be used
 */
void MISRADeclRule::CheckUnionKeyword(const clang::RecordDecl *decl) {
  if (decl->isUnion()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_19_2, decl);
    std::string ref_msg = "The union keyword should not be used";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRADeclRule::CheckUnionKeyword(const clang::TypedefDecl *decl) {
  auto type = decl->getTypeSourceInfo()->getType();
  if (auto elaborated_type = clang::dyn_cast<clang::ElaboratedType>(type)) {
    auto named_type = elaborated_type->getNamedType();
    if (named_type->isUnionType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_19_2, decl);
      std::string ref_msg = "The union keyword should not be used";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 2-10-6
 * If an identifier refers to a type, it shall not also refer to an
 * object or a function in the same scope.
 */
void MISRADeclRule::CheckObjectOrFunctionConflictWithType() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::TYPE |
                            IdentifierManager::FUNCTION;


  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        bool res = false, has_func = false, has_var = false, has_typedef = false;
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        std::vector<const clang::Decl *> sinks;
        if (auto record = clang::dyn_cast<clang::RecordDecl>(decl)) {
          auto record_name = record->getNameAsString();
          has_func = id_mgr->HasFunctionName(record_name);
          has_var = id_mgr->HasVariableName<false>(record_name);
          has_typedef = id_mgr->HasTypeDef<false>(record_name);

          res = has_func | has_var | has_typedef;

          if (has_func) {
            auto func_range = id_mgr->GetFunctionDecls(record_name);
            for (auto it = func_range.first; it != func_range.second; it++) {
              sinks.push_back(it->second);
            }
          }

          if (has_var) {
            std::vector<const clang::VarDecl *> vars;
            id_mgr->GetVariables<false>(record_name, vars);
            sinks.insert(sinks.end(), vars.begin(), vars.end());
          }

          if (has_typedef) {
            std::vector<const clang::TypedefDecl *> typedecls;
            id_mgr->GetTypedefs<false>(record_name, typedecls);
            sinks.insert(sinks.begin(), typedecls.begin(), typedecls.end());
          }
        }

        if (res) {
          issue = report->ReportIssue(MISRA, M_R_2_10_6, decl);
          std::string ref_msg = "If an identifier refers to a type, it shall not also refer to an "
                                "object or a function in the same scope.";
          issue->SetRefMsg(ref_msg);
          if (!sinks.empty()) {
            for (const auto &it : sinks) issue->AddDecl(it);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 8-3-1
 * Parameters in an overriding virtual function shall either use the
 * same default arguments as the function they override, or else
 * shall not specify any default arguments.
 */
void MISRADeclRule::CheckOverriddenVirtualFuncHasDiffParam(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->methods().empty()) return;

  auto ctx = XcalCheckerManager::GetAstContext();

  bool need_report = false;
  std::vector<const clang::Decl *> sinks;
  for (const auto &method : decl->methods()) {
    if (!method->isVirtual()) continue;
    for (const auto &origin_method : method->overridden_methods()) {
      const auto size = origin_method->param_size();
      if (size != method->param_size()) continue;
      for (auto i = 0; i < size; i++) {
        auto o_param = origin_method->getParamDecl(i);
        auto c_param = method->getParamDecl(i);
        if (c_param->hasDefaultArg()) {
          auto o_default = o_param->getDefaultArg();
          auto c_default = c_param->getDefaultArg();
          clang::Expr::EvalResult o_val, c_val;
          o_default->EvaluateAsRValue(o_val, *ctx);
          c_default->EvaluateAsRValue(c_val, *ctx);

          // TODO: only check builtin type here
          if (c_val.Val.isInt()) {
            if (c_val.Val.getInt() == o_val.Val.getInt()) {
              continue;
            }
          } else if (c_val.Val.isFloat()) {
            if (c_val.Val.getFloat() == o_val.Val.getFloat()) continue;
          } else {
            continue;;
          }

          need_report = true;
          sinks.push_back(c_param);
          sinks.push_back(o_param);
        }
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_3_1, decl);
    std::string ref_msg = "Parameter init value of virtual overridden function should keep same";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) {
      issue->AddDecl(it);
    }
  }
}

/*
* MISRA: 8-4-4
* A function identifier shall either be used to call the function or it shall be preceded by &.
*/
void MISRADeclRule::CheckUseFunctionNotCallOrDereference(const clang::VarDecl *decl) {
  if (!decl->hasInit()) return;
  bool need_report = false;
  auto init = decl->getInit()->IgnoreParenCasts();
  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(init)) {
    auto _decl = decl_ref->getDecl();
    if (_decl && clang::isa<clang::FunctionDecl>(_decl)) need_report = true;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_4_4, decl);
    std::string ref_msg = "A function identifier shall either be used to call the function "
                          "or it shall be preceded by &.";
    issue->SetRefMsg(ref_msg);
  }

}

/* MISRA
 * Rule: 10-1-3
 * base class should not be both virtual and non-virtual in the same hierarchy
 */
void MISRADeclRule::CheckDifferentVirtualInSameHierarchy(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() == 0) return;

  bool need_report = false;
  std::unordered_set<const clang::RecordDecl *> vbases;
  std::unordered_set<const clang::RecordDecl *> sinks;

  auto getParentClassDecl = [](const clang::QualType &type) -> const clang::CXXRecordDecl * {
    auto record_type = clang::dyn_cast<clang::RecordType>(type);
    if (record_type == nullptr) return nullptr;
    auto parent_decl = clang::dyn_cast<clang::CXXRecordDecl>(record_type->getDecl());
    return parent_decl;
  };

  for (const auto &parent : decl->bases()) {
    auto parent_decl = getParentClassDecl(parent.getType());
    if (parent_decl == nullptr) continue;
    if (parent_decl->getNumBases() == 0) continue;

    for (auto &super_class : parent_decl->bases()) {
      auto super_decl = getParentClassDecl(super_class.getType());
      if (super_decl == nullptr) continue;
      if (!parent_decl->isVirtuallyDerivedFrom(super_decl)) {
        if (vbases.find(super_decl) != vbases.end()) {
          need_report = true;
          sinks.insert(parent_decl);
        }
      } else {
        vbases.insert(super_decl);
      }
    }

  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_1_3, decl);
    std::string ref_msg = "base class should not be both virtual and non-virtual in the same hierarchy";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddDecl(it);
  }
}

/* MISRA
 * Rule: 10-2-1
 * all visible names within a inheritance hierarchy must be unique
 */
void MISRADeclRule::CheckUniqueNameInHierarchy(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() < 1) return;

  std::unordered_set<const clang::Decl *> sinks;
  std::unordered_map<std::string, const clang::FieldDecl *> field_records;
  std::unordered_map<std::string, const clang::CXXMethodDecl *> method_records;

  for (const auto &it : decl->fields()) {
    field_records.insert({it->getNameAsString(), it});
  }

  for (const auto &it : decl->methods()) {
    method_records.insert({it->getNameAsString(), it});
    if (it->isDefaulted()) continue;
  }

  for (const auto &it : decl->bases()) {
    auto base = GetBaseDecl(it);
    if (base == nullptr) continue;
    for (const auto &field : base->fields()) {
      auto field_name = field->getNameAsString();
      auto res = field_records.find(field_name);
      if (res != field_records.end()) {
        sinks.insert(field);
        sinks.insert(res->second);
      } else {
        field_records.insert({field_name, field});
      }
    }

    for (const auto &method : base->methods()) {
      if (method->isDefaulted()) continue;
      if (clang::isa<clang::CXXConstructorDecl>(method) ||
          clang::isa<clang::CXXDestructorDecl>(method))
        continue;
      auto method_name = method->getNameAsString();
      auto res = method_records.find(method_name);
      if (res != method_records.end()) {
        sinks.insert(method);
        sinks.insert(res->second);
      } else {
        method_records.insert({method_name, method});
      }
    }
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_2_1, decl);
    std::string ref_msg = "All visible names within a inheritance hierarchy must be unique";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddDecl(it);
  }

}

/* MISRA
 * Rule: 10-3-2
 * Overridden virtual functions in derived class should be noted with virtual
 */
void MISRADeclRule::CheckOverriddenVirtualFunction(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &method : decl->methods()) {
    if (method->isVirtualAsWritten()) continue;
    for (const auto &it : method->overridden_methods()) {
      if (it->isVirtual()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(MISRA, M_R_10_3_2, decl);
          std::string ref_msg = "Overridden virtual functions in derived class should be noted with virtual";
          issue->SetRefMsg(ref_msg);
          issue->AddDecl(method);
        }
      }
    }
  }
}

/* MISRA
 * Rule: 11-0-1
 * Member data in non-POD class types shall be private.
 */
void MISRADeclRule::CheckNonPrivateFieldsInNormalClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->isPOD()) return;

  std::unordered_set<const clang::Decl *> sinks;
  for (const auto &field : decl->fields()) {
    if (field->getAccess() == clang::AccessSpecifier::AS_private) continue;
    sinks.insert(field);
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_0_1, decl);
    std::string ref_msg = "Member data in non-POD class types shall be private.";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddDecl(it);
  }
}

/*
 * MISRA: 12-1-2
 * Derived class should contain constructor of base class
 */
void MISRADeclRule::CheckDerivedClassContainConstructorOfBaseClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() == 0) return;

  // get base class
  std::unordered_set<clang::CXXRecordDecl *> bases;
  for (const auto &base : decl->bases()) {
    const clang::RecordType *base_type = clang::dyn_cast<clang::RecordType>(base.getType());
    if (base_type && base_type->getAsCXXRecordDecl())
      bases.insert(base_type->getAsCXXRecordDecl());
  }

  bool need_report = false;
  std::vector<clang::CXXConstructorDecl *> sinks;
  if (!decl->hasUserDeclaredConstructor()) {
    need_report = true;
  } else {
    for (const auto &method : decl->methods()) {
      auto tmp = bases;
      if (auto constructor = clang::dyn_cast<clang::CXXConstructorDecl>(method)) {
        for (const auto init : constructor->inits()) {
          auto expr = init->getInit();
          if (expr == nullptr) continue;
          if (auto constructor_expr = clang::dyn_cast<clang::CXXConstructExpr>(expr)) {

            // check if this is implicit default constructor
            if (constructor_expr->getBeginLoc() == constructor_expr->getEndLoc()) {
              continue;
            }

            auto parent = constructor_expr->getConstructor()->getParent();
            auto it = tmp.find(parent);
            if (it != tmp.end()) tmp.erase(it);
          }
        }

        if (!tmp.empty()) {
          need_report = true;
          sinks.push_back(constructor);
        }
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_1_2, decl);
    std::string ref_msg = "Derived class should contain constructor of base class";
    issue->SetRefMsg(ref_msg);
    for (const auto &sink : sinks) {
      issue->AddDecl(sink);
    }
  }
}


/*
 * MISRA: 12-1-3
 * All constructors that are callable with a single argument of fundamental type shall be declared explicit.
 */
void MISRADeclRule::CheckExplicitConstructorWithSingleParam(const clang::FunctionDecl *decl) {
  if (auto constructor = clang::dyn_cast<clang::CXXConstructorDecl>(decl)) {
    if (constructor->isDefaulted() || (constructor->param_size() != 1)) return;

    if (constructor->isExplicit()) return;
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_1_3, decl);
    std::string ref_msg = "Construct functions which contains only one parameter should be note by \"explicit\"";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 12-8-2
 * The copy assignment operator shall be declared protected or private in an abstract class.
 */
void MISRADeclRule::CheckUnPrivateCopyAssigmentOpOfAbstractClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (!decl->isAbstract()) return;

  std::unordered_set<const clang::Decl *> sinks;
  for (const auto &it : decl->methods()) {
    if (!it->isCopyAssignmentOperator() || it->isImplicit()) continue;
    if (it->getAccess() == clang::AS_public) {
      sinks.insert(it);
    }
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_8_2, decl);
    std::string ref_msg = "The copy assignment operator shall be declared protected or private in an abstract class.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 14-5-2
 * A copy constructor shall be declared when there is a template constructor with
 * a single parameter that is a generic parameter.
 */
void MISRADeclRule::CheckCTorWithTemplateWithoutCopyCtor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  bool flag = false;
  for (const auto &method : decl->decls()) {
    if (IsSingleTemplateTypeParamFunction(method)) {
      auto func = clang::cast<clang::FunctionTemplateDecl>(method)->getTemplatedDecl();
      if (clang::isa<clang::CXXConstructorDecl>(func)) {
        flag = true;
        break;
      }
    }
  }

  if (flag == true && !decl->hasUserDeclaredCopyConstructor()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_14_5_2, decl);
    std::string ref_msg = "A copy constructor shall be declared when there is a template constructor"
                          " with a single parameter that is a generic parameter.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 14-5-3
 * A copy assignment operator shall be declared when there is a template
 * assignment operator with a parameter that is a generic parameter.
 */
void MISRADeclRule::CheckCopyAssignmentWithTemplate(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  bool flag = false;
  for (const auto &method : decl->decls()) {
    if (IsSingleTemplateTypeParamFunction(method)) {
      auto func = clang::cast<clang::FunctionTemplateDecl>(method)->getTemplatedDecl();
      if (func->getNameAsString() == "operator=") {
        flag = true;
        break;
      }
    }
  }

  if (flag == true && !decl->hasUserDeclaredCopyAssignment()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_14_5_3, decl);
    std::string ref_msg = "A copy assignment operator shall be declared when there is a template assignment "
                          "operator with a parameter that is a generic parameter.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 15-4-1
 * If a function is declared with an exception-specification, then all declarations of the same
 * function (in other translation units) shall be declared with the same set of type-ids.
 */
void MISRADeclRule::CheckExceptionSpecification(const clang::FunctionDecl *decl) {
  if (decl->isThisDeclarationADefinition()) {
    auto canonical_decl = decl->getCanonicalDecl();
    auto func_tp = decl->getType()->getAs<clang::FunctionProtoType>();
    auto canonical_tp = canonical_decl->getType()->getAs<clang::FunctionProtoType>();
    bool need_report = false;
    if (func_tp && canonical_tp) {
      std::vector<clang::QualType> types;
      for (auto type : func_tp->exceptions()) {
        types.push_back(type);
      }

      if (canonical_tp->exceptions().size() != types.size()) {
        need_report = true;
      } else {
        for (const auto &it : canonical_tp->exceptions()) {
          auto res = std::find_if(types.begin(), types.end(), [&it](const clang::QualType &type) {
            return type == it;
          });
          if (res != types.end()) {
            types.erase(res);
          } else {
            need_report = true;
            break;
          }
        }

        if (!types.empty()) need_report = true;
      }
    }

    if (need_report) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_14_5_3, decl);
      std::string ref_msg = "Exception specifier should keep same.";
      issue->SetRefMsg(ref_msg);
    }
  }

}

/*
 * MISRA: 15-5-2
 * Where a function’s declaration includes an exception-specification, the function
 * shall only be capable of throwing exceptions of the indicated type(s).
 */
void MISRADeclRule::CheckThrownUnSpecifiedType() {

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::FUNCTION;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&top_scope](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        bool need_report = false;
        auto func = clang::cast<clang::FunctionDecl>(decl);
        auto except_types = top_scope->GetExceptionSpec(func);
        auto thrown_types = top_scope->GetThrowType(func);
        if (except_types.empty()) return;
        for (const auto &thrown : thrown_types) {
          if (std::find(except_types.begin(), except_types.end(), thrown) == except_types.end()) {
            need_report = true;
            break;
          }
        }

        if (need_report) {
          XcalIssue *issue = nullptr;
          XcalReport *report = XcalCheckerManager::GetReport();
          issue = report->ReportIssue(MISRA, M_R_15_5_2, decl);
          std::string ref_msg = "Function decl with exception spec, throw exception can be of indicated type in its decl";
          issue->SetRefMsg(ref_msg);
        }
      }, true);
}


}
}
