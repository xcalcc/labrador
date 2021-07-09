/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include <unordered_map>
#include <unordered_set>
#include <clang/AST/Decl.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/ASTContext.h>
#include <llvm/ADT/APFloat.h>

#include "GJB8114_enum.inc"
#include "GJB8114_decl_rule.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

// check if function has new expr
bool GJB8114DeclRule::HasNewExpr(const clang::Stmt *stmt) {

  bool res = false;
  for (const auto &it : stmt->children()) {
    // return true if this statement is new expr
    if (it->getStmtClass() == clang::Stmt::StmtClass::CXXNewExprClass) return true;

    if (it->child_begin() == it->child_end()) continue;
    for (const auto &sub : stmt->children()) {
      res |= HasNewExpr(sub);
      if (res) break;
    }
  }
  return res;
}

/*
 * GJB8114: 5.1.1.8
 * identifier is a must in declaration of struct, enum and union
 */
void GJB8114DeclRule::CheckAnonymousRecord(const clang::RecordDecl *decl) {
  auto name = decl->getNameAsString();
  if (name.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
    std::string ref_msg = "Identifier is a must in declaration of struct, enum and union";
    issue->SetRefMsg(ref_msg);
  }
}

void GJB8114DeclRule::CheckAnonymousEnum(const clang::EnumDecl *decl) {
  auto name = decl->getNameAsString();
  if (name.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
    std::string ref_msg = "Identifier is a must in declaration of struct, enum and union";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.1.1.9
 * Anonymous struct in struct is forbidden
 */
void GJB8114DeclRule::CheckAnonymousStructInRecord(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  std::unordered_map<std::string, clang::RecordDecl *> records;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->decls()) {
    if (auto record = clang::dyn_cast<clang::RecordDecl>(it)) {
      auto name = record->getNameAsString();
      if (name == decl->getNameAsString()) continue;
      records.insert({name, record});
    }
  }

  for (const auto &it : decl->fields()) {
    if (!it->getType()->isRecordType()) continue;
    auto field_name = it->getType().getAsString();
    auto pos = field_name.find("struct");
    auto record_name = field_name.substr(pos + 7);
    auto res = records.find(record_name);
    if (res != records.end()) {
      records.erase(res);
    }
  }

  if (records.empty()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_8, decl);
  std::string ref_msg = "Anonymous struct in struct is forbidden";
  issue->SetRefMsg(ref_msg);
  for (const auto &it : records) {
    issue->AddDecl(it.second);
  }

}

/*
 * GJB8114: 5.1.1.12
 * Bit-fields should be the same length and within the length of its origin type
 */
void GJB8114DeclRule::CheckUniformityOfBitFields(const clang::RecordDecl *decl) {
  if (decl->field_empty()) return;
  auto ctx = XcalCheckerManager::GetAstContext();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  bool need_report = false;
  unsigned length = 0;
  for (const auto &it : decl->fields()) {
    if (!it->isBitField()) continue;
    unsigned width = it->getBitWidthValue(*ctx);
    if (length == 0) {
      length = width;
    } else if (length != width) {
      need_report = true;
    } else {
      auto f_type = it->getType();
      auto typeInfo = ctx->getTypeInfo(f_type);
      auto origin_width = typeInfo.Width;
      if (origin_width < width) {
        need_report = true;
      }
    }

    if (need_report) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G5_1_1_12, decl);
        std::string ref_msg = "Bit-fields should be the same length and within the length of its origin type";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(it);
      need_report = false;
    }
  }
}

/*
 * GJB8114: 5.1.1.17
 * Using "extern" variable in function is forbidden
 */
void GJB8114DeclRule::CheckExternVariableInFunction(const clang::VarDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto decl_ctx = decl->isLocalVarDeclOrParm();
  if (!const_cast<clang::VarDecl *>(decl)->isLocalExternDecl()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_17, decl);
  std::string ref_msg = "Using \"extern\" variable in function is forbidden";
  issue->SetRefMsg(ref_msg);
}

