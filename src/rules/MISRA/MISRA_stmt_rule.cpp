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
// MISRA_stmt_rule.cpp
// ====================================================================
//
// implement all stmt related rules in MISRA
//

#include <clang/AST/ParentMapContext.h>
#include "MISRA_stmt_rule.h"

namespace xsca {
namespace rule {

clang::QualType MISRAStmtRule::GetRawTypeOfTypedef(const clang::QualType type) {
  clang::QualType res;
  if (auto tmp = clang::dyn_cast<clang::TypedefType>(type)) {
    res = tmp->getDecl()->getTypeForDecl()->getCanonicalTypeInternal();
  }
  return res;
}

void MISRAStmtRule::HasThisFunctionThenReport(const std::vector<std::string> &fid_func, const std::string &str,
                                              const clang::CallExpr *stmt, const char *std_id,
                                              const std::string &info) {
  auto res = std::find(fid_func.begin(), fid_func.end(), str);
  if (res != fid_func.end()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, std_id, stmt);
    auto callee = GetCalleeDecl(stmt);
    if (callee) issue->SetName(callee->getNameAsString());
    issue->SetRefMsg(info);
  }
}

const clang::FunctionDecl *MISRAStmtRule::GetCalleeDecl(const clang::CallExpr *stmt) {
  auto callee = stmt->getCalleeDecl();
  if (callee == nullptr) return nullptr;
  auto decl = callee->getAsFunction();
  return decl;
}

clang::BuiltinType::Kind MISRAStmtRule::UnifyBTTypeKind(const clang::BuiltinType::Kind &kind) {
  if (kind > clang::BuiltinType::Kind::UInt128)
    return (clang::BuiltinType::Kind) (kind - 8);
  else return kind;
}

// check if the expr is an IntegerLiteral expression
bool MISRAStmtRule::IsIntegerLiteralExpr(const clang::Expr *expr) {
  clang::Expr::EvalResult eval_result;
  auto ctx = XcalCheckerManager::GetAstContext();
  return expr->EvaluateAsInt(eval_result, *ctx);
}

// check if the stmt has side effect
bool MISRAStmtRule::HasSideEffect(const clang::Stmt *stmt) {
  bool res = false;

  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
    if (unary->isIncrementDecrementOp()) return true;
  } else if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(stmt)) {
    auto decl = decl_ref->getDecl();
    if (decl->getType().isVolatileQualified()) return true;
  }

  for (const auto &it : stmt->children()) {
    if (HasSideEffect(it)) return true;
  }
  return false;
}

// check if the expr has Inc/Dec expr
bool MISRAStmtRule::HasIncOrDecExpr(const clang::Stmt *stmt) {
  if (stmt == nullptr) return false;
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
    if (unary->isIncrementDecrementOp()) return true;
  }

  for (const auto &it : stmt->children()) {
    if (HasIncOrDecExpr(it)) return true;
  }
  return false;
}

// report template
void MISRAStmtRule::ReportTemplate(const std::string &str, const char *rule, const clang::Stmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, rule, stmt);
  issue->SetRefMsg(str);
}


/* MISRA
 * Rule: 4.1
 * Octal and hexadecimal escape sequences shall be terminated
 */
