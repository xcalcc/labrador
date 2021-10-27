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

#include "cert_stmt_rule.h"

namespace xsca {
namespace rule {


/*
 * DCL-30
 * Declare objects with appropriate storage durations
 */
clang::Decl *CERTStmtRule::GetLocalAddress(const clang::Stmt *stmt) {
  if (auto ref = clang::dyn_cast<clang::DeclRefExpr>(stmt)) {
    auto decl = ref->getDecl();
    if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
      if (var_decl->isLocalVarDeclOrParm()) {
        return (clang::Decl *) var_decl;
      }
    }
  } else if (auto unary = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
    if (unary->getOpcode() == clang::UnaryOperator::Opcode::UO_AddrOf) {
      if (auto ret_ref = clang::dyn_cast<clang::DeclRefExpr>(unary->getSubExpr())) {
        if (auto ref_decl = clang::dyn_cast<clang::VarDecl>(ret_ref->getDecl())) {
          if (ref_decl->isLocalVarDeclOrParm()) {
            return ref_decl;
          }
        }
      }
    }
  }
  return nullptr;
}

void CERTStmtRule::CheckStorageDuration(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp() && !stmt->isCompoundAssignmentOp()) return;
  auto lhs = stmt->getLHS();
  if (!lhs->getType()->isPointerType()) return;
  bool need_report = false;
  clang::Decl *sink = nullptr;
  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
    auto decl = decl_ref->getDecl();
    if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
      if (var_decl->hasGlobalStorage()) {
        // check if rhs is local pointer
        auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

        // return if not pointer or array type
        if (!rhs->getType()->isPointerType() && !rhs->getType()->isArrayType()) return;

        if (auto res = GetLocalAddress(rhs)) {
          need_report = true;
          sink = res;
        }
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue("CERT", "DCL-30", lhs);
    std::string ref_msg = "Declare objects with appropriate storage durations";
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(sink);
  }
}

void CERTStmtRule::CheckStorageDuration(const clang::ReturnStmt *stmt) {
  auto ret_value = stmt->getRetValue()->IgnoreParenImpCasts();
  if (ret_value == nullptr) return;
  if (!ret_value->getType()->isPointerType() && !ret_value->getType()->isArrayType()) return;
  if (auto res = GetLocalAddress(ret_value)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue("CERT", "DCL-30", stmt);
    std::string ref_msg = "Declare objects with appropriate storage durations";
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(res);
  }
}

}
}