void GJB8114DeclRule::CheckExternVariableInFunction(const clang::FunctionDecl *decl) {
  auto decl_ctx = decl->getParent();
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (!const_cast<clang::FunctionDecl *>(decl)->isLocalExternDecl()) return;
  issue = report->ReportIssue(GJB8114, G5_1_1_17, decl);
  std::string ref_msg = "Using \"extern\" variable in function is forbidden";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 5.1.2.4
 * Variables should be declared at the beginning of function body
 */
void GJB8114DeclRule::CheckVariableDeclPosition(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (!decl->doesThisDeclarationHaveABody()) return;
  bool hit_stmt = false;
  for (const auto &it : decl->getBody()->children()) {
    if (it->getStmtClass() == clang::Stmt::StmtClass::DeclStmtClass) {
      if (!hit_stmt) continue; // continue if not meet other statement
      else {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB8114, G5_1_2_4, decl);
          std::string ref_msg = "Variables should be declared at the beginning of function body: ";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        if (auto decl_stmt = clang::dyn_cast<clang::DeclStmt>(it)) {
          if (decl_stmt->isSingleDecl()) {
            auto it_decl = clang::dyn_cast<clang::DeclStmt>(it)->getSingleDecl();
            issue->AddDecl(it_decl);
          } else {
            for (const auto &it_decl : decl_stmt->decls()) {
              issue->AddDecl(&(*it_decl));
            }
          }
        }
      }
    } else hit_stmt = true;
  }
}

/*
 * GJB8114: 5.1.2.5
 * Struct should not nest more than three levels
 */
void GJB8114DeclRule::CheckNestedStructure(const clang::RecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  if (decl->decls_empty()) return;

  for (const auto &it : decl->decls()) {
    if (auto record = clang::dyn_cast<clang::RecordDecl>(it)) {
      for (const auto &sub_it : record->decls()) {
        if (clang::dyn_cast<clang::RecordDecl>(sub_it)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_1_2_5, decl);
            std::string ref_msg = "Struct should not nest more than three levels: ";
            ref_msg += decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
          issue->AddDecl(sub_it);
        }
      }
    }
  }
}

/*
 * GJB8114: 5.3.1.7
 * Pointers should be initialized as NULL
 */
void GJB8114DeclRule::CheckPointerInitWithNull(const clang::VarDecl *decl) {
  if (!decl->getType()->isPointerType()) return;

  auto ctx = XcalCheckerManager::GetAstContext();

  if (decl->hasInit()) {
    auto init_val = decl->getInit()->IgnoreParenImpCasts();
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(init_val)) {
      int value;
      clang::Expr::EvalResult eval_result;

      // try to fold the const expr
      if (literal->EvaluateAsInt(eval_result, *ctx)) {
        value = eval_result.Val.getInt().getZExtValue();
      } else {
        value = literal->getValue().getZExtValue();
      }

      // return if pointer has been initialized with NULL
      if (value == 0) return;
    }
  }

  // report issue
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB8114, G5_3_1_7, decl);
  std::string ref_msg = "Pointers should be initialized as NULL";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 5.3.2.2
 * Using void pointer carefully
 */
void GJB8114DeclRule::CheckVoidPointer(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isPointerType()) return;
  if (type->getPointeeType()->isVoidType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_3_2_2, decl);
    std::string ref_msg = "Using void pointer carefully: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 5.3.2.2
 * Using void pointer carefully
 */
void GJB8114DeclRule::CheckVoidPointer(const clang::FunctionDecl *decl) {
  if (decl->param_empty()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &it : decl->parameters()) {
    if (!it->getType()->isPointerType()) continue;
    if (it->getType()->getPointeeType()->isVoidType()) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G5_3_2_2, decl);
        std::string ref_msg = "Using void pointer carefully: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(&(*it));
    }
  }
}

/*
 * GJB8114: 5.7.1.13
 * static function must be used
 */
void GJB8114DeclRule::CheckUnusedStaticFunction() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::FUNCTION,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        auto func = clang::dyn_cast<clang::FunctionDecl>(decl);
        if (!func->isStatic()) return;

        if (!func->isUsed()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_7_1_13, decl);
            std::string ref_msg = "static function must be used";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.7.2.1
 * Avoid using too much parameters in function
 */
