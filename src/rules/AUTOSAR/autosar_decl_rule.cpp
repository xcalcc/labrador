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


#include "autosar_enum.inc"
#include "autosar_decl_rule.h"
#include <unordered_set>
#include <clang/AST/Attr.h>
#include <clang/AST/ExprCXX.h>

namespace xsca {
namespace rule {

bool AUTOSARDeclRule::IsAssign(clang::OverloadedOperatorKind kind) const {
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

bool AUTOSARDeclRule::IsCmp(clang::OverloadedOperatorKind kind) const {
  if (kind >= clang::OverloadedOperatorKind::OO_EqualEqual &&
      kind <= clang::OverloadedOperatorKind::OO_Spaceship)
    return true;

  if (kind >= clang::OverloadedOperatorKind::OO_Exclaim &&
      kind <= clang::OverloadedOperatorKind::OO_Greater)
    return true;

  return false;
}

bool AUTOSARDeclRule::IsCmp(clang::NamedDecl *decl) const {
  std::unordered_set<std::string> cmp_names = {
      "operator==", "operator<=", "operator>=", "operator>", "operator<", "operator!="
  };
  if (!decl || !clang::isa<clang::FunctionDecl>(decl)) return false;
  auto func = clang::dyn_cast<clang::FunctionDecl>(decl);
  auto name = func->getNameAsString();
  for (const auto &it : cmp_names) {
    if (it == name) return true;
  }
  return false;
}

// strip all parenthesis expression and implicit cast expression
const clang::Expr *AUTOSARDeclRule::StripAllParenImpCast(const clang::Expr *stmt) {
  auto res = stmt;
  auto stmt_class = stmt->getStmtClass();
  while (stmt_class == clang::Stmt::ParenExprClass ||
         stmt_class == clang::Stmt::ImplicitCastExprClass) {
    res = res->IgnoreParenImpCasts();
    stmt_class = res->getStmtClass();
  }
  return res;
}

/*
 * AUTOSAR: A2-10-1
 * An identifier declared in an inner scope shall not hide an
 * identifier declared in an outer scope.
 */
void AUTOSARDeclRule::CheckLocalVarCollideWithGlobal() {
  XcalIssue *var_issue = nullptr, *param_issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto scope_mgr = XcalCheckerManager::GetScopeManager();
  auto top_scope = scope_mgr->GlobalScope();

  using IdentifierKind = IdentifierManager::IdentifierKind;
  constexpr uint32_t kind = IdentifierKind::VAR;

  for (const auto &it : top_scope->Children()) {
    if (it->GetScopeKind() != SK_FUNCTION) continue;
    it->TraverseAll<kind,
        const std::function<void(const std::string &, const clang::Decl *, IdentifierManager *)>>(
        [&](const std::string &var_name, const clang::Decl *decl, IdentifierManager *id_mgr) {
          if (top_scope->HasVariableName<false>(var_name)) {

            if (auto param_decl = clang::dyn_cast<clang::ParmVarDecl>(decl)) {
              if (param_issue == nullptr) {
                param_issue = report->ReportIssue(AUTOSAR, A2_10_1, decl);

                std::string ref_msg = "An identifier declared in an inner scope shall not hide an "
                                      "identifier declared in an outer scope.";
                ref_msg += param_decl->getNameAsString();
                param_issue->SetRefMsg(ref_msg);
              } else {
                param_issue->AddDecl(param_decl);
              }
            } else {
              if (var_issue == nullptr) {
                var_issue = report->ReportIssue(AUTOSAR, A2_10_1, decl);
                std::string ref_msg = "An identifier declared in an inner scope shall not hide an "
                                      "identifier declared in an outer scope.";
                ref_msg += clang::dyn_cast<clang::VarDecl>(decl)->getNameAsString();
                var_issue->SetRefMsg(ref_msg);
              } else {
                var_issue->AddDecl(decl);
              }
            }
          }

        },
        true);
  }
}

/*
 * AUTOSAR: A2-11-1
 * Volatile keyword shall not be used.
 */
void AUTOSARDeclRule::CheckVolatile(const clang::VarDecl *decl) {
  if (decl->getType().isVolatileQualified()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A2_11_1, decl);
    std::string ref_msg = "Volatile keyword shall not be used.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A2-13-4
 * String literals shall not be assigned to non-constant pointers.
 */
void AUTOSARDeclRule::CheckStringLiteralToNonConstantPtr(const clang::VarDecl *decl) {
  if (!decl->hasInit()) return;
  auto decl_ty = decl->getType();
  auto init = decl->getInit()->IgnoreParenImpCasts();
  if (!decl_ty->isPointerType() && !decl_ty->isArrayType()) return;
  if (!decl_ty->getPointeeOrArrayElementType()->isCharType()) return;
  if (decl_ty->isConstantArrayType()) return;
  if (decl_ty->isPointerType() && decl_ty->getPointeeType().isConstQualified()) return;
  if (init->getStmtClass() != clang::Stmt::StringLiteralClass) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A2_13_4, decl);
  std::string ref_msg = "String literals shall not be assigned to non-constant pointers.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A7-2-2
 * Enumeration underlying base type shall be explicitly defined.
 */
void AUTOSARDeclRule::CheckEnumUnderlyingType(const clang::EnumDecl *decl) {
  if (decl->getIntegerTypeSourceInfo()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_2_2, decl);
  std::string ref_msg = "Enumeration underlying base type shall be explicitly defined.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A7-2-3
 * Enumerations shall be declared as scoped enum classes.
 */
void AUTOSARDeclRule::CheckEnumScoped(const clang::EnumDecl *decl) {
  if (decl->isScoped()) return;
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_2_3, decl);
  std::string ref_msg = "Enumerations shall be declared as scoped enum classes.";
  issue->SetRefMsg(ref_msg);
}

/* AUTOSAR
 * Rule: 7-2-4
 * In an enumeration, either (1) none, (2) the first or (3) all enumerators shall be initialized.
 */
void AUTOSARDeclRule::CheckEnumDeclInit(const clang::EnumDecl *decl) {
  auto enum_begin = decl->enumerator_begin();
  auto enum_end = decl->enumerator_end();
  if (decl->enumerators().empty()) return;
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

    issue = report->ReportIssue(AUTOSAR, A7_2_4, decl);
    std::string ref_msg = "Initial value is a must for the enum: ";
    ref_msg += decl->getNameAsString();
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-3-3
 * There shall be no unnamed namespaces in header files.
 */
void AUTOSARDeclRule::CheckUnnamedNamespaceInHeaderFile(const clang::NamespaceDecl *decl) {
  if (!decl->isAnonymousNamespace()) return;
  auto loc = decl->getLocation();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto filename = src_mgr->getFilename(loc);
  if (filename.find(".h") != std::string::npos) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_3, decl);
    std::string ref_msg = "There shall be no unnamed namespaces in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-3-4
 * Using-directives shall not be used.
 */
void AUTOSARDeclRule::CheckUsingDirective(const clang::UsingDirectiveDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, M7_3_4, decl);
  std::string ref_msg = "Using-directives shall not be used.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARDeclRule::CheckUsingDirectiveInHeaderFile(const clang::UsingDirectiveDecl *decl) {
  auto loc = decl->getLocation();
  auto src_mgr = XcalCheckerManager::GetSourceManager();
  auto filename = src_mgr->getFilename(loc);
  if (filename.find(".h") != std::string::npos) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_6, decl);
    std::string ref_msg = "Using-directives and using-declarations shall not be used in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-3-6
 * Using-directives and using-declarations
 * (excluding class scope or function scope using-declarations)
 * shall not be used in header files.
 */
void AUTOSARDeclRule::CheckUsingDeclInHeaderFile(const clang::UsingDecl *decl) {
  clang::Decl *sink = nullptr;
  for (const auto &it : decl->shadows()) {
    auto target = it->getTargetDecl();
    if (clang::isa<clang::FunctionDecl>(target) ||
        clang::isa<clang::CXXRecordDecl>(target)) {
      continue;
    }
    sink = it;
    break;
  }

  if (sink != nullptr) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A7_3_6, decl);
    std::string ref_msg = "Using-directives and using-declarations shall not be used in header files.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A7-1-6
 * The typedef specifier shall not be used.
 */
void AUTOSARDeclRule::CheckTypedefDecl(const clang::TypedefDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A7_1_6, decl);
  std::string ref_msg = "The typedef specifier shall not be used.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A8-2-1
 * When declaring function templates, the trailing return type syntax shall
 * be used if the return type depends on the type of parameters.
 */
void AUTOSARDeclRule::CheckTrailingReturnWhenDependTypeParameter(const clang::FunctionTemplateDecl *decl) {
  auto func_decl = decl->getAsFunction();
  auto ret_type = func_decl->getReturnType();
  if (ret_type->isDependentType()) {
    if (!func_decl->getType()->hasAutoForTrailingReturnType()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(AUTOSAR, A8_2_1, decl);
      std::string ref_msg = "When declaring function templates, the trailing return type syntax shall be used if the return "
                            "type depends on the type of parameters.";
      issue->SetRefMsg(ref_msg);
    }
  }
}

/*
 * AUTOSAR: A10-1-1
 * Class shall not be derived from more than one base class which is not an interface class.
 */
void AUTOSARDeclRule::CheckMultiNonAbstractBaseClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition() || decl->bases().empty()) return;
  int count = 0;

  for (const auto &base : decl->bases()) {
    auto base_decl = base.getType()->getAsCXXRecordDecl();
    if (!base_decl) return;

    bool is_abstract = false;
    for (const auto &method : base_decl->methods()) {
      if (method->isPure()) is_abstract = true;
    }
    if (is_abstract) continue;

    count++;
    if (count > 1) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(AUTOSAR, A10_1_1, decl);
      std::string ref_msg = "Class shall not be derived from more than one base class which is not an interface class.";
      issue->SetRefMsg(ref_msg);
      return;
    }
  }

}

/*
 * AUTOSAR: A10-3-1
 * Virtual function declaration shall contain exactly one of the three specifiers:
 * (1) virtual, (2) override, (3) final.
 */
void AUTOSARDeclRule::CheckMethodSpecifier(const clang::CXXRecordDecl *decl) {
  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  for (const auto &method : decl->methods()) {
    if (method->isDefaulted() || !method->isVirtual()) continue;
    bool explict_virtual = method->isVirtualAsWritten();
    bool explict_override = method->getAttr<clang::OverrideAttr>();
    bool explict_final = method->getAttr<clang::FinalAttr>();

    // check virtual function
    if (explict_virtual && (!explict_override && !explict_final)) continue;

    // check override function
    if (method->size_overridden_methods()) {
      if ((explict_override || explict_final) && !explict_virtual) continue;
    }

    if (issue == nullptr) {
      issue = report->ReportIssue(AUTOSAR, A10_3_1, decl);
      std::string ref_msg = "Virtual function declaration shall contain exactly one of the three specifiers:"
                            "virtual, override, final.";
      issue->SetRefMsg(ref_msg);
    }
    issue->AddDecl(method);
  }
}

/*
 * AUTOSAR: A10-3-2
 * Each overriding virtual function shall be declared with the override or final specifier.
 */
void AUTOSARDeclRule::CheckExplictOverriddenFunction(const clang::CXXMethodDecl *decl) {
  if (decl->isPure() || (decl->size_overridden_methods() == 0)) return;
  if (decl->isVirtual()) {
    bool explict_overridden = decl->getAttr<clang::OverrideAttr>();
    bool explict_final = decl->getAttr<clang::FinalAttr>();
    if (explict_overridden ^ explict_final) return;

    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A10_3_2, decl);
    std::string ref_msg = "Each overriding virtual function shall be declared with the override or final specifier.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A10-3-3
 * Virtual functions shall not be introduced in a final class.
 */
void AUTOSARDeclRule::CheckVirtualFunctionsInFinalClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasAttr<clang::FinalAttr>()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &method : decl->methods()) {
    if (method->isVirtual()) {
      if (issue == nullptr) {
        issue = report->ReportIssue(AUTOSAR, A10_3_3, decl);
        std::string ref_msg = "Virtual functions shall not be introduced in a final class.";
        issue->SetRefMsg(ref_msg);
      }
      issue->AddDecl(method);
    }
  }
}

