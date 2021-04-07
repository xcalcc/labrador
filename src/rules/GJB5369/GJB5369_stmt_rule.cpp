/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_stmt_rule.cpp
// ====================================================================
//
// implement all stmt related rules in GJB5369
//

#include "GJB5369_enum.inc"
#include "GJB5369_stmt_rule.h"

namespace xsca {
namespace rule {

/* Check if addiction is overflowed
 */
bool GJB5369StmtRule::AddOverflowed(int a, int b) {
  if (a > 0 && b > INT_MAX - a) {
    /* handle overflow */
    return true;
  } else if (a < 0 && b < INT_MIN - a) {
    /* handle underflow */
    return true;
  }
  return false;
}

bool GJB5369StmtRule::MulOverflowed(int a, int b) {
  int x = a * b;
  if (a != 0 && x / a != b) {
    return true;
  }
  return false;
}

bool GJB5369StmtRule::CheckExprParentheses(const clang::Expr *expr) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto expr_loc = expr->getBeginLoc();
  const char *start = src_mgr->getCharacterData(expr_loc);
  if (*start != '(') { return true; }
  return false;
}

bool GJB5369StmtRule::IsCaseStmt(const clang::Stmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto stmtClass = stmt->getStmtClass();
  if ((stmtClass == StmtClass::CaseStmtClass) ||
      (stmtClass == StmtClass::DefaultStmtClass)) {
    return true;
  }
  return false;
}

bool GJB5369StmtRule::HasAssignmentSubStmt(const clang::Stmt *stmt) {
  bool has_assignment = false;
  if (auto binary = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    if (binary->isAssignmentOp()) { return true; }
  }

  for (const auto &it : stmt->children()) {
    if (auto binary_stmt = clang::dyn_cast<clang::BinaryOperator>(it)) {
      if (binary_stmt->isAssignmentOp()) {
        return true;
      }
    }
    if (it->child_begin() != it->child_end()) {
      has_assignment |= HasAssignmentSubStmt(it);
    }
  }
  return has_assignment;
}

bool GJB5369StmtRule::HasBitwiseSubStmt(const clang::Stmt *stmt) {
  bool has_bitwise = false;
  if (auto binary = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    if (binary->isBitwiseOp()) { return true; }
  }

  for (const auto &it : stmt->children()) {
    if (auto binary_stmt = clang::dyn_cast<clang::BinaryOperator>(it)) {
      if (binary_stmt->isBitwiseOp()) {
        return true;
      }
    }
    if (it->child_begin() != it->child_end()) {
      has_bitwise |= HasBitwiseSubStmt(it);
    }
  }
  return has_bitwise;
}

// check if the statement has function call expr
bool GJB5369StmtRule::HasCallExpr(const clang::Stmt *stmt) {
  bool has_callexpr = false;
  using StmtClass = clang::Stmt::StmtClass;
  if (stmt->getStmtClass() == StmtClass::CallExprClass) return true;
  for (const auto &it : stmt->children()) {
    if (it->getStmtClass() == StmtClass::CallExprClass) return true;
    if (it->child_begin() != it->child_end()) {
      has_callexpr |= HasCallExpr(it);
      if (has_callexpr) return has_callexpr;
    }
  }
  return has_callexpr;
}

/*
 * GJB5369 4.1.1.4
 * Check multiple consecutive labels.
 */
void GJB5369StmtRule::CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
  if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_1_1_4, stmt);
    std::string ref_msg = "Multiple consecutive labels: ";
    ref_msg += stmt->getDecl()->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.2.1.2
 * the loop must be enclosed in braces
 */
void GJB5369StmtRule::CheckLoopBrace(const clang::WhileStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto body_loc = stmt->getBody()->getBeginLoc();
  const char *start = src_mgr->getCharacterData(body_loc);
  if (*start != '{') {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_2_1_2, stmt);
    std::string ref_msg = "The while-loop must be enclosed in braces";
    issue->SetRefMsg(ref_msg);
  }
}

