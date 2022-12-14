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

// Check if stmt contains bitwise operator
// used by CheckBitwiseOpInLogicStmt
bool GJB8114StmtRule::HasBitwiseSubStmt(const clang::Stmt *stmt) {
  bool has_assignment = false;
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
      has_assignment |= HasBitwiseSubStmt(it);
    }
  }
  return has_assignment;
}

// check if this statement is single statement
bool IsSingleStmt(const clang::Stmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.size() == 0)
    return true;
  auto parent = parents[0].get<clang::Stmt>();

  if (parent == nullptr) {
    auto parent = parents[0].get<clang::Decl>();
    if (clang::dyn_cast<clang::FunctionDecl>(parent)) {
      return true;
    }
    return false;
  };

  auto stmtClass = parent->getStmtClass();
  if ((stmtClass != StmtClass::CompoundStmtClass) &&
      (stmtClass != StmtClass::IfStmtClass) &&
      (stmtClass != StmtClass::ForStmtClass) &&
      (stmtClass != StmtClass::WhileStmtClass) &&
      (stmtClass != StmtClass::DoStmtClass)) {

    if (stmtClass == StmtClass::ParenExprClass) {
      return IsSingleStmt(parent);
    }
    return false;
  }
  return true;
}

// get parent statement
const clang::Stmt *GetParentStmt(const clang::Stmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  auto parent = parents[0].get<clang::Stmt>();

  if (parent == nullptr) return nullptr;
  auto stmtClass = parent->getStmtClass();

  if (stmtClass == StmtClass::ParenExprClass) {
    return GetParentStmt(parent);
  }
  return parent;
}

// check if this node is in cpp or hpp file
bool GJB8114StmtRule::IsInCPPFile(clang::SourceLocation location) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto langOpts = ctx->getLangOpts();
  if (bool(langOpts.CPlusPlus) == true) return true;
  return false;
}

const clang::FunctionDecl *GJB8114StmtRule::GetCalleeDecl(const clang::CallExpr *stmt) {
  auto callee = stmt->getCalleeDecl();
  if (callee == nullptr) return nullptr;
  auto decl = callee->getAsFunction();
  return decl;
}


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
 * GJB8114: 5.4.1.8
 * Cases of switch should have the same hierarchy range
 */
