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

#include "cert_enum.inc"
#include "cert_decl_rule.h"
namespace xsca {
namespace rule {

/*
 * DCL-38
 * Use the correct syntax when declaring a flexible array member
 */
void CERTDeclRule::CheckFlexibleArrayMember(const clang::RecordDecl *decl) {
  if (decl->field_empty()) return;
  clang::FieldDecl *last_field = nullptr;
  for (auto it = decl->field_begin(); it != decl->field_end(); it++) {
    auto tmp = it;
    if (++tmp == decl->field_end()) {
      last_field = *it;
      break;
    }
  }
  if (!last_field->getType()->isArrayType()) return;
  auto array_type =  clang::dyn_cast<clang::ConstantArrayType>(last_field->getType());
  if (array_type != nullptr) {
    if (array_type->getSize() == 1 || array_type->getSize() == 0) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(CERT, DCL_38_C, decl);
      std::string ref_msg = "Use the correct syntax when declaring a flexible array member";
      issue->SetRefMsg(ref_msg);
    }
  }
}

}
}
