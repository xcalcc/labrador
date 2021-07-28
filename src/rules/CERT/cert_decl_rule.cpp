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