void GJB5369StmtRule::CheckLoopBrace(const clang::ForStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto body_loc = stmt->getBody()->getBeginLoc();
  const char *start = src_mgr->getCharacterData(body_loc);
  if (*start != '{') {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_2_1_2, stmt);
    std::string ref_msg = "The for-loop must be enclosed in braces";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.2.1.3
 * if/else block must be enclosed in braces
 */
void GJB5369StmtRule::CheckIfBrace(const clang::IfStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  const char *start;
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  for (const auto &it : stmt->children()) {
    if (clang::dyn_cast<clang::IfStmt>(it) ||
        it == stmt->getCond()) {
      continue;
    }
    auto body_loc = it->getBeginLoc();
    start = src_mgr->getCharacterData(body_loc);
    if (*start != '{') {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB5369, G5_4_2_1_3, stmt);
        std::string ref_msg = "if/else block must be enclosed in braces";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddStmt(&(*it));
    }
  }
}

/*
 * GJB5369: 4.2.1.4
 * logic expression should be enclosed in parentheses
 */
void GJB5369StmtRule::CheckLogicExprParen(const clang::BinaryOperator *stmt) {
  if (!stmt->isLogicalOp()) return;
  auto lhs = stmt->getLHS();
  auto rhs = stmt->getRHS();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (CheckExprParentheses(lhs)) {
    issue = report->ReportIssue(GJB5369, G5_4_2_1_4, stmt);
    std::string ref_msg = "Logic expression should be enclosed in parentheses";
    issue->SetRefMsg(ref_msg);
    issue->AddStmt(lhs);
  }

  if (CheckExprParentheses(rhs)) {
    if (issue == nullptr) {
      issue = report->ReportIssue(GJB5369, G5_4_2_1_4, stmt);
      std::string ref_msg = "Logic expression should be enclosed in parentheses";
      issue->SetRefMsg(ref_msg);
    }
    issue->AddStmt(rhs);
  }
}

/*
 * GJB5369: 4.2.1.7
 * the assemble procedure should be pure assemble
 */
void GJB5369StmtRule::CheckAsmInProcedure(const clang::Stmt *stmt) {
  int stmt_num = 0;
  for (const auto &it : stmt->children()) {
    stmt_num++;
    if (it->getStmtClass() == clang::Stmt::StmtClass::GCCAsmStmtClass) {
      if (stmt_num > 1) {
        _is_single_asm_stmt = false;
        return;
      }
    } else {
      _is_single_asm_stmt = false;
      return;
    }
  }
}

void GJB5369StmtRule::CheckAsmInProcedure(const clang::GCCAsmStmt *stmt) {
  if (!_is_single_asm_stmt) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_2_1_7, stmt);
    std::string ref_msg = "The assemble procedure should be pure assemble";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.3.1.1
 * non-statement is forbidden as the conditional
 * judgement is true:
 * 1. if (...) else
 * 2. if (...) {} else
 * 3. if (...) {;} else
 *
 * GJB5369: 4.3.1.3
 * else block should not be empty
 * 1. else;
 * 2. else {}
 * 3. else {;}
 */
