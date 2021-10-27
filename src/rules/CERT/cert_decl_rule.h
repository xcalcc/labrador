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

#ifndef LABRADOR_CERT_DECL_RULE_H
#define LABRADOR_CERT_DECL_RULE_H

#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {
class CERTDeclRule : public DeclNullHandler {

private:
  /*
   * DCL-38
   * Use the correct syntax when declaring a flexible array member
   */
  void CheckFlexibleArrayMember(const clang::RecordDecl *decl);

public:
  void VisitRecord(const clang::RecordDecl *decl) {
    CheckFlexibleArrayMember(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckFlexibleArrayMember(decl);
  }
};
}
}

#endif //LABRADOR_CERT_DECL_RULE_H
