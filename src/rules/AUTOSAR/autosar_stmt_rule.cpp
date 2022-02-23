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

#include "autosar_stmt_rule.h"

namespace xsca {
namespace rule {

bool AUTOSARStmtRule::IsAssign(clang::OverloadedOperatorKind kind) const {
  using OverOp = clang::OverloadedOperatorKind;
  switch (kind) {
    case OverOp::OO_Equal:
    case OverOp::OO_LessLessEqual:
    case OverOp::OO_GreaterGreaterEqual:
      return true;
    default:
      break;
  }

  if (kind >= OverOp::OO_PlusEqual && kind <= OverOp::OO_PipeEqual) return true;

  return false;
};

/* AUTOSAR
 * Rule: A2-13-5
 * Hexadecimal constants should be upper case.
 */
void AUTOSARStmtRule::CheckHexadecimalUpperCase(const clang::IntegerLiteral *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto start = src_mgr->getCharacterData(stmt->getBeginLoc());
  auto end = src_mgr->getCharacterData(stmt->getEndLoc());

  while (start) {
    if (*start == 'x') {
      start++;
      continue;
    }
    if (!isalnum(*start)) break;
    if (*start <= 'z' && *start >= 'a') {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(AUTOSAR, A2_13_5, stmt);
      std::string ref_msg = "Hexadecimal constants should be upper case.";
      issue->SetRefMsg(ref_msg);
      return;
    }
    start++;
  }
}

/* AUTOSAR
 * Rule: A4-5-1
 * Expressions with type enum or enum class shall not be used as operands to built-in and overloaded
 * operators other than the subscript operator [ ], the assignment operator =, the equality operators == and ! =,
 * the unary & operator, and the relational operators <, <=, >, >=.
 */
void AUTOSARStmtRule::CheckEnumBeyondLimit(const clang::BinaryOperator *stmt) {
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

    issue = report->ReportIssue(AUTOSAR, A4_5_1, stmt);
    std::string ref_msg = "Using enumeration types beyond the limit is forbidden";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARStmtRule::CheckInappropriateCast(const clang::CStyleCastExpr *stmt) {
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
    if (to_type->isEnumeralType()) {
      need_report = true;
    } else if (to_type->isBooleanType()) {
      if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(stmt->getSubExpr()->IgnoreParenImpCasts())) {
        auto ctx = XcalCheckerManager::GetAstContext();
        int value;
        clang::Expr::EvalResult eval_result;

        // try to fold the const expr
        if (literal->EvaluateAsInt(eval_result, *ctx)) {
          value = eval_result.Val.getInt().getZExtValue();
        } else {
          value = literal->getValue().getZExtValue();
        }

        if (value != 0 && value != 1) need_report = true;
      }
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
    issue = report->ReportIssue(AUTOSAR, A5_0_3, stmt);
    std::string ref_msg = "The value of an expression should not be cast to an inappropriate essential type";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A5-1-2
 * Variables shall not be implicitly captured in a lambda expression.
 */
void AUTOSARStmtRule::CheckLambdaImplicitlyCaptured(const clang::LambdaExpr *stmt) {
  if (!stmt->implicit_captures().empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_2, stmt);
    std::string ref_msg = "Variables shall not be implicitly captured in a lambda expression.";
    issue->SetRefMsg(ref_msg);
  }
}

void AUTOSARStmtRule::CheckLambdaParameterList(const clang::LambdaExpr *stmt) {
  if (stmt->hasExplicitParameters()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A5_1_3, stmt);
  std::string ref_msg = "Parameter list (possibly empty) shall be included in every lambda expression.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARStmtRule::CheckLambdaExplictReturnType(const clang::LambdaExpr *stmt) {
  if (stmt->hasExplicitResultType()) return;

  clang::QualType result_type = stmt->getLambdaClass()->getLambdaCallOperator()->getType();
  DBG_ASSERT(result_type.isNull() == false, "get lambda return type failed");

  if (auto lambda_type = clang::dyn_cast<clang::FunctionProtoType>(result_type)) {
    if (lambda_type->getReturnType()->isVoidType()) return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A5_1_6, stmt);
  std::string ref_msg = "Return type of a non-void return type lambda expression should be explicitly specified.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARStmtRule::CheckLambdaInTypeidtype(const clang::CXXTypeidExpr *stmt) {
  auto op = stmt->getExprOperand();
  if (auto ref_expr = clang::dyn_cast<clang::DeclRefExpr>(op)) {
    auto ref_type = ref_expr->getType();

    auto auto_type = clang::dyn_cast<clang::AutoType>(ref_type);
    if (!auto_type) return;

    auto record_type = auto_type->getAs<clang::RecordType>();
    if (!record_type || !record_type->getDecl()->isLambda()) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_7, stmt);
    std::string ref_msg = "A lambda shall not be an operand to decltype or typeid.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A5-1-8
 * Lambda expressions should not be defined inside another lambda expression.
 */
void AUTOSARStmtRule::CheckNestedLambdaExpr(const clang::LambdaExpr *stmt) {
  if (this->LambdaDepth() > 1) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_1_8, stmt);
    std::string ref_msg = "Lambda expressions should not be defined inside another lambda expression.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A5-16-1
 * The ternary conditional operator shall not be used as a sub-expression.
 */
void AUTOSARStmtRule::CheckConditionalOperatorAsSubExpr(const clang::ConditionalOperator *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (!parents.empty()) {
    auto parent_decl = parents[0].get<clang::Decl>();
    if (parent_decl && clang::isa<clang::VarDecl>(parent_decl)) return;

    auto parent_stmt = parents[0].get<clang::Stmt>();
    if (parent_stmt) {
      if (auto bin_stmt = clang::dyn_cast<clang::BinaryOperator>(parent_stmt)) {
        if (bin_stmt->isAssignmentOp() || bin_stmt->isCompoundAssignmentOp()) return;
      } else if (clang::isa<clang::CompoundStmt>(parent_stmt)) {
        return;
      } else if (clang::isa<clang::ReturnStmt>(parent_stmt)) {
        return;
      } else {
        if (clang::isa<clang::ParenExpr>(parent_stmt) || clang::isa<clang::ImplicitCastExpr>(parent_stmt)) {
          const clang::Stmt *tmp = parent_stmt;
          do {
            parents = ctx->getParents(*tmp);
            if (parents.empty()) return;
            tmp = parents[0].get<clang::Stmt>();
            if (tmp == nullptr) return;
          } while (clang::isa<clang::ParenExpr>(tmp) || clang::isa<clang::ImplicitCastExpr>(tmp));

          switch (tmp->getStmtClass()) {
            case clang::Stmt::StmtClass::ReturnStmtClass:
            case clang::Stmt::StmtClass::CompoundStmtClass:
              return;
            case clang::Stmt::StmtClass::BinaryOperatorClass: {
              auto bin = clang::dyn_cast<clang::BinaryOperator>(tmp);
              if (bin->isAssignmentOp() || bin->isCompoundAssignmentOp()) return;
            }
            default:
              break;
          }
        }
      }
    }

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A5_16_1, stmt);
    std::string ref_msg = "The ternary conditional operator shall not be used as a sub-expression.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-1-7 (partial)
 * Each expression statement and identifier declaration shall be placed on a separate line.
 */
void AUTOSARStmtRule::CheckDeclsInSameLine(const clang::DeclStmt *stmt) {
  if (stmt->isSingleDecl()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_1_7, stmt);
  std::string ref_msg = "Each expression statement and identifier declaration shall be placed on a separate line.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A7-5-2
 * Functions shall not call themselves, either directly or indirectly.
 */
void AUTOSARStmtRule::CheckFunctionCallThemselves(const clang::CallExpr *stmt) {
  if (stmt->getCalleeDecl() == this->_current_function_decl) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_5_2, stmt);
    std::string ref_msg = "Functions shall not call themselves, either directly or indirectly.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A9-3-1
 * Member functions shall not return non-const “raw” pointers or references to
 * private or protected data owned by the class.
 */
void AUTOSARStmtRule::CheckMethodReturnPrivateOrProtectFields(const clang::ReturnStmt *stmt) {
  if (clang::isa<clang::CXXMethodDecl>(_current_function_decl)) {
    auto ret_value = stmt->getRetValue();
    if (!ret_value) return;
    if (auto member = clang::dyn_cast<clang::MemberExpr>(ret_value)) {
      if (auto field_decl = clang::dyn_cast<clang::FieldDecl>(member->getMemberDecl())) {
        auto access = field_decl->getAccess();
        if (access == clang::AccessSpecifier::AS_private || access == clang::AccessSpecifier::AS_protected) {
          XcalIssue *issue = nullptr;
          XcalReport *report = XcalCheckerManager::GetReport();
          issue = report->ReportIssue(AUTOSAR, A9_3_1, stmt);
          std::string ref_msg = "Member functions shall not return non-const “raw” pointers or references to "
                                "private or protected data owned by the class.";
          issue->SetRefMsg(ref_msg);
        }
      }
    }
  }
}

/*
 * AUTOSAR: A13-2-1
 * An assignment operator shall return a reference to “this”.
 */
void AUTOSARStmtRule::CheckAssignmentOperatorReturnThisRef(const clang::ReturnStmt *stmt) {
  if (!_current_function_decl->isOverloadedOperator() ||
      !clang::isa<clang::CXXMethodDecl>(_current_function_decl))
    return;

  auto kind = clang::dyn_cast<clang::CXXMethodDecl>(_current_function_decl)->getOverloadedOperator();
  if (!IsAssign(kind)) return;

  auto ret_value = stmt->getRetValue()->IgnoreParenImpCasts();
  if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(ret_value)) {
    if (clang::isa<clang::CXXThisExpr>(unary_op->getSubExpr()))
      return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_2_1, stmt);
  std::string ref_msg = "An assignment operator shall return a reference to “this”.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-2-2
 * A binary arithmetic operator and a bitwise operator shall return a “prvalue”.
 * TODO: NEED MAKE SURE THE RESULT OF "isRValue"
 */
void AUTOSARStmtRule::CheckBinaryOpOrBitwiseOpReturnPRValue(const clang::ReturnStmt *stmt) {
  if (!_current_function_decl->isOverloadedOperator()) return;

  auto op = _current_function_decl->getOverloadedOperator();

  if (op < clang::OverloadedOperatorKind::OO_Plus ||
      op > clang::OverloadedOperatorKind::OO_Greater)
    return;

  auto ret_value = stmt->getRetValue();

  // TODO: NEED FIX HERE
#if 0
  if (ret_value->isRValue() && !ret_value->isXValue()) {

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A13_2_2, stmt);
    std::string ref_msg = "A binary arithmetic operator and a bitwise operator shall return a “prvalue”.";
    issue->SetRefMsg(ref_msg);
  }
#endif
}

/*
 * AUTOSAR: A13-2-3
 * A relational operator shall return a boolean value.
 */
void AUTOSARStmtRule::CheckRelationalOpReturnBool(const clang::ReturnStmt *stmt) {
  if (!_current_function_decl->isOverloadedOperator()) return;

  using OpKind = clang::OverloadedOperatorKind;
  auto op = _current_function_decl->getOverloadedOperator();

  bool is_relational = false;

  if (op == OpKind::OO_Greater || op == OpKind::OO_Less ||
      (op >= OpKind::OO_ExclaimEqual && op <= OpKind::OO_Spaceship))
    is_relational = true;

  if (!is_relational) return;
  if (stmt->getRetValue()->getType()->isBooleanType()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_2_3, stmt);
  std::string ref_msg = "A relational operator shall return a boolean value.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-5-3
 * User-defined conversion operators should not be used.
 */
void AUTOSARStmtRule::CheckUsingUserDefinedConversionOp(const clang::CStyleCastExpr *stmt) {
  // get call expr
  auto sub = stmt->getSubExpr()->IgnoreParenImpCasts();
  auto call_expr = clang::dyn_cast<clang::CXXMemberCallExpr>(sub);
  if (!call_expr) return;

  // get conversion declaration
  auto callee = call_expr->getCalleeDecl();
  auto conv = clang::dyn_cast<clang::CXXConversionDecl>(callee);
  if (!conv) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_5_3, stmt);
  std::string ref_msg = "User-defined conversion operators should not be used.";
  issue->SetRefMsg(ref_msg);
}

}
}
