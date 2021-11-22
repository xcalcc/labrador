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

#ifndef LABRADOR_AUTOSAR_DECL_RULE_H
#define LABRADOR_AUTOSAR_DECL_RULE_H

#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {
class AUTOSARDeclRule : public DeclNullHandler {
public:
  ~AUTOSARDeclRule() = default;

  AUTOSARDeclRule() {
    _disabled = XcalCheckerManager::GetDisableOption().getValue().find("AUTOSAR") != std::string::npos;
  }

private:

  /*
   * AUTOSAR: A7-1-6
   * The typedef specifier shall not be used.
   */
  void CheckTypedefDecl(const clang::TypedefDecl *decl);

  /*
   * AUTOSAR: A7-2-2
   * Enumeration underlying base type shall be explicitly defined.
   */
  void CheckEnumUnderlyingType(const clang::EnumDecl *decl);

  /*
   * AUTOSAR: A7-2-3
   * Enumerations shall be declared as scoped enum classes.
   */
  void CheckEnumScoped(const clang::EnumDecl *decl);

  /*
   * AUTOSAR: A7-3-3
   * There shall be no unnamed namespaces in header files.
   */
  void CheckUnnamedNamespaceInHeaderFile(const clang::NamespaceDecl *decl);

  /*
   * AUTOSAR: A7-3-4
   * Using-directives shall not be used.
   */
  void CheckUsingDirective(const clang::UsingDirectiveDecl *decl);

  /*
   * AUTOSAR: A7-3-6
   * Using-directives and using-declarations
   * (excluding class scope or function scope using-declarations)
   * shall not be used in header files.
   */
  void CheckUsingDeclInHeaderFile(const clang::UsingDecl *decl);
  void CheckUsingDirectiveInHeaderFile(const clang::UsingDirectiveDecl *decl);

  /*
   * AUTOSAR: A8-2-1
   * When declaring function templates, the trailing return type syntax shall
   * be used if the return type depends on the type of parameters.
   */
  void CheckTrailingReturnWhenDependTypeParameter(const clang::FunctionTemplateDecl *decl);

  /*
   * AUTOSAR: A10-1-1
   * Class shall not be derived from more than one base class which is not an interface class.
   */
  void CheckMultiNonAbstractBaseClass(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A10-3-1
   * Virtual function declaration shall contain exactly one of the three specifiers:
   * (1) virtual, (2) override, (3) final.
   */
  void CheckMethodSpecifier(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A10-3-2
   * Each overriding virtual function shall be declared with the override or final specifier.
   */
  void CheckExplictOverriddenFunction(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A10-3-3
   * Virtual functions shall not be introduced in a final class.
   */
  void CheckVirtualFunctionsInFinalClass(const clang::CXXRecordDecl *decl);

public:
  void VisitEnum(const clang::EnumDecl *decl) {
    CheckEnumUnderlyingType(decl);
    CheckEnumScoped(decl);
  }

  void VisitUsingDirective(const clang::UsingDirectiveDecl *decl) {
    CheckUsingDirective(decl);
    CheckUsingDirectiveInHeaderFile(decl);
  }

  void VisitUsing(const clang::UsingDecl *decl) {
    CheckUsingDeclInHeaderFile(decl);
  }

  void VisitNamespace(const clang::NamespaceDecl *decl) {
    CheckUnnamedNamespaceInHeaderFile(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckMultiNonAbstractBaseClass(decl);
    CheckMethodSpecifier(decl);
    CheckVirtualFunctionsInFinalClass(decl);
  }

  void VisitCXXMethod(const clang::CXXMethodDecl *decl) {
    CheckExplictOverriddenFunction(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckTypedefDecl(decl);
  }

  void VisitFunctionTemplate(const clang::FunctionTemplateDecl *decl) {
    CheckTrailingReturnWhenDependTypeParameter(decl);
  }
};
}
}

#endif //LABRADOR_AUTOSAR_DECL_RULE_H
