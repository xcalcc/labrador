/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_decl_rule.cpp
// ====================================================================
//
// implement Decl related rules for MISRA
//

#include <unordered_set>
#include <clang/AST/ASTContext.h>
#include "xsca_report.h"
#include "MISRA_decl_rule.h"

namespace xsca {
namespace rule {


std::string MISRADeclRule::GetTypeString(clang::QualType type) {
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

bool MISRADeclRule::IsExplicitSign(const std::string &type_name) {
  if (type_name.find("unsigned ") != std::string::npos) {
    return true;
  } else {
    if (type_name.find("signed ") != std::string::npos) {
      return true;
    }
  }
  return false;
};

const clang::CXXRecordDecl *MISRADeclRule::GetBaseDecl(const clang::CXXBaseSpecifier &BS) {
  auto record_type = clang::dyn_cast<clang::RecordType>(BS.getType());
  if (!record_type) return nullptr;
  auto record_decl = record_type->getAsCXXRecordDecl();
  if (!record_decl) return nullptr;

  if (!record_decl->hasDefinition()) return nullptr;

  return record_decl;
}

/* MISRA
 * Rule: 2.3
 * A project should not contain unused type declarations
 */
void MISRADeclRule::CheckUnusedTypedef(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (auto typedefType = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedecl = typedefType->getDecl();
    _used_typedef.insert(typedecl);
  }
}

void MISRADeclRule::CheckUnusedTypedef(const clang::FieldDecl *decl) {
  auto type = decl->getType();
  if (auto typedefType = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedecl = typedefType->getDecl();
    _used_typedef.insert(typedecl);
  }
}

void MISRADeclRule::CheckUnusedTypedef(const clang::TypedefDecl *decl) {
  auto type = decl->getTypeSourceInfo()->getType();
  if (auto typedef_type = clang::dyn_cast<clang::TypedefType>(type)) {
    auto typedef_decl = typedef_type->getDecl();
    _used_typedef.insert(typedef_decl);
  }
}

void MISRADeclRule::CheckUnusedTypedef() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::TYPEDEF;
  auto used_typedefs = &(this->_used_typedef);

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&used_typedefs, &issue, &report](const std::string &x, const clang::Decl *decl,
                                        IdentifierManager *id_mgr) -> void {
        auto typedef_decl = clang::dyn_cast<const clang::TypedefDecl>(decl);
        if (used_typedefs->find(reinterpret_cast<const clang::TypedefDecl *const>(typedef_decl)) ==
            used_typedefs->end()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_2_3, decl);
            std::string ref_msg = "A project should not contain unused type declarations: ";
            ref_msg += typedef_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 2.6
 * A function should not contain unused label declarations
 */
void MISRADeclRule::CheckUnusedLabelInFunction() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::LABEL;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        if (!decl->isUsed()) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_2_6, decl);
            std::string ref_msg = "A project should not contain unused type declarations: ";
            ref_msg += clang::dyn_cast<clang::LabelDecl>(decl)->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule: 2.7
 * There should be no unused parameters in functions
 */
void MISRADeclRule::CheckUnusedParameters(const clang::FunctionDecl *decl) {
  if (decl->getBody()) {
    if (decl->getNumParams() == 0) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    for (const auto &it : decl->parameters()) {
      if (!it->isUsed()) {
        if (issue == nullptr) {
          issue = report->ReportIssue(MISRA, M_R_2_7, decl);
          std::string ref_msg = "There should be no unused parameters in functions";
          ref_msg += decl->getNameAsString();
          issue->SetRefMsg(ref_msg);
        }
        issue->AddDecl(&(*it));
      }
    }
  }
}

/* MISRA
 * Rule: 5.1
 * External identifiers shall be distinct
 */
void MISRADeclRule::CheckUndistinctExternalIdent() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::unordered_map<std::string, const clang::VarDecl *> vars;

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&vars, &issue, &report](const std::string &x, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        const auto *var_decl = clang::dyn_cast<clang::VarDecl>(decl);
        if (var_decl) {
          auto name = var_decl->getNameAsString();

          if (name.length() > 31) {
            bool found = false;
            for (const auto &it : vars) {
              if (name.substr(0, 31) == it.first.substr(0, 31)) {
                found = true;
                if (issue == nullptr) {
                  issue = report->ReportIssue(MISRA, M_R_5_1, it.second);
                  std::string ref_msg = "External identifiers shall be distinct: ";
                  ref_msg += var_decl->getNameAsString();
                  issue->SetRefMsg(ref_msg);
                } else {
                  issue->AddDecl(it.second);
                }
                issue->AddDecl(var_decl);
              }
            }

            if (!found) {
              vars.emplace(std::make_pair(name, var_decl));
            }

          }
        }
      }, true);

}