void MISRAStmtRule::CheckOctalAndHexadecimalEscapeWithoutTerminated(const clang::StringLiteral *stmt) {
}

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
  auto decl = GetCalleeDecl(stmt);
  if (decl == nullptr) return;

  int i = 0;
  for (const auto &it : stmt->arguments()) {
    if (it->IgnoreParenImpCasts()->getStmtClass() == clang::Stmt::StringLiteralClass) {
      if (i >= decl->param_size()) break;
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

// collect object types within try block
std::vector<clang::QualType>
MISRAStmtRule::RecordThrowObjectTypes(const clang::Stmt *stmt) {
  std::vector<clang::QualType> obj_types;
  if (auto throw_stmt = clang::dyn_cast<clang::CXXThrowExpr>(stmt)) {
    auto sub_stmt = throw_stmt->getSubExpr()->IgnoreParenImpCasts();
    if (sub_stmt != nullptr) {
      auto obj_type = sub_stmt->getType();
      obj_types.push_back(obj_type);
    }
  } else {
    for (const auto &it : stmt->children()) {
      if (it == nullptr) continue;
      auto sub_res = RecordThrowObjectTypes(it);
      obj_types.insert(obj_types.begin(), sub_res.begin(), sub_res.end());
    }
  }
  return std::move(obj_types);
}

/* MISRA
 * Rule: 10.2
 * Expressions of essentially character type shall not be used inappropriately in addition and subtraction operations
 */
void MISRAStmtRule::CheckAddOrSubOnCharacter(const clang::BinaryOperator *stmt) {
  if (!stmt->isAdditiveOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  auto lhs_char = clang::dyn_cast<clang::CharacterLiteral>(lhs);
  auto rhs_char = clang::dyn_cast<clang::CharacterLiteral>(rhs);
  if ((lhs_char == nullptr) && (rhs_char == nullptr)) return;

  bool need_report = false;
  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
  if (lhs_type->isCharType() != rhs_type->isCharType()) {
    need_report = true;
  } else {
    if (lhs_char) {
      auto value = lhs_char->getValue();
      if (value < '0' || value > '9') need_report = true;
    }

    if (rhs_char) {
      auto value = rhs_char->getValue();
      if (value < '0' || value > '9') need_report = true;
    }
  }
  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_2, stmt);
    std::string ref_msg = "Expressions of essentially character type shall not be"
                          " used inappropriately in addition and subtraction operations";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 10.3
 * The value of an expression shall not be assigned to an object with a narrower
 * essential type or of a different essential type category
 */
void MISRAStmtRule::CheckIntToShorter(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp() && !stmt->isCompoundAssignmentOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS();

  auto rhs_cast = clang::dyn_cast<clang::CastExpr>(rhs);
  if (!rhs_cast) return;

  if (auto subStmtBT = clang::dyn_cast<clang::BuiltinType>(rhs_cast->getSubExpr()->IgnoreParenImpCasts()->getType())) {
    auto stmtBT = clang::dyn_cast<clang::BuiltinType>(rhs_cast->getType());
    // check if stmt is builtin type
    if (stmtBT == nullptr) return;

    if (subStmtBT->isInteger() && stmtBT->isInteger()) {
      // convert signed type to unsigned type to compare size
      auto resolve = [&](const clang::BuiltinType *type) -> clang::BuiltinType::Kind {
        if (type->isUnsignedInteger()) {
          return static_cast<clang::BuiltinType::Kind>(type->getKind() - clang::BuiltinType::Kind::Bool);
        }
        return type->getKind();
      };

      auto stmtKind = resolve(stmtBT);
      auto subStmtKind = resolve(subStmtBT);

      if (stmtKind < subStmtKind) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();

        issue = report->ReportIssue(MISRA, M_R_10_3, stmt);
        std::string ref_msg = "Convert int to shorter int carefully";
        issue->SetRefMsg(ref_msg);
      }
    }
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
  auto ctx = XcalCheckerManager::GetAstContext();

  bool need_report = false;
  using StmtClass = clang::Stmt::StmtClass;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  // try to evaluate the expression
  if (IsIntegerLiteralExpr(lhs) || IsIntegerLiteralExpr(rhs)) return;

  if (stmt->isAdditiveOp() || stmt->isComparisonOp() || stmt->isCompoundAssignmentOp()) {
    auto lhs_type = lhs->getType();
    auto rhs_type = rhs->getType();
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
 * Rule: 10.6
 * The value of a composite expression shall not be assigned to an object with wider essential type
 */
void MISRAStmtRule::CheckCompositeExprAssignToWiderTypeVar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;
  auto rhs = stmt->getRHS()->IgnoreCasts();

  // check only when the rhs is binary operator
  if (rhs->getStmtClass() != clang::Stmt::StmtClass::BinaryOperatorClass) return;

  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = rhs->getType();
  bool need_report = false;

  /* Get raw type of typedef */
  if (clang::isa<clang::TypedefType>(lhs_type)) lhs_type = GetRawTypeOfTypedef(lhs_type);
  if (clang::isa<clang::TypedefType>(rhs_type)) rhs_type = GetRawTypeOfTypedef(rhs_type);

  if (rhs_type < lhs_type) need_report = true;
  if (lhs_type->isIntegerType() && (rhs_type == lhs_type)) {
    auto lhs_bt = clang::dyn_cast<clang::BuiltinType>(lhs_type);
    if (lhs_bt == nullptr) return;

    // convert signed type to unsigned type to compare size
    auto resolve = [&](const clang::BuiltinType *type) -> clang::BuiltinType::Kind {
      if (type->isUnsignedInteger()) {
        return static_cast<clang::BuiltinType::Kind>(type->getKind() - clang::BuiltinType::Kind::Bool);
      }
      return type->getKind();
    };

    if (auto bin_sub = clang::dyn_cast<clang::BinaryOperator>(stmt->getRHS()->IgnoreCasts())) {
      auto sub_lhs_type = bin_sub->getLHS()->IgnoreParenImpCasts()->getType();
      auto sub_rhs_type = bin_sub->getRHS()->IgnoreParenImpCasts()->getType();
      if (sub_lhs_type->isIntegerType() && sub_rhs_type->isIntegerType()) {
        auto prim_kind = resolve(lhs_bt);
        auto sub_lhs_bt = clang::dyn_cast<clang::BuiltinType>(sub_lhs_type);
        auto sub_rhs_bt = clang::dyn_cast<clang::BuiltinType>(sub_rhs_type);
        if (sub_lhs_bt == nullptr || sub_rhs_bt == nullptr) return;
        auto lhs_kind = resolve(sub_lhs_bt);
        auto rhs_kind = resolve(sub_rhs_bt);
        if (lhs_kind < prim_kind && rhs_kind < prim_kind) need_report = true;
      }
    }
  }

  if (need_report) {
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

  if (clang::isa<clang::TypedefType>(lhs_type)) lhs_type = GetRawTypeOfTypedef(lhs_type);
  if (clang::isa<clang::TypedefType>(rhs_type)) rhs_type = GetRawTypeOfTypedef(rhs_type);

  bool need_report = false;

  // unify
  auto bt_lhs = clang::dyn_cast<clang::BuiltinType>(lhs_type);
  auto bt_rhs = clang::dyn_cast<clang::BuiltinType>(rhs_type);
  if (!bt_lhs || !bt_rhs) return;
  if (!bt_lhs->isInteger() || !bt_rhs->isInteger()) return;

  auto lhs_kind = UnifyBTTypeKind(bt_lhs->getKind());
  auto rhs_kind = UnifyBTTypeKind(bt_rhs->getKind());

  using Stmt = clang::Stmt;
  if (lhs_kind < rhs_kind) {
    if (auto lhs_bin_op = clang::dyn_cast<clang::BinaryOperator>(lhs)) {
      if (IsIntegerLiteralExpr(lhs_bin_op->getLHS()->IgnoreParenImpCasts()) &&
          IsIntegerLiteralExpr(lhs_bin_op->getRHS()->IgnoreParenImpCasts()))
        return;
      need_report = true;
    }
  } else if (rhs_kind < lhs_kind) {
    if (auto rhs_bin_op = clang::dyn_cast<clang::BinaryOperator>(rhs)) {
      if (IsIntegerLiteralExpr(rhs_bin_op->getLHS()->IgnoreParenImpCasts()) &&
          IsIntegerLiteralExpr(rhs_bin_op->getRHS()->IgnoreParenImpCasts()))
        return;
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

  if (clang::isa<clang::TypedefType>(type)) type = GetRawTypeOfTypedef(type);
  if (clang::isa<clang::TypedefType>(sub_type)) sub_type = GetRawTypeOfTypedef(sub_type);

  auto bt_type = clang::dyn_cast<clang::BuiltinType>(type);
  auto bt_sub = clang::dyn_cast<clang::BuiltinType>(sub_type);
  if (!bt_type || !bt_sub) return;
  auto type_kind = UnifyBTTypeKind(bt_type->getKind());
  auto subtype_kind = UnifyBTTypeKind(bt_sub->getKind());

  if (sub_expr->getStmtClass() != clang::Stmt::BinaryOperatorClass) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  if (subtype_kind < type_kind) {
    need_report = true;
  } else if (type_kind < subtype_kind) {
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
 * Rule: 11.1
 * Conversions shall not be performed between a pointer to a function and any other type
 */
void MISRAStmtRule::CheckCastFunctionPointerType(const clang::CStyleCastExpr *stmt) {

}

/* MISRA
 * Rule: 11.2
 * Conversions shall not be performed between a pointer to an incomplete type and any other type
 */
void MISRAStmtRule::CheckIncompleteTypePointerCastToAnotherType(const clang::CStyleCastExpr *stmt) {
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();
  if (!sub_type->isPointerType()) return;

  auto pointee = clang::dyn_cast<clang::PointerType>(sub_type);
  if (!pointee) return;
  auto pointee_type = pointee->getPointeeType();
  if (!pointee_type->isStructureType()) return;
  auto struct_decl = pointee_type->getAs<clang::RecordType>()->getDecl();
  if (!struct_decl || (!struct_decl->isCompleteDefinition())) return;

  if (sub_type == stmt->getType()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_11_2, stmt);
  std::string ref_msg = "Conversions shall not be performed between a pointer to an "
                        "incomplete type and any other type";
  issue->SetRefMsg(ref_msg);
}

/* MISRA
 * Rule: 11.3
 * A cast shall not be performed between a pointer to object type and a pointer to a different object type
 */
void MISRAStmtRule::CheckCastPointerToDifferentType(const clang::CStyleCastExpr *stmt) {
  auto type = stmt->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();
  if (type != sub_type) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_3, stmt);
    std::string ref_msg = "A cast shall not be performed between a pointer to "
                          "object type and a pointer to a different object type";
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
  auto ctx = XcalCheckerManager::GetAstContext();
  auto type = stmt->IgnoreParenImpCasts()->getType();
  auto sub_type = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();

  // except for (void *)0
  auto sub_stmt = stmt->getSubExpr();
  if (sub_stmt->IgnoreCasts()->isNullPointerConstant(*ctx, clang::Expr::NPC_ValueDependentIsNull))
    return;

  if ((type->isPointerType() && !type->isVoidPointerType()) && sub_type->isVoidPointerType()) {
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

  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(sub_expr)) {
    int value;
    auto ctx = XcalCheckerManager::GetAstContext();
    clang::Expr::EvalResult eval_result;

    // try to fold the const expr
    if (literal->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = literal->getValue().getZExtValue();
    }

    if (value == 0) return;

  }

  auto sub_type = sub_expr->IgnoreParenImpCasts()->getType();


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

  auto type_bt = clang::dyn_cast<clang::BuiltinType>(type);
  if (!type_bt && !type->isPointerType()) return;
  auto sub_bt = clang::dyn_cast<clang::BuiltinType>(sub_type);
  if (!sub_bt && !sub_type->isPointerType()) return;
  if (sub_bt == nullptr && type_bt == nullptr) return;

  if ((type->isPointerType() && sub_bt && (sub_bt->getKind() != clang::BuiltinType::Kind::Int)) ||
      (type_bt && (type_bt->getKind() != clang::BuiltinType::Kind::Int) && sub_type->isPointerType())) {
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
    if (!sub_type->isPointerType()) return;

    bool isConst = false, isVolatile = false;
    isConst = sub_type->getPointeeType().isConstQualified();
    isVolatile = sub_type->getPointeeType().isVolatileQualified();
    if (!isConst && !isVolatile) return;

    auto cast_type = cast_inst->getType();
    if (!cast_type->isPointerType()) return;
    if ((!cast_type->getPointeeType().isConstQualified() && isConst) ||
        (!cast_type->getPointeeType().isVolatileQualified() && isVolatile)) {
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

  if (clang::isa<clang::TypedefType>(lhs_type)) lhs_type = GetRawTypeOfTypedef(lhs_type);
  if (clang::isa<clang::TypedefType>(rhs_type)) rhs_type = GetRawTypeOfTypedef(rhs_type);

  bool need_report = false;

  // int a = 2; if (a == NULL)
  if (!lhs_type->isPointerType() && clang::isa<clang::GNUNullExpr>(rhs)) need_report = true;
  if (!rhs_type->isPointerType() && clang::isa<clang::GNUNullExpr>(lhs)) need_report = true;

  // int *p2 = (int *)0; if (p2 == 0)
  if (lhs_type->isPointerType())
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(rhs->IgnoreParenImpCasts()))
      if (literal->getValue().getZExtValue() == 0) need_report = true;

  if (rhs_type->isPointerType())
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(lhs->IgnoreParenImpCasts()))
      if (literal->getValue().getZExtValue() == 0) need_report = true;

  if (need_report) {
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
 * Rule: 13.1
 * Initializer lists shall not contain persistent side effects
 */
void MISRAStmtRule::CheckSideEffectWithinInitListExpr(const clang::InitListExpr *stmt) {
  std::vector<const clang::Expr *> sinks;
  for (const auto &it : stmt->inits()) {
    auto init = it->IgnoreParenImpCasts();
    if (HasSideEffect(init)) sinks.push_back(it);
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_1, stmt);
    std::string ref_msg = "Initializer lists shall not contain persistent side effects";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddStmt(it);
  }
}

/* MISRA
 * Rule: 13.2
 * The value of an expression and its persistent side
 * effects shall be the same under all permitted evaluation orders
 */
bool MISRAStmtRule::isInc(const clang::Expr *expr) {
  if (expr == nullptr) return false;
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(expr)) {
    return unary->isPostfix() || unary->isPrefix();
  }
  return false;
}


void MISRAStmtRule::CheckSideEffectWithOrder(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (isInc(lhs) && isInc(rhs)) {
    ReportSideEffect(stmt);
  }
}

void MISRAStmtRule::CheckSideEffectWithOrder(const clang::CallExpr *stmt) {
  for (const auto &args : stmt->arguments()) {
    if (isInc(args)) {
      ReportSideEffect(stmt);
      break;
    }
  }
}

void MISRAStmtRule::ReportSideEffect(const clang::Stmt *stmt) {
  std::string ref_msg = "The value of an expression and its persistent side effects"
                        " shall be the same under all permitted evaluation orders";
  ReportTemplate(ref_msg, M_R_13_2, stmt);
}

/* MISRA
 * Rule: 13.3
 * A full expression containing an increment (++) or decrement (--) operator should have no other potential side
 * effects other than that caused by the increment or decrement operator
 */
void MISRAStmtRule::CheckMultiIncOrDecExpr(const clang::BinaryOperator *stmt) {
  bool need_report = false;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (stmt->isAssignmentOp() || stmt->isCompoundAssignmentOp()) {
    if (HasIncOrDecExpr(lhs) || HasIncOrDecExpr(rhs)) need_report = true;
  } else {
    if (HasIncOrDecExpr(lhs) && HasIncOrDecExpr(rhs)) need_report = true;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_3, stmt);
    std::string ref_msg = "A full expression containing an increment (++) or decrement (--) operator should have no "
                          "other potential side effects other than that caused by the increment or decrement operator";
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

void MISRAStmtRule::ReportAssignment(const clang::Stmt *stmt) {
  std::string ref_msg = "The result of an assignment operator should not be used";
  ReportTemplate(ref_msg, M_R_13_4, stmt);
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::ArraySubscriptExpr *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs == nullptr) return;
  if (rhs->getStmtClass() != clang::Stmt::BinaryOperatorClass) return;
  if (IsAssignmentStmt(rhs)) ReportAssignment(stmt);
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::BinaryOperator *stmt) {
  if (stmt->getOpcode() == clang::BO_Comma) return;
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs && IsAssignmentStmt(rhs)) ReportAssignment(stmt);
}

/* MISRA
 * Rule: 13.5
 * The right hand operand of a logical && or || operator shall not contain
 * persistent side effects
 */
void MISRAStmtRule::CheckRHSOfLogicalOpHasSideEffect(const clang::BinaryOperator *stmt) {
  if (!stmt->isLogicalOp()) return;
  if (HasSideEffect(stmt->getRHS()->IgnoreParenImpCasts())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_5, stmt);
    std::string ref_msg = "The right hand operand of a logical && or || operator shall not contain persistent side effects";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 13.6
 * The operand of the sizeof operator shall not contain any expression which
 * has potential side effects
 */
void MISRAStmtRule::CheckSideEffectInSizeof(const clang::UnaryExprOrTypeTraitExpr *stmt) {
  bool need_report = false;
  if (stmt->getKind() == clang::UnaryExprOrTypeTrait::UETT_SizeOf) {
    for (const auto &it : stmt->children()) {
      if (HasSideEffect(it)) {
        need_report = true;
        break;
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_6, stmt);
    std::string ref_msg = "The operand of the sizeof operator shall not contain any expression which has potential side effects";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 14.1
 * A loop counter shall not have essentially floating type
 */
void MISRAStmtRule::ReportLoopVariable(const clang::Stmt *stmt) {
  std::string ref_msg = "A loop counter shall not have essentially floating type";
  ReportTemplate(ref_msg, M_R_14_1, stmt);
}

void MISRAStmtRule::CheckLoopVariable(const clang::ForStmt *stmt) {
  bool need_report = false;
  auto init_stmt = stmt->getInit();

  if (init_stmt == nullptr) { return; }
  if (auto bin_init_stmt = clang::dyn_cast<clang::BinaryOperator>(init_stmt)) {
    auto lhs = bin_init_stmt->getLHS()->IgnoreParenImpCasts();

    if (bin_init_stmt->isAssignmentOp() || bin_init_stmt->isCompoundAssignmentOp()) {
      auto lhs_type = lhs->getType();

      if (lhs_type->isFloatingType()) {
        need_report = true;
      }

    } else if (auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(init_stmt)) {
      if (!decl_stmt->isSingleDecl()) return;
      auto decl = decl_stmt->getSingleDecl();
      if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
        if (var_decl->getType()->isFloatingType()) {
          need_report = true;
        }
      }
    }

    if (need_report) {
      ReportLoopVariable(init_stmt);
    }
  }
}

/* MISRA
 * Rule: 14.4
 * The controlling expression of an if statement and the controlling expression
 * of an iteration-statement shall have essentially Boolean type
 */
void MISRAStmtRule::CheckControlStmtImpl(const clang::Expr *stmt) {
  if (stmt == nullptr) return;
  if (!stmt->getType()->isBooleanType()) {
    if (auto const_value = clang::dyn_cast<clang::IntegerLiteral>(stmt)) {
      auto val = const_value->getValue().getZExtValue();
      if (val == 1 || val == 0) return;
    }

    // check if comparison operator or logical operator
    if (auto bin_op = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
      if (bin_op->isComparisonOp() || bin_op->isLogicalOp()) return;
    }

    if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
      if (unary_op->getOpcode() == clang::UnaryOperator::Opcode::UO_LNot) return;
    }

    // check if it is special case
    // TODO: this case might need a white list
    if (auto call_expr = clang::dyn_cast<clang::CallExpr>(stmt)) {
      auto callee = GetCalleeDecl(call_expr);
      if (callee && callee->getNameAsString() == "__builtin_expect") return;
    }

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_14_4, stmt);
    std::string ref_msg = "The controlling expression of an if statement and the controlling expression "
                          "of an iteration-statement shall have essentially Boolean type";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 15.1
 * The goto statement should not be used
 */
void MISRAStmtRule::CheckGotoStmt(const clang::GotoStmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_15_1, stmt);
  std::string ref_msg = "\"goto\" statement is forbidden";
  issue->SetRefMsg(ref_msg);
}

/* MISRA
 * Rule: 15.2
 * The goto statement shall jump to a label declared later in the same function
 */
void MISRAStmtRule::CheckGotoBackward(const clang::GotoStmt *stmt) {
  auto loc = stmt->getBeginLoc();
  auto target = stmt->getLabel()->getStmt()->getBeginLoc();
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
  if (label_parents.empty()) return;
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
  if (stmt == nullptr) return;
  if ((stmt->getStmtClass() == clang::Stmt::BreakStmtClass) ||
      (stmt->getStmtClass() == clang::Stmt::GotoStmtClass)) {
    _terminates.insert(stmt);
    if (_terminates.size() >= 2) return;
  }

  for (const auto &it : stmt->children()) {
    if (it == nullptr) continue;
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
 * Rule: 15.7
 * All if ... else if constructs shall be terminated with an else statement
 */
void MISRAStmtRule::CheckIfWithoutElseStmt(const clang::IfStmt *stmt) {
  if (!stmt->hasElseStorage()) return;

  auto elif = clang::dyn_cast<clang::IfStmt>(stmt->getElse());
  if (!elif) return;
  if (elif->hasElseStorage()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_15_7, stmt);
  std::string ref_msg = "All if ... else if constructs shall be terminated with an else statement";
  issue->SetRefMsg(ref_msg);
}

/* MISRA
 * Rule: 16.4
 * Every switch statement shall have a default label
 */
void MISRAStmtRule::CheckSwitchWithoutDefault(const clang::SwitchStmt *stmt) {
  bool has_default = false;
  auto caseList = stmt->getSwitchCaseList();
  if (!caseList) return;
  do {
    if (caseList->getStmtClass() ==
        clang::Stmt::StmtClass::DefaultStmtClass) {
      has_default = true;
      break;
    }
  } while ((caseList = caseList->getNextSwitchCase()) != nullptr);

  if (!has_default) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_16_4, stmt);
    std::string ref_msg = "Every switch statement shall have a default label";
    issue->SetRefMsg(ref_msg);
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
  auto decl = GetCalleeDecl(stmt);

  // TODO: call function pointer would return nullptr
  if (decl == nullptr) return;

  unsigned int i = 0;
  for (const auto &it : stmt->arguments()) {
    if (i >= decl->param_size()) break;
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
    if (array_arg_type == nullptr || array_param_type == nullptr) {
      i++;
      continue;
    }
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
    i++;
  }
}

/* MISRA
 * Rule: 17.7
 * The value returned by a function having non-void return type shall be used
 */
void MISRAStmtRule::CheckUnusedCallExprWithoutVoidCast(const clang::CallExpr *stmt) {
  // check if this function is void return type
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  if (callee->getReturnType()->isVoidType()) return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.size() == 0) return;
  auto parent = parents[0].get<clang::Stmt>();
  if (parent == nullptr) return;

  if (auto block = clang::dyn_cast<clang::CompoundStmt>(parent)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_17_7, stmt);
    std::string ref_msg = "The value returned by a function having non-void return type shall be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 17.8
 * A function parameter should not be modified
 */
void MISRAStmtRule::CheckModifyParameters(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp() && !stmt->isCompoundAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  if (auto decl_expr = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
    auto decl = decl_expr->getDecl();
    if (decl->getKind() == clang::Decl::Kind::ParmVar) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_17_8, stmt);
      std::string ref_msg = "A function parameter should not be modified";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 18.4
 * The +, -, += and -= operators should not be applied to an expression of pointer type
 */
void MISRAStmtRule::CheckAddOrSubOnPointer(const clang::BinaryOperator *stmt) {
  if (!stmt->isAdditiveOp() && !stmt->isCompoundAssignmentOp()) return;
  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
  if (lhs_type->isPointerType() || rhs_type->isPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_4, stmt);
    std::string ref_msg = "The +, -, += and -= operators should not be applied to an expression of pointer type";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 21.3
 * The memory allocation and deallocation functions of <stdlib.h> shall not be used
 */
void MISRAStmtRule::CheckStdMemoryAllocationFunction(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;

  // call function pointer would return nullptr
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  if (conf_mgr->IsMemAllocFunction(name)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_21_3, stmt);
    std::string ref_msg = "The memory allocation and deallocation functions of <stdlib.h> shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 21.6
 * The Standard Library input/output functions shall not be used
 */
void MISRAStmtRule::CheckIOFunctionInStdio(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  if (conf_mgr->IsStdIoFunction(name)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_21_6, stmt);
    std::string ref_msg = "The Standard Library input/output functions shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 21.7
 * The atof, atoi, atol and atoll functions of <stdlib.h> shall not be used
 */
void MISRAStmtRule::CheckIntConvertFunctionInStdlib(const clang::CallExpr *stmt) {
  std::vector<std::string> fid_funcs = {"atoi", "atol", "atoll"};
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();

  std::string info = "The atof, atoi, atol and atoll functions of <stdlib.h> shall not be used";
  HasThisFunctionThenReport(fid_funcs, name, stmt, M_R_21_7, info);
}


/* MISRA
 * Rule: 21.8
 * The library functions abort, exit, getenv and system of <stdlib.h> shall not be used
 */
void MISRAStmtRule::CheckSystemFuncInStdlib(const clang::CallExpr *stmt) {
  std::vector<std::string> fid_funcs = {"abort", "exit", "getenv", "system"};
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();

  std::string info = "The library functions abort, exit, getenv and system of <stdlib.h> shall not be used";
  HasThisFunctionThenReport(fid_funcs, name, stmt, M_R_21_8, info);
}

/* MISRA
 * Rule: 21.9
 * The library functions bsearch and qsort of <stdlib.h> shall not be used
 */
void MISRAStmtRule::CheckBsearchAndQsortInStdlib(const clang::CallExpr *stmt) {
  std::vector<std::string> fid_funcs = {"bsearch", "qsort"};
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();

  std::string info = "The library functions bsearch and qsort of <stdlib.h> shall not be used";
  HasThisFunctionThenReport(fid_funcs, name, stmt, M_R_21_9, info);
}

/* MISRA
 * Rule: 21.10
 * The Standard Library time and date functions shall not be used
 */
void MISRAStmtRule::CheckTimeFunctionInStdlib(const clang::CallExpr *stmt) {
  std::vector<std::string> fid_funcs = {"wcsftime"};
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();

  std::string info = "The Standard Library time and date functions shall not be used";
  HasThisFunctionThenReport(fid_funcs, name, stmt, M_R_21_10, info);
}

/* MISRA
 * Rule: 21.12
 * The exception handling features of <fenv.h> should not be used
 */
void MISRAStmtRule::CheckExceptionFeaturesInFenv(const clang::CallExpr *stmt) {
  std::vector<std::string> fid_funcs = {"feclearexcept", "fegetexceptflag", "feraiseexcept",
                                        "fesetexceptflag", "fetestexcept"};
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();

  std::string info = "The exception handling features of <fenv.h> should not be used";
  HasThisFunctionThenReport(fid_funcs, name, stmt, M_R_21_12, info);
}

/* MISRA
 * Rule: 12-1-1
 * ctor and dtor cannot use dynamic type
 */
void MISRAStmtRule::CheckDynamicTypeInCtorAndDtor(const clang::CXXMemberCallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  auto method_decl = clang::dyn_cast<clang::CXXMethodDecl>(callee);
  if (method_decl == nullptr || !method_decl->isVirtual()) return;
  ReportDynamicInCTorAndDtor(stmt);
}


void MISRAStmtRule::ReportDynamicInCTorAndDtor(const clang::Stmt *stmt) {
  bool is_ctor = clang::isa<clang::CXXConstructorDecl>(_current_function_decl);
  bool is_dtor = clang::isa<clang::CXXDestructorDecl>(_current_function_decl);
  if (is_ctor || is_dtor) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_1_1, stmt);
    std::string ref_msg = "ctor and dtor cannot use dynamic type";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * QUAN ZHI temp
 */
void MISRAStmtRule::CheckBitwiseWithOutParen(const clang::BinaryOperator *stmt) {
  if (!stmt->isBitwiseOp()) return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.empty()) return;
  auto parent = parents[0].get<clang::Stmt>();

  if (parent && clang::isa<clang::ParenExpr>(parent)) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_99_99_99, stmt);
  std::string ref_msg = "Parentheses is required with bitwise operator";
  issue->SetRefMsg(ref_msg);
}


/* MISRA
 * Rule: 12.4
 * Evaluation of constant expressions should not lead to unsigned integer wrap-around
 */
void MISRAStmtRule::CheckUnsignedIntWrapAround(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (!lhs->getType()->isUnsignedIntegerType() || !rhs->getType()->isUnsignedIntegerType()) return;

  auto lhs_literal = clang::dyn_cast<clang::IntegerLiteral>(lhs);
  auto rhs_literal = clang::dyn_cast<clang::IntegerLiteral>(rhs);
  if (!lhs_literal || !rhs_literal) return;

  const auto UN_MAX = 0xffffffffu;
  using Opcode = clang::BinaryOperator::Opcode;

  bool need_report = false;
  auto opcode = stmt->getOpcode();
  auto lhs_val = lhs_literal->getValue().getZExtValue();
  auto rhs_val = rhs_literal->getValue().getZExtValue();
  if (opcode == Opcode::BO_Add || opcode == Opcode::BO_AddAssign) {
    if ((lhs_val + rhs_val) > UN_MAX) need_report = true;
  } else if (opcode == Opcode::BO_Mul || opcode == Opcode::BO_MulAssign) {
    if ((lhs_val * rhs_val) > UN_MAX) need_report = true;
  } else if (opcode == Opcode::BO_Shl || opcode == Opcode::BO_ShlAssign) {
    if ((lhs_val << rhs_val) > UN_MAX) need_report = true;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_12_4, stmt);
    std::string ref_msg = "Evaluation of constant expressions should not lead to unsigned integer wrap-around";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 3-1-2
 * Functions shall not be declared at block scope.
 */
void MISRAStmtRule::CheckFunctionDeclInBlock(const clang::DeclStmt *stmt) {
  if (!stmt->isSingleDecl()) return;
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.empty()) return;
  auto parent = parents[0].get<clang::Stmt>();
  if (auto compound_stmt = clang::dyn_cast<clang::CompoundStmt>(parent)) {
    if (clang::isa<clang::FunctionDecl>(stmt->getSingleDecl())) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_3_1_2, stmt);
      std::string ref_msg = "Functions shall not be declared at block scope.";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * MISRA: 4-5-1
 * Expressions with type bool shall not be used as operands to built-in operators
 * other than the assignment operator =, the logical operators &&, ||, !, the
 * equality operators == and !=, the unary & operator, and the conditional operator.
 */
void MISRAStmtRule::CheckBoolUsedAsNonLogicalOperand(const clang::BinaryOperator *stmt) {
  if (stmt->isLogicalOp() || stmt->isAssignmentOp()) return;
  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = stmt->getRHS()->IgnoreParenImpCasts()->getType();
  if (!lhs_type->isBooleanType() && !rhs_type->isBooleanType()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_4_5_1, stmt);
  std::string ref_msg = "Bool shall be used as logical operands";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: A4-10-2
 * Literal zero (0) shall not be used as the null-pointer-constant.
 * `-ImplicitCastExpr 0x7ff60a850ab0 <col:5> 'int *' <NullToPointer>
 *   `-IntegerLiteral 0x7ff60a850a18 <col:5> 'int' 0
 */
void MISRAStmtRule::CheckUsingNullWithPointer(const clang::ImplicitCastExpr *stmt) {
  auto ctx = XcalCheckerManager::GetAstContext();
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();

  auto k1 = stmt->isNullPointerConstant(*ctx, clang::Expr::NullPointerConstantValueDependence::NPC_NeverValueDependent);
  auto k2 = sub_expr->isNullPointerConstant(*ctx, clang::Expr::NullPointerConstantValueDependence::NPC_NeverValueDependent);
  auto zero_literal = clang::Expr::NullPointerConstantKind::NPCK_ZeroLiteral;

  if (k1 == zero_literal && k2 == zero_literal) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_4_10_2, stmt);
    std::string ref_msg = "Using NULL to stand a nullptr instead of using 0";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 6-4-1
 * An if ( condition ) construct shall be followed by a compound statement.
 * The else keyword shall be followed by either a compound statement, or
 * another if statement.
 */
void MISRAStmtRule::CheckIfBrace(const clang::IfStmt *stmt) {
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
        issue = report->ReportIssue(MISRA, M_R_6_4_1, stmt);
        std::string ref_msg = "if/else block must be enclosed in braces";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddStmt(&(*it));
    }
  }
}

/* MISRA
 * Rule: 5-2-3
 * cast from base class to derived class cannot have polymorphic type
 */
void MISRAStmtRule::CheckDownCastToDerivedClass(const clang::CastExpr *stmt) {
  auto origin_type = stmt->getSubExpr()->getType();
  auto target_type = stmt->getType();
  if (origin_type->isPointerType() && target_type->isPointerType()) {
    origin_type = origin_type->getPointeeType();
    target_type = target_type->getPointeeType();
  }
  if (!origin_type->isRecordType() || !target_type->isRecordType()) return;

  auto origin_record_ty = clang::dyn_cast<clang::RecordType>(origin_type);
  auto target_record_ty = clang::dyn_cast<clang::RecordType>(target_type);
  if (!origin_record_ty || !target_record_ty) return;
  if (!origin_record_ty->isClassType() || !target_record_ty->isClassType()) return;

  auto origin_class = clang::dyn_cast<clang::CXXRecordDecl>(origin_record_ty->getDecl());
  auto target_class = clang::dyn_cast<clang::CXXRecordDecl>(target_record_ty->getDecl());
  if (!origin_class || !target_class) return;

  // check if this class is polymorphic
  if (!origin_class->isPolymorphic()) return;

  if (target_class->isDerivedFrom(origin_class)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_5_2_3, stmt);
    std::string ref_msg = "Cast from base class to derived class cannot have polymorphic type";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 6-5-2
 * If loop-counter is not modified by -- or ++, then, within condition, the loop-counter
 * shall only be used as an operand to <=, <, > or >=.
 */
void MISRAStmtRule::CheckForStmtLoopCounter(const clang::ForStmt *stmt) {
  auto init = stmt->getInit();
  if (init == nullptr) return;
  auto cond = stmt->getCond();
  if (cond == nullptr) return;
  auto inc = stmt->getInc();
  if (inc == nullptr) return;

  // get loop-counter
  const clang::VarDecl *loop_counter = nullptr;
  if (auto bin_op = clang::dyn_cast<clang::BinaryOperator>(init)) {
    if (!bin_op->isAssignmentOp()) return;
    auto lhs = bin_op->getLHS()->IgnoreParenImpCasts();
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(lhs))
      loop_counter = clang::dyn_cast<clang::VarDecl>(decl_ref->getDecl());
  } else if (auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(init)) {
    loop_counter = clang::dyn_cast<clang::VarDecl>(decl_stmt->getSingleDecl());
  }
  if (loop_counter == nullptr) return;


  if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(inc)) {
    if (unary_op->isIncrementDecrementOp()) return;
  }

  if (auto bin_op = clang::dyn_cast<clang::BinaryOperator>(cond)) {
    if (bin_op->isComparisonOp()) {
      auto op = bin_op->getOpcode();
      if (op >= clang::BO_LT && op <= clang::BO_GE) return;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_6_5_2, stmt);
  std::string ref_msg = "If loop-counter is not modified by -- or ++, then, within condition, the loop-counter"
                        "shall only be used as an operand to <=, <, > or >=";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: 7-5-3
 * A function shall not return a reference or a pointer to a parameter that is
 * passed by reference or const reference.
 */
void MISRAStmtRule::CheckReturnParamRefOrPtr(const clang::ReturnStmt *stmt) {
  auto val = stmt->getRetValue()->IgnoreParenImpCasts();
  auto type = XcalCheckerManager::GetCurrentFunction()->getReturnType();
  if (!type->isPointerType() && !type->isReferenceType()) return;

  if (type->isPointerType()) {
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(val)) {
      if (auto decl = clang::dyn_cast<clang::ParmVarDecl>(decl_ref->getDecl())) {
        // ignore int *f(int *a) { return a; }
        if (type == decl->getType()) return;
      } else return;
    } else if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(val)) {
      if (unary_op->getOpcode() != clang::UO_AddrOf) return;
      if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(unary_op->getSubExpr())) {
        if (!clang::isa<clang::ParmVarDecl>(decl_ref->getDecl())) return;
      } else return;
    } else return;
  } else {
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(val)) {
      if (!clang::isa<clang::ParmVarDecl>(decl_ref->getDecl()))
        return;
    } else return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_7_5_3, stmt);
  std::string ref_msg = "A function shall not return a reference or a pointer to a parameter that is passed "
                        "by reference or const reference.";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: 15-0-2
 * An exception object should not have pointer type.
 */
void MISRAStmtRule::CheckThrowPointer(const clang::CXXThrowExpr *stmt) {
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  if (sub_expr && sub_expr->getType()->isPointerType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(MISRA, M_R_15_0_2, stmt);
    std::string ref_msg = "An exception object should not have pointer type.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 15-1-1
 * The assignment-expression of a throw statement shall not itself cause an exception to be thrown.
 */
void MISRAStmtRule::CheckThrowExceptionItselfHasThrow(const clang::CXXThrowExpr *stmt) {
  auto sub_expr = stmt->getSubExpr();
  if (sub_expr == nullptr) return;
  if (auto tmp_obj = clang::dyn_cast<clang::CXXConstructExpr>(sub_expr)) {
    std::vector<const clang::Stmt *> sinks;

    auto record_decl = tmp_obj->getConstructor()->getParent();
    for (const auto &ctor : record_decl->ctors()) {
      if (!ctor->doesThisDeclarationHaveABody()) continue;
      for (const auto &it : ctor->getBody()->children()) {
        if (clang::isa<clang::CXXThrowExpr>(it)) {
          sinks.push_back(it);
        }
      }
    }

    if (!sinks.empty()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(MISRA, M_R_15_1_1, stmt);
      std::string ref_msg = "The assignment-expression of a throw statement shall not itself cause an exception to be thrown.";
      issue->SetRefMsg(ref_msg);
      for (const auto &sink : sinks) issue->AddStmt(sink);
    }
  }
}

/*
 * MISRA: 15-1-2
 * NULL shall not be thrown explicitly.
 */
void MISRAStmtRule::CheckThrowNullExpr(const clang::CXXThrowExpr *stmt) {
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  if (sub_expr && (sub_expr->getStmtClass() == clang::Stmt::GNUNullExprClass)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(MISRA, M_R_15_1_2, stmt);
    std::string ref_msg = "NULL shall not be thrown explicitly.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 15-1-3
 * An empty throw (throw;) shall only be used in the compound-statement of a catch handler.
 */
void MISRAStmtRule::CheckEmptyThrowInNonCatchBlock(const clang::CXXThrowExpr *stmt) {
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  if (sub_expr) return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  while (!parents.empty()) {
    auto parent = parents[0];
    if (parent.get<clang::Decl>()) break;
    else {
      auto expr = parent.get<clang::Stmt>();
      if (clang::isa<clang::CXXCatchStmt>(expr)) return;
      else parents = ctx->getParents(*expr);
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(MISRA, M_R_15_1_3, stmt);
  std::string ref_msg = "An empty throw (throw;) shall only be used in the compound-statement of a catch handler.";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: 15-3-2
 * There should be at least one exception handler to catch all otherwise unhandled exceptions
 */
void MISRAStmtRule::CheckTryWithoutDefaultCatch(const clang::CXXTryStmt *stmt) {
  for (const auto it : stmt->children()) {
    if (it == stmt->getTryBlock()) continue;
    if (auto catch_case = clang::dyn_cast<clang::CXXCatchStmt>(it)) {
      if (catch_case->getExceptionDecl() == nullptr) return;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(MISRA, M_R_15_3_2, stmt);
  std::string ref_msg = "There should be at least one exception handler to catch all otherwise unhandled exceptions";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: 15-3-4
 * Each specified throw must have a matching catch
 * TODO: need refine
 */
void MISRAStmtRule::CheckMissingCatchStmt(const clang::CXXTryStmt *stmt) {
  auto obj_types = RecordThrowObjectTypes(stmt->getTryBlock());
  for (const auto &it : stmt->children()) {
    if (it == stmt->getTryBlock()) continue;
    if (auto catch_case = clang::dyn_cast<clang::CXXCatchStmt>(it)) {
      if (!catch_case->getExceptionDecl()) continue;

      auto catch_type = catch_case->getCaughtType();
      if (auto ref_type = clang::dyn_cast<clang::ReferenceType>(catch_type)) {
        catch_type = ref_type->getPointeeType();
      } else if (auto pointer_type = clang::dyn_cast<clang::PointerType>(catch_type)) {
        catch_type = pointer_type->getPointeeType();
      }


      auto res = std::find_if(obj_types.begin(), obj_types.end(), [&catch_type](clang::QualType type) {
//        bool qual = catch_type.getQualifiers() == type.getQualifiers();
        bool res = false;
        if (catch_type->isBuiltinType()) {
          if (catch_type->getTypeClass() == type->getTypeClass()) res = true;
        } else if (catch_type->isRecordType()) {
          auto catch_record_tp = clang::cast<clang::RecordType>(catch_type);
          if (auto record_tp = clang::dyn_cast<clang::RecordType>(type)) {
            if (record_tp->getDecl()->getNameAsString() == catch_record_tp->getDecl()->getNameAsString()) {
              res = true;
            }
          }
        }
        return res;
      });

      if (res != obj_types.end()) obj_types.erase(res);

    }
  }

  if (!obj_types.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(MISRA, M_R_15_3_4, stmt);
    std::string ref_msg = "Each specified throw must have a matching catch";
    issue->SetRefMsg(ref_msg);
  }
}


/*
 * MISRA: 15-3-5
 * A class type exception shall always be caught by reference.
 */
void MISRAStmtRule::CheckCatchTypeNotReference(const clang::CXXCatchStmt *stmt) {
  if (!stmt->getExceptionDecl()) return;

  if (!stmt->getCaughtType()->isReferenceType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(MISRA, M_R_15_3_5, stmt);
    std::string ref_msg = "Exception objects should be catched as reference";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 15-5-1
 * A class destructor shall not exit with an exception.
 */
void MISRAStmtRule::CheckDTorExitWithThrow(const clang::CXXThrowExpr *stmt) {
  if (!_current_function_decl) return;
  auto dtor = clang::dyn_cast<clang::CXXDestructorDecl>(_current_function_decl);
  if (!dtor) return;

  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  while (!parents.empty()) {
    auto parent = parents.begin();
    if (parent->get<clang::Decl>()) break;
    if (auto p_expr = parent->get<clang::Stmt>()) {
      if (clang::isa<clang::CXXTryStmt>(p_expr)) {
        auto try_stmt = clang::cast<clang::CXXTryStmt>(p_expr);
        if (try_stmt->getNumHandlers() == 0) {
          break;
        }
        return;
      } else {
        parents = ctx->getParents(*p_expr);
      }
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  issue = report->ReportIssue(MISRA, M_R_15_5_1, stmt);
  std::string ref_msg = "A class destructor shall not exit with an exception.";
  issue->SetRefMsg(ref_msg);
}

/*
 * MISRA: 15-5-2
 * Where a function’s declaration includes an exception- specification, the function
 * shall only be capable of throwing exceptions of the indicated type(s).
 */
void MISRAStmtRule::CollectThrowType(const clang::CXXThrowExpr *stmt) {
  if (!_current_function_decl) return;
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto sub_expr = stmt->getSubExpr();
  if (sub_expr) scope_mgr->GlobalScope()->AddThrowType(_current_function_decl, sub_expr->getType());
}

void MISRAStmtRule::CollectThrowType(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (!callee) return;
  auto scope_mgr = XcalCheckerManager::GetScopeManager();

  auto callee_thrown_types = scope_mgr->GlobalScope()->GetThrowType(callee);
  for (const auto &it : callee_thrown_types) scope_mgr->GlobalScope()->AddThrowType(_current_function_decl, it);
}


}
}
