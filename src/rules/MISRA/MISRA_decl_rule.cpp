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
      [&issue, &report](const std::string &x, const clang::Decl *decl,
                        IdentifierManager *id_mgr) -> void {
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
 * A function should not contain unused label declarations
 */
void MISRADeclRule::CheckUnusedParameters(const clang::FunctionDecl *decl) {
  if (decl->getNumParams() == 0) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->parameters()) {
    if (!it->isUsed()) {
      if (issue == nullptr) {
        issue = report->ReportIssue(MISRA, M_R_2_7, decl);
        std::string ref_msg = "A function should not contain unused label declarations: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

}
}