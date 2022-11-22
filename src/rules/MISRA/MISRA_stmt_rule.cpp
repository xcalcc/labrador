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

#include <clang/AST/DeclCXX.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/AST/ParentMapContext.h>
#include "MISRA_stmt_rule.h"

namespace xsca {
namespace rule {

std::string MISRAStmtRule::GetTypeString(clang::QualType type) {
  std::string type_name;
  if (type->getTypeClass() == clang::Type::Typedef) {
    auto underlying_tp =
        clang::dyn_cast<clang::TypedefType>(type)->getDecl()->getUnderlyingType();
    type_name = underlying_tp.getAsString();
  } else {
    type_name = type.getAsString();
  }
  return type_name;
}

clang::QualType MISRAStmtRule::GetRawTypeOfTypedef(const clang::QualType type) {
  clang::QualType res = type;
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
bool MISRAStmtRule::IsIntegerLiteralExpr(const clang::Expr *expr, uint64_t *res) {
  clang::Expr::EvalResult eval_result;
  auto ctx = XcalCheckerManager::GetAstContext();
  bool state = expr->EvaluateAsInt(eval_result, *ctx);
  if (state && res) *res = eval_result.Val.getInt().getZExtValue();
  return state;
}

// check if the stmt is volatile qualified
bool MISRAStmtRule::IsVolatileQualified(const clang::Stmt *stmt) {
  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(stmt)) {
    auto decl = decl_ref->getDecl();
    if (decl->getType().isVolatileQualified())
      return true;
  }
  return false;
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
XcalIssue *MISRAStmtRule::ReportTemplate(const std::string &str, const char *rule, const clang::Stmt *stmt) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, rule, stmt);
  issue->SetRefMsg(str);
  return issue;
}

/*
 * check if it is CaseStmt/DefaultStmt
 */
bool MISRAStmtRule::IsCaseStmt(const clang::Stmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto stmtClass = stmt->getStmtClass();
  if ((stmtClass == StmtClass::CaseStmtClass) ||
      (stmtClass == StmtClass::DefaultStmtClass)) {
    return true;
  }
  return false;
}

/*
 * check if the expr has specific stmt
 */
bool MISRAStmtRule::HasSpecificStmt(const clang::Stmt *stmt, std::function<bool(const clang::Stmt *)> check) {
  if (check(stmt)) {
    return true;
  }

  for (const auto &it : stmt->children()) {
    if (check(it)) return true;
  }
  return false;
}


/*
 * get builtin type of typedef
 */
clang::QualType MISRAStmtRule::GetUnderlyingType(clang::QualType *type) {
  return GetUnderlyingType(*type);
}

clang::QualType MISRAStmtRule::GetUnderlyingType(clang::QualType type) {
  if (auto type_def = clang::dyn_cast<clang::TypedefType>(type)) {
    return type_def->desugar();
  } else if (auto elaborated_type = clang::dyn_cast<clang::ElaboratedType>(type)) {
    return elaborated_type->desugar();
  } else if (auto substtmp_type = clang::dyn_cast<clang::SubstTemplateTypeParmType>(type)) {
    return substtmp_type->desugar();
  } else if (auto auto_type = clang::dyn_cast<clang::AutoType>(type)) {
    return auto_type->desugar();
  }
  return type;
}

// get builtin type kind
clang::BuiltinType::Kind MISRAStmtRule::GetBTKind(clang::QualType type, bool &status) {
  auto ud_type = GetUnderlyingType(type);
  auto prev_type = ud_type;
  while (!clang::isa<clang::BuiltinType>(ud_type)) {
    ud_type = GetUnderlyingType(ud_type);
    if (ud_type == prev_type) break;
    else prev_type = ud_type;
  }
  if (const clang::BuiltinType *bt = ud_type->getAs<clang::BuiltinType>()) {
    DBG_ASSERT(ud_type->isBuiltinType(), "This is not BuiltinType");
    status = true;
    return bt->getKind();
  } else {
    status = false;
    return clang::BuiltinType::Kind::Bool;
  }
}

// if this binary statement is composite expression
bool MISRAStmtRule::IsComposite(const clang::Stmt *stmt) {
  if (auto bin = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    using Opcode = clang::BinaryOperator::Opcode;
    switch (bin->getOpcode()) {
      case Opcode::BO_Mul:
      case Opcode::BO_Div:
      case Opcode::BO_Rem:
      case Opcode::BO_Add:
      case Opcode::BO_Sub:
      case Opcode::BO_Shl:
      case Opcode::BO_Shr:
        return true;
      default:
        return false;
    }
  } else if (clang::isa<clang::ConditionalOperator>(stmt)) {
    return true;
  }
  return false;
}

// if this expression is arithmetic expression
bool MISRAStmtRule::IsArithmetic(const clang::Stmt *stmt) {
  using Opcode = clang::BinaryOperator::Opcode;

  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
    return unary->isArithmeticOp();
  } else if (auto bin = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    switch (bin->getOpcode()) {
      case Opcode::BO_Add:
      case Opcode::BO_Sub:
      case Opcode::BO_Mul:
      case Opcode::BO_Div:
      case Opcode::BO_Rem:
      case Opcode::BO_And:
      case Opcode::BO_Or:
      case Opcode::BO_Xor:
      case Opcode::BO_Shl:
      case Opcode::BO_Shr:
        return true;
      default:
        return false;
    }
  }
  return false;
}

// strip all parenthesis expression and implicit cast expression
const clang::Expr *MISRAStmtRule::StripAllParenImpCast(const clang::Expr *stmt) {
  auto res = stmt;
  auto stmt_class = stmt->getStmtClass();
  while (stmt_class == clang::Stmt::ParenExprClass ||
         stmt_class == clang::Stmt::ImplicitCastExprClass) {
    res = res->IgnoreParenImpCasts();
    stmt_class = res->getStmtClass();
  }
  return res;
}

uint16_t MISRAStmtRule::getLineNumber(clang::SourceLocation loc) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto SpellingLoc = src_mgr->getSpellingLoc(loc);
  clang::PresumedLoc PLoc = src_mgr->getPresumedLoc(SpellingLoc);
  if (PLoc.isInvalid()) return 0;
  return PLoc.getLine();
}

/* MISRA
 * Directive: 4.8
 * If a pointer to a structure or union is never dereferenced within a
 * translation unit, then the implementation of the object should be hidden
 */
void MISRAStmtRule::CheckDereferencedDecl(const clang::MemberExpr *stmt) {
  auto base = stmt->getBase()->IgnoreParenImpCasts();
  if (base == nullptr) return;
  auto type = base->getType();
  if (!type->isPointerType()) return;
  auto ptr_type = GetUnderlyingType(type->getPointeeType());
  if (auto record_type = clang::dyn_cast<clang::RecordType>(ptr_type)) {
    const clang::RecordDecl *rd= record_type->getDecl();
    rd = rd->getDefinition();
    if (rd) {
      _dereferenced_decl.insert(rd);
    }
  }
}

void MISRAStmtRule::CheckDereferencedDecl() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  constexpr uint32_t kind = IdentifierManager::VAR;
  auto dereferenced = &(this->_dereferenced_decl);

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        if (clang::isa<clang::ParmVarDecl>(decl)) return;
        if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
          auto type = var_decl->getType();
          if (auto tmp = clang::dyn_cast<clang::TypedefType>(type)) {
            type = tmp->getDecl()->getTypeForDecl()->getCanonicalTypeInternal();
          }
          if (!type->isPointerType()) return;
          if (auto record_type = clang::dyn_cast<clang::RecordType>(type->getPointeeType())) {
            const clang::RecordDecl *rd= record_type->getDecl();
            rd = rd->getDefinition();
            if (rd) {
              if (dereferenced->find(reinterpret_cast<const clang::RecordDecl *const>(rd)) ==
                  dereferenced->end()) {
                std::string ref_msg = "If a pointer to a structure or union is never dereferenced within a "
                                      "translation unit, then the implementation of the object should be hidden";
                issue = report->ReportIssue(MISRA, M_D_4_8, decl);
                issue->SetRefMsg(ref_msg);
              }
            }
          }
        }
      }, true);
}

/* MISRA
 * Directive: 4.12
 * Dynamic memory allocation shall not be used
 */