void GJB8114DeclRule::CheckTooManyParams(const clang::FunctionDecl *decl) {
  if (decl->param_size() > 10) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G5_7_2_1, decl);
    std::string ref_msg = "Avoid using too much parameters(more than 10) in function";
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
void GJB8114DeclRule::CheckLiteralSuffixInit(const clang::VarDecl *decl) {
  if (!decl->hasInit()) return;
  auto stmt = decl->getInit();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  char ch;
  auto data = src_mgr->getCharacterData(stmt->getBeginLoc());
  do {
    ch = *data++;
    if (ch == '.') continue;
    if (std::isdigit(ch)) {
      continue;
    } else if (std::isalpha(ch)) {
      if (std::isupper(ch)) return;
      issue = report->ReportIssue(GJB8114, G5_8_1_5, stmt);
      std::string ref_msg = "Suffix of number must use upper case letters";
      issue->SetRefMsg(ref_msg);
    } else {
      issue = report->ReportIssue(GJB8114, G5_8_2_4, stmt);
      std::string ref_msg = "Using suffix with number is recommended";
      issue->SetRefMsg(ref_msg);
    }
    break;
  } while (true);
}

/*
 * GJB8114: 5.11.1.2
 * Omitting init value which depends on the system is forbidden
 */
void GJB8114DeclRule::CheckOmitInitValueDependOnSystem() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  // check static local variable
  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (varDecl->isStaticLocal() || varDecl->hasGlobalStorage()) {
          if (!varDecl->hasInit()) {
            if (issue == nullptr) {
              issue = report->ReportIssue(GJB8114, G5_11_1_2, decl);
              std::string ref_msg = "Omitting init value which depends on the system is forbidden";
              issue->SetRefMsg(ref_msg);
            } else {
              issue->AddDecl(decl);
            }
          }
        }
      }, true);

}

/*
 * GJB8114: 5.11.2.1
 * Init the variable at its declaration
 */
void GJB8114DeclRule::CheckInitWithDecl() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  // check static local variable
  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (!varDecl->hasInit()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_11_2_1, decl);
            std::string ref_msg = "Init the variable at its declaration";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.13.1.3
 * Variable names conflicting with function names is forbidden
 */
void GJB8114DeclRule::CheckVariableConflictWithFunction() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (top_scope->HasFunctionName(varDecl->getNameAsString())) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_13_1_3, decl);
            std::string ref_msg = "Variable names conflicting with function names is forbidden";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.13.1.4
 * Variable names conflicting with identifiers in forbidden
 */
void GJB8114DeclRule::CheckVariableConflictWithIdentifiers() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (top_scope->HasRecordName<true>(varDecl->getNameAsString())) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_13_1_4, decl);
            std::string ref_msg = "Variable names conflicting with identifiers in forbidden";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.13.1.5
 * Variable names conflicting with enum elements is forbidden
 */
void GJB8114DeclRule::CheckVariableConflictWithEnumElem() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (top_scope->HasValueName<true>(varDecl->getNameAsString())) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_13_1_5, decl);
            std::string ref_msg = "Variable names conflicting with enum elements is forbidden";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 5.13.1.6
 * Variable names conflict with typedef value is forbidden
 */
void GJB8114DeclRule::CheckVariableConflictWithTypeDef() {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::IdentifierKind::VAR,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        auto varDecl = clang::dyn_cast<clang::VarDecl>(decl);
        if (top_scope->HasTypeDef<true>(varDecl->getNameAsString())) {
          if (issue == nullptr) {
            issue = report->ReportIssue(GJB8114, G5_13_1_6, decl);
            std::string ref_msg = "Variable names conflict with typedef value is forbidden";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/*
 * GJB8114: 6.1.1.1
 * Copy construct function is a must for classes which has dynamic allocated memory members
 */
void GJB8114DeclRule::CheckCopyConstructor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->hasTrivialCopyConstructor()) return;

  bool hasNewExpr = false;
  for (const auto &it : decl->methods()) {
    if (it->getDeclKind() == clang::Decl::Kind::CXXConstructor) {
      if (!it->doesThisDeclarationHaveABody()) continue;
      auto body = clang::dyn_cast<clang::CompoundStmt>(it->getBody());
      if (body) {
        hasNewExpr |= HasNewExpr(body);
        if (hasNewExpr) break;
      }
    }
  }

  if (hasNewExpr) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_1_1_1, decl);
    std::string ref_msg = "Copy construct function is a must for classes which has dynamic allocated memory members";
    issue->SetRefMsg(ref_msg);
  }

}

