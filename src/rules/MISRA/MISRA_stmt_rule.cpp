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
      std::string ref_msg = "A string literal shall not be assigned to an object unless "
                            "the object’s type is \"pointer to const-qualified char\"";
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
          std::string ref_msg = "A string literal shall not be assigned to an object "
                                "unless the object’s type is \"pointer to const-qualified char\"";
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(param_decl);
      }
      i++;
    }
  }
}

/* MISRA
 * Rule: 10 .2
 * Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations
 */
void MISRAStmtRule::CheckAddOrSubOnCharacter(const clang::BinaryOperator *stmt) {
  if (!stmt->isAdditiveOp()) return;

  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
  if (lhs_type->isCharType() || rhs_type->isCharType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_2, stmt);
    std::string ref_msg = "Expressions of essentially character type shall not be"
                          " used inappropriately in addition and subtraction operations";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 10.4
 * Both operands of an operator in which the usual arithmetic conversions are performed
 * shall have the same essential type category
 */
bool MISRAStmtRule::IsTypeFit(clang::QualType lhs_type, clang::QualType rhs_type) {
  bool type_fit = true;
  if (lhs_type->isUnsignedIntegerType() != rhs_type->isUnsignedIntegerType()) {
    type_fit = false;
  } else if (lhs_type->isCharType() != rhs_type->isCharType()) {
    if (!lhs_type->isIntegerType() || !rhs_type->isIntegerType()) {
      type_fit = false;
    }
  } else if (lhs_type->isFloatingType() != rhs_type->isFloatingType()) {
    type_fit = false;
  }
  return type_fit;
}

void MISRAStmtRule::CheckArithmeticWithDifferentType(const clang::BinaryOperator *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  if (stmt->isAdditiveOp() || stmt->isComparisonOp()) {
    auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
    auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
    need_report = !IsTypeFit(lhs_type, rhs_type);
  }

  if (need_report) {
    issue = report->ReportIssue(MISRA, M_R_10_4, stmt);
    std::string ref_msg = "Both operands of an operator in which the usual"
                          " arithmetic conversions are performed shall have the same essential type category";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRAStmtRule::CheckArithmeticWithDifferentType(const clang::CompoundAssignOperator *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  if (stmt->isCompoundAssignmentOp()) {
    auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
    auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
    need_report = !IsTypeFit(lhs_type, rhs_type);
  }

  if (need_report) {
    issue = report->ReportIssue(MISRA, M_R_10_4, stmt);
    std::string ref_msg = "Both operands of an operator in which the usual"
                          " arithmetic conversions are performed shall have the same essential type category";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 10.5
 * The value of an expression should not be cast to an inappropriate essential type
 */
void MISRAStmtRule::CheckInappropriateCast(const clang::CStyleCastExpr *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto from_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();
  auto to_type = stmt->IgnoreParenImpCasts()->getType();

  bool need_report = false;
  if (from_type->isBooleanType()) {
    if (!to_type->isBooleanType()) {
      need_report = true;
    }
  } else if (from_type->isCharType()) {
    if (to_type->isBooleanType() || to_type->isEnumeralType() || to_type->isFloatingType()) {
      need_report = true;
    }
  } else if (from_type->isEnumeralType()) {
    if (to_type->isBooleanType()) {
      need_report = true;
    }
  } else if (from_type->isSignedIntegerType()) {
    if (to_type->isBooleanType() || to_type->isEnumeralType()) {
      need_report = true;
    }
  } else if (from_type->isUnsignedIntegerType()) {
    if (to_type->isBooleanType() || to_type->isEnumeralType()) {
      need_report = true;
    }
  } else if (from_type->isFloatingType()) {
    if (to_type->isBooleanType() || to_type->isCharType() || to_type->isEnumeralType()) {
      need_report = true;
    }
  }

  if (need_report) {
    issue = report->ReportIssue(MISRA, M_R_10_5, stmt);
    std::string ref_msg = "The value of an expression should not be cast to an inappropriate essential type";
    issue->SetRefMsg(ref_msg);
  }
}


}
}
