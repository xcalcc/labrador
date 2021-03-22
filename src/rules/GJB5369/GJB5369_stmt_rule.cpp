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

#include "GJB5369_stmt_rule.h"

namespace xsca {
namespace rule {

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

/*
 * GJB5369 4.1.1.4
 * Check multiple consecutive labels.
 */
void GJB5369StmtRule::CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
  if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
    REPORT("GJB5396:4.1.1.4: Multiple consecutive labels: %s and %s\n",
           stmt->getDecl()->getNameAsString().c_str(),
           clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())
               ->getDecl()
               ->getNameAsString()
               .c_str());
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
    REPORT("GJB5396:4.2.1.2: The while-loop must be enclosed in braces: %s\n",
           body_loc.printToString(*src_mgr).c_str());
  }
}

void GJB5369StmtRule::CheckLoopBrace(const clang::ForStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto body_loc = stmt->getBody()->getBeginLoc();
  const char *start = src_mgr->getCharacterData(body_loc);
  if (*start != '{') {
    REPORT("GJB5396:4.2.1.2: The for-loop must be enclosed in braces: %s\n",
           body_loc.printToString(*src_mgr).c_str());
  }
}

/*
 * GJB5369: 4.2.1.3
 * if/else block must be enclosed in braces
 */
void GJB5369StmtRule::CheckIfBrace(const clang::IfStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  const char *start;
  for (const auto &it : stmt->children()) {

    if (clang::dyn_cast<clang::IfStmt>(it) ||
        it == stmt->getCond()) {
      continue;
    }
    auto body_loc = it->getBeginLoc();
    start = src_mgr->getCharacterData(body_loc);
    if (*start != '{') {
      REPORT("GJB5396:4.2.1.3: if/else block must be enclosed in braces: %s\n",
             body_loc.printToString(*src_mgr).c_str());
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
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  if (CheckExprParentheses(lhs)) {
    auto lhs_loc = lhs->getBeginLoc();
    REPORT("GJB5396:4.2.1.4: logic expression should be enclosed in parentheses: %s\n",
           lhs_loc.printToString(*src_mgr).c_str());
  }

  if (CheckExprParentheses(rhs)) {
    auto rhs_loc = rhs->getBeginLoc();
    REPORT("GJB5396:4.2.1.4: logic expression should be enclosed in parentheses: %s\n",
           rhs_loc.printToString(*src_mgr).c_str());
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
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto location = stmt->getBeginLoc();
  if (!_is_single_asm_stmt) {
    REPORT("GJB5396:4.2.1.7: The assemble procedure should be pure assemble:"
           " %s\n",
           location.printToString(*src_mgr).c_str());
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
  auto _else = stmt->getElse();
  if (stmt->hasElseStorage()) {
    if (clang::dyn_cast<clang::NullStmt>(_else)) {
      need_report_else = true;
    } else if (clang::dyn_cast<clang::CompoundStmt>(_else)) {
      if (_else->child_begin() == _else->child_end()) {
        need_report_else = true;
      }
    } else {
      DBG_ASSERT(0, "unknown else body");
    }
  }

  if (need_report_if) {
    auto location = stmt->getBeginLoc();
    REPORT("GJB5396:4.3.1.1: if block should not be empty:"
           " %s\n",
           location.printToString(*src_mgr).c_str());
  }

  if (need_report_else) {
    auto location = stmt->getBeginLoc();
    REPORT("GJB5396:4.3.1.3: else block should not be empty:"
           " %s\n",
           location.printToString(*src_mgr).c_str());
  }
}

/*
 * GJB5369: 4.3.1.2
 * 'else' must be used in the "if...else if" statement
 */
void GJB5369StmtRule::CheckIfWithoutElseStmt(const clang::IfStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  if (!stmt->hasElseStorage()) {
    auto location = stmt->getBeginLoc();
    REPORT("GJB5396:4.3.1.2: 'else' must be used in "
           "the \"if...else if\" statement: %s\n",
           location.printToString(*src_mgr).c_str());
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

  if (!has_default) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();

    REPORT("GJB5396:4.3.1.4: \"default\" statement should be"
           " used in the \"switch\" statement: %s\n",
           location.printToString(*src_mgr).c_str());
  }

  if (!has_other &&
      ((caseList == nullptr) ||
       (caseList->getNextSwitchCase() == nullptr))) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();

    REPORT("GJB5396:4.3.1.6: \"switch\" only containing"
           " \"default\" is forbidden: %s\n",
           location.printToString(*src_mgr).c_str());
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
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();

    REPORT("GJB5396:4.3.1.5: \"switch\" without statement is forbidden: %s\n",
           location.printToString(*src_mgr).c_str());
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
              REPORT("GJB5396:4.3.1.7: \"case\" statement without "
                     "\"break\" is forbidden: %s\n",
                     location.printToString(*src_mgr).c_str());
            }
          }
        } else {
          REPORT("GJB5396:4.3.1.7: \"case\" statement without "
                 "\"break\" is forbidden: %s\n",
                 location.printToString(*src_mgr).c_str());
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
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();

    REPORT("GJB5396:4.3.1.8: The empty \"case\" statement is forbidden: %s\n",
           location.printToString(*src_mgr).c_str());
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
      auto src_mgr = XcalCheckerManager::GetSourceManager();
      auto lhs_loc = lhs->getBeginLoc();
      auto rhs_loc = rhs->getBeginLoc();
      REPORT("GJB5396:4.4.2.1: comparing pointer should be careful: "
             "lhs: %s -> rhs: %s\n",
             lhs_loc.printToString(*src_mgr).c_str(),
             rhs_loc.printToString(*src_mgr).c_str());
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
      auto src_mgr = XcalCheckerManager::GetSourceManager();
      auto lhs_loc = lhs->getBeginLoc();
      auto rhs_loc = rhs->getBeginLoc();
      REPORT("GJB5396:4.4.2.2: using pointer in the algebraic "
             "operation should be careful: "
             "lhs: %s -> rhs: %s\n",
             lhs_loc.printToString(*src_mgr).c_str(),
             rhs_loc.printToString(*src_mgr).c_str());
    }
  }
}

/*
 * GJB5369: 4.5.1.2
 * "goto" statement is forbidden
 */
void GJB5369StmtRule::CheckGotoStmt(const clang::GotoStmt *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto location = stmt->getBeginLoc();
  REPORT("GJB5396:4.5.1.2: \"goto\" statement is forbidden: %s\n",
         location.printToString(*src_mgr).c_str());
}

/*
 * GJB5369: 4.5.2.1
 * setjmp/longjmp is forbidden
 */
void GJB5369StmtRule::CheckSetjumpAndLongjump(const clang::CallExpr *stmt) {
  auto callee = stmt->getCalleeDecl();
  auto func_name = callee->getAsFunction()->getNameAsString();
  if (func_name == "setjmp" || func_name == "longjmp") {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = stmt->getBeginLoc();
    REPORT("GJB5396:4.5.2.1: \"goto\" statement is forbidden: %s\n",
           location.printToString(*src_mgr).c_str());
  }
}

} // rule
} // xsca