/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_stmt_rule.cpp
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include "GJB8114_enum.inc"
#include "GJB8114_stmt_rule.h"


namespace xsca {
namespace rule {

/*
 * GJB8114: 5.1.2.6
 * Loop body should be enclosed with brace
 */


void GJB8114StmtRule::CheckLoopBodyWithBrace(const clang::Stmt *stmt) {
  if (CheckStmtWithBrace(stmt)) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB8114, G5_1_2_6, stmt);
  std::string ref_msg = "Loop body should be enclosed with brace";
  issue->SetRefMsg(ref_msg);
}

bool GJB8114StmtRule::CheckStmtWithBrace(const clang::Stmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto location = stmt->getBeginLoc();
  auto data = src_mgr->getCharacterData(location);

  if (*data != '{') {
    return false;
  }
  return true;
}

/*
 * GJB8114: 5.3.1.3
 * Assigning to pointer parameters is forbidden
 */
void GJB8114StmtRule::CheckAssignToPointerParam(const clang::BinaryOperator *stmt) {
  if (stmt->getOpcode() != clang::BinaryOperatorKind::BO_Assign) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();

  if (auto decl_ref_expr = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
    auto decl = decl_ref_expr->getDecl();
    if (auto param_decl = clang::dyn_cast<clang::ParmVarDecl>(decl)) {
      if (!param_decl->getType()->isPointerType()) return;
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB8114, G5_3_1_3, stmt);
      std::string ref_msg = "Assigning to pointer parameters is forbidden";
      issue->SetRefMsg(ref_msg);
      issue->AddDecl(decl);
    }
  }
}

}
}