void MISRAStmtRule::CheckDynamicMemoryAllocation(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;

  // call function pointer would return nullptr
  if (callee == nullptr) return;
  auto name = callee->getNameAsString();
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();

  if (conf_mgr->IsMemAllocFunction(name)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_D_4_12, stmt);
    std::string ref_msg = "Dynamic memory allocation shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 2.2
 * There shall be no dead code
 */
void MISRAStmtRule::CheckDeadCode(const clang::BinaryOperator *stmt) {
  if (!stmt->isAdditiveOp() && !stmt->isMultiplicativeOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  bool need_report = false;
  std::string ref_msg = "There shall be no dead code";
  uint64_t val;
  if (stmt->isAdditiveOp()) {
    if (IsIntegerLiteralExpr(rhs, &val) && (val == 0)) {
      ReportTemplate(ref_msg, M_R_2_2, rhs);
    } else {
      if (stmt->getOpcode() == clang::BO_Add &&
          IsIntegerLiteralExpr(lhs, &val) && (val == 0)) {
        ReportTemplate(ref_msg, M_R_2_2, lhs);
      }
    }
  } else {
    if (IsIntegerLiteralExpr(rhs, &val) && (val == 1)) {
      ReportTemplate(ref_msg, M_R_2_2, rhs);
    }
  }
}

/* MISRA
 * Rule: 2.4
 * A project should not contain unused tag declarations
 */
void MISRAStmtRule::CheckUnusedTag(const clang::DeclRefExpr *stmt) {
  auto decl = stmt->getDecl();
  if (auto enum_const_decl = clang::dyn_cast<clang::EnumConstantDecl>(decl)) {
    if (auto enum_decl = clang::cast<clang::EnumDecl>(enum_const_decl->getDeclContext())) {
      _used_tag.insert(enum_decl);
    }
  }
}

void MISRAStmtRule::CheckUnusedTag(clang::QualType type) {
  if (auto elaborated_type = clang::dyn_cast<clang::ElaboratedType>(type)) {
    type = elaborated_type->desugar();
  }
  if (auto ptr_type = clang::dyn_cast<clang::PointerType>(type)) {
    type = ptr_type->getPointeeType();
  }
  if (auto typedef_type = clang::dyn_cast<clang::TypedefType>(type)) {
    auto decl = typedef_type->getDecl();
    _used_tag.insert(decl);
    if (type->isRecordType()) {
      auto record_decl = type->getAs<clang::RecordType>()->getDecl();
      auto record_line = getLineNumber(record_decl->getBeginLoc());
      auto decl_line = getLineNumber(decl->getBeginLoc());
      if (record_line && decl_line) {
        if (record_line != decl_line) {
          _used_tag.insert(record_decl);
          auto prev_decl = record_decl->getPreviousDecl();
          if (prev_decl) _used_tag.insert(prev_decl);
        } else {
          auto record_name = record_decl->getName();
          auto typedecl_name = decl->getName();
          if (!record_name.empty() &&
              !typedecl_name.empty() &&
              record_name == typedecl_name) {
            _used_tag.insert(record_decl);
          }
        }
      }
    }
  } else if (type->isRecordType()) {
    auto record_type = type.getCanonicalType()->getAs<clang::RecordType>();
    _used_tag.insert(record_type->getDecl());
  } else if (const auto *enum_type = type->getAs<clang::EnumType>()) {
    _used_tag.insert(enum_type->getDecl());
  }
}

void MISRAStmtRule::CheckUnusedTag() {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  top_scope->TraverseAll<IdentifierManager::VAR | IdentifierManager::FIELD,
    const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
    [&](const std::string &x, const clang::Decl *decl,
        IdentifierManager *id_mgr) -> void {
      if (auto enum_decl = clang::dyn_cast<clang::EnumDecl>(decl->getDeclContext())) {
        _used_tag.insert(enum_decl);
      } else if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
        CheckUnusedTag(var_decl->getType());
      } else if (auto field_decl = clang::dyn_cast<clang::FieldDecl>(decl)) {
        CheckUnusedTag(field_decl->getType());
      }
    }, true);

  constexpr uint32_t kind = IdentifierManager::VALUE |
                            IdentifierManager::TYPE |
                            IdentifierManager::TYPEDEF;
  auto used_tags= &(this->_used_tag);

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  top_scope->TraverseAll<kind,
    const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
    [&](const std::string &x, const clang::Decl *decl,
        IdentifierManager *id_mgr) -> void {
      if (auto enum_const_decl = clang::dyn_cast<clang::EnumConstantDecl>(decl)) {
        if (auto enum_decl = clang::cast<clang::EnumDecl>(enum_const_decl->getDeclContext())) {
          decl = enum_decl;
        }
      } else if (auto typeDef_decl = clang::dyn_cast<clang::TypedefDecl>(decl)) {
        auto type = typeDef_decl->getTypeForDecl();
        if (!type || !type->isRecordType()) return;
      }

      if (used_tags->find(decl) == used_tags->end()) {
        if (auto record_decl = clang::dyn_cast<clang::RecordDecl>(decl)) {
          if (record_decl->getName().empty()) return;
        }
        issue = report->ReportIssue(MISRA, M_R_2_4, decl);
        std::string ref_msg = "A project should not contain unused tag declarations";
        issue->SetRefMsg(ref_msg);
      }
    }, true);
}

/* MISRA
 * Rule: 4.1
 * Octal and hexadecimal escape sequences shall be terminated
 */
void MISRAStmtRule::CheckOctalAndHexadecimalEscapeWithoutTerminated(const clang::Expr *stmt) {
  char quote;
  if (auto string_literal = clang::dyn_cast<clang::StringLiteral>(stmt)) {
    if (!string_literal->getLength()) return;
    quote = '"';
  } else if (auto char_literal = clang::dyn_cast<clang::CharacterLiteral>(stmt)) {
    if (!char_literal->getValue()) return;
    quote = '\'';
  }

  auto src_mgr = XcalCheckerManager::GetSourceManager();
  const char *start = src_mgr->getCharacterData(stmt->getBeginLoc());
  clang::LangOptions lang_ops;
  auto end_loc = stmt->getEndLoc();
  auto loc = clang::Lexer::getLocForEndOfToken(end_loc.isMacroID() ? src_mgr->getSpellingLoc(end_loc) : end_loc,
                                               0, *src_mgr, lang_ops);
  const char *end = src_mgr->getCharacterData(loc);

  bool need_report = false, pre_is_terminal = false;
  if (!(*start == quote && *(start + 1) == '\\')) return;
  if (*(start + 2) == 'x') {
    // hexadecimal escape sequences
    do {
      if (*end == ';' || *end == quote ||
          (*end >= '0' && *end <= '9') ||
          (*end >= 'A' && *end <= 'F')) {
        end--;
        continue;
      } else {
        pre_is_terminal = true;
        if (*end == '\\' ||
            *(end - 1) == '\\' ||
            *(end - 1) == quote) {
          break;
        }
        need_report = true;
      }
    } while (!pre_is_terminal);
  } else {
    // octal escape sequences
    do {
      if (*end == ';' || *end == quote ||
          (*end >= '0' && *end <= '7')) {
        end--;
        continue;
      } else {
        pre_is_terminal = true;
        if (*end == '\\' ||
            *(end - 1) == '\\' ||
            *(end - 1) == quote) {
          break;
        }
        need_report = true;
      }
    } while (!pre_is_terminal);
  }
  if (need_report) {
    std::string ref_msg = "Octal and hexadecimal escape sequences shall be terminated";
    ReportTemplate(ref_msg, M_R_4_1, stmt);
  }
}

/* MISRA
 * Rule: 7.1
 * Octal constants shall not be used
 */
void MISRAStmtRule::CheckOctalConstants(const clang::IntegerLiteral *stmt) {
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  clang::SourceLocation sl = stmt->getLocation();
  if (sl.isMacroID()) {
    sl = src_mgr->getSpellingLoc(sl);
  }
  clang::LangOptions langOps;
  clang::SmallString<256> buffer;
  llvm::StringRef val = clang::Lexer::getSpelling(sl, buffer, *src_mgr, langOps);
  if (val.size() > 1 && val[0] == '0') {
    if ('0' <= val[1] && val[1] <= '7') {
      std::string ref_msg = "Octal constants shall not be used";
      ReportTemplate(ref_msg, M_R_7_1, stmt);
    }
  }
}

/* MISRA
 * Rule: 7.2
 * A "u" or "U" suffix shall be applied to all integer constants that are
 * represented in an unsigned type
 */
void MISRAStmtRule::CheckUnsignedIntegerSuffix(const clang::IntegerLiteral *stmt) {
  auto type = stmt->getType();
  if (type->isUnsignedIntegerType()) {
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    clang::SourceLocation sl = stmt->getLocation();
    if (sl.isMacroID()) {
      sl = src_mgr->getSpellingLoc(sl);
    }
    clang::LangOptions lang_ops;
    clang::SmallString<256> buffer;
    llvm::StringRef val = clang::Lexer::getSpelling(sl, buffer, *src_mgr, lang_ops);
    if (val.find("u") == std::string::npos &&
        val.find("U") == std::string::npos) {
      std::string ref_msg = "A \"u\" or \"U\" suffix shall be applied to all integer constants that are "
                            "represented in an unsigned type";
      ReportTemplate(ref_msg, M_R_7_2, stmt);
    }
  }
}

void MISRAStmtRule::CheckIntegralCastFromIntegerLiteral(const clang::ImplicitCastExpr *stmt) {
  if (!stmt->getType()->isUnsignedIntegerType()) return;
  auto sub_expr = stmt->getSubExpr();
  if (sub_expr->getType()->isSignedIntegerType() && clang::isa<clang::IntegerLiteral>(sub_expr)) {
    std::string ref_msg = "A \"u\" or \"U\" suffix shall be applied to all integer constants that are "
                          "represented in an unsigned type";
    ReportTemplate(ref_msg, M_R_7_2, stmt);
  }
}

/* MISRA
 * Rule: 7.3
 * The lowercase character 'l' shall not be used in a literal suffix
 */
void MISRAStmtRule::CheckLiteralSuffix(const clang::Expr *stmt) {
  bool is_string_literal = clang::isa<clang::IntegerLiteral>(stmt);
  bool is_float_literal = clang::isa<clang::FloatingLiteral>(stmt);
  if (!is_string_literal && !is_float_literal) return;

  bool status = false;
  clang::BuiltinType::Kind kind = GetBTKind(stmt->getType(), status);
  if (!status) return;
  if (kind != clang::BuiltinType::ULong &&
      kind != clang::BuiltinType::ULongLong &&
      kind != clang::BuiltinType::Long &&
      kind != clang::BuiltinType::LongLong &&
      kind != clang::BuiltinType::Int128 &&
      kind != clang::BuiltinType::Float128 &&
      kind != clang::BuiltinType::LongDouble) {
    return;
  }
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  clang::SourceLocation sl = is_string_literal ?
                               clang::dyn_cast<clang::IntegerLiteral>(stmt)->getLocation() :
                               clang::dyn_cast<clang::FloatingLiteral>(stmt)->getLocation();
  if (sl.isMacroID()) {
    sl = src_mgr->getSpellingLoc(sl);
  }
  clang::LangOptions lang_ops;
  clang::SmallString<256> buffer;
  llvm::StringRef val = clang::Lexer::getSpelling(sl, buffer, *src_mgr, lang_ops);
  if (val.find("l") != std::string::npos) {
    std::string ref_msg = "The lowercase character \'l\' shall not be used in a literal suffix";
    ReportTemplate(ref_msg, M_R_7_3, stmt);
  }
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
        continue;
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
    }
    i++;
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
 * Rule: 8.13
 * A pointer should point to a const-qualified type whenever possible
 */