/*
 * GJB8114: 6.1.1.3
 * "virtual" is needed when inheriting from base class in derivative design of diamond structure.
 */
void GJB8114DeclRule::CheckDiamondDerivativeWithoutVirtual(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  // base class of LHS and RHS
  std::unordered_map<clang::CXXRecordDecl *, clang::CXXRecordDecl *> bases;
  for (const auto &it : decl->bases()) {
    auto record_type = clang::dyn_cast<clang::RecordType>(it.getType());
    if (!record_type) continue;
    auto record_decl = record_type->getAsCXXRecordDecl();
    if (!record_decl) continue;

    if (!record_decl->hasDefinition()) continue;
    if (record_decl->getNumBases() != 1) continue;

    auto base = record_decl->bases_begin()->getType()->getAsCXXRecordDecl();
    if (!base) return;

    // continue if record is virtully derived from base class
    if (record_decl->isVirtuallyDerivedFrom(base)) continue;
    bases.insert({record_decl, base});
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : bases) {
    auto count = std::count_if(bases.begin(), bases.end(), [&it](auto &pair) {
      if (pair.second == it.second) return true;
      return false;
    });
    if (count > 1) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G6_1_1_3, decl);
        std::string ref_msg = "\"virtual\" is needed when inheriting from base class in derivative design of diamond structure.";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(it.first);
    }
  }

}

/*
 * GJB8114: 6.1.1.4
 * Overloaded assigment operator in abstract classes should be private or protect.
 */
void GJB8114DeclRule::CheckAssignOperatorOverload(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (!decl->hasUserDeclaredCopyAssignment()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto method : decl->methods()) {
    if (!method->isCopyAssignmentOperator()) continue;
    if (method->getAccess() == clang::AccessSpecifier::AS_public) {
      if (issue == nullptr) {
        issue = report->ReportIssue(GJB8114, G6_1_1_4, decl);
        std::string ref_msg = "Overloaded assigment operator in abstract classes should be private or protect.";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(method);
    }
  }
}

/*
 * GJB8114: 6.1.2.1
 * Deriving from virtual base class should be carefully
 */
void GJB8114DeclRule::CheckDerivedFromAbstractClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumVBases() == 0) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(GJB8114, G6_1_2_1, decl);
  std::string ref_msg = "Deriving from virtual base class should be carefully";
  issue->SetRefMsg(ref_msg);
}

/*
 * GJB8114: 6.1.2.2
 * Using inline functions carefully
 */
void GJB8114DeclRule::CheckInlineFunction(const clang::FunctionDecl *decl) {
  if (decl->getDeclKind() == clang::Decl::Kind::CXXConstructor) return;

  if (decl->isInlined()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_1_2_2, decl);
    std::string ref_msg = "Using inline functions carefully";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 6.2.1.2
 * Default construct functions should be defined explicitly in class
 */

void GJB8114DeclRule::CheckExplictConstructor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (!decl->hasUserDeclaredConstructor()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_2_1_2, decl);
    std::string ref_msg = "Default construct functions should be defined explicitly in class";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 6.2.1.3
 * Construct functions which contains only one parameter should be note by "explicit"
 */
void GJB8114DeclRule::CheckExplicitConstructorWithSingleParam(const clang::FunctionDecl *decl) {
  if (auto constructor = clang::dyn_cast<clang::CXXConstructorDecl>(decl)) {
    if (constructor->param_size() != 1) return;

    if (constructor->isExplicit()) return;
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_2_1_3, decl);
    std::string ref_msg = "Construct functions which contains only one parameter should be note by \"explicit\"";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * GJB8114: 6.2.1.4
 * All class members should be initialized in constructor
 */
void GJB8114DeclRule::CheckInitFieldsInConstructor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->field_empty()) return;

  std::unordered_set<clang::FieldDecl *> fields;
  for (const auto &field : decl->fields()) {
    fields.insert(field);
  }

  bool need_report = false;
  std::vector<clang::CXXConstructorDecl *> sinks;
  for (const auto &method : decl->methods()) {
    if (auto constructor = clang::dyn_cast<clang::CXXConstructorDecl>(method)) {
      auto tmp = fields;
      for (const auto init : constructor->inits()) {
        auto it = tmp.find(init->getMember());
        if (it != tmp.end()) tmp.erase(it);
      }

      // break if all fields have been initialized
      if (tmp.empty()) continue;

      // check if initialized in function body
      if (constructor->doesThisDeclarationHaveABody()) {
        for (const auto &stmt : constructor->getBody()->children()) {
          // return if it is not BinaryOperator
          if (stmt->getStmtClass() != clang::Stmt::StmtClass::BinaryOperatorClass) continue;
          auto binary_stmt = clang::dyn_cast<clang::BinaryOperator>(stmt);

          // return if it is not assignment
          if (!binary_stmt->isAssignmentOp() && !binary_stmt->isCompoundAssignmentOp()) continue;
          auto lhs = binary_stmt->getLHS()->IgnoreParenImpCasts();

          // check if lhs is field declaration
          if (auto decl_reference = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
            if (auto field_decl = clang::dyn_cast<clang::FieldDecl>(decl_reference->getDecl())) {
              auto it = tmp.find(field_decl);
              if (it != tmp.end()) tmp.erase(it);
            }
          }
        }
      }

      if (!tmp.empty()) {
        need_report = true;
        sinks.push_back(constructor);
      }
    }

  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_2_1_4, decl);
    std::string ref_msg = "All class members should be initialized in constructor";
    issue->SetRefMsg(ref_msg);
    for (const auto &sink : sinks) {
      issue->AddDecl(sink);
    }
  }
}