/*
 * AUTOSAR: A10-3-5
 * A user-defined assignment operator shall not be virtual.
 */
void AUTOSARDeclRule::CheckVirtualUserDefinedAssignmentOperator(const clang::CXXMethodDecl *decl) {
  if (!decl->isVirtual()) return;
  if (!decl->isOverloadedOperator()) return;

  if (IsAssign(decl->getOverloadedOperator())) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A10_3_5, decl);
    std::string ref_msg = "A user-defined assignment operator shall not be virtual.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A11-0-1
 * A non-POD type should be defined as class.
 */
void AUTOSARDeclRule::CheckNonPODStruct(const clang::CXXRecordDecl *decl) {
  if (!decl->isStruct() || !decl->hasDefinition()) return;
  if (decl->isPOD()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A11_0_1, decl);
    std::string ref_msg = "A non-POD type should be defined as class.";
    issue->SetRefMsg(ref_msg);
  }

}

/*
 * AUTOSAR: A11-0-2
 * A type defined as struct shall:
 * (1) provide only public data members,
 * (2) not provide any special member functions or methods,
 * (3) not be a base of another struct or class,
 * (4) not inherit from another struct or class.
 */
void AUTOSARDeclRule::CheckStruct(const clang::CXXRecordDecl *decl) {
  if (!decl->isStruct() || !decl->hasDefinition()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  auto Report = [&issue, &report, &decl]() {
    issue = report->ReportIssue(AUTOSAR, A11_0_2, decl);
    std::string ref_msg = "Struct definition is non-compliant.";
    issue->SetRefMsg(ref_msg);
  };

  // 1. provide only public data members
  if (decl->hasPrivateFields() || decl->hasProtectedFields()) {
    Report();
    return;
  }

  // 2. not provide any special member functions or methods
  if (!decl->methods().empty()) {
    Report();
    return;
  }

  // 3. not be a base of another struct or class
  // 4. not inherit from another struct or class
  if (decl->getNumBases() || decl->getNumVBases()) {
    Report();
    for (const auto &it : decl->bases())
      issue->AddDecl(it.getType()->getAsCXXRecordDecl());

    for (const auto &it : decl->vbases())
      issue->AddDecl(it.getType()->getAsCXXRecordDecl());
    return;
  }
}

/*
 * AUTOSAR: A11-3-1
 * Friend declarations shall not be used.
 * It is allowed to declare comparison operators as friend functions
 */
void AUTOSARDeclRule::CheckFriendDeclarations(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  if (!decl->hasFriends()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();

  for (const auto &it : decl->friends()) {
    if (IsCmp(it->getFriendDecl())) continue;

    if (issue == nullptr) {
      issue = report->ReportIssue(AUTOSAR, A11_3_1, decl);
      std::string ref_msg = "Friend declarations shall not be used.";
      issue->SetRefMsg(ref_msg);
    }
    issue->AddDecl(it);
  }
}

/*
 * AUTOSAR: A12-1-6
 * Derived classes that do not need further explicit initialization
 * and require all the constructors from the base class shall use inheriting constructors.
 */
void AUTOSARDeclRule::CheckUnnecessaryCTor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition() || !decl->hasUserDeclaredConstructor()) return;
  if ((decl->getNumBases() + decl->getNumVBases()) == 0) return;

  // check if ctor only has parents constructors
  for (const auto &ctor : decl->ctors()) {
    if (ctor->isDefaultConstructor()) continue;
    if (!ctor->doesThisDeclarationHaveABody()) continue;
    if (ctor->getBody()->children().empty()) {
      // check initializer
      bool only_has_bctor = true;
      for (const auto &init : ctor->inits()) {
        if (!init->isBaseInitializer()) {
          only_has_bctor = false;
          break;
        }

        // check parameter
        auto cxx_ctor = clang::dyn_cast<clang::CXXConstructExpr>(init->getInit());
        if (!cxx_ctor) return;
        for (std::size_t i = 0; i < cxx_ctor->getNumArgs() && i < ctor->getNumParams(); i++) {
          auto arg = StripAllParenImpCast(cxx_ctor->getArg(i));
          auto param = ctor->getParamDecl(i);
          auto decl_ref = clang::dyn_cast<clang::DeclRefExpr>(arg);
          if (!decl_ref) return;
          if (decl_ref->getDecl() != param) return;
        }
      }
      if (!only_has_bctor) return;
    } else {
      return;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A12_1_6, decl);
  std::string ref_msg = "Derived classes that do not need further explicit initialization and require all the constructors "
                        "from the base class shall use inheriting constructors.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A12-4-1
 * Destructor of a base class shall be public virtual, public override or protected non-virtual.
 */
void AUTOSARDeclRule::CheckNonVirtualDestructor(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;
  for (const auto &it : decl->bases()) {
    auto base = it.getType()->getAsCXXRecordDecl();
    if (base->hasUserDeclaredDestructor())
      CheckNonVirtualDestructor(base->getDestructor());
  }

  for (const auto &it : decl->vbases()) {
    auto base = it.getType()->getAsCXXRecordDecl();
    if (base->hasUserDeclaredDestructor())
      CheckNonVirtualDestructor(base->getDestructor());
  }
}

void AUTOSARDeclRule::CheckNonVirtualDestructor(const clang::CXXMethodDecl *decl) {
  if (!clang::isa<clang::CXXDestructorDecl>(decl)) return;

  // public virtual, public override
  if (decl->getAccess() == clang::AccessSpecifier::AS_public) {
    if (decl->isVirtual()) return;
    if (decl->getAttr<clang::OverrideAttr>()) return;
  }

  // protected non-virtual
  if (decl->getAccess() == clang::AccessSpecifier::AS_protected) {
    if (!decl->isVirtual() && !decl->getAttr<clang::OverrideAttr>()) return;
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A12_4_1, decl);
  std::string ref_msg = "Destructor of a base class shall be public virtual, public override or protected non-virtual.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A12-4-2
 * If a public destructor of a class is non-virtual, then the class should be declared final.
 */
void AUTOSARDeclRule::CheckNonVirtualDestructorInNonFinalClass(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition() || !decl->hasUserDeclaredDestructor()) return;
  if (decl->getAttr<clang::FinalAttr>()) return;
  auto dtor = decl->getDestructor();
  if (dtor->isVirtual()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A12_4_2, decl);
  std::string ref_msg = "If a public destructor of a class is non-virtual, then the class should be declared final.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A12-8-7
 * Assignment operators should be declared with the ref-qualifier &.
 */
void AUTOSARDeclRule::CheckAssignmentWithoutRefQualifier(const clang::CXXMethodDecl *decl) {
  if (!decl->isOverloadedOperator()) return;
  if (!IsAssign(decl->getOverloadedOperator())) return;

  if (!decl->getRefQualifier()) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(AUTOSAR, A12_8_7, decl);
    std::string ref_msg = "Assignment operators should be declared with the ref-qualifier &.";
    issue->SetRefMsg(ref_msg);
  }
}

/*
 * AUTOSAR: A13-1-2
 * User defined suffixes of the user defined literal operators shall start with
 * underscore followed by one or more letters.
 */
void AUTOSARDeclRule::CheckUserDefinedSuffixes(const clang::FunctionDecl *decl) {
  if (decl->getDeclName().getNameKind() != clang::DeclarationName::CXXLiteralOperatorName) return;

  auto name = clang::StringRef(decl->getNameAsString()).trim("operator\"\"");
  if (name.startswith("_")) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_1_2, decl);
  std::string ref_msg = "User defined suffixes of the user defined literal operators shall start with underscore followed by one or more letters.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-2-1
 * An assignment operator shall return a reference to “this”.
 */
void AUTOSARDeclRule::CheckAssignmentOperatorReturnThisRef(const clang::CXXMethodDecl *decl) {
  if (!decl->isOverloadedOperator() || !IsAssign(decl->getOverloadedOperator())) return;

  auto ret_type = decl->getReturnType();
  if (ret_type->isReferenceType()) {
    auto ref_type = ret_type.getNonReferenceType();
    auto record = ref_type->getAsCXXRecordDecl();
    if (record && record == decl->getParent() && !ref_type.isConstQualified()) {
      return;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_2_1, decl);
  std::string ref_msg = "An assignment operator shall return a reference to “this”.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-5-1
 * If “operator[]” is to be overloaded with a non-const version,
 * const version shall also be implemented.
 */
void AUTOSARDeclRule::CheckBracketOpOverloadedWithOnlyNonConstVersion(const clang::CXXRecordDecl *decl) {
  if (!decl->hasDefinition()) return;

  bool has_const_version = false, need_const = false;
  for (const auto &it : decl->methods()) {
    if (!it->isOverloadedOperator()) continue;
    if (it->getOverloadedOperator() == clang::OverloadedOperatorKind::OO_Subscript) {
      if (it->isConst()) {
        if (need_const) return;
        has_const_version = true;
      } else {
        if (has_const_version) return;
        need_const = true;
      }
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_5_1, decl);
  std::string ref_msg = "If “operator[]” is to be overloaded with a non-const version, const version shall also be implemented.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-5-2
 * All user-defined conversion operators shall be defined explicit.
 */
void AUTOSARDeclRule::CheckExplictUserDefinedConversionOp(const clang::FunctionDecl *decl) {
  auto conv = clang::dyn_cast<clang::CXXConversionDecl>(decl);
  if (!conv || !conv->isUserProvided()) return;
  if (conv->isExplicit()) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_5_2, decl);
  std::string ref_msg = "All user-defined conversion operators shall be defined explicit.";
  issue->SetRefMsg(ref_msg);
}

/*
 * AUTOSAR: A13-5-5
 * Comparison operators shall be non-member functions with identical
 * parameter types and noexcept.
 */
void AUTOSARDeclRule::CheckComparisonOpDecl(const clang::CXXMethodDecl *decl) {
  if (!decl->isOverloadedOperator()) return;
  if (!IsCmp(decl->getOverloadedOperator())) return;

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_5_5, decl);
  std::string ref_msg = "Comparison operators shall be non-member functions with identical parameter types and noexcept.";
  issue->SetRefMsg(ref_msg);
}

void AUTOSARDeclRule::CheckComparisonOpDecl(const clang::FriendDecl *decl) {
  auto frd_decl = decl->getFriendDecl();
  if (!frd_decl || !IsCmp(frd_decl)) return;

  auto func = clang::dyn_cast<clang::FunctionDecl>(frd_decl);
  if (!func) return;
  if (func->getExceptionSpecType() == clang::ExceptionSpecificationType::EST_BasicNoexcept) {
    if (func->param_size() == 2) {
      auto tp1 = func->parameters()[0]->getType();
      auto tp2 = func->parameters()[1]->getType();
      if (tp1 == tp2) return;
    }
  }

  XcalIssue *issue = nullptr;
  XcalReport *report = XcalCheckerManager::GetReport();
  issue = report->ReportIssue(AUTOSAR, A13_5_5, decl);
  std::string ref_msg = "Comparison operators shall be non-member functions with identical parameter types and noexcept.";
  issue->SetRefMsg(ref_msg);
}

}
}