void MISRAStmtRule::CheckModifiedPointerDecl(const clang::Expr* expr) {
  auto ptr_expr = expr;
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(expr)) {
    ptr_expr = unary->getSubExpr()->IgnoreParenImpCasts();
    if (auto cast_expr = clang::dyn_cast<clang::CStyleCastExpr>(ptr_expr)) {
      ptr_expr = cast_expr->getSubExpr()->IgnoreParenImpCasts();
    }
  }
  if (auto decl_expr = clang::dyn_cast<clang::DeclRefExpr>(ptr_expr)) {
    auto decl = decl_expr->getDecl();
    if (!decl->getType()->isPointerType()) return;
    if (clang::isa<clang::VarDecl>(decl)) {
      _modified_pointer_decl.insert(decl);
    }
  }
}
void MISRAStmtRule::CheckAssignmentOfPointer(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp() && !stmt->isCompoundAssignmentOp()) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  CheckModifiedPointerDecl(lhs);
}

void MISRAStmtRule::CheckAssignmentOfPointer(const clang::CompoundAssignOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  CheckModifiedPointerDecl(lhs);
}

void MISRAStmtRule::CheckAssignmentOfPointer(const clang::CallExpr *stmt) {
  auto decl = GetCalleeDecl(stmt);
  if (decl == nullptr) return;
  int i = 0;
  for (const clang::Expr *arg :  stmt->arguments()) {
    if (i >= decl->param_size()) break;
    auto arg_type = arg->IgnoreParenImpCasts()->getType();
    if (!arg_type->isPointerType()) continue;
    auto param_decl = decl->getParamDecl(i);
    if (param_decl == nullptr) {
      i++;
      continue;
    }
    auto param_type = param_decl->getType();
    if (!param_type->isPointerType()) continue;
    if (!arg_type->getPointeeType().isConstQualified() &&
        !param_type->getPointeeType().isConstQualified()) {
      if (auto decl_expr = clang::dyn_cast<clang::DeclRefExpr>(arg->IgnoreParenImpCasts())) {
        _modified_pointer_decl.insert(decl_expr->getDecl());
      }
    }
    i++;
  }
}

void MISRAStmtRule::ReportNeedConstQualifiedVar() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
    const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
    [&](const std::string &x, const clang::Decl *decl,
        IdentifierManager *id_mgr) -> void {
      if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
        auto type = var_decl->getType();
        if (!type->isPointerType()) return;
        if (_modified_pointer_decl.find(decl) == _modified_pointer_decl.end()) {
          if (!type->getPointeeType().isConstQualified()) {
            const clang::DeclContext *decl_context = var_decl->getDeclContext();
            if (auto func_decl = clang::dyn_cast<clang::FunctionDecl>(decl_context)) {
              if (!func_decl->getBody()) return;
            }
            issue = report->ReportIssue(MISRA, M_R_8_13, decl);
            std::string ref_msg = "A pointer should point to a const-qualified type whenever possible: ";
            ref_msg += var_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          }
        }
      }
    }, true);
}

/* MISRA
 * Rule: 10.1
 * Operands shall not be of an inappropriate essential type
 */
void MISRAStmtRule::ReportInappropriateEssentialType(const clang::Stmt *stmt) {
  std::string ref_msg = "Operands shall not be of an inappropriate essential type";
  ReportTemplate(ref_msg, M_R_10_1, stmt);
}

void MISRAStmtRule::CheckInappropriateEssentialTypeOfOperands(const clang::BinaryOperator *stmt) {
  if (stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();
  bool status = false;
  clang::BuiltinType::Kind lhs_kind = GetBTKind(lhs_type, status);
  // ignore checking the status, since the type may be EnumeralType
  clang::BuiltinType::Kind rhs_kind = GetBTKind(rhs_type, status);

  auto opcode = stmt->getOpcode();
  bool need_report = false;
  const clang::Stmt *report_stmt = nullptr;
  if (stmt->isLogicalOp()) {
    if (!lhs_type->isBooleanType()) ReportInappropriateEssentialType(lhs);
    if (!rhs_type->isBooleanType()) ReportInappropriateEssentialType(rhs);
  } else if (stmt->isMultiplicativeOp()) {
    if (opcode == clang::BO_Rem) {
      if (lhs_type->isFloatingType()) ReportInappropriateEssentialType(lhs);
      if (rhs_type->isFloatingType()) ReportInappropriateEssentialType(rhs);
    }
    if (lhs_type->isBooleanType() ||
        lhs_type->isEnumeralType() ||
        lhs_kind == clang::BuiltinType::Char_U ||
        lhs_kind == clang::BuiltinType::Char_S) {
      ReportInappropriateEssentialType(lhs);
    }
    if (rhs_type->isBooleanType() ||
        rhs_type->isEnumeralType() ||
        rhs_kind == clang::BuiltinType::Char_U ||
        rhs_kind == clang::BuiltinType::Char_S) {
      ReportInappropriateEssentialType(rhs);
    }
  } else if (stmt->isBitwiseOp() || stmt->isShiftOp()) {
    if (lhs_type->isBooleanType() ||
        lhs_type->isEnumeralType() ||
        lhs_type->isSignedIntegerType() ||
        lhs_type->isFloatingType() ||
        lhs_kind == clang::BuiltinType::Char_U ||
        lhs_kind == clang::BuiltinType::Char_S) {
      ReportInappropriateEssentialType(lhs);
    }
    if (rhs_type->isBooleanType() ||
        rhs_type->isEnumeralType() ||
        rhs_type->isFloatingType() ||
        rhs_kind == clang::BuiltinType::Char_U ||
        rhs_kind == clang::BuiltinType::Char_S) {
      ReportInappropriateEssentialType(rhs);
    } else if (rhs_type->isSignedIntegerType()) {
      // A non-negative integer constant expression of essentially signed type
      // may be used as the right hand operand to a shift operator.
      auto ctx = XcalCheckerManager::GetAstContext();
      if (stmt->isShiftOp() && rhs->isIntegerConstantExpr(*ctx)) {
        clang::Expr::EvalResult res;
        if (rhs->EvaluateAsInt(res, *ctx)) {
          auto value = res.Val.getInt().getSExtValue();
          if (value >= 0) return;
        }
      }
      ReportInappropriateEssentialType(rhs);
    }
  } else if (stmt->isAdditiveOp()) {
    if (lhs_type->isBooleanType() || lhs_type->isEnumeralType()) {
      ReportInappropriateEssentialType(lhs);
    }
    if (rhs_type->isBooleanType() || rhs_type->isEnumeralType()) {
      ReportInappropriateEssentialType(rhs);
    }
  } else if (opcode >= clang::BO_LT && opcode <= clang::BO_GE) {
    if (lhs_type->isBooleanType()) ReportInappropriateEssentialType(lhs);
    if (rhs_type->isBooleanType()) ReportInappropriateEssentialType(rhs);
  }
}

void MISRAStmtRule::CheckInappropriateEssentialTypeOfOperands(const clang::UnaryOperator *stmt) {
  auto sub = stmt->getSubExpr()->IgnoreParenImpCasts();
  auto sub_type = sub->getType();
  bool status = false;
  clang::BuiltinType::Kind sub_kind = GetBTKind(sub_type, status);

  auto opcode = stmt->getOpcode();
  bool need_report = false;
  if (stmt->isIncrementDecrementOp()) {
    if (sub_type->isBooleanType() || sub_type->isEnumeralType()) need_report = true;
  } else if (opcode == clang::UO_Plus || opcode == clang::UO_Minus) {
    if (sub_type->isBooleanType() ||
        sub_type->isEnumeralType() ||
        sub_kind == clang::BuiltinType::Char_U ||
        sub_kind == clang::BuiltinType::Char_S) {
      need_report = true;
    } else if (opcode == clang::UO_Minus && sub_type->isUnsignedIntegerType()) {
      need_report = true;
    }
  } else if (opcode == clang::UO_LNot) {
    if (!sub_type->isBooleanType()) need_report = true;
  } else if (opcode == clang::UO_Not) {
    if (sub_type->isBooleanType() ||
        sub_type->isEnumeralType() ||
        sub_type->isSignedIntegerType() ||
        sub_type->isFloatingType() ||
        sub_kind == clang::BuiltinType::Char_U ||
        sub_kind == clang::BuiltinType::Char_S) {
      need_report = true;
    }
  }
  if (need_report) {
    std::string ref_msg = "Operands shall not be of an inappropriate essential type";
    ReportTemplate(ref_msg, M_R_10_1, sub);
  }
}

void MISRAStmtRule::CheckInappropriateEssentialTypeOfOperands(const clang::ConditionalOperator *stmt) {
  auto cond = stmt->getCond()->IgnoreParenImpCasts();
  if (!cond->getType()->isBooleanType()) {
    std::string ref_msg = "Operands shall not be of an inappropriate essential type";
    ReportTemplate(ref_msg, M_R_10_1, stmt);
  }
}

void MISRAStmtRule::CheckInappropriateEssentialTypeOfOperands(const clang::CompoundAssignOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();
  auto opcode = stmt->getOpcode();
  if (opcode == clang::BO_AddAssign || opcode == clang::BO_SubAssign) {
    if (lhs_type->isBooleanType() || lhs_type->isEnumeralType() ||
        rhs_type->isBooleanType() ||rhs_type->isEnumeralType()) {
      std::string ref_msg = "Operands shall not be of an inappropriate essential type";
      ReportTemplate(ref_msg, M_R_10_1, stmt);
    }
  }
}

void MISRAStmtRule::CheckInappropriateEssentialTypeOfOperands(const clang::ArraySubscriptExpr *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs == nullptr) return;
  auto rhs_type = rhs->getType();
  bool status = false;
  clang::BuiltinType::Kind rhs_kind = GetBTKind(rhs_type, status);
  if (rhs_type->isBooleanType() ||
      rhs_type->isFloatingType() ||
      rhs_kind == clang::BuiltinType::Char_U ||
      rhs_kind == clang::BuiltinType::Char_S) {
    std::string ref_msg = "Operands shall not be of an inappropriate essential type";
    ReportTemplate(ref_msg, M_R_10_1, rhs);
  }
}


/* MISRA
 * Rule: 8.9
 * An object should be defined at block scope if its identifier only appears in
 * a single function
 */
