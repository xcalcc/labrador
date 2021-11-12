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
  void CheckUnnamedNamespace(const clang::NamespaceDecl *decl);

  /*
   * AUTOSAR: A7-3-4
   * Using-directives shall not be used.
   */
  void CheckUsingDirective(const clang::UsingDirectiveDecl *decl);

public:
  void VisitEnum(const clang::EnumDecl *decl) {
    CheckEnumUnderlyingType(decl);
    CheckEnumScoped(decl);
  }

  void VisitUsingDirective(const clang::UsingDirectiveDecl *decl) {
    CheckUsingDirective(decl);
  }

  void VisitNamespace(const clang::NamespaceDecl *decl) {
    CheckUnnamedNamespace(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
  }
};
}
}

#endif //LABRADOR_AUTOSAR_DECL_RULE_H