/* MISRA
 * Rule: 5.3
 * An identifier declared in an inner scope shall not hide an identifier declared in an outer scope
 */
void MISRADeclRule::CheckIdentifierNameConflict() {
#if 0
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::TYPEDEF;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) -> void {
        std::vector<const clang::VarDecl *> vars;
        id_mgr->GetOuterVariables(var_name, vars);

        if (!vars.empty()) {
          auto var_decl = clang::dyn_cast<clang::VarDecl>(decl);
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_5_3, decl);
            std::string ref_msg = "External identifiers shall be distinct: ";
            ref_msg += var_decl->getNameAsString();
            issue->SetRefMsg(ref_msg);
          } else {
            var_decl->dumpColor();
            issue->AddDecl(var_decl);
          }
          for (const auto &it : vars) {
            issue->AddDecl(&(*it));
          }
        }
      }, true);
#endif
}

/* MISRA
 * Rule: 5.6
 * A typedef name shall be a unique identifier
 */
void MISRADeclRule::CheckTypedefUnique() {
  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();
  constexpr uint32_t kind = IdentifierManager::VAR | IdentifierManager::FIELD | IdentifierManager::TYPE |
                            IdentifierManager::FUNCTION | IdentifierManager::LABEL | IdentifierManager::VALUE;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  top_scope->TraverseAll<kind,
      const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
      [&issue, &report, &top_scope](const std::string &name, const clang::Decl *decl, IdentifierManager *id_mgr) {
        if (top_scope->HasTypeDef<true>(name)) {
          if (issue == nullptr) {
            issue = report->ReportIssue(MISRA, M_R_5_6, decl);
            std::string ref_msg = "A typedef name shall be a unique identifier";
            issue->SetRefMsg(ref_msg);
          } else {
            issue->AddDecl(decl);
          }
        }
      }, true);
}

/* MISRA
 * Rule 6.1
 * Bit-fields shall only be declared with an appropriate type
 * Note: This assumes that the "int" type is 32 bit
 */