void GJB8114StmtRule::CheckDifferentHierarchySwitchCase(const clang::SwitchStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto switchCase = stmt->getSwitchCaseList();
  auto ctx = XcalCheckerManager::GetAstContext();
  if (switchCase != nullptr && ctx->getParents(*switchCase).size() > 0) {
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

/*
 * GJB8114: 5.4.2.1
 * Branches nested more than 7 level is forbidden
 */
void GJB8114StmtRule::CheckBranchNestedTooMuch(const clang::IfStmt *stmt) {
  int i = 1;
  XcalIssue *issue = nullptr;
  auto thenBlock = stmt->getThen();
  auto ctx = XcalCheckerManager::GetAstContext();
  XcalReport *report = XcalCheckerManager::GetReport();

  // Record all IfStmt predecessors, break when meet function decl.
  if (thenBlock != nullptr) {
    auto parents = ctx->getParents(*thenBlock);
    if (parents.size() == 0)
      return;

    const clang::Decl *parentDecl;
    const clang::Stmt *parentStmt;
    do {
      parentDecl = parents[0].get<clang::Decl>();
      if (parentDecl == nullptr) {
        parentStmt = parents[0].get<clang::Stmt>();
        if (parentStmt != nullptr) {
          parents = ctx->getParents(*parentStmt);
          if (clang::dyn_cast<clang::IfStmt>(parentStmt)) i++;
        }
      }

      // break if it nested too deeply
      if (i > 7) {
        break;
      }

    } while (parentDecl == nullptr);

    if (i > 7) {
      issue = report->ReportIssue(GJB8114, G5_4_2_1, stmt);
      std::string ref_msg = "Branches nested more than 7 level is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 5.6.1.4
 * Bitwise operator within logic statement is forbidden
 */
void GJB8114StmtRule::CheckBitwiseOpInLogicStmt(const clang::IfStmt *stmt) {
  if (HasBitwiseSubStmt(stmt->getCond())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_6_1_4, stmt);
    std::string ref_msg = "Bitwise operator within logic statement is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.6.1.5
 * Using ++ or -- in arithmetic statement or function parameters is forbidden
 */
void GJB8114StmtRule::CheckIncOrDecUnaryInStmt(const clang::UnaryOperator *stmt) {

  if (!stmt->isPostfix() && !stmt->isPrefix()) return;

  if (!IsSingleStmt(stmt)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_6_1_5, stmt);
    std::string ref_msg = "Using ++ or -- in arithmetic statement or function parameters is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}


/*
 * GJB8114: 5.6.1.10
 * Performing logic-not on ingeter literal is forbidden
 */
void GJB8114StmtRule::CheckNotOperatorOnConstant(const clang::UnaryOperator *stmt) {
  if (stmt->getOpcode() != clang::UnaryOperatorKind::UO_LNot) return;
  auto substmt = stmt->getSubExpr();
  if (substmt->getStmtClass() == clang::Stmt::StmtClass::IntegerLiteralClass) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_6_1_10, stmt);
    std::string ref_msg = "Performing logic-not on ingeter literal is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.6.1.11
 * Enum value used by non-enum variable is forbidden
 */
void GJB8114StmtRule::CheckUsingEnumByOtherTypeVar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp() && !stmt->isCompoundAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (lhs->getType()->isEnumeralType()) return;
  if (auto declRef = clang::dyn_cast<clang::DeclRefExpr>(rhs)) {
    auto decl = declRef->getDecl();
    if (clang::dyn_cast<clang::EnumConstantDecl>(decl)) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB8114, G5_6_1_11, stmt);
      std::string ref_msg = "Enum value used by non-enum variable is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 5.6.1.18
 * Using gets function is forbidden
 * TODO: need to refine
 */
void GJB8114StmtRule::CheckUsingGetsFunction(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  auto funcName = callee->getNameAsString();
  if (conf_mgr->IsDangerFunction(funcName)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_6_1_18, stmt);
    std::string ref_msg = "Using gets function is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.6.2.2
 * Be careful with the division of integer and integer
 */
void GJB8114StmtRule::CheckIntegerDivision(const clang::BinaryOperator *stmt) {
  if (stmt->getOpcode() != clang::BinaryOperatorKind::BO_Div) return;
  auto lhsType = stmt->getLHS()->getType();
  auto rhsType = stmt->getRHS()->getType();
  if (lhsType->isIntegerType() && rhsType->isIntegerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_6_1_18, stmt);
    std::string ref_msg = "Be careful with the division of integer and integer";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.6.2.4
 * Avoid using strcpy function
 */
void GJB8114StmtRule::CheckUsingStrcpy(const clang::CallExpr *stmt) {
  // TODO: need to refine
}

/*
 * GJB8114: 5.6.2.5
 * Avoid using strcat function
 */
void GJB8114StmtRule::CheckUsingStrcat(const clang::CallExpr *stmt) {
  // TODO: need to refine
}

/*
 * GJB8114: 5.7.1.11
 * void is required as the function which has return value is called but the return value is not used
 */
void GJB8114StmtRule::CheckUnusedFunctionCast(const clang::CallExpr *stmt) {
  auto decl = GetCalleeDecl(stmt);
  if (decl == nullptr) return;
  auto ret_type = decl->getReturnType();

  if (ret_type->isVoidType()) return;

  bool need_report = false;
  if (IsSingleStmt(stmt)) {
    need_report = true;
  } else {
    auto parent = GetParentStmt(stmt);

    // Return null if parent is declaration. This means it has been used as init value
    if (parent == nullptr)
      return;

    if (auto cast = clang::dyn_cast<clang::CStyleCastExpr>(parent)) {
      if (!cast->getType()->isVoidType()) {
        TRACE0();
        need_report = true;
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_7_1_11, stmt);
    std::string ref_msg = "void is required as the function which has return value is called but the return value is not used";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.7.1.12
 * Void is not required as the function which is void type is called
 */
void GJB8114StmtRule::CheckNotRequiredFunctionCast(const clang::CallExpr *stmt) {
  if (!stmt->getType()->isVoidType())
    return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.size() == 0)
    return;
  auto parent = parents[0].get<clang::Stmt>();
  if (parent == nullptr)
    return;

  if (auto castStmt = clang::dyn_cast<clang::CStyleCastExpr>(parent)) {
    if (!castStmt->getType()->isVoidType())
      return;
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_7_1_11, stmt);
    std::string ref_msg = "void is required as the function which has return value is called but the return value is not used";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.8.1.5
 * Suffix of number must use upper case letters
 *
 * GJB8114: 5.8.2.4
 * Using suffix with number is recommended
 */
void GJB8114StmtRule::CheckLiteralSuffix(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (auto float_literal = clang::dyn_cast<clang::FloatingLiteral>(lhs)) {
    CheckLiteralSuffix(float_literal);
  } else if (auto int_literal = clang::dyn_cast<clang::IntegerLiteral>(lhs)) {
    CheckLiteralSuffix(int_literal);
  }

  if (auto float_literal = clang::dyn_cast<clang::FloatingLiteral>(rhs)) {
    CheckLiteralSuffix(float_literal);
  } else if (auto int_literal = clang::dyn_cast<clang::IntegerLiteral>(rhs)) {
    CheckLiteralSuffix(int_literal);
  }
}

/*
 * GJB8114: 5.9.1.4
 * Infinite loop must use while(1)
 */
void GJB8114StmtRule::CheckInfiniteForLoop(const clang::ForStmt *stmt) {
  auto cond = stmt->getCond();
  if (cond == nullptr) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_9_1_4, stmt);
    std::string ref_msg = "Infinite loop must use while(1)";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.10.1.1
 * Explicit cast is required when assigning float value to int variable
 * GJB8114: 5.10.2.1
 * Rounding need to be taken in account when convert float to integer
 */
void GJB8114StmtRule::CheckFloatAssignToInt(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS();
  auto rhs = stmt->getRHS()->IgnoreImpCasts();
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();

  if (lhs_type->isBuiltinType() && rhs_type->isBuiltinType()) {
    if (lhs_type->isIntegerType() && rhs_type->isFloatingType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB8114, G5_10_1_1, stmt);
      std::string ref_msg = "Explicit cast is required when assigning float value to int variable";
      issue->SetRefMsg(ref_msg);

      issue = report->ReportIssue(GJB8114, G5_10_1_5, stmt);
      ref_msg = "Rounding need to be taken in account when convert float to integer";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 5.10.1.2
 * Explicit cast is required when assigning int value to shorter int variable
 */
void GJB8114StmtRule::CheckTruncWithoutCastInAssign(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto lhsType = lhs->getType();
  auto rhsType = rhs->getType();
  if (!lhsType->isIntegerType() || !rhsType->isIntegerType()) return;
  if (auto declExpr = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
    auto rhsDecl = declExpr->getDecl();
    if (auto varDecl = clang::dyn_cast<clang::VarDecl>(rhsDecl)) {
      auto lhsBT = clang::dyn_cast<clang::BuiltinType>(lhsType);
      auto rhsBT = clang::dyn_cast<clang::BuiltinType>(rhsType);

      if (lhsBT == nullptr || rhsBT == nullptr) return;

      // convert signed type to unsigned type to compare size
      auto resolve = [&](const clang::BuiltinType *type) -> clang::BuiltinType::Kind {
        if (type->isUnsignedInteger()) {
          return static_cast<clang::BuiltinType::Kind>(type->getKind() - clang::BuiltinType::Kind::Bool);
        }
        return type->getKind();
      };

      auto lhsKind = resolve(lhsBT);
      auto rhsKind = resolve(rhsBT);

      if (lhsKind < rhsKind) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(GJB8114, G5_10_1_2, stmt);
        std::string ref_msg = "Explicit cast is required when assigning int value to shorter int variable";
        issue->SetRefMsg(ref_msg);
      }
    }
  }

}

/*
 * GJB8114: 5.10.1.3
 * Explicit cast is required when assigning double value to float variable
 */
void GJB8114StmtRule::CheckDoubleToFloatWithoutCast(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhsType = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhsType = stmt->getRHS()->IgnoreParenImpCasts()->getType();

  if (auto lhsBT = clang::dyn_cast<clang::BuiltinType>(lhsType)) {
    if (lhsBT->getKind() != clang::BuiltinType::Float) return;
  } else {
    return;
  }

  if (auto rhsBT = clang::dyn_cast<clang::BuiltinType>(rhsType)) {
    if (rhsBT->getKind() == clang::BuiltinType::Double) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB8114, G5_10_1_3, stmt);
      std::string ref_msg = "Explicit cast is required when assigning double value to float variable";
      issue->SetRefMsg(ref_msg);
    }
  } else {
    return;
  }
}

/*
 * GJB8114: 5.10.1.5
 * Explicit cast is required by assignments between pointer type value and non-pointer type value
 */
void GJB8114StmtRule::CheckAssignPointerAndNonPointerWithoutCast(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhsType = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhsType = stmt->getRHS()->IgnoreParenImpCasts()->getType();

  if (lhsType->isPointerType() != rhsType->isPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB8114, G5_10_1_5, stmt);
    std::string ref_msg = "Explicit cast is required by assignments between pointer type value and non-pointer type value";
    issue->SetRefMsg(ref_msg);

    issue = report->ReportIssue(GJB8114, G5_10_2_4, stmt);
    ref_msg = "Assignments between pointer type value and non-pointer type value should be deliberated";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.10.2.2
 * Convert double to float carefully
 */
void GJB8114StmtRule::CheckDoubleToFloat(const clang::CastExpr *stmt) {
  if (auto stmtBT = clang::dyn_cast<clang::BuiltinType>(stmt->getType())) {
    if (stmtBT->getKind() == clang::BuiltinType::Kind::Float) {
      if (auto subStmtBT = clang::dyn_cast<clang::BuiltinType>(stmt->getSubExpr()->IgnoreParenImpCasts()->getType())) {
        if (subStmtBT->getKind() == clang::BuiltinType::Double) {
          XcalIssue *issue = nullptr;
          XcalReport *report = XcalCheckerManager::GetReport();

          issue = report->ReportIssue(GJB8114, G5_10_2_2, stmt);
          std::string ref_msg = "Convert double to float carefully";
          issue->SetRefMsg(ref_msg);
        }
      }
    }
  }
}

/*
 * GJB8114: 5.12.1.1
 * Comparing logic values is forbidden
 */
void GJB8114StmtRule::CheckComparedLogicValue(const clang::BinaryOperator *stmt) {
  if (!stmt->isComparisonOp()) return;

  auto lhsType = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhsType = stmt->getRHS()->IgnoreParenImpCasts()->getType();

  if (lhsType->isBooleanType() || rhsType->isBooleanType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB8114, G5_12_1_1, stmt);
    std::string ref_msg = "Comparing logic values is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.12.1.4
 * Comparing(bigger or less) unsigned integers with zero is forbidden
 */
void GJB8114StmtRule::CheckCompareUnsignedWithZero(const clang::BinaryOperator *stmt) {
  if (!stmt->isComparisonOp()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto isZero = [&](const clang::IntegerLiteral *literal) -> bool {
    int value;
    clang::Expr::EvalResult eval_result;
    auto ctx = XcalCheckerManager::GetAstContext();

    // try to fold the const expr
    if (literal->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = literal->getValue().getZExtValue();
    }
    if (value == 0) return true;
    return false;
  };

  bool need_report = false;
  if (lhs->getType()->isUnsignedIntegerType()) {
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(rhs)) {

      if (isZero(literal)) need_report = true;
    }
  } else if (rhs->getType()->isUnsignedIntegerType()) {
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(lhs)) {
      if (isZero(literal)) need_report = true;
    }
  }

  if (need_report) {
    issue = report->ReportIssue(GJB8114, G5_12_1_4, stmt);
    std::string ref_msg = "Comparing(bigger or less) unsigned integers with zero is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.12.1.5
 * Comparing unsigned number with signed number is forbidden
 */
void GJB8114StmtRule::CheckCompareUnsignedWithSigned(const clang::BinaryOperator *stmt) {
  if (!stmt->isComparisonOp()) return;

  auto lhsType = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhsType = stmt->getRHS()->IgnoreParenImpCasts()->getType();

  if (lhsType->isIntegerType() && rhsType->isIntegerType()) {
    if (lhsType->isUnsignedIntegerType() != rhsType->isUnsignedIntegerType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB8114, G5_12_1_5, stmt);
      std::string ref_msg = "Comparing unsigned number with signed number is forbidden";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 5.12.2.1
 * Constant value should stay at left side of the compare operator
 */
void GJB8114StmtRule::CheckCompareConstantWithVariable(const clang::BinaryOperator *stmt) {
  if (!stmt->isComparisonOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto isLiteral = [&](const clang::Stmt *stmt) -> bool {
    if (clang::dyn_cast<clang::IntegerLiteral>(stmt) ||
        clang::dyn_cast<clang::FloatingLiteral>(stmt) ||
        clang::dyn_cast<clang::CXXBoolLiteralExpr>(stmt) ||
        clang::dyn_cast<clang::CXXNullPtrLiteralExpr>(stmt) ||
        clang::dyn_cast<clang::CharacterLiteral>(stmt) ||
        clang::dyn_cast<clang::FixedPointLiteral>(stmt) ||
        clang::dyn_cast<clang::StringLiteral>(stmt)) {
      return true;
    }
    return false;
  };

  if (isLiteral(lhs)) return;
  if (isLiteral(rhs)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB8114, G5_12_2_1, stmt);
    std::string ref_msg = "Constant value should stay at left side of the compare operator";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.13.1.15
 * Assigning negative value to unsigned variables is forbidden
 */
void GJB8114StmtRule::CheckAssignNegToUnsignedVar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhsType = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  if (!lhsType->isUnsignedIntegerType()) return;

  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(rhs)) {
    if (unary->getOpcode() == clang::UnaryOperator::Opcode::UO_Minus) {
      if (clang::dyn_cast<clang::IntegerLiteral>(unary->getSubExpr())) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(GJB8114, G5_13_1_15, stmt);
        std::string ref_msg = "Assigning negative value to unsigend variables is forbidden";
        issue->SetRefMsg(ref_msg);
      }
    }

  }
}

/*
 * GJB8114: 6.1.1.2
 * Virtual base class converting to derived class should use dynamic_cast
 */
void GJB8114StmtRule::CheckVirtualBaseClassCastToDerivedClass(const clang::CXXReinterpretCastExpr *stmt) {
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  auto cast_type = stmt->getType();


  // handle reference
  if (auto unaryInst = clang::dyn_cast<clang::UnaryOperator>(sub_expr)) {
    if (unaryInst->getOpcode() != clang::UnaryOperator::Opcode::UO_Deref) return;
    sub_expr = unaryInst->getSubExpr()->IgnoreParenImpCasts();
  }

  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(sub_expr)) {

    // check sub expr is Record type
    auto decl = decl_ref->getDecl();
    auto sub_type = decl->getType();

    // get pointee type
    if (sub_type->isPointerType()) {
      auto ptr_type = clang::dyn_cast<clang::PointerType>(sub_type);
      sub_type = ptr_type->getPointeeType();
      if (!sub_type->isRecordType()) return;
    }

    const clang::RecordType *rec_type = clang::dyn_cast<clang::RecordType>(sub_type);
    if (rec_type == nullptr)
      return;
    const clang::CXXRecordDecl *cxx_record = rec_type->getAsCXXRecordDecl();
    if (cxx_record) {

      // get pointee type
      if (cast_type->isPointerType()) {
        auto ptr_type = clang::dyn_cast<clang::PointerType>(cast_type);
        cast_type = ptr_type->getPointeeType();
        if (!cast_type->isRecordType()) return;
      }

      // check if virtual base class trying to cast to its derived class
      auto record_type = clang::dyn_cast<clang::RecordType>(cast_type);
      auto record_decl = record_type->getAsCXXRecordDecl();
      if (!record_decl->isVirtuallyDerivedFrom(cxx_record)) return;

      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(GJB8114, G6_1_1_2, stmt);
      std::string ref_msg = "Virtual base class converting to derived class should use dynamic_cast";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * GJB8114: 6.2.1.1
 * Using global variables in construct function is forbidden
 */
void GJB8114StmtRule::CheckUsingGlobalVarInConstructor(const clang::DeclRefExpr *stmt) {
  auto decl = stmt->getDecl();
  if (auto varDecl = clang::dyn_cast<clang::VarDecl>(decl)) {
    if (varDecl->hasGlobalStorage()) {
      if (_current_function_decl && clang::dyn_cast<clang::CXXConstructorDecl>(_current_function_decl)) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(GJB8114, G6_2_1_1, stmt);
        std::string ref_msg = "Using global variables in construct function is forbidden";
        issue->SetRefMsg(ref_msg);
      }
    }
  }
}

/*
 * GJB8114: 6.5.1.1
 * Converting unrelated pointers to object pointers is forbidden
 */
void GJB8114StmtRule::CheckUnrelatedCastToObject(const clang::CXXReinterpretCastExpr *stmt) {
  // check if record type
  auto target_type = stmt->getType();
  if (!target_type->isRecordType()) return;
  auto target_record_type = clang::dyn_cast<clang::RecordType>(target_type);

  // try to get class declaration
  auto target_decl = target_record_type->getAsCXXRecordDecl();
  if (target_decl == nullptr) return;

  auto subType = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();
  if (subType->isRecordType()) {
    auto record_type = clang::dyn_cast<clang::RecordType>(subType);
    if (auto record_decl = record_type->getAsCXXRecordDecl()) {
      if (record_decl->isDerivedFrom(target_decl) ||
          target_decl->isDerivedFrom(record_decl) ||
          target_decl == record_decl) {
        return;
      }
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(GJB8114, G6_5_1_1, stmt);
  std::string ref_msg = "Converting unrelated pointers to object pointers is forbidden";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 6.5.1.2
 * Removing const or volatile from type conversion of pointer or reference
 */
void GJB8114StmtRule::CheckConstCastOnPointerOrReference(const clang::CXXConstCastExpr *stmt) {
  if (stmt->getType()->isPointerType() || stmt->getType()->isReferenceType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB8114, G6_5_1_2, stmt);
    std::string ref_msg = "Removing const or volatile from type conversion of pointer or reference";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 6.5.2.1
 * Using C++ style type converting operator is recommended
 */
void GJB8114StmtRule::CheckCStyleCastInCPPFile(const clang::CStyleCastExpr *stmt) {
  if (IsInCPPFile(stmt->getBeginLoc())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(GJB8114, G6_5_2_1, stmt);
    std::string ref_msg = "Using C++ style type converting operator is recommended";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 6.7.1.1
 * Using reference to pass a array whose size is constant
 */
void GJB8114StmtRule::CheckConstLenghtArrayPassToFunction(const clang::CallExpr *stmt) {
  if (!IsInCPPFile(stmt->getBeginLoc())) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  int index = 0;  // record the index of arguments
  for (const auto &arg : stmt->arguments()) {
    if (arg->IgnoreParenImpCasts()->getType()->isConstantArrayType()) {
      auto callee = GetCalleeDecl(stmt);
      if (callee == nullptr || callee->param_empty()) {
        index++;
        continue;
      }

      if (index >= callee->getNumParams())
        break;

      auto param = callee->parameters()[index];
      if (!param->getType()->isReferenceType()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB8114, G6_7_1_1, callee);
          std::string ref_msg = "Using reference to pass a array whose size is constant";
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(param);
      }
    }

    index++;
  }
}


}
}