void MISRAStmtRule::CheckDefinitionOfVarDeclInSingleFunction(const clang::DeclRefExpr *stmt) {
  auto decl = stmt->getDecl();
  if (!decl || !clang::isa<clang::VarDecl>(decl)) return;
  if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
    if (var_decl->hasGlobalStorage() && !var_decl->isStaticLocal()) {
      auto func_decl = XcalCheckerManager::GetCurrentFunction();
      if (func_decl) {
        auto res = _var_to_func.find(var_decl);
        if (res != _var_to_func.end()) {
          if (func_decl != res->second) {
            _var_to_func[var_decl] = nullptr;
          }
        } else {
          _var_to_func.insert({var_decl, func_decl});
        }
      }
    }
  }
}

void MISRAStmtRule::ReportDefinitionOfVarDeclInSingleFunction() {
  for (const auto &iter : _var_to_func) {
    if (iter.second != nullptr) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_8_9, iter.first);
      std::string ref_msg = "An object should be defined at block scope if its identifier only appears in "
                            "a single function";
      issue->SetRefMsg(ref_msg);
    }
  }
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


  if (!lhs_type->isIntegerType() && !rhs_type->isIntegerType()) {
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

  auto rhs_cast = clang::dyn_cast<clang::ImplicitCastExpr>(rhs);
  if (!rhs_cast || IsIntegerLiteralExpr(rhs_cast->getSubExpr())) return;
  if (auto conditional = clang::dyn_cast<clang::ConditionalOperator>(rhs_cast->getSubExpr())) {
    if (IsIntegerLiteralExpr(conditional->getLHS()) || IsIntegerLiteralExpr(conditional->getRHS())) return;
  }

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

void MISRAStmtRule::CheckIntToShorter(const clang::SwitchStmt *stmt) {
  auto cond = stmt->getCond()->IgnoreParenImpCasts();
  auto cond_ty = cond->getType();
  if (cond_ty->isEnumeralType()) return;

  if (auto bt = clang::dyn_cast<clang::BuiltinType>(cond->getType())) {
    auto head = stmt->getSwitchCaseList();
    if (head == nullptr) return;
    if (head->getStmtClass() == clang::Stmt::DefaultStmtClass) head = head->getNextSwitchCase();
    if (head == nullptr) return;
    auto cases = clang::dyn_cast<clang::CaseStmt>(head);
    std::vector<const clang::Stmt *> sinks;

    while (cases) {
      if (cases->getLHS()->IgnoreParenImpCasts()->getType() != cond_ty) {
        sinks.push_back(cases);
      }

      auto next = cases->getNextSwitchCase();
      if (next == nullptr) break;
      cases = clang::dyn_cast<clang::CaseStmt>(next);
    }

    if (!sinks.empty()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue(MISRA, M_R_10_3, stmt);
      std::string ref_msg = "Convert int to shorter int carefully";
      issue->SetRefMsg(ref_msg);
      for (const auto &it : sinks) issue->AddStmt(it);
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

  // check only when the rhs is composite stmt
  if (!IsComposite(rhs) || stmt->isShiftOp()) return;

  // not handle integer expression
  if (IsIntegerLiteralExpr(rhs)) return;

  auto lhs_type = stmt->getLHS()->IgnoreParenImpCasts()->getType();
  auto rhs_type = rhs->getType();
  bool need_report = false;

  /* Get raw type of typedef */
  if (clang::isa<clang::TypedefType>(lhs_type)) lhs_type = GetRawTypeOfTypedef(lhs_type);
  if (clang::isa<clang::TypedefType>(rhs_type)) rhs_type = GetRawTypeOfTypedef(rhs_type);

//  if (rhs_type < lhs_type) need_report = true;
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
  if (!IsArithmetic(stmt)) return;
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
      if (!IsComposite(lhs_bin_op)) return;
      if (IsIntegerLiteralExpr(lhs_bin_op->getLHS()->IgnoreParenImpCasts()) &&
          IsIntegerLiteralExpr(lhs_bin_op->getRHS()->IgnoreParenImpCasts()))
        return;
      need_report = true;
    }
  } else if (rhs_kind < lhs_kind) {
    if (auto rhs_bin_op = clang::dyn_cast<clang::BinaryOperator>(rhs)) {
      if (IsComposite(rhs_bin_op)) return;
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

  // return if it is not composite expression
  if (!IsComposite(sub_expr)) return;

  if (clang::isa<clang::TypedefType>(type)) type = GetRawTypeOfTypedef(type);
  if (clang::isa<clang::TypedefType>(sub_type)) sub_type = GetRawTypeOfTypedef(sub_type);

  auto bt_type = clang::dyn_cast<clang::BuiltinType>(type);
  auto bt_sub = clang::dyn_cast<clang::BuiltinType>(sub_type);
  if (!bt_type || !bt_sub) return;
  auto type_kind = UnifyBTTypeKind(bt_type->getKind());
  auto subtype_kind = UnifyBTTypeKind(bt_sub->getKind());

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
  bool need_report = false;
  auto type = stmt->getType();
  auto sub_ty = stmt->getSubExpr()->IgnoreParenImpCasts()->getType();
  auto cmp_func_ptr_ty = [](clang::QualType type, clang::QualType sub_ty) -> bool {
    if (auto ptr_ty = clang::dyn_cast<clang::PointerType>(type)) {
      if (ptr_ty->isFunctionPointerType()) {
        auto ptr_func_ty = clang::dyn_cast<clang::FunctionType>(ptr_ty->getPointeeType());

        if (!sub_ty->isFunctionType() && !sub_ty->isFunctionPointerType()) return true;
        else if (sub_ty->isFunctionPointerType()) {
          if (sub_ty != type) return true;
        }
      }
    }
    return false;
  };
  type = GetRawTypeOfTypedef(type);
  sub_ty = GetRawTypeOfTypedef(sub_ty);
  need_report = cmp_func_ptr_ty(type, sub_ty) || cmp_func_ptr_ty(sub_ty, type);
  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_11_1, stmt->getSubExpr()->IgnoreParenImpCasts());
    std::string ref_msg = "Conversions shall not be performed between a pointer to a function and any other type";
    issue->SetRefMsg(ref_msg);
  }
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
  if (!pointee_type->isRecordType()) return;
  auto struct_decl = pointee_type->getAs<clang::RecordType>()->getDecl();
  if (!struct_decl || struct_decl->isCompleteDefinition()) return;

  if (sub_type == stmt->getType()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_11_2, stmt->getSubExpr()->IgnoreParenImpCasts());
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
  if (!type->isPointerType() || !sub_type->isPointerType()) return;
  // exceptions:
  // 1. It is permitted to convert a pointer to object type into a pointer to
  //    one of the object types char, signed char or unsigned char
  // 2. The rule applies to the unqualified pointer types.
  auto pointee_type = type->getPointeeType().getUnqualifiedType();
  if (GetUnderlyingType(pointee_type)->isCharType()) return;
  if (pointee_type != sub_type->getPointeeType().getUnqualifiedType()) {
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

  // ignore (void *)0
  if (stmt->getSubExpr()->IgnoreCasts()
      ->isNullPointerConstant(*XcalCheckerManager::GetAstContext(),
                              clang::Expr::NPC_ValueDependentIsNull))
    return;

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

  auto isArithIntType = [](const clang::BuiltinType *type) -> bool {
    if (type->isBooleanType() || type->isEnumeralType() || type->isCharType() || type->isFloatingType()) {
      return false;
    } else {
      return true;
    }
  };

  if ((type->isPointerType() && sub_bt && !isArithIntType(sub_bt)) ||
      (type_bt && !isArithIntType(type_bt) && sub_type->isPointerType())) {
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
bool MISRAStmtRule::UsingZeroAsPointer(const clang::Expr *lhs, const clang::Expr *rhs) {
  auto lhs_type = lhs->getType();
  auto rhs_type = rhs->getType();
  if (clang::isa<clang::TypedefType>(lhs_type)) lhs_type = GetRawTypeOfTypedef(lhs_type);
  if (clang::isa<clang::TypedefType>(rhs_type)) rhs_type = GetRawTypeOfTypedef(rhs_type);
  if (lhs_type->isPointerType() && rhs_type->isIntegerType()) {
    uint64_t val;
    if (IsIntegerLiteralExpr(rhs, &val) && (val == 0)) return true;
  }
  if (rhs_type->isPointerType() && lhs_type->isIntegerType()) {
    uint64_t val;
    if (IsIntegerLiteralExpr(lhs, &val) && (val == 0)) return true;
  }
  return false;
}


void MISRAStmtRule::CheckZeroAsPointerConstant(const clang::BinaryOperator *stmt) {
  auto opcode = stmt->getOpcode();
  if (opcode != clang::BO_EQ && opcode != clang::BO_NE) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (UsingZeroAsPointer(lhs, rhs)) {
    ReportZeroAsPointer(stmt);
  }
}

void MISRAStmtRule::CheckZeroAsPointerConstant(const clang::ConditionalOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  if (UsingZeroAsPointer(lhs, rhs)) {
    ReportZeroAsPointer(stmt);
  }
}

void MISRAStmtRule::ReportZeroAsPointer(const clang::Stmt *stmt) {
  std::string ref_msg = "The macro NULL shall be the only permitted form of integer null pointer constant";
  ReportTemplate(ref_msg, M_R_11_9, stmt);
}

/* MISRA
 * Rule: 12.1
 * The precedence of operators within expressions should be made explicit
 */
void MISRAStmtRule::ReportPrecedenceOfOperators(const clang::Stmt *stmt) {
  std::string ref_msg = "The precedence of operators within expressions should be made explicit";
  ReportTemplate(ref_msg, M_R_12_1, stmt);
}

void MISRAStmtRule::CheckPrecedenceOfSizeof(const clang::UnaryExprOrTypeTraitExpr *stmt) {
  if (stmt->getKind() == clang::UnaryExprOrTypeTrait::UETT_SizeOf) {
    if (!stmt->isArgumentType()) {
      auto expr = stmt->getArgumentExpr();
      if (!clang::isa<clang::ParenExpr>(expr)) {
        ReportPrecedenceOfOperators(expr);
      }
    }
  }
}

void MISRAStmtRule::CheckPrecedenceOfOperator(const clang::ConditionalOperator *stmt) {
  auto cond = stmt->getCond();
  auto lhs = stmt->getLHS();
  auto rhs = stmt->getRHS();
  if (auto opr = clang::dyn_cast<clang::BinaryOperator>(cond)) {
    ReportPrecedenceOfOperators(cond);
  }
  if (auto opr = clang::dyn_cast<clang::BinaryOperator>(lhs)) {
    ReportPrecedenceOfOperators(lhs);
  }
  if (auto opr = clang::dyn_cast<clang::BinaryOperator>(rhs)) {
    ReportPrecedenceOfOperators(rhs);
  }
}

bool MISRAStmtRule::IsSamePrecedenceOfBinaryOperator(const clang::BinaryOperator *expr1,
                                                     const clang::BinaryOperator *expr2) {
  if (expr1->getOpcode() == expr2->getOpcode()) return true;
  if ((expr1->isPtrMemOp() && expr2->isPtrMemOp()) ||
      (expr1->isMultiplicativeOp() && expr2->isMultiplicativeOp()) ||
      (expr1->isAdditiveOp() && expr2->isAdditiveOp()) ||
      (expr1->isShiftOp() && expr2->isShiftOp()) ||
      (expr1->isRelationalOp() && expr2->isRelationalOp()) ||
      (expr1->isEqualityOp() && expr2->isEqualityOp()) ||
      (expr1->isAssignmentOp() && expr2->isAssignmentOp()))
    return true;
  return false;
}

void MISRAStmtRule::CheckPrecedenceOfOperator(const clang::BinaryOperator *stmt) {
  auto opcode = stmt->getOpcode();
  if (opcode < clang::BO_Add || opcode > clang::BO_LOr) return;

  auto lhs_expr = clang::dyn_cast<clang::BinaryOperator>(stmt->getLHS());
  auto rhs_expr = clang::dyn_cast<clang::BinaryOperator>(stmt->getRHS());
  if (lhs_expr) {
    auto lhs_op = lhs_expr->getOpcode();
    if (lhs_op < clang::BO_Mul || lhs_op > clang::BO_LOr) return;
    if (lhs_op > opcode) return;
    if (IsSamePrecedenceOfBinaryOperator(lhs_expr, stmt)) return;
    ReportPrecedenceOfOperators(lhs_expr);
  }
  if (rhs_expr) {
    auto rhs_op = rhs_expr->getOpcode();
    if (rhs_op < clang::BO_Mul || rhs_op > clang::BO_LOr) return;
    if (rhs_op > opcode) return;
    if (IsSamePrecedenceOfBinaryOperator(rhs_expr, stmt)) return;
    ReportPrecedenceOfOperators(rhs_expr);
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
bool MISRAStmtRule::IsInc(const clang::Stmt *stmt, const clang::Expr *&base) {
  if (stmt == nullptr) return false;
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(stmt)) {
    if (unary->isIncrementDecrementOp()) {
      base = unary->getSubExpr();
      return true;
    }
  }
  for (const auto &it : stmt->children()) {
    const clang::Expr *child_base = nullptr;
    if (IsInc(it, child_base)) {
      base = child_base;
      return true;
    }
  }
  return false;
}

bool MISRAStmtRule::IsSameDeclaration(const clang::Expr *expr1,
                                      const clang::Expr *expr2) {
  if (expr1 == nullptr || expr2 == nullptr) return false;
  expr1 = expr1->IgnoreParenImpCasts();
  expr2 = expr2->IgnoreParenImpCasts();
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(expr1)) {
    expr1 = unary->getSubExpr();
  }
  if (auto unary = clang::dyn_cast<clang::UnaryOperator>(expr2)) {
    expr2 = unary->getSubExpr();
  }
  if (!clang::isa<clang::DeclRefExpr>(expr1) ||
      !clang::isa<clang::DeclRefExpr>(expr2)) {
    return false;
  }
  const clang::Decl *decl1 = clang::cast<clang::DeclRefExpr>(expr1)->getDecl();
  const clang::Decl *decl2 = clang::cast<clang::DeclRefExpr>(expr2)->getDecl();
  if (!decl1 || !decl2) return false;
  if (auto named_decl1 = clang::dyn_cast<clang::NamedDecl>(decl1))
    decl1 = named_decl1->getUnderlyingDecl();
  if (auto named_decl2 = clang::dyn_cast<clang::NamedDecl>(decl2))
    decl2 = named_decl2->getUnderlyingDecl();
  return decl1->getCanonicalDecl() == decl2->getCanonicalDecl();
}

void MISRAStmtRule::CheckSideEffectWithOrder(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();

  // ingore comma
  if (stmt->isCommaOp()) return;

  const clang::Expr *lhs_base = nullptr;
  const clang::Expr *rhs_base = nullptr;
  if (IsInc(lhs, lhs_base) && IsInc(rhs, rhs_base) &&
      lhs_base && rhs_base &&
      IsSameDeclaration(lhs_base, rhs_base)) {
    ReportSideEffect(stmt);
  }
  if (IsVolatileQualified(lhs) && IsVolatileQualified(rhs)) {
    ReportSideEffect(stmt);
  }
}

void MISRAStmtRule::CheckSideEffectWithOrder(const clang::CallExpr *stmt) {
  const clang::Expr *callee_base = nullptr;
  if (!stmt->getDirectCallee()) {
    auto mem_expr = clang::dyn_cast<clang::MemberExpr>(stmt->getCallee()->IgnoreParenImpCasts());
    if (mem_expr) {
      callee_base = mem_expr->getBase();
    }
  }
  if (callee_base == nullptr) return;
  /*
   * case : p->fn(g2(&p));
   * the relative order of evaluation of a function designator and function arguments
   * is unspecified. if p is modified in function g2, it is unspecified whether p->fn
   * uses the value of p prior to the call of g2 or after it.
   */
  for (const auto &args : stmt->arguments()) {
    if (auto call_expr = clang::dyn_cast<clang::CallExpr>(args)) {
      if (!call_expr->getNumArgs()) continue;
      auto decl = call_expr->getDirectCallee();
      if (!decl) continue;
      auto func_decl = clang::dyn_cast<clang::FunctionDecl>(decl);
      if (!func_decl) continue;
      uint32_t idx = 0;
      for (const auto &it : call_expr->arguments()) {
        if (auto unary = clang::dyn_cast<clang::UnaryOperator>(it)) {
          if (unary->getOpcode() == clang::UO_AddrOf &&
              IsSameDeclaration(unary->getSubExpr(), callee_base)) {
            if (idx >= func_decl->getNumParams()) continue;
            const clang::Expr *arg_base = nullptr;
            if (IsInc(func_decl->getBody(), arg_base)) {
              arg_base = arg_base->IgnoreParenImpCasts();
              if (auto unary = clang::dyn_cast<clang::UnaryOperator>(arg_base)) {
                arg_base = unary->getSubExpr()->IgnoreParenImpCasts();
              }
              if (auto decl_expr = clang::dyn_cast<clang::DeclRefExpr>(arg_base)) {
                auto arg_decl = decl_expr->getDecl();
                if (arg_decl == func_decl->getParamDecl(idx)) ReportSideEffect(stmt);
              }
            }
          }
        }
        idx++;
      }
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

  // ignore i++,k++
  if (stmt->isCommaOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (stmt->isAssignmentOp() || stmt->isCompoundAssignmentOp() || stmt->isEqualityOp()) {
    if (HasIncOrDecExpr(lhs) || HasIncOrDecExpr(rhs)) need_report = true;
  } else {
    if (HasIncOrDecExpr(lhs) && HasIncOrDecExpr(rhs)) need_report = true;
    if (HasIncOrDecExpr(lhs) || HasIncOrDecExpr(rhs)) {
      auto is_function = [](const clang::Stmt *stmt) -> bool {
        if (stmt->getStmtClass() == clang::Stmt::CallExprClass) return true;
        return false;
      };
      if (HasSpecificStmt(lhs, is_function) || HasSpecificStmt(rhs, is_function)) {
        need_report = true;
      }
    }
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

void MISRAStmtRule::CheckMultiIncOrDecExpr(const clang::CallExpr *stmt) {
  for (const auto &args : stmt->arguments()) {
    const clang::Expr *base = nullptr;
    if (IsInc(args, base)) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_13_3, stmt);
      issue->AddStmt(args);
      std::string ref_msg = "A full expression containing an increment (++) or decrement (--) operator should have no "
                            "other potential side effects other than that caused by the increment or decrement operator";
      issue->SetRefMsg(ref_msg);
      break;
    }
  }
}

void MISRAStmtRule::CheckMultiIncOrDecExpr(const clang::InitListExpr *stmt) {
  std::vector<const clang::Expr *> sinks;
  for (const auto &it : stmt->inits()) {
    auto init = it->IgnoreParenImpCasts();
    if (HasIncOrDecExpr(init)) sinks.push_back(it);
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_13_3, stmt);
    std::string ref_msg = "A full expression containing an increment (++) or "
                          "decrement (--) operator should have no other "
                          "potential side effects other than that caused "
                          "by the increment or decrement operator";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddStmt(it);
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
  ref_msg = "Assignment operators shall not be used in sub- expressions.";
  ReportTemplate(ref_msg, M_R_6_2_1, stmt);
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::ArraySubscriptExpr *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs == nullptr) return;
  if (rhs->getStmtClass() != clang::Stmt::BinaryOperatorClass) return;
  if (IsAssignmentStmt(rhs)) ReportAssignment(stmt);
}

void MISRAStmtRule::CheckUsingAssignmentAsResult(const clang::BinaryOperator *stmt) {
  if (stmt->getOpcode() == clang::BO_Comma) return;
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if ((lhs && IsAssignmentStmt(lhs)) ||
      (rhs && IsAssignmentStmt(rhs)))
    ReportAssignment(stmt);
}

/* MISRA
 * Rule: 13.5
 * The right hand operand of a logical && or || operator shall not contain
 * persistent side effects
 */
void MISRAStmtRule::CheckRHSOfLogicalOpHasSideEffect(const clang::BinaryOperator *stmt) {
  if (!stmt->isLogicalOp()) return;
  bool res = false;
  res = HasSpecificStmt(stmt->getRHS()->IgnoreParenImpCasts(), [](const clang::Stmt *st) -> bool {
    if (auto call = clang::dyn_cast<clang::CallExpr>(st)) return true;
    else if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(st)) {
      auto decl = decl_ref->getDecl();
      if (decl->getType().isVolatileQualified()) return true;
    }
    return false;
  });
  if (res) {
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

/* MISRA
 * Rule: 14.3
 * Controlling expressions shall not be invariant
 */
void MISRAStmtRule::CheckControlStmtVariant(const clang::Expr *stmt) {
  if (stmt == nullptr) return;
  bool need_report = false;

  uint64_t val;
  if (IsIntegerLiteralExpr(stmt, &val) && (val == 0 || val == 1)) {
    need_report = true;
  } else if (auto decl_expr = clang::dyn_cast<clang::DeclRefExpr>(stmt)) {
    auto decl = decl_expr->getDecl();
    if (clang::isa<clang::ParmVarDecl>(decl)) {
      need_report = true;
    } else if (auto var_decl = clang::dyn_cast<clang::VarDecl>(decl)) {
      if (var_decl->hasInit()) need_report = true;
    }
  } else if (auto bin_op = clang::dyn_cast<clang::BinaryOperator>(stmt)) {
    auto opcode = bin_op->getOpcode();
    auto lhs = bin_op->getLHS()->IgnoreParenImpCasts();
    auto lhs_type = lhs->getType();
    auto rhs = bin_op->getRHS()->IgnoreParenImpCasts();
    auto rhs_type = rhs->getType();
    if (lhs_type->isUnsignedIntegerType() &&
        (opcode == clang::BO_LT || opcode == clang::BO_GE)) {
      if (IsIntegerLiteralExpr(rhs, &val) && (val == 0)) need_report = true;
    } else if (lhs_type->isUnsignedIntegerType() && opcode == clang::BO_LE) {
      if (IsIntegerLiteralExpr(rhs, &val) && (val == 65535)) need_report = true;
    } else if (lhs_type->isSignedIntegerType() && lhs_type->isCharType()) {
      if (opcode == clang::BO_LT) {
        if (IsIntegerLiteralExpr(rhs, &val) && (val > 128)) need_report = true;
      } else if (opcode == clang::BO_LE) {
        if (IsIntegerLiteralExpr(rhs, &val) && (val >= 128)) need_report = true;
      }
    }
  }

  if (need_report) {
    std::string ref_msg = "Controlling expressions shall not be invariant";
    ReportTemplate(ref_msg, M_R_14_3, stmt);
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
    auto st_class = it->getStmtClass();
    if (st_class == clang::Stmt::WhileStmtClass || st_class == clang::Stmt::ForStmtClass ||
        st_class == clang::Stmt::DoStmtClass || st_class == clang::Stmt::SwitchStmtClass)
      continue;
    if ((st_class == clang::Stmt::BreakStmtClass) ||
        (st_class == clang::Stmt::GotoStmtClass)) {
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
 * Rule: 15.5
 * A function should have a single point of exit at the end
 */
void MISRAStmtRule::CheckExitPoint(const clang::ReturnStmt *stmt) {
  bool need_report = false;
  auto ctx = XcalCheckerManager::GetAstContext();
  auto parents = ctx->getParents(*stmt);
  if (parents.empty()) return;
  auto parent = parents[0].get<clang::Stmt>();
  if (parent == nullptr) {
    need_report = true;
  } else {
    if (clang::isa<clang::CompoundStmt>(parent)) {
      parents = ctx->getParents(*parent);
      auto parent_decl = parents[0].get<clang::Decl>();
      if (parent_decl == nullptr || !clang::isa<clang::FunctionDecl>(parent_decl)) {
        need_report = true;
      }
    }
  }
  if (need_report) {
    std::string ref_msg = "A function should have a single point of exit at the end";
    ReportTemplate(ref_msg, M_R_15_5, stmt);
  }
}

/* MISRA
 * Rule: 15.6
 * The body of an iteration-statement or a selection-statement shall be a compound-statement
 */
void MISRAStmtRule::CheckIfWithCompoundStmt(const clang::IfStmt *stmt) {
  auto then = stmt->getThen();
  if (then && clang::isa<clang::CompoundStmt>(then)) return;
  if (stmt->hasElseStorage()) {
    if (clang::isa<clang::CompoundStmt>(stmt->getElse())) return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_15_6, stmt);
  std::string ref_msg = "The body of an iteration-statement or a selection-statement"
                        "shall be a compound-statement";
  issue->SetRefMsg(ref_msg);
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
 * Rule: 16.1
 * All switch statements shall be well-formed
 */
void MISRAStmtRule::ReportSyntaxRuleOfSWitchStmt(const clang::Stmt *stmt) {
  std::string ref_msg = "All switch statements shall be well-formed";
  ReportTemplate(ref_msg, M_R_16_1, stmt);
}

void MISRAStmtRule::CheckSyntaxRuleOfSWitchStmt(const clang::SwitchStmt *stmt) {
  auto cases = stmt->getSwitchCaseList();
  if (cases == nullptr) ReportSyntaxRuleOfSWitchStmt(stmt);

  auto cond = stmt->getCond()->IgnoreParenImpCasts();
  if (cond == nullptr) ReportSyntaxRuleOfSWitchStmt(stmt);
  if (clang::isa<clang::BinaryOperator>(cond)) ReportSyntaxRuleOfSWitchStmt(cond);

  auto body = stmt->getBody();
  if (!clang::isa<clang::CompoundStmt>(body)) {
    ReportSyntaxRuleOfSWitchStmt(body);
  }
  for (const auto &it : body->children()) {
    if (!IsCaseStmt(it) && it->getStmtClass() != clang::Stmt::BreakStmtClass) {
      ReportSyntaxRuleOfSWitchStmt(it);
    }
  }
}

void MISRAStmtRule::CheckSyntaxRuleOfCaseStmt(const clang::CaseStmt *stmt) {
  auto sub = stmt->getSubStmt();
  if (IsCaseStmt(sub)) ReportSyntaxRuleOfSWitchStmt(sub);
  if (clang::isa<clang::CompoundStmt>(sub)) {
    for (const auto &it : sub->children()) {
      if (IsCaseStmt(it)) ReportSyntaxRuleOfSWitchStmt(it);
    }
  }
}

/* MISRA
 * Rule: 16.2
 * A switch label shall only be used when the most closely-enclosing compound statement
 * is the body of a switch statement
 */
void MISRAStmtRule::CheckCaseStmtInSwitchBody(const clang::SwitchStmt *stmt) {
  auto cases = stmt->getSwitchCaseList();
  if (cases == nullptr) return;
  auto body = stmt->getBody();
  auto ctx = XcalCheckerManager::GetAstContext();

  std::vector<const clang::Stmt *> sinks;

  while (cases->getNextSwitchCase()) {
    auto parents = ctx->getParents(*cases);
    if (parents.empty()) {
      cases = cases->getNextSwitchCase();
      continue;
    }
    auto parent = parents[0].get<clang::Stmt>();
    if (parent == body) {
      cases = cases->getNextSwitchCase();
      continue;
    }

    sinks.push_back(cases);
    cases = cases->getNextSwitchCase();
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_16_2, stmt);
    std::string ref_msg = "A switch label shall only be used when the most closely-enclosing compound"
                          " statement is the body of a switch statement";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 16.3
 * Every switch statement shall have a default label
 */
bool MISRAStmtRule::HasBreakStmt(const clang::Stmt *stmt) {
  for (const auto &child : stmt->children()) {
    if (child == nullptr) continue;
    if (child->getStmtClass() == clang::Stmt::BreakStmtClass) return true;
  }
  return false;
}

void MISRAStmtRule::CheckCaseEndWithBreak(const clang::SwitchStmt *stmt) {
  using StmtClass = clang::Stmt::StmtClass;
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto switch_body = stmt->getBody();
  if (switch_body != nullptr) {

    auto it = switch_body->child_begin();
    auto case_end = switch_body->child_end();

    bool need_report = false;
    for (; it != case_end; it++) {
      if (IsCaseStmt(*it)) {
        /*
         * 1. case: ...; break;
         * 2. case: { ...; break; }
         */
        bool has_break = false;
        for (const auto &sub : it->children()) has_break |= HasBreakStmt(sub);
        has_break |= ((std::next(it) != case_end) &&
                      (std::next(it)->getStmtClass() == clang::Stmt::BreakStmtClass));

        auto next = std::next(it);
        while ((next != case_end) &&
               (next->getStmtClass() != clang::Stmt::CaseStmtClass)) {
          if (next->getStmtClass() == clang::Stmt::BreakStmtClass) {
            has_break = true;
            break;
          }
          next = std::next(next);
        }

        if (!has_break && !HasBreakStmt(*it)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_16_3, stmt);
            std::string ref_msg = R"("case" statement without "break" is forbidden)";
            issue->SetRefMsg(ref_msg);
          }
          issue->AddStmt(*it);
        }
      }
    }
  }
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
    bool has_default = false;
    if (cases->getStmtClass() == clang::Stmt::DefaultStmtClass) return;
    while (cases->getNextSwitchCase()) {
      cases = cases->getNextSwitchCase();
      if (cases->getStmtClass() == clang::Stmt::DefaultStmtClass) {
        has_default = true;
        if (cases->getNextSwitchCase() == nullptr) {
          return;
        } else break;
      }
    }

    if (!has_default) return;
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
 * Rule: 17.3
 * A function shall not be declared implicitly
 */
void MISRAStmtRule::CheckImplicitlyDeclaredFunction(const clang::CallExpr *stmt) {
  auto callee_decl = GetCalleeDecl(stmt);
  if (callee_decl == nullptr) return;
  if (callee_decl->getBuiltinID() &&
      callee_decl->getName().startswith("__builtin_")) {
    return;
  }
  if (auto decl = clang::dyn_cast<clang::Decl>(callee_decl)) {
    if (decl->isImplicit()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_17_3, stmt);
      std::string ref_msg = "A function shall not be declared implicitly";
      issue->SetRefMsg(ref_msg);
    }
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
    issue->AddStmt(stmt->getArg(i));
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
      issue->AddStmt(lhs);
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
  * Rule: 18.6
  * The address of an object with automatic storage shall not be copied to another object
  * that persists after the first object has ceased to exist
  */
void MISRAStmtRule::CheckAssignAddrOfLocalVar(const clang::BinaryOperator *stmt) {
  if (!stmt->isAssignmentOp()) return;

  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (auto member = clang::dyn_cast<clang::MemberExpr>(lhs)) {
    lhs = member->getBase()->IgnoreParenImpCasts();
  }
  if (auto expr = clang::dyn_cast<clang::DeclRefExpr>(lhs)) {
    if (auto left_decl = clang::dyn_cast<clang::VarDecl>(expr->getDecl()->getCanonicalDecl())) {
      if (auto param = clang::dyn_cast<clang::ParmVarDecl>(left_decl)) {
        auto type = param->getType();
        if (!type->isPointerType() && !type->isReferenceType()) return;
      }
      if (left_decl->isLocalVarDecl() && !left_decl->isStaticLocal()) {
        return;
      }
      if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(rhs)) {
        if (auto decl = clang::dyn_cast<clang::VarDecl>(decl_ref->getDecl())) {
          if (!decl->hasInit()) return;
          rhs = decl->getInit()->IgnoreParenImpCasts();
        } else return;
      }
      if (auto unary = clang::dyn_cast<clang::UnaryOperator>(rhs)) {
        if (unary->getOpcode() != clang::UO_AddrOf) return;
        if (auto expr = clang::dyn_cast<clang::DeclRefExpr>(unary->getSubExpr()->IgnoreParenImpCasts())) {
          if (auto decl = clang::dyn_cast<clang::VarDecl>(expr->getDecl()->getCanonicalDecl())) {
            if (decl->isLocalVarDecl() && !decl->isStaticLocal()) {
              std::string ref_msg = "The address of an object with automatic storage shall not be copied to "
                                    "another object that persists after the first object has ceased to exist";
              ReportTemplate(ref_msg, M_R_18_6, stmt);
            }
          }
        }
      }
    }
  }
}

void MISRAStmtRule::CheckReturnAddrOfLocalVar(const clang::ReturnStmt *stmt) {
  auto val = stmt->getRetValue()->IgnoreParenImpCasts();
  if (val == nullptr) return;
  if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(val)) {
    if (auto decl = clang::dyn_cast<clang::VarDecl>(decl_ref->getDecl())) {
      if (!decl->hasInit()) return;
      val = decl->getInit()->IgnoreParenImpCasts();
    } else return;
  }
  if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(val)) {
    if (unary_op->getOpcode() != clang::UO_AddrOf) return;
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(unary_op->getSubExpr())) {
      if (auto decl = clang::dyn_cast<clang::VarDecl>(decl_ref->getDecl())) {
        if (decl->isLocalVarDecl() && !decl->isStaticLocal()) {
          std::string ref_msg = "The address of an object with automatic storage shall not be copied to "
                                "another object that persists after the first object has ceased to exist";
          ReportTemplate(ref_msg, M_R_18_6, stmt);
        }
      }
    }
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
  if (conf_mgr->IsStdMemAllocFunction(name)) {
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
 * Rule: 21.13
 * Any value passed to a function in <ctype.h> shall be representable as
 * an unsigned char or be the value EOF
 */
void MISRAStmtRule::CheckValueTypeForCtype(const clang::BinaryOperator *stmt) {
  std::vector<std::string> name_list = {"_ISalnum", "_ISalpha", "_IScntrl", "_ISdigit", "_ISgraph",
                                        "_ISlower", "_ISprint", "_ISpunct", "_ISxdigit", "_ISspace",
                                        "_ISupper", "_ISblank"};
  auto lhs = stmt->getLHS()->IgnoreParenImpCasts();
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (auto rhs_expr = clang::dyn_cast<clang::CStyleCastExpr>(rhs)) {
    auto sub_expr = rhs_expr->getSubExpr();
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(sub_expr)) {
      auto decl = decl_ref->getDecl();
      if (auto enum_const_decl = clang::dyn_cast<clang::EnumConstantDecl>(decl)) {
        auto enum_name = enum_const_decl->getNameAsString();
        auto res = std::find(name_list.begin(), name_list.end(), enum_name);
        if (res == name_list.end()) return;

        if (auto array = clang::dyn_cast<clang::ArraySubscriptExpr>(lhs)) {
          auto array_rhs = array->getRHS()->IgnoreParenImpCasts();
          if (array_rhs == nullptr) return;
          if (auto array_rhs_expr = clang::dyn_cast<clang::CStyleCastExpr>(array_rhs)) {
            auto sub = array_rhs_expr->getSubExpr()->IgnoreParenImpCasts();
            while (clang::isa<clang::CStyleCastExpr>(sub)) {
              sub = clang::dyn_cast<clang::CStyleCastExpr>(sub)->getSubExpr()->IgnoreParenImpCasts();
            }
            if (auto sub_char = clang::dyn_cast<clang::CharacterLiteral>(sub)) return;
            if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(sub)) {
              if (unary_op->getOpcode() == clang::UO_Minus) {
                uint64_t val;
                if (IsIntegerLiteralExpr(unary_op->getSubExpr(), &val) && (val == 1)) return;
              }
            }
            if (auto sub_decl_ref = clang::dyn_cast<clang::DeclRefExpr>(sub)) {
              auto decl = sub_decl_ref->getDecl();
              if (!decl || decl->getType()->isCharType()) return;
            }
            XcalIssue *issue = nullptr;
            XcalReport *report = XcalCheckerManager::GetReport();
            issue = report->ReportIssue(MISRA, M_R_21_13, sub);
            std::string ref_msg = "Any value passed to a function in <ctype.h> shall be representable as "
                                 "an unsigned char or be the value EOF";
            issue->SetRefMsg(ref_msg);
          }
        }
      }
    }
  }
}

/* MISRA
 * Rule: 21.16
 * The pointer arguments to the Standard Library function memcmp shall point to
 * either a pointer type, an essentially signed type, an essentially unsigned type,
 * an essentially Boolean type or an essentially enum type
 */
void MISRAStmtRule::CheckArgumentsOfMemcmp(const clang::CallExpr *stmt) {
  auto callee = GetCalleeDecl(stmt);
  if (callee == nullptr) return;
  if (callee->getNameAsString() != "memcmp") return;

  bool need_report = false;
  auto arg1_type = stmt->getArg(0)->IgnoreParenImpCasts()->getType();
  auto arg2_type = stmt->getArg(1)->IgnoreParenImpCasts()->getType();
  if (arg1_type->isConstantArrayType() || arg2_type->isConstantArrayType()) {
    need_report = true;
  } else {
    if (!arg1_type->isPointerType() || !arg2_type->isPointerType()) return;
    auto type1 = arg1_type->getPointeeType();
    auto type2 = arg2_type->getPointeeType();
    if (!type1->isPointerType() && !type2->isPointerType() &&
        !type1->isSignedIntegerType() && !type2->isSignedIntegerType() &&
        !type1->isUnsignedIntegerType() && !type2->isUnsignedIntegerType() &&
        !type1->isBooleanType() && !type2->isBooleanType() &&
        !type1->isEnumeralType() && !type2->isEnumeralType()) {
      need_report = true;
    }
  }
  if (need_report) {
    std::string ref_msg = "The pointer arguments to the Standard Library function memcmp shall point to "
                          "either a pointer type, an essentially signed type, an essentially unsigned type, "
                          "an essentially Boolean type or an essentially enum type";
    ReportTemplate(ref_msg, M_R_21_16, stmt);
  }
}

/* MISRA
  * Rule: 22.5
  * A pointer to a FILE object shall not be dereferenced
  */
void MISRAStmtRule::CheckFILEPointerDereference(const clang::UnaryOperator *stmt) {
  if (stmt->getOpcode() == clang::UO_Deref) {
    auto sub = stmt->getSubExpr()->IgnoreParenImpCasts();
    ReportFILEPointer(sub->getType(), stmt);
  }
}

void MISRAStmtRule::CheckDirectManipulationOfFILEPointer(const clang::MemberExpr *stmt) {
  auto base = stmt->getBase()->IgnoreParenImpCasts();
  if (base == nullptr) return;
  ReportFILEPointer(base->getType(), stmt);
}

void MISRAStmtRule::ReportFILEPointer(const clang::QualType type, const clang::Stmt *stmt) {
    if (!type->isPointerType()) return;
    std::string type_name = GetTypeString(type->getPointeeType());
    if (type_name.find("FILE") != std::string::npos) {
      std::string ref_msg = "A pointer to a FILE object shall not be dereferenced";
      ReportTemplate(ref_msg, M_R_22_5, stmt);
    }
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

  // ignore CLASS::method();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto begin = src_mgr->getCharacterData(stmt->getBeginLoc());
  auto end = src_mgr->getCharacterData(stmt->getEndLoc());
  while (begin != end) {
    if (*begin == ':') {
      if ((begin + 1 != end) && (*(begin + 1) == ':')) return;
    }
    begin++;
  }
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

  auto ctx = XcalCheckerManager::GetAstContext();
  auto bt_lhs_ty = clang::dyn_cast<clang::BuiltinType>(lhs_literal->getType())->getKind();
  auto bt_rhs_ty = clang::dyn_cast<clang::BuiltinType>(rhs_literal->getType())->getKind();
  uint64_t width = std::max(ctx->getTypeSize(lhs_literal->getType()), ctx->getTypeSize(rhs_literal->getType()));

  uint64_t half = 1UL << (width - 1);
  uint64_t UN_MAX = (half - 1) + half;

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
  if (_current_function_decl == nullptr) return;
  if (clang::isa<clang::FunctionDecl>(stmt->getSingleDecl())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_3_1_2, stmt);
    std::string ref_msg = "Functions shall not be declared at block scope.";
    issue->SetRefMsg(ref_msg);
  }

}

/*
 * MISRA: 4-5-1
 * Expressions with type bool shall not be used as operands to built-in operators
 * other than the assignment operator =, the logical operators &&, ||, !, the
 * equality operators == and !=, the unary & operator, and the conditional operator.
 */
void MISRAStmtRule::CheckBoolUsedAsNonLogicalOperand(const clang::UnaryOperator *stmt) {
  auto op = stmt->getOpcode();
  if (op == clang::UnaryOperator::Opcode::UO_LNot ||
      op == clang::UnaryOperator::Opcode::UO_AddrOf)
    return;
  auto sub_type = StripAllParenImpCast(stmt->getSubExpr())->getType();
  if (!sub_type->isBooleanType()) return;

  std::string ref_msg = "Bool shall be used as logical operands";
  ReportTemplate(ref_msg, M_R_4_5_1, stmt);
}

void MISRAStmtRule::CheckBoolUsedAsNonLogicalOperand(const clang::BinaryOperator *stmt) {
  auto opcode = stmt->getOpcode();
  if (stmt->isLogicalOp() || stmt->isAssignmentOp() || stmt->isCommaOp() ||
      (opcode == clang::BinaryOperator::Opcode::BO_EQ) ||
      (opcode == clang::BinaryOperator::Opcode::BO_NE))
    return;
  auto lhs_type = StripAllParenImpCast(stmt->getLHS())->getType();
  auto rhs_type = StripAllParenImpCast(stmt->getRHS())->getType();
  if (!lhs_type->isBooleanType() && !rhs_type->isBooleanType()) return;

  std::string ref_msg = "Bool shall be used as logical operands";
  ReportTemplate(ref_msg, M_R_4_5_1, stmt);
}

/*
 * MISRA: 4-10-1
 * NULL shall not be used as an integer value.
 */
void MISRAStmtRule::CheckNULLUsedAsInteger(const clang::CastExpr *stmt) {
  if (!stmt->getType()->isIntegerType()) return;
  if (!stmt->getSubExpr()->getType()->isPointerType()) return;
  bool need_report = false;
  auto ctx = XcalCheckerManager::GetAstContext();
  auto &src_mgr = ctx->getSourceManager();
  if (clang::isa<clang::GNUNullExpr>(stmt->getSubExpr())) need_report = true;
  if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(stmt->getSubExpr()->IgnoreParenCasts())) {
    clang::Expr::EvalResult eval_result;
    int value;

    // try to fold the const expr
    if (literal->EvaluateAsInt(eval_result, *ctx)) {
      value = eval_result.Val.getInt().getZExtValue();
    } else {
      value = literal->getValue().getZExtValue();
    }
    if (value == 0) need_report = true;
  }
  if (need_report) {
    auto loc = src_mgr.getExpansionLoc(stmt->getSubExpr()->getExprLoc());
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_4_10_1, loc);
    std::string ref_msg = "NULL shall not be used as an integer value.";
    issue->SetRefMsg(ref_msg);
    issue->SetIgnore(false);
  }
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
  if (!stmt->getType()->isPointerType()) return;

  auto k1 = stmt->isNullPointerConstant(*ctx,
                                        clang::Expr::NullPointerConstantValueDependence::NPC_NeverValueDependent);
  auto k2 = sub_expr->isNullPointerConstant(*ctx,
                                            clang::Expr::NullPointerConstantValueDependence::NPC_NeverValueDependent);
  auto zero_literal = clang::Expr::NullPointerConstantKind::NPCK_ZeroLiteral;

  // ignore (void *) 0
  // because this expression maybe expended from #define NULL (void *)0
  if (auto explict_cast = clang::dyn_cast<clang::CStyleCastExpr>(sub_expr)) {
    if (explict_cast->getType()->isVoidPointerType()) return;
  }

  if (k1 == zero_literal && k2 == zero_literal) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_4_10_2, stmt);
    std::string ref_msg = "Using NULL to stand a nullptr instead of using 0";
    issue->SetRefMsg(ref_msg);
    issue->SetIsMaybe();
  }
}

/*
 * MISRA: 5-0-8
 * An explicit integral or floating-point conversion shall not increase the size of the
 * underlying type of a cvalue expression.
 */
void MISRAStmtRule::CheckExplictCastOnIntOrFloatIncreaseSize(const clang::CXXNamedCastExpr *stmt) {
  auto type = stmt->getType();
  auto sub_expr = stmt->getSubExpr()->IgnoreParenImpCasts();
  auto sub_type = sub_expr->getType();

  auto getSubKind = [this](const clang::BinaryOperator *bin_op, bool &status) {
    // check operators of binary expr
    auto lhs = bin_op->getLHS()->IgnoreParenImpCasts();
    auto rhs = bin_op->getRHS()->IgnoreParenImpCasts();
    bool l_status = false, r_status = false;
    auto lkind = GetBTKind(lhs->getType(), l_status);
    auto rkind = GetBTKind(rhs->getType(), r_status);

    if (!l_status || !r_status)
      status = false;
    else
      status = true;

    lkind = UnifyBTTypeKind(lkind);
    rkind = UnifyBTTypeKind(rkind);
    return std::max(lkind, rkind);
  };

  // only check binary expression
  if (auto bin_op = clang::dyn_cast<clang::BinaryOperator>(sub_expr)) {
    bool need_report = false, status = false;
    clang::BuiltinType::Kind bt_kind = GetBTKind(type, status);
    if (!status) return;

    auto type_kind = UnifyBTTypeKind(bt_kind);
    if ((type->isIntegerType() && sub_type->isIntegerType()) ||
        type->isFloatingType() && sub_type->isFloatingType()) {
      auto lhs_ty = bin_op->getLHS()->IgnoreParenImpCasts()->getType();
      auto rhs_ty = bin_op->getRHS()->IgnoreParenImpCasts()->getType();
      if (!(lhs_ty->isIntegerType() || lhs_ty->isFloatingType()) ||
          !(rhs_ty->isIntegerType() || rhs_ty->isFloatingType()))
        return;

      auto bin_kind = getSubKind(bin_op, status);
      if (!status) return;

      if (type_kind > bin_kind) {
        XcalIssue *issue = nullptr;
        XcalReport *report = XcalCheckerManager::GetReport();
        issue = report->ReportIssue(MISRA, M_R_5_0_8, stmt);
        std::string ref_msg = "An explicit integral or floating-point conversion shall not increase the size of the "
                              "underlying type of a cvalue expression.";
        issue->SetRefMsg(ref_msg);
      }
    }
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
  if (!origin_class->hasDefinition() || !origin_class->isPolymorphic()) return;

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
    if (decl_stmt->isSingleDecl()) {
      loop_counter = clang::dyn_cast<clang::VarDecl>(decl_stmt->getSingleDecl());
    }
  }
  if (loop_counter == nullptr || !loop_counter->getType()->isIntegerType()) return;


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

  bool need_report = false;
  if (type->isPointerType()) {
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(val)) {
      if (auto decl = clang::dyn_cast<clang::ParmVarDecl>(decl_ref->getDecl())) {
        // ignore int *f(int *a) { return a; }
        if (decl->getType()->isReferenceType()) need_report = true;
      } else return;
    } else if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(val)) {
      if (unary_op->getOpcode() != clang::UO_AddrOf) return;
      if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(unary_op->getSubExpr())) {
        if (auto param = clang::dyn_cast<clang::ParmVarDecl>(decl_ref->getDecl())) {
          if (param->getType()->isReferenceType()) need_report = true;
        }
      } else return;
    } else return;
  } else {
    if (auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(val)) {
      if (auto param = clang::dyn_cast<clang::ParmVarDecl>(decl_ref->getDecl())) {
        if (param->getType()->isReferenceType()) need_report = true;
      } else return;
    } else return;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_7_5_3, stmt);
    std::string ref_msg = "A function shall not return a reference or a pointer to a parameter that is passed "
                          "by reference or const reference.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * MISRA: 8-4-4
 * A function identifier shall either be used to call the function or it shall be preceded by &.
 */
void MISRAStmtRule::CheckUseFunctionNotCallOrDereference(const clang::BinaryOperator *stmt) {
  auto lhs = stmt->getLHS()->IgnoreParenCasts();
  auto rhs = stmt->getRHS()->IgnoreParenCasts();

  bool lhs_res = IsFunction(lhs);
  bool rhs_res = IsFunction(rhs);
  if (lhs_res || rhs_res) {
    std::string ref_msg = "A function identifier shall either be used to call the function or "
                          "it shall be preceded by &.";
    XcalIssue *issue = ReportTemplate(ref_msg, M_R_8_4_4, stmt);
    if (lhs_res) issue->AddStmt(lhs);
    if (rhs_res) issue->AddStmt(rhs);
  }
}

void MISRAStmtRule::CheckUseFunctionNotCallOrDereference(const clang::UnaryOperator *stmt) {
  if (stmt->getOpcode() == clang::UnaryOperator::Opcode::UO_AddrOf) return;
  if (IsFunction(stmt->getSubExpr()->IgnoreParenCasts())) {
    std::string ref_msg = "A function identifier shall either be used to call the function or "
                          "it shall be preceded by &.";
    XcalIssue *issue = ReportTemplate(ref_msg, M_R_8_4_4, stmt);
  }
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
  while (true) {
    if (parents.empty()) return;
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

/*
 * MISRA: 18.1
 * A pointer resulting from arithmetic on a pointer operand shall address
 * an element of the same arrays as that pointer operand.
 */
void MISRAStmtRule::CheckArrayBoundsExceeded(const clang::ArraySubscriptExpr *stmt) {
  auto rhs = stmt->getRHS()->IgnoreParenImpCasts();
  if (rhs == nullptr) return;
  auto base = stmt->getBase()->IgnoreParenImpCasts();
  auto base_type = base->getType();
  unsigned int num_elem = 0;
  if (auto array_type = clang::dyn_cast<clang::ConstantArrayType>(base_type)) {
    num_elem = array_type->getSize().getZExtValue();
  }

  bool need_report = false;
  if (auto integer_literal = clang::dyn_cast<clang::IntegerLiteral>(rhs)) {
    unsigned int size = integer_literal->getValue().getZExtValue();
    if (num_elem && size > num_elem - 1) need_report = true;
  } else if (auto unary_op = clang::dyn_cast<clang::UnaryOperator>(rhs)) {
    if (unary_op->getOpcode() == clang::UO_Minus && IsIntegerLiteralExpr(unary_op->getSubExpr())) {
      need_report = true;
    }
  }

  if (need_report) {
    std::string ref_msg = "A pointer resulting from arithmetic on a pointer operand shall address"
                          " an element of the same arrays as that pointer operand.";
    ReportTemplate(ref_msg, M_R_18_1, stmt);
  }
}

}
}
