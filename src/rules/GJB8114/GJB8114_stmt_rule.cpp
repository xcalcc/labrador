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

#include <clang/AST/ParentMapContext.h>

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

/*
 * GJB8114: 5.3.1.9
 * Using NULL to stand a nullptr instead of using 0
 */
void GJB8114StmtRule::CheckUsingNullWithPointer(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  // reuturn if not operating on pointer
  if (!lhs->getType()->isPointerType()) return;

  if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(rhs)) {
    rhs->dumpColor();

    int value;
    clang::Expr::EvalResult eval_result;
    auto ctx = XcalCheckerManager::GetAstContext();

    // try to fold the const expr
    if (literal->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = literal->getValue().getZExtValue();
    }
    if (value != 0) return;

    // check if rhs is NULL
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto data = src_mgr->getCharacterData(literal->getBeginLoc());
    auto end = src_mgr->getCharacterData(literal->getEndLoc());
    std::string init_val;
    while (data != end) {
      init_val += *data;
      data++;
    }

    if (init_val != "NULL") {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB8114, G5_3_1_9, stmt);
      std::string ref_msg = "Using NULL to stand a nullptr instead of using 0";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 5.4.1.8
 * Cases of switch should have the same hierarchy range
 */
void GJB8114StmtRule::CheckDifferentHierarchySwitchCase(const clang::SwitchStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto switchCase = stmt->getSwitchCaseList();
  auto ctx = XcalCheckerManager::GetAstContext();
  if (switchCase != nullptr) {
    auto caseParents = ctx->getParents(*switchCase)[0].get<clang::Stmt>();
    do {
      auto currentParent = ctx->getParents(*switchCase)[0].get<clang::Stmt>();
      if (caseParents != currentParent) {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB8114, G5_4_1_8, stmt);
          std::string ref_msg = "Cases of switch should have the same hierarchy range";
          issue->SetRefMsg(ref_msg);
        }
        issue->AddStmt(switchCase);
      }
    } while ((switchCase = switchCase->getNextSwitchCase()) != nullptr);
  }
}


}
}
