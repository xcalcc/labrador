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