/*
 * GJB8114: 6.2.1.5
 * Derived class should contain constructor of base class
 */
void GJB8114DeclRule::CheckDerivedClassContainConstructorOfBaseClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() == 0) return;

  // get base class
  std::unordered_set<clang::CXXRecordDecl *> bases;
  for (const auto &base : decl->bases()) {
    auto record_decl = clang::dyn_cast<clang::RecordType>(base.getType())->getAsCXXRecordDecl();
    bases.insert(record_decl);
  }

  bool need_report = false;
  std::vector<clang::CXXConstructorDecl *> sinks;
  if (!decl->hasUserDeclaredConstructor()) {
    need_report = true;
  } else {
    for (const auto &method : decl->methods()) {
      auto tmp = bases;
      if (auto constructor = clang::dyn_cast<clang::CXXConstructorDecl>(method)) {
        for (const auto init : constructor->inits()) {
          auto expr = init->getInit();
          if (expr == nullptr) continue;
          if (auto constructor_expr = clang::dyn_cast<clang::CXXConstructExpr>(expr)) {

            // check if this is implicit default constructor
            if (constructor_expr->getBeginLoc() == constructor_expr->getEndLoc()) {
              continue;
            }

            auto parent = constructor_expr->getConstructor()->getParent();
            auto it = tmp.find(parent);
            if (it != tmp.end()) tmp.erase(it);
          }
        }

        if (!tmp.empty()) {
          need_report = true;
          sinks.push_back(constructor);
        }
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_2_1_5, decl);
    std::string ref_msg = "Derived class should contain constructor of base class";
    issue->SetRefMsg(ref_msg);
    for (const auto &sink : sinks) {
      issue->AddDecl(sink);
    }
  }
}

/*
 * GJB8114: 6.3.1.1
 * Destruct function of classes which contain the virtual functions should be virtual
 */
void GJB8114DeclRule::CheckVirtualDestructor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  bool has_virtual = false;
  clang::CXXMethodDecl *sink;
  for (const auto &method: decl->methods()) {
    if (auto destructor = clang::dyn_cast<clang::CXXDestructorDecl>(method)) {
      if (destructor->isVirtual()) return;
      sink = destructor;
    }

    if (method->isVirtual()) has_virtual = true;
  }

  if (has_virtual) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(GJB8114, G6_3_1_1, decl);
    std::string ref_msg = "Destruct functon of classes which contain the virtual functions should be virtual";
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(sink);
  }
}

/*
 * GJB8114: 6.4.1.1
 * Default parameters in virtual function of base shouldn't be changed by derived classes
 */
