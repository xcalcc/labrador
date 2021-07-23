/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for MISRA
//

#include "xsca_report.h"
#include "MISRA_enum.inc"
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

/* MISRA
 * Rule: 2.3
 * A project should not contain unused type declarations
 */
void MISRADeclRule::CheckUnusedTypedef(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (auto typedefType = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedecl = typedefType->getDecl();
    _used_typedef.insert(typedecl);
  }
}

void MISRADeclRule::CheckUnusedTypedef(const clang::TypedefDecl *decl) {
  auto type = clang::dyn_cast<clang::TypedefType>(decl->getUnderlyingType());
  if (type) {
    auto typedecl = type->getDecl();
    _used_typedef.insert(typedecl);
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
        auto typedef_decl = clang::dyn_cast<const clang::TypedefDecl>(decl);
        if (used_typedefs->find(reinterpret_cast<const clang::TypedefDecl *const>(typedef_decl)) ==
            used_typedefs->end()) {
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

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        if (!decl->isUsed()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_2_6, decl);
            std::string ref_msg = "A project should not contain unused type declarations: ";
            ref_msg += clang::dyn_cast<clang::LabelDecl>(decl)->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
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
          std::string ref_msg = "There should be no unused parameters in functions";
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

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::unordered_map<std::string, const clang::VarDecl *> vars;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&vars, &issue, &report](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        const auto *var_decl = clang::dyn_cast<clang::VarDecl>(decl);
        if (var_decl) {
          auto name = var_decl->getNameAsString();

          if (name.length() > 31) {
            bool found = false;
            for (const auto &it : vars) {
              if (name.substr(0, 31) == it.first.substr(0, 31)) {
                found = true;
                if (issue == nullptr) {
                  issue = report->ReportIssue(MISRA, M_R_5_1, it.second);
                  std::string ref_msg = "External identifiers shall be distinct: ";
                  ref_msg += var_decl->getNameAsString();
                  issue->SetRefMsg(ref_msg);
                } else {
                  issue->AddDecl(it.second);
                }
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
#if 0
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::TYPEDEF;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        std::vector<const clang::VarDecl *> vars;
        id_mgr->GetOuterVariables(var_name, vars);

        if (!vars.empty()) {
          auto var_decl = clang::dyn_cast<clang::VarDecl>(decl);
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_5_3, decl);
            std::string ref_msg = "External identifiers shall be distinct: ";
            ref_msg += var_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            var_decl->dumpColor();
            issue->AddDecl(var_decl);
          }
          for (const auto &it : vars) {
            issue->AddDecl(&(*it));
          }
        }
      }, true);
#endif
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
    bool need_report = false;
    auto type = field->getType();
    if (!type->isIntegerType()) continue;

    auto start = src_mgr->getCharacterData(field->getBeginLoc());
    auto end = src_mgr->getCharacterData(field->getEndLoc());
    std::string token;
    while (start != end) {
      token += *start;
      start++;
    }

    if (!IsExplicitSign(GetTypeString(type)) && !IsExplicitSign(token)) need_report = true;

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

}
}