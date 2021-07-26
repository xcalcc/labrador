/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_stmt_rule.cpp
// ====================================================================
//
// implement all stmt related rules in MISRA
//

#include "MISRA_stmt_rule.h"

namespace xsca {
namespace rule {

/* MISRA
 * Rule: 7.4
 * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
 */
void MISRAStmtRule::CheckStringLiteralToNonConstChar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (rhs->getStmtClass() != clang::Stmt::StringLiteralClass) return;

  auto lhs_type = lhs->getType();
  if (lhs_type->isPointerType() && lhs_type->getPointeeType()->isCharType()) {
    if (!lhs_type->getPointeeType().isConstQualified()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_7_4, stmt);
      std::string ref_msg = "A string literal shall not be assigned to an object unless the object’s type is \"pointer to const-qualified char\"";
      issue->SetRefMsg(ref_msg);
    }
  }
}

void MISRAStmtRule::CheckStringLiteralToNonConstChar(const clang::CallExpr *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto decl = stmt->getCalleeDecl()->getAsFunction();

  int i = 0;
  for (const auto &it : stmt->arguments()) {
    if (it->IgnoreParenImpCasts()->getStmtClass() == clang::Stmt::StringLiteralClass) {
      auto param_decl = decl->getParamDecl(i);
      if (param_decl == nullptr) {
        i++;
        continue;;
      }
      auto param_type = param_decl->getType();
      if (param_type->isPointerType() &&
          param_type->getPointeeType()->isCharType() && !param_type->getPointeeType().isConstQualified()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(MISRA, M_R_7_4, stmt);
          std::string ref_msg = "A string literal shall not be assigned to an object unless the object’s type is \"pointer to const-qualified char\"";
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(param_decl);
      }
      i++;
    }
  }
}


}
}