void MISRADeclRule::CheckInappropriateBitField(const clang::RecordDecl *decl) {
  if (decl->field_empty()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  for (const auto &field : decl->fields()) {
    bool need_report = false;
    auto type = field->getType();
    if (!type->isIntegerType()) continue;

    auto start = src_mgr->getCharacterData(field->getBeginLoc());
    auto end = src_mgr->getCharacterData(field->getEndLoc());
    std::string token;
    while (start != end) {
      token += *start;
      start++;
    }

    if (!IsExplicitSign(GetTypeString(type)) && !IsExplicitSign(token)) need_report = true;

    auto bt_type = clang::dyn_cast<clang::BuiltinType>(type);
    if (bt_type != nullptr) {
      auto kind = bt_type->getKind();
      if ((kind > clang::BuiltinType::Int && kind <= clang::BuiltinType::Int128) ||
          (kind > clang::BuiltinType::UInt && kind <= clang::BuiltinType::UInt128))
        need_report = true;
    }

    if (need_report) {
      if (issue == nullptr) {
        issue = report->ReportIssue(MISRA, M_R_6_1, decl);
        std::string ref_msg = "Bit-fields shall only be declared with an appropriate type: ";
        ref_msg += decl->getNameAsString();
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(field);
    }
  }
}

/* MISRA
 * Rule: 7.4
 * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
 */
void MISRADeclRule::CheckStringLiteralToNonConstChar(const clang::VarDecl *decl) {
  auto decl_type = decl->getType();
  if (decl_type->isPointerType() && decl_type->getPointeeType()->isCharType() &&
      !decl_type->getPointeeType().isConstQualified()) {
    if (!decl->hasInit()) return;
    auto init = decl->getInit()->IgnoreParenImpCasts();
    if (init->getStmtClass() == clang::Stmt::StringLiteralClass) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_7_4, decl);
      std::string ref_msg = "A string literal shall not be assigned to an object unless the object’s type is \"pointer to const-qualified char\"";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8.8
 * The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage
 */
void MISRADeclRule::CheckStaticSpecifier(const clang::FunctionDecl *decl) {
  if (decl->isThisDeclarationADefinition()) {
    auto canonical_decl = decl->getCanonicalDecl();
    if (canonical_decl->isStatic() != decl->isStatic()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_8_8, decl);
      std::string ref_msg = "The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8.10
 * An inline function shall be declared with the static storage class
 */
void MISRADeclRule::CheckInlineFunctionWithExternalLinkage(const clang::FunctionDecl *decl) {
  if (decl->isInlineSpecified() && (decl->getStorageClass() == clang::StorageClass::SC_Extern)) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_10, decl);
    std::string ref_msg = "An inline function shall be declared with the static storage class";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 8.11
 * When an array with external linkage is declared, its size should be explicitly specified
 */
void MISRADeclRule::CheckImplicitSizeWithExternalArray(const clang::VarDecl *decl) {
  if (!decl->getType()->isArrayType()) return;
  if (decl->getStorageClass() != clang::StorageClass::SC_Extern) return;

  auto array_type = clang::dyn_cast<clang::ConstantArrayType>(decl->getType());
  if (!array_type) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_11, decl);
    std::string ref_msg = "When an array with external linkage is declared, its size should be explicitly specified";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 8.12
 * Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique
 */
void MISRADeclRule::CheckUniqueImplicitEnumerator(const clang::EnumDecl *decl) {
  bool need_report = false;
  const clang::EnumConstantDecl *sink = nullptr;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  std::unordered_map<long long unsigned, const clang::EnumConstantDecl *> inits;
  for (const auto &it : decl->enumerators()) {
    auto val = it->getInitVal().getZExtValue();
    if (it->getInitExpr() == nullptr) {
      auto res = inits.find(val);
      if (res != inits.end()) {
        need_report = true;
        sink = it;
        break;
      }
    } else {
      auto res = inits.find(val);
      if (res != inits.end()) {
        if (res->second->getInitExpr() == nullptr) {
          need_report = true;
          sink = res->second;
        }
      }
    }
    inits.insert({val, it});
  }

  if (need_report) {
    issue = report->ReportIssue(MISRA, M_R_8_12, decl);
    std::string ref_msg = "the value of an implicitly-specified enumeration constant shall be unique";
    issue->SetRefMsg(ref_msg);
    issue->AddDecl(sink);
  }
}

/* MISRA
 * Rule: 9.3
 * Arrays shall not be partially initialized
 */
void MISRADeclRule::CheckArrayPartialInitialized(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  if (!decl->hasInit()) return;
  auto init = decl->getInit();

  auto const_array_type = clang::dyn_cast<clang::ConstantArrayType>(type);
  if (!const_array_type) return;
  auto size = const_array_type->getSize().getZExtValue();

  auto initList = clang::dyn_cast<clang::InitListExpr>(init);
  if (initList == nullptr) return;

  auto inits = initList->inits();
  if (inits.size() == 1) {
    auto head = inits[0];
    if (auto literal = clang::dyn_cast<clang::IntegerLiteral>(head)) {
      if (literal->getValue() == 0) {
        return;
      }
    }
  } else if (inits.size() == size) {
    return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(MISRA, M_R_9_3, decl);
  std::string ref_msg = "Arrays shall not be partially initialized";
  issue->SetRefMsg(ref_msg);

}

/* MISRA
 * Rule: 9.5
 * Where designated initializers are used to initialize an array object the size of the array shall be specified explicitly
 */
void MISRADeclRule::CheckDesignatedInitWithImplicitSizeArray(const clang::VarDecl *decl) {
#if 0
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  if (!decl->hasInit()) return;

  auto inits = clang::dyn_cast<clang::InitListExpr>(decl->getInit());
  for (const auto &it : inits->inits()) {
    if (it->getStmtClass() == clang::Stmt::DesignatedInitExprClass) {
      it->dumpColor();
    }
  }
#endif
}

/* MISRA
 * Rule: 17.6
 * The declaration of an array parameter shall not contain the static keyword between the [ ]
 */
void MISRADeclRule::CheckStaticBetweenBracket(const clang::FunctionDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &param : decl->parameters()) {
    auto param_type = param->getType();
    auto decay_type = clang::dyn_cast<clang::DecayedType>(param_type);
    if (decay_type == nullptr) continue;
    auto origin_type = decay_type->getOriginalType();
    auto array_type = clang::dyn_cast<clang::ArrayType>(origin_type);
    if (array_type == nullptr) continue;
    if (array_type->getSizeModifier() == clang::ArrayType::Static) {
      if (issue == nullptr) {
        issue = report->ReportIssue(MISRA, M_R_17_6, decl);
        std::string ref_msg = "The declaration of an array parameter shall not contain the static keyword between the [ ]";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(param);
    }
  }
}

/* MISRA
 * Rule: 18.7
 * Flexible array members shall not be declared
 */
void MISRADeclRule::CheckFlexibleArray(const clang::RecordDecl *decl) {
  if (decl->hasFlexibleArrayMember()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_7, decl);
    std::string ref_msg = "Flexible array members shall not be declared";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 18.8
 * Variable-length array types shall not be used
 */
void MISRADeclRule::CheckVariableAsArrayLength(const clang::VarDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  auto array_type = clang::dyn_cast<clang::ArrayType>(type);
  if (!array_type) return;
  if (array_type->hasSizedVLAType()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_8, decl);
    std::string ref_msg = "Variable-length array types shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRADeclRule::CheckVariableAsArrayLength(const clang::FieldDecl *decl) {
  auto type = decl->getType();
  if (!type->isArrayType()) return;
  auto array_type = clang::dyn_cast<clang::ArrayType>(type);
  if (!array_type) return;
  bool need_report = false;
  if (array_type->hasSizedVLAType()) {
    need_report = true;
  } else if (auto const_array_type = clang::dyn_cast<clang::ConstantArrayType>(array_type)) {
    auto size = const_array_type->getSize().getZExtValue();
    if (size == 0) need_report = true;
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_18_8, decl);
    std::string ref_msg = "Variable-length array types shall not be used";
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 19.2
 * The union keyword should not be used
 */
void MISRADeclRule::CheckUnionKeyword(const clang::RecordDecl *decl) {
  if (decl->isUnion()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_19_2, decl);
    std::string ref_msg = "The union keyword should not be used";
    issue->SetRefMsg(ref_msg);
  }
}

void MISRADeclRule::CheckUnionKeyword(const clang::TypedefDecl *decl) {
  auto type = decl->getTypeSourceInfo()->getType();
  if (auto elaborated_type = clang::dyn_cast<clang::ElaboratedType>(type)) {
    auto named_type = elaborated_type->getNamedType();
    if (named_type->isUnionType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_19_2, decl);
      std::string ref_msg = "The union keyword should not be used";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/* MISRA
 * Rule: 8-3-1
 * Parameters in an overriding virtual function shall either use the
 * same default arguments as the function they override, or else
 * shall not specify any default arguments.
 */
void MISRADeclRule::CheckOverriddenVirtualFuncHasDiffParam(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->methods().empty()) return;

  auto ctx = XcalCheckerManager::GetAstContext();

  bool need_report = false;
  std::vector<const clang::Decl *> sinks;
  for (const auto &method : decl->methods()) {
    if (!method->isVirtual()) continue;
    for (const auto &origin_method : method->overridden_methods()) {
      const auto size = origin_method->param_size();
      if (size != method->param_size()) continue;
      for (auto i = 0; i < size; i++) {
        auto o_param = origin_method->getParamDecl(i);
        auto c_param = method->getParamDecl(i);
        if (c_param->hasDefaultArg()) {
          auto o_default = o_param->getDefaultArg();
          auto c_default = c_param->getDefaultArg();
          clang::Expr::EvalResult o_val, c_val;
          o_default->EvaluateAsRValue(o_val, *ctx);
          c_default->EvaluateAsRValue(c_val, *ctx);

          // TODO: only check builtin type here
          if (c_val.Val.isInt()) {
            if (c_val.Val.getInt() == o_val.Val.getInt()) {
              continue;
            }
          } else if (c_val.Val.isFloat()) {
            if (c_val.Val.getFloat() == o_val.Val.getFloat()) continue;
          } else {
            continue;;
          }

          need_report = true;
          sinks.push_back(c_param);
          sinks.push_back(o_param);
        }
      }
    }
  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_8_3_1, decl);
    std::string ref_msg = "Parameter init value of virtual overridden function should keep same";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) {
      issue->AddDecl(it);
    }
  }
}

/* MISRA
 * Rule: 8-5-3
 * initial value is a must for the enum
 */
void MISRADeclRule::CheckEnumDeclInit(const clang::EnumDecl *decl) {
  auto enum_begin = decl->enumerator_begin();
  auto enum_end = decl->enumerator_end();
  auto init_expr = enum_begin->getInitExpr();

  bool need_report = false;
  if (init_expr == nullptr) {
    for (; enum_begin != enum_end; enum_begin++) {
      init_expr = enum_begin->getInitExpr();
      if (init_expr != nullptr) {
        need_report = true;
        break;
      }
    }
  } else {
    enum_begin++;

    if (enum_begin != enum_end) {
      // check the second
      bool init_all = false;
      init_expr = enum_begin->getInitExpr();
      if (init_expr == nullptr) {
        init_all = false;
      } else {
        init_all = true;
      }
      enum_begin++;
      if (enum_begin != enum_end) {
        for (; enum_begin != enum_end; enum_begin++) {
          init_expr = enum_begin->getInitExpr();
          if (init_all) {
            if (init_expr == nullptr) need_report = true;
          } else {
            if (init_expr != nullptr) need_report = true;
          }
        }
      }

    }

  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();

    issue = report->ReportIssue(MISRA, M_R_8_5_3, decl);
    std::string ref_msg = "Initial value is a must for the enum: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/* MISRA
 * Rule: 10-1-3
 * base class should not be both virtual and non-virtual in the same hierarchy
 */
void MISRADeclRule::CheckDifferentVirtualInSameHierarchy(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() == 0) return;

  bool need_report = false;
  std::unordered_set<const clang::RecordDecl *> vbases;
  std::unordered_set<const clang::RecordDecl *> sinks;

  auto getParentClassDecl = [](const clang::QualType &type) -> const clang::CXXRecordDecl * {
    auto record_type = clang::dyn_cast<clang::RecordType>(type);
    if (record_type == nullptr) return nullptr;
    auto parent_decl = clang::dyn_cast<clang::CXXRecordDecl>(record_type->getDecl());
    return parent_decl;
  };

  for (const auto &parent : decl->bases()) {
    auto parent_decl = getParentClassDecl(parent.getType());
    if (parent_decl == nullptr) continue;
    if (parent_decl->getNumBases() == 0) continue;

    for (auto &super_class : parent_decl->bases()) {
      auto super_decl = getParentClassDecl(super_class.getType());
      if (super_decl == nullptr) continue;
      if (!parent_decl->isVirtuallyDerivedFrom(super_decl)) {
        if (vbases.find(super_decl) != vbases.end()) {
          need_report = true;
          sinks.insert(parent_decl);
        }
      } else {
        vbases.insert(super_decl);
      }
    }

  }

  if (need_report) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_1_3, decl);
    std::string ref_msg = "base class should not be both virtual and non-virtual in the same hierarchy";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddDecl(it);
  }
}

/* MISRA
 * Rule: 10-2-1
 * all visible names within a inheritance hierarchy must be unique
 */
void MISRADeclRule::CheckUniqueNameInHierarchy(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  if (decl->getNumBases() < 2) return;

  std::unordered_set<const clang::Decl *> sinks;
  std::unordered_map<std::string, const clang::FieldDecl *> field_records;
  std::unordered_map<std::string, const clang::CXXMethodDecl *> method_records;

  for (const auto &it : decl->bases()) {
    auto base = GetBaseDecl(it);
    if (base == nullptr) continue;
    for (const auto &field : base->fields()) {
      auto field_name = field->getNameAsString();
      auto res = field_records.find(field_name);
      if (res != field_records.end()) {
        sinks.insert(field);
        sinks.insert(res->second);
      } else {
        field_records.insert({field_name, field});
      }
    }

    for (const auto &method : base->methods()) {
      auto method_name = method->getNameAsString();
      auto res = method_records.find(method_name);
      if (res != method_records.end()) {
        sinks.insert(method);
        sinks.insert(res->second);
      } else {
        method_records.insert({method_name, method});
      }
    }
  }

  if (!sinks.empty()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_10_2_1, decl);
    std::string ref_msg = "All visible names within a inheritance hierarchy must be unique";
    issue->SetRefMsg(ref_msg);
    for (const auto &it : sinks) issue->AddDecl(it);
  }

}


}
}