void GJB5369StmtRule::CheckEmptyIfElseStmt(const clang::IfStmt *stmt) {
  bool need_report_if = false, need_report_else = false;
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  // check if-blcok
  auto _then = stmt->getThen();
  if (clang::dyn_cast<clang::NullStmt>(_then)) {
    need_report_if = true;
  } else if (clang::dyn_cast<clang::CompoundStmt>(_then)) {
    if (_then->child_begin() == _then->child_end()) {
      need_report_if = true;
    }
  } else {
    DBG_ASSERT(0, "unknown if body");
  }

  // check else-block
  /*
   * 1. else is NullStmt
   * 2. else is CompoundStmt which without child
   * 3. else is CompoundStmt which with only NullStmt
   */
  auto _else = stmt->getElse();
  if (stmt->hasElseStorage()) {
    if (clang::dyn_cast<clang::NullStmt>(_else)) {
      need_report_else = true;
    } else if (clang::dyn_cast<clang::CompoundStmt>(_else)) {
      if (_else->child_begin() == _else->child_end()) {
        need_report_else = true;
      } else {
        for (const auto &it : _else->children()) {
          if (!clang::dyn_cast<clang::NullStmt>(it)) {
            need_report_else = true;
            break;
          }
        }
        need_report_else = true;
      }
    } else {
      need_report_else = false;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (need_report_if) {
    issue = report->ReportIssue(GJB5369, G5_4_3_1_3, stmt);
    std::string ref_msg = "If block should not be empty";
    issue->SetRefMsg(ref_msg);
  }

  if (need_report_else) {
    issue = report->ReportIssue(GJB5369, G5_4_3_1_3, stmt);
    std::string ref_msg = "Else block should not be empty";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.3.1.2
 * 'else' must be used in the "if...else if" statement
 */
void GJB5369StmtRule::CheckIfWithoutElseStmt(const clang::IfStmt *stmt) {
  if (!stmt->hasElseStorage()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_3_1_2, stmt);
    std::string ref_msg = "'else' must be used in the \"if...else if\" statement";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.3.1.4
 * "default" statement should be used in the "switch" statement
 *
 * GJB5369: 4.3.1.6
 * "switch" only containing "default" is forbidden
 */
void GJB5369StmtRule::CheckSwitchWithoutDefaultStmt(const clang::SwitchStmt *stmt) {
  if (CheckEmptySwitch(stmt)) return;

  bool has_default = false, has_other = false;
  auto caseList = stmt->getSwitchCaseList();
  if (caseList != nullptr) {
    do {
      if (caseList->getStmtClass() ==
          clang::Stmt::StmtClass::DefaultStmtClass) {
        has_default = true;
        break;
      } else {
        has_other = true;
      }
    } while ((caseList = caseList->getNextSwitchCase()) != nullptr);
  }


  XcalIssue *no_default_issue = nullptr, *no_case_issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  if (!has_default) {
    no_default_issue = report->ReportIssue(GJB5369, G5_4_3_1_4, stmt);
    std::string ref_msg = R"("default" statement should be used in the "switch" statement)";
    no_default_issue->SetRefMsg(ref_msg);
  }

  if (!has_other &&
      ((caseList == nullptr) || (caseList->getNextSwitchCase() == nullptr))) {
    no_case_issue = report->ReportIssue(GJB5369, G5_4_3_1_6, stmt);
    std::string ref_msg = R"("switch" only containing "default" is forbidden)";
    no_case_issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.3.1.5
 * "switch" without statement is forbidden
 */
bool GJB5369StmtRule::CheckEmptySwitch(const clang::SwitchStmt *stmt) {
  bool need_report = false;

  auto switch_body = stmt->getBody();
  if (clang::dyn_cast<clang::NullStmt>(switch_body)) {
    need_report = true;
  } else if (clang::dyn_cast<clang::CompoundStmt>(switch_body)) {
    if (switch_body->child_begin() == switch_body->child_end()) {
      need_report = true;
    }
  } else {
    DBG_ASSERT(0, "unknown switch body");
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G5_4_3_1_5, stmt);
    std::string ref_msg = "\"switch\" without statement is forbidden";
    issue->SetRefMsg(ref_msg);
    return true;
  }
  return false;
}

/*
 * GJB5369: 4.3.1.7
 * "case" statement without "break" is forbidden
 */
void GJB5369StmtRule::CheckCaseEndWithBreak(const clang::SwitchStmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto switch_body = stmt->getBody();
  if (switch_body != nullptr) {

    auto it = switch_body->child_begin();
    auto case_end = switch_body->child_end();
    clang::SourceLocation location;

    for (; it != case_end; it++) {
      if (IsCaseStmt(*it)) {
        CheckEmptyCaseStmt(clang::dyn_cast<clang::SwitchCase>(*it));

        location = it->getBeginLoc();

        auto next = it;
        next++;

        /*
         * 1. case: ...; break;
         * 2. case: { ...; break; }
         */
        if (next != case_end) {
          // case: ...; break;
          if (next->getStmtClass() == StmtClass::BreakStmtClass) {
            continue;
          } else {

            bool need_report = true;
            auto sub_stmt = clang::dyn_cast<clang::SwitchCase>(*it)->getSubStmt();

            // case: { ...; break; }
            if (clang::dyn_cast<clang::CompoundStmt>(sub_stmt)) {
              for (const auto &sub_it : sub_stmt->children()) {
                if (sub_it->getStmtClass() == StmtClass::BreakStmtClass) {
                  need_report = false;
                  break;
                }
              }
            }

            if (need_report) {
              if (issue == nullptr) {
                issue = report->ReportIssue(GJB5369, G5_4_3_1_7, stmt);
                std::string ref_msg = R"("case" statement without "break" is forbidden)";
                issue->SetRefMsg(ref_msg);
              }
              issue->AddStmt(*it);
            }
          }
        } else {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB5369, G5_4_3_1_7, stmt);
            std::string ref_msg = R"("case" statement without "break" is forbidden)";
            issue->SetRefMsg(ref_msg);
          }
          issue->AddStmt(*it);
          break;
        }

      }
    }
  }
}

/*
 * GJB5369: 4.3.1.8
 * the empty "case" statement is forbidden
 */
void GJB5369StmtRule::CheckEmptyCaseStmt(const clang::SwitchCase *stmt) {
  auto sub_stmt = stmt->getSubStmt();
  if (IsCaseStmt(sub_stmt) || clang::dyn_cast<clang::NullStmt>(sub_stmt)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G5_4_3_1_8, stmt);
    std::string ref_msg = "The empty \"case\" statement is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.4.2.1
 * comparing two pointer should be careful
 */
void GJB5369StmtRule::CheckPointerCompareStmt(const clang::BinaryOperator *stmt) {
  if (stmt->isComparisonOp()) {
    auto lhs = stmt->getLHS();
    auto rhs = stmt->getRHS();
    if (lhs->getType()->isPointerType() || rhs->getType()->isPointerType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB5369, G5_4_4_2_1, stmt);
      std::string ref_msg = "Comparing pointer should be careful";
      issue->SetRefMsg(ref_msg);
      issue->AddStmt(lhs);
      issue->AddStmt(rhs);
    }
  }
}

/*
 * GJB5369: 4.4.2.2
 * using pointer in the algebraic operation should be careful
 */
void GJB5369StmtRule::CheckPointerCalculateStmt(const clang::BinaryOperator *stmt) {
  if (stmt->isAdditiveOp() || stmt->isLogicalOp() || stmt->isBitwiseOp()) {
    auto lhs = stmt->getLHS();
    auto rhs = stmt->getRHS();
    if (lhs->getType()->isPointerType() || rhs->getType()->isPointerType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB5369, G5_4_4_2_1, stmt);
      std::string ref_msg = "Using pointer in the algebraic operation should be careful";
      issue->SetRefMsg(ref_msg);
      issue->AddStmt(lhs);
      issue->AddStmt(rhs);
    }
  }
}

/*
 * GJB5369: 4.5.1.2
 * "goto" statement is forbidden
 */
void GJB5369StmtRule::CheckGotoStmt(const clang::GotoStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB5369, G5_4_5_1_2, stmt);
  std::string ref_msg = "\"goto\" statement is forbidden";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB5369: 4.5.2.1
 * setjmp/longjmp is forbidden
 */
void GJB5369StmtRule::CheckSetjumpAndLongjump(const clang::CallExpr *stmt) {
  auto callee = stmt->getCalleeDecl();
  auto func_decl = callee->getAsFunction();

  if (func_decl == nullptr) return;
  auto func_name = func_decl->getNameAsString();

  if (func_name == "setjmp" || func_name == "longjmp") {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB5369, G5_4_5_2_1, stmt);
    std::string ref_msg = "setjmp/longjmp is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.1.8
 * The value assigned to a variable should be the same type
 * as the variable
 */
void GJB5369StmtRule::CheckDifferentTypeAssign(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS();
  auto rhs = stmt->getRHS()->IgnoreImpCasts();
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();

  if (lhs_type->isBuiltinType() && rhs_type->isBuiltinType()) {
    auto lhs_kind = clang::dyn_cast<clang::BuiltinType>(lhs_type)->getKind();
    auto rhs_kind = clang::dyn_cast<clang::BuiltinType>(rhs_type)->getKind();
    if (lhs_kind != rhs_kind) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_1_8, stmt);
      std::string ref_msg = "The value assigned to a variable should be the same type";
      issue->SetRefMsg(ref_msg);
    }
  }
}


/*
 * GJB5369: 4.6.1.11
 * logic non on const value is forbidden
 */
void GJB5369StmtRule::CheckNonOperationOnConstant(const clang::UnaryOperator *stmt) {
  if (stmt->getOpcode() == clang::UnaryOperator::Opcode::UO_LNot) {
    auto sub = clang::dyn_cast<clang::ImplicitCastExpr>(stmt->getSubExpr());

    if (sub && (sub->getSubExpr()->getStmtClass() == clang::Stmt::StmtClass::IntegerLiteralClass)) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_1_8, stmt);
      std::string ref_msg = "Logic non on const value is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.6.1.12
 * bit-wise operation on signed-int is forbidden
 */
void GJB5369StmtRule::CheckBitwiseOperationOnSignedValue(const clang::BinaryOperator *stmt) {
  if (stmt->isBitwiseOp()) {
    auto lhs = stmt->getLHS();
    if (lhs->getType()->isSignedIntegerType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_1_12, stmt);
      std::string ref_msg = "Bit-wise operation on signed-int is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.6.1.13
 * using enumeration types beyond the limit if forbidden
 */
void GJB5369StmtRule::CheckEnumBeyondLimit(const clang::BinaryOperator *stmt) {
  auto lhs_type = stmt->getLHS()->getType();
  auto rhs_type = stmt->getRHS()->getType();
  if (!lhs_type->isEnumeralType() && !rhs_type->isEnumeralType()) {
    auto cast_stmt = clang::dyn_cast<clang::ImplicitCastExpr>(stmt->getRHS());
    if (!cast_stmt || !cast_stmt->getSubExpr()->getType()->isEnumeralType()) {
      return;
    }
  }

  if (!stmt->isComparisonOp()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_1_13, stmt);
    std::string ref_msg = "Using enumeration types beyond the limit if forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.1.14
 * overflow should be avoided
 */
void GJB5369StmtRule::CheckArithmOverflow(const clang::BinaryOperator *stmt) {
  using Opcode = clang::BinaryOperator::Opcode;
  if (!stmt->isAdditiveOp() && !stmt->isMultiplicativeOp()) return;
  auto lhs = clang::dyn_cast<clang::IntegerLiteral>(stmt->getLHS()->IgnoreImpCasts());
  auto rhs = clang::dyn_cast<clang::IntegerLiteral>(stmt->getRHS()->IgnoreImpCasts());
  if (lhs == nullptr || rhs == nullptr)
    return;

  int lhs_value = lhs->getValue().getZExtValue();
  int rhs_value = rhs->getValue().getZExtValue();

  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto location = stmt->getBeginLoc();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  switch (stmt->getOpcode()) {
    case Opcode::BO_Add: {
      if (AddOverflowed(lhs_value, rhs_value)) {
        issue = report->ReportIssue(GJB5369, G5_4_6_1_14, stmt);
        std::string ref_msg = "Overflow should be avoided";
        issue->SetRefMsg(ref_msg);
      }
      break;
    }
    case Opcode::BO_Sub: {
      // TODO: sub overflow -> Overflowed(a, -b);
      // TODO: but the rhs is UnaryOperator which need further processing
      break;
    }
    case Opcode::BO_Mul: {
      if (MulOverflowed(lhs_value, rhs_value)) {
        issue = report->ReportIssue(GJB5369, G5_4_6_1_14, stmt);
        std::string ref_msg = "Overflow should be avoided";
        issue->SetRefMsg(ref_msg);
      }
    }
    default:
      return;
  }
}

/*
 * GJB5369: 4.6.1.15
 * '=' used in logical expression is forbidden
 */
void GJB5369StmtRule::CheckAssignInLogicExpr(const clang::IfStmt *stmt) {
  auto cond = stmt->getCond()->IgnoreImpCasts();
  if (auto binary_stmt = clang::dyn_cast<clang::BinaryOperator>(cond)) {
    if (binary_stmt->isAssignmentOp()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_1_15, stmt);
      std::string ref_msg = "'=' used in logical expression is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.6.1.16
 * "&&" or "||" used with "=" is forbidden
 * TODO: the sub stmt will be reported twice, maybe I can change its param to IfStmt and check cond-stmt
 */
void GJB5369StmtRule::CheckLogicalOpFollowedByAssign(const clang::BinaryOperator *stmt) {
  if (!stmt->isLogicalOp()) return;
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  bool has_assignment = HasAssignmentSubStmt(rhs);
  if (has_assignment) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_1_16, stmt);
    std::string ref_msg = R"("&&" or "||" used with "=" is forbidden)";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.1.17
 * bit-wise operation on bool is forbidden
 */
void GJB5369StmtRule::CheckBitwiseOpOnBool(const clang::BinaryOperator *stmt) {
  if (!stmt->isBitwiseOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (lhs->getType()->isBooleanType() || rhs->getType()->isBooleanType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_1_17, stmt);
    std::string ref_msg = "Bit-wise operation on bool is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.1.18
 * bit-wise operation is forbidden in the boolean expression
 */
void GJB5369StmtRule::CheckBitwiseOpInBooleanExpr(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  // return if the lhs is boolean type
  if (!lhs->getType()->isBooleanType()) return;
  if (HasBitwiseSubStmt(rhs)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_1_18, stmt);
    std::string ref_msg = "Bit-wise operation is forbidden in the boolean expression";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.2.1
 * avoid using ',' operator
 */

void GJB5369StmtRule::CheckCommaStmt(const clang::BinaryOperator *stmt) {
  if (stmt->isCommaOp()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_2_1, stmt);
    std::string ref_msg = "Avoid using ',' operator";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.2.2
 * "sizeof()" should be used carefully
 * TODO: collect clang's warning
 */
void GJB5369StmtRule::CheckSizeofOnExpr(const clang::UnaryExprOrTypeTraitExpr *stmt) {
  if (stmt->getKind() != clang::UnaryExprOrTypeTrait::UETT_SizeOf) { return; }
  auto ctx = XcalCheckerManager::GetAstContext();
  if (stmt->getArgumentExpr()->HasSideEffects(*ctx, false)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_6_2_2, stmt);
    std::string ref_msg = "\"sizeof()\" should be used carefully";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.6.2.3
 * different types of variable mixed operation should be carefully
 * TODO: check user defined class type
 */
void GJB5369StmtRule::CheckDifferentTypeArithm(const clang::BinaryOperator *stmt) {
  if (!stmt->isAdditiveOp() && !stmt->isMultiplicativeOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();


  if (lhs_type->isBuiltinType() && rhs_type->isBuiltinType()) {
    auto lhs_kind = clang::dyn_cast<clang::BuiltinType>(lhs_type)->getKind();
    auto rhs_kind = clang::dyn_cast<clang::BuiltinType>(rhs_type)->getKind();
    if (lhs_kind != rhs_kind) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_2_3, stmt);
      std::string ref_msg = "Different types of variable mixed operation should be carefully";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.6.2.4
 * dead code is forbidden
 */
void GJB5369StmtRule::CheckFalseIfContidion(const clang::IfStmt *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto cond = stmt->getCond()->IgnoreParenImpCasts();
  if (auto literial = clang::dyn_cast<clang::IntegerLiteral>(cond)) {
    int value;
    clang::Expr::EvalResult eval_result;

    // try to fold the const expr
    if (literial->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = literial->getValue().getZExtValue();
    }

    if (value == 0) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_6_2_4, stmt);
      std::string ref_msg = "Dead code is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.7.1.6
 * Only one function call could be contain within one single statement
 */
void GJB5369StmtRule::CheckMultiCallExprInSingleStmt(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (HasCallExpr(rhs) && HasCallExpr(lhs)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_7_1_6, stmt);
    std::string ref_msg = "Only one function call could be contain within one single statement";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.7.1.7
 * function return void used in statement is forbidden
 * TODO: Need to collect clang's error report
 */
void GJB5369StmtRule::CheckVoidReturnType(const clang::CallExpr *stmt) {
#if 0
  auto func_decl = stmt->getCalleeDecl()->getAsFunction();
  auto ret_type = func_decl->getReturnType();
  if (ret_type->isVoidType()) {
    auto location = stmt->getBeginLoc();
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    REPORT("GJB5396:4.7.1.7: function return void used in statement is forbidden: "
           ": %s -> %s\n",
           func_decl->getNameAsString().c_str(),
           location.printToString(*src_mgr).c_str());
  }
#endif
}

/*
 * GJB5369: 4.7.1.9
 * formal and real parameters' type should be the same
 */
void GJB5369StmtRule::CheckParamTypeMismatch(const clang::CallExpr *stmt) {
  if (!stmt->getNumArgs()) return;

  int param_index = 0;
  auto func_decl = stmt->getCalleeDecl()->getAsFunction();
  if (func_decl == nullptr) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : func_decl->parameters()) {
    auto formal_param_type = it->getType();
    if (formal_param_type->isBuiltinType()) {
      auto real_param_type = stmt->getArg(param_index)->IgnoreParenImpCasts()->getType();
      auto real_param_kind = clang::dyn_cast<clang::BuiltinType>(real_param_type)->getKind();
      auto formal_param_kind = clang::dyn_cast<clang::BuiltinType>(formal_param_type)->getKind();
      if (real_param_kind != formal_param_kind) {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB5369, G5_4_7_1_9, stmt);
          std::string ref_msg = "Formal and real parameters' type should be the same";
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(&(*(it)));
      }
    }
    param_index++;
    if (param_index >= stmt->getNumArgs()) break;
  }
}

/*
 * GJB5369: 4.7.2.2
 * using function not by calling is forbidden
 */
void GJB5369StmtRule::CheckUsingFunctionNotByCalling(const clang::IfStmt *stmt) {
  auto cond = stmt->getCond()->IgnoreParenImpCasts();
  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(cond)) {
    auto decl = decl_ref->getDecl();
    if (decl->isFunctionOrFunctionTemplate()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB5369, G5_4_7_2_2, stmt);
      std::string ref_msg = "Using function not by calling is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB5369: 4.7.2.3
 * use abort/exit carefully
 */
void GJB5369StmtRule::CheckExitAndAbortFunction(const clang::CallExpr *stmt) {
  const clang::Decl *calleeDecl;
  const clang::FunctionDecl *decl;
  if ((calleeDecl = stmt->getCalleeDecl()) == nullptr) return;
  if ((decl = calleeDecl->getAsFunction()) == nullptr) return;

  auto conf_mgr = XcalCheckerManager::GetConfigureManager();

  auto func_name = decl->getNameAsString();
  if (conf_mgr->IsDangerFunction(func_name)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_7_2_3, stmt);
    std::string ref_msg = "use abort/exit carefully";
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(decl);
  }
}

/*
 * GJB5369: 4.8.2.1
 * avoid using += or -=
 */
void GJB5369StmtRule::CheckCompoundAssignOperator(const clang::CompoundAssignOperator *stmt) {
  if (stmt->getOpcode() == clang::BinaryOperatorKind::BO_AddAssign ||
      stmt->getOpcode() == clang::BinaryOperatorKind::BO_SubAssign) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_8_2_1, stmt);
    std::string ref_msg = "avoid using += or -=";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.8.2.2
 * using ++ or -- should be carefully
 */
void GJB5369StmtRule::CheckPreIncrementAndPostIncrement(const clang::UnaryOperator *stmt) {
  if (stmt->isPostfix() || stmt->isPrefix()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB5369, G5_4_8_2_2, stmt);
    std::string ref_msg = "Using ++ or -- should be carefully";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB5369: 4.8.2.3
 * avoid using continue statement
 */
void GJB5369StmtRule::CheckContinueStmt(const clang::ContinueStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(GJB5369, G5_4_8_2_3, stmt);
  std::string ref_msg = "Avoid using continue statement";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB5369: 4.8.2.4
 * Binocular operation should be using carefully
 */
void GJB5369StmtRule::CheckBinocularOper(const clang::ConditionalOperator *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(GJB5369, G5_4_8_2_4, stmt);
  std::string ref_msg = "Binocular operation should be using carefully";
  issue->SetRefMsg(ref_msg);
}

} // rule
} // xsca