void GJB8114DeclRule::CheckDefaultParamChangedInDerivedClassVirtualMethod(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() == 0) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto ctx = XcalCheckerManager::GetAstContext();

  bool need_report = false;
  std::unordered_map<const clang::CXXMethodDecl *, const clang::CXXMethodDecl *> sinks;
  for (const auto &method : decl->methods()) {
    // get current method's parameters
    if (method->param_empty()) continue;
    std::vector<clang::Expr *> current_default_args;
    for (const auto &it : method->parameters()) {
      if (it->hasDefaultArg()) {
        current_default_args.push_back(it->getDefaultArg());
      }
    }

    // compare with its overridden methods
    for (const auto &it : method->overridden_methods()) {
      std::vector<clang::Expr *> tmp;
      if (it->param_empty()) continue;

      // collect parameters of overridden methods
      for (const auto &param : it->parameters()) {
        if (param->hasDefaultArg()) {
          auto default_arg = param->getDefaultArg();
          tmp.push_back(default_arg);
        }
      }

      // diff two parameter vectors
      // 1. check size
      if (current_default_args.size() != tmp.size()) {
        need_report = true;
      }

      // 2. compare each parameter
      for (int i = 0; i < current_default_args.size(); i++) {
        auto cur_param = current_default_args[i];
        auto over_param = tmp[i];
        if (cur_param->getStmtClass() != over_param->getStmtClass()) {
          need_report = true;
          sinks.insert({method, it});
          break;
        }

        auto stmt_class = cur_param->getStmtClass();
        if (stmt_class == clang::Stmt::StmtClass::CXXBoolLiteralExprClass) {
          bool b1, b2;
          if (cur_param->EvaluateAsBooleanCondition(b1, *ctx)) {
            if (over_param->EvaluateAsBooleanCondition(b2, *ctx)) {
              if (b1 != b2) {
                need_report = true;
                sinks.insert({method, it});
                break;
              }
            }
          }
        } else if (stmt_class == clang::Stmt::StmtClass::IntegerLiteralClass) {
          int i1, i2;
          clang::Expr::EvalResult e1, e2;

          // get as int
          if (cur_param->EvaluateAsInt(e1, *ctx)) {
            i1 = e1.Val.getInt().getExtValue();
          } else {
            continue;
          }

          if (over_param->EvaluateAsInt(e2, *ctx)) {
            i2 = e2.Val.getInt().getExtValue();
          } else {
            continue;
          }

          // compare two result
          if (i1 != i2) {
            need_report = true;
            sinks.insert({method, it});
            break;
          }
        } else if (stmt_class == clang::Stmt::StmtClass::FloatingLiteralClass) {
          llvm::APFloat e1(double(0)), e2(double(0));

          if (cur_param->EvaluateAsFloat(e1, *ctx)) {
            if (over_param->EvaluateAsFloat(e2, *ctx)) {
              if (e1 != e2) {
                need_report = true;
                sinks.insert({method, it});
                break;
              }
            }
          }
        } else {
          // TODO:
          TRACE0();
        }

      }
    }
  }

  if (need_report) {
    issue = report->ReportIssue(GJB8114, G6_4_1_1, decl);
    std::string ref_msg = "Default parameters in virtual function of base shouldn't be changed by derived classes";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) {
      issue->AddDecl(it.first);
      issue->AddDecl(it.second);
    }
  }
}

/*
 * GJB8114: 6.4.1.2
 * Overridden virtual functions in derived class should be noted with virtual
 */
void GJB8114DeclRule::CheckOverriddenVirtualFunction(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &method : decl->methods()) {
    if (method->isVirtualAsWritten()) continue;
    for (const auto &it : method->overridden_methods()) {
      if (it->isVirtual()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(GJB8114, G6_4_1_2, decl);
          std::string ref_msg = "Overridden virtual functions in derived class should be noted with virtual";
          issue->SetRefMsg(ref_msg);
          issue->AddDecl(method);
        }
      }
    }
  }
}

/*
 * GJB8114: 6.4.1.3
 * Non-pure virtual function being overridden as pure virtual function is forbidden
 */
void GJB8114DeclRule::CheckNonVirtualMethodOverriddenAsPure(const clang::CXXMethodDecl *decl) {
  if (!decl->isPure()) return;

  for (const auto &it : decl->overridden_methods()) {
    if (!it->isPure()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(GJB8114, G6_4_1_3, decl);
      std::string ref_msg = "Non-pure virtual function being overridden as pure virtual function is forbidden";
      issue->SetRefMsg(ref_msg);
      issue->AddDecl(it);
    }
  }
}


}
}
