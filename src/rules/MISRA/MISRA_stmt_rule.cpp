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

#include <clang/AST/ParentMapContext.h>
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
 * Rule: 10.2
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

/* MISRA
 * Rule: 10.6
 * The value of a composite expression shall not be assigned to an object with wider essential type
 */
void MISRAStmtRule::CheckCompositeExprAssignToWiderTypeVar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();

  if (rhs_type < lhs_type) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_6, stmt);
    std::string ref_msg = "The value of a composite expression shall not be assigned to an object with wider essential type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 10.7
 * If a composite expression is used as one operand of an operator in which the usual
 * arithmetic conversions are performed then the other operand shall not have wider essential type
 */
void MISRAStmtRule::CheckCompositeMixTypeExpr(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();

  bool need_report = false;
  if (lhs_type < rhs_type) {
    if (lhs->getStmtClass() == clang::Stmt::BinaryOperatorClass) {
      need_report = true;
    }
  } else if (rhs_type < lhs_type) {
    if (rhs->getStmtClass() == clang::Stmt::BinaryOperatorClass) {
      need_report = true;
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_7, stmt);
    std::string ref_msg = "Composite expression and mixed type is not allow";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 10.8
 * The value of a composite expression shall not be cast to a different
 * essential type category or a wider essential type
 */
void MISRAStmtRule::CheckCompositeExprCastToWiderType(const clang::CStyleCastExpr *stmt) {
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  auto sub_type = sub_expr->getType();
  auto type = stmt->IgnoreParenImpCasts()->getType();

  if (sub_expr->getStmtClass() != clang::Stmt::BinaryOperatorClass) return;
//  auto bin_inst = clang::dyn_cast<clang::BinaryOperator>(sub_expr);

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  if (sub_type < type) {
    need_report = true;
  } else if (type < sub_type) {
    if (type->isUnsignedIntegerType() != sub_type->isUnsignedIntegerType()) {
      need_report = true;
    } else if (type->isIntegerType() != sub_type->isIntegerType()) {
      need_report = true;
    }
  }

  if (need_report) {
    issue = report->ReportIssue(MISRA, M_R_10_8, stmt);
    std::string ref_msg = "The value of a composite expression shall not be cast to a "
                          "different essential type category or a wider essential type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 11.4
 * A conversion should not be performed between a pointer to object and an integer type
 */
void MISRAStmtRule::CheckCastBetweenIntAndPointer(const clang::CastExpr *stmt) {
  auto type = stmt->IgnoreParenImpCasts()->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();

  if ((type->isIntegerType() && sub_type->isPointerType()) ||
      (type->isPointerType() && sub_type->isIntegerType())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_4, stmt);
    std::string ref_msg = "A conversion should not be performed between a pointer to object and an integer type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 11.5
 * A conversion should not be performed from pointer to void into pointer to object
 */
void MISRAStmtRule::CheckVoidPointerToOtherTypePointer(const clang::CastExpr *stmt) {
  auto type = stmt->IgnoreParenImpCasts()->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();

  if (type->isPointerType() && sub_type->isVoidPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_5, stmt);
    std::string ref_msg = "A conversion should not be performed from pointer to void into pointer to object";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 11.6
 * A cast shall not be performed between pointer to void and an arithmetic type
 */
void MISRAStmtRule::CheckArithTypeCastToVoidPointerType(const clang::CastExpr *stmt) {
  auto type = stmt->IgnoreParenImpCasts()->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();

  if (sub_type->isIntegerType() && type->isVoidPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_6, stmt);
    std::string ref_msg = "A cast shall not be performed between pointer to void and an arithmetic type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 11.7
 * A cast shall not be performed between pointer to object and a non-integer arithmetic type
 */
void MISRAStmtRule::CheckCastBetweenPointerAndNonIntType(const clang::CastExpr *stmt) {
  auto type = stmt->IgnoreParenImpCasts()->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();

  if ((type->isPointerType() && !sub_type->isIntegerType()) ||
      (!type->isIntegerType() && sub_type->isPointerType())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_7, stmt);
    std::string ref_msg = "A cast shall not be performed between pointer to object and a non-integer arithmetic type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 11.8
 * A cast shall not remove any const or volatile qualification from the type pointed to by a pointer
 */
void MISRAStmtRule::CheckAssignRemoveConstOrVolatile(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;
  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  if (!lhs_type->isPointerType()) return;

  auto rhs = stmt->getRHS();
  if (auto cast_inst = clang::dyn_cast<clang::CastExpr>(rhs)) {
    auto sub_type = cast_inst->getSubExpr()->IgnoreParenImpCasts()->getType();
    if (sub_type->getPointeeType().isConstQualified() || sub_type->getPointeeType().isVolatileQualified()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_11_8, stmt);
      std::string ref_msg = "A cast shall not remove any const or volatile "
                            "qualification from the type pointed to by a pointer";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 11.9
 * The macro NULL shall be the only permitted form of integer null pointer constant
 */
void MISRAStmtRule::CheckZeroAsPointerConstant(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();

  if (!lhs_type->isPointerType() && !rhs_type->isPointerType()) return;

  if (lhs_type->isIntegerType() || rhs_type->isIntegerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_9, stmt);
    std::string ref_msg = "The macro NULL shall be the only permitted form of integer null pointer constant";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 12.2
 * The right hand operand of a shift operator shall lie in the range zero to one less than the
 * width in bits of the essential type of the left hand operand
 */
void MISRAStmtRule::CheckShiftOutOfRange(const clang::BinaryOperator *stmt) {
  if (!stmt->isShiftOp() && !stmt->isShiftAssignOp()) return;
  if ((stmt->getOpcode() != clang::BinaryOperatorKind::BO_Shl) &&
      (stmt->getOpcode() != clang::BinaryOperatorKind::BO_ShlAssign))
    return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  // handle lhs
  while (lhs->getStmtClass() == clang::Stmt::ImplicitCastExprClass) {
    lhs = clang::dyn_cast<clang::ImplicitCastExpr>(lhs)->getSubExpr();
  }

  auto lhs_type = lhs->getType();
  if (rhs->getStmtClass() != clang::Stmt::IntegerLiteralClass) return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto value = clang::dyn_cast<clang::IntegerLiteral>(rhs)->getValue().getZExtValue();
  auto lhs_size = ctx->getTypeSize(lhs_type);
  if (lhs_size <= value) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_2, stmt);
    std::string ref_msg = "The right hand operand of a shift operator shall lie in the range zero to one less than the "
                          "width in bits of the essential type of the left hand operand";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 12.3
 * The comma operator should not be used
 */
void MISRAStmtRule::CheckCommaStmt(const clang::BinaryOperator *stmt) {
  if (stmt->isCommaOp()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_3, stmt);
    std::string ref_msg = "The comma operator should not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 13.4
 * The result of an assignment operator should not be used
 */
bool MISRAStmtRule::IsAssignmentStmt(const clang::Stmt *stmt) {
  if (auto bin_inst = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    if (bin_inst->isCompoundAssignmentOp() || bin_inst->isAssignmentOp()) {
      return true;
    }
  }
  return false;
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::ArraySubscriptExpr *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs->getStmtClass() != clang::Stmt::BinaryOperatorClass) return;
  if (IsAssignmentStmt(rhs)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_4, stmt);
    std::string ref_msg = "The result of an assignment operator should not be used";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::BinaryOperator *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (IsAssignmentStmt(rhs)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_4, stmt);
    std::string ref_msg = "The result of an assignment operator should not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 14.4
 * The controlling expression of an if statement and the controlling expression
 * of an iteration-statement shall have essentially Boolean type
 */
bool MISRAStmtRule::CheckControlStmt(const clang::Expr *stmt) {
  if (!stmt->getType()->isBooleanType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_14_4, stmt);
    std::string ref_msg = "The controlling expression of an if statement and the controlling expression "
                          "of an iteration-statement shall have essentially Boolean type";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRAStmtRule::CheckControlStmt(const clang::IfStmt *stmt) {
  CheckControlStmt(stmt->getCond()->IgnoreParenImpCasts());
}

void MISRAStmtRule::CheckControlStmt(const clang::WhileStmt *stmt) {
  CheckControlStmt(stmt->getCond()->IgnoreParenImpCasts());
}

void MISRAStmtRule::CheckControlStmt(const clang::DoStmt *stmt) {
  CheckControlStmt(stmt->getCond()->IgnoreParenImpCasts());
}

void MISRAStmtRule::CheckControlStmt(const clang::ForStmt *stmt) {
  CheckControlStmt(stmt->getCond()->IgnoreParenImpCasts());
}

/* MISRA
 * Rule: 15.2
 * The goto statement shall jump to a label declared later in the same function
 */
void MISRAStmtRule::CheckGotoBackward(const clang::GotoStmt *stmt) {
  auto loc = stmt->getBeginLoc();
  auto target = stmt->getLabel()->getLocation();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  bool need_report = false;
  if (!src_mgr->isWrittenInSameFile(loc, target)) {
    need_report = true;
  } else {
    if (target < loc) {
      need_report = true;
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_15_2, stmt);
    std::string ref_msg = "The goto statement shall jump to a label declared later in the same function";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 15.3
 * Any label referenced by a goto statement shall be declared in
 * the same block, or in any block enclosing the goto statement
 */
void MISRAStmtRule::CheckLabelNotEncloseWithGoto(const clang::GotoStmt *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto label = stmt->getLabel()->getStmt();
  auto label_parents = ctx->getParents(*label);
  auto label_parent = label_parents[0].get<clang::Stmt>();

  const clang::Stmt *tmp = stmt;
  while (true) {
    if (tmp == nullptr) break;
    auto goto_parents = ctx->getParents(*tmp);
    if (goto_parents.empty()) break;
    tmp = goto_parents[0].get<clang::Stmt>();
    if (tmp == label_parent) return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_15_3, stmt);
  std::string ref_msg = "Any label referenced by a goto statement shall be declared"
                        " in the same block, or in any block enclosing the goto statement";
  issue->SetRefMsg(ref_msg);
}

/* MISRA
 * Rule: 15.4
 * There should be no more than one break or goto statement used to terminate any iteration statement
 */
void MISRAStmtRule::CollectTerminate(const clang::Stmt *stmt) {
  for (const auto &it : stmt->children()) {
    if ((it->getStmtClass() == clang::Stmt::BreakStmtClass) ||
        (it->getStmtClass() == clang::Stmt::GotoStmtClass)) {
      _terminates.insert(it);
      if (_terminates.size() >= 2) return;
    }
    for (const auto &sub_stmt : it->children()) {
      CollectTerminate(sub_stmt);
    }
  }
}

void MISRAStmtRule::CheckMultiTerminate(const clang::Stmt *stmt) {
  if (stmt == nullptr) return;
  CollectTerminate(stmt);
  if (_terminates.size() >= 2) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_15_4, stmt);
    std::string ref_msg = "There should be no more than one break or goto "
                          "statement used to terminate any iteration statement";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : _terminates) issue->AddStmt(it);
  }
}

/* MISRA
 * Rule: 16.5
 * A default label shall appear as either the first or the last switch label of a switch statement
 */
void MISRAStmtRule::CheckDefaultStmtPosition(const clang::SwitchStmt *stmt) {
  auto cases = stmt->getSwitchCaseList();
  if (cases != nullptr) {
    if (cases->getStmtClass() == clang::Stmt::DefaultStmtClass) return;
    while (cases->getNextSwitchCase()) {
      cases = cases->getNextSwitchCase();
    }
    if (cases->getStmtClass() == clang::Stmt::DefaultStmtClass) return;
  }
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_16_5, stmt);
  std::string ref_msg = "A default label shall appear as either the "
                        "first or the last switch label of a switch statement";
  issue->SetRefMsg(ref_msg);
}

/* MISRA
 * Rule: 16.6
 * Every switch statement shall have at least two switch-clauses
 */
void MISRAStmtRule::CheckCaseStmtNum(const clang::SwitchStmt *stmt) {
  auto cases = stmt->getSwitchCaseList();
  if (cases == nullptr || cases->getNextSwitchCase() == nullptr) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_16_6, stmt);
    std::string ref_msg = "Every switch statement shall have at least two switch-clauses";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 17.5
 * The function argument corresponding to a parameter declared to have an
 * array type shall have an appropriate number of elements
 */
void MISRAStmtRule::CheckArrayArgumentSize(const clang::CallExpr *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto decl = stmt->getCalleeDecl()->getAsFunction();

  unsigned int i = 0;
  for (const auto &it : stmt->arguments()) {
    auto arg_type = it->IgnoreParenImpCasts()->getType();
    auto param_decay_type = clang::dyn_cast<clang::DecayedType>(decl->getParamDecl(i)->getType());
    if (!param_decay_type) {
      i++;
      continue;
    }
    auto param_type = param_decay_type->getOriginalType();

    if (!param_type->isConstantArrayType() || !arg_type->isConstantArrayType()) {
      i++;
      continue;
    }
    auto array_arg_type = clang::dyn_cast<clang::ConstantArrayType>(arg_type);
    auto array_param_type = clang::dyn_cast<clang::ConstantArrayType>(param_type);
    if (array_arg_type->getSize() == array_param_type->getSize()) {
      i++;
      continue;
    }

    if (issue == nullptr) {
      issue = report->ReportIssue(MISRA, M_R_17_5, stmt);
      std::string ref_msg = "The function argument corresponding to a parameter declared to have an array type"
                            " shall have an appropriate number of elements";
      issue->SetRefMsg(ref_msg);
    }
    issue->AddDecl(decl->getParamDecl(i));
  }
}

/* MISRA
 * Rule: 17.7
 * The value returned by a function having non-void return type shall be used
 */
void MISRAStmtRule::CheckUnusedCallExprWithoutVoidCast(const clang::CallExpr *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parent = ctx->getParents(*stmt)[0].get<clang::Stmt>();
  if (parent == nullptr) return;
  if (auto block = clang::dyn_cast<clang::CompoundStmt>(parent)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_17_7, stmt);
    std::string ref_msg = "The value returned by a function having non-void return type shall be used";
    issue->SetRefMsg(ref_msg);
  }
}


}
}
