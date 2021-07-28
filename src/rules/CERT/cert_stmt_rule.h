#ifndef LABRADOR_CERT_STMT_RULE_H
#define LABRADOR_CERT_STMT_RULE_H

#include "cert_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class CERTStmtRule : public StmtNullHandler {
public:
  ~CERTStmtRule() = default;

private:
  /*
   * DCL-30
   * Declare objects with appropriate storage durations
   */
  void CheckStorageDuration(const clang::BinaryOperator *stmt);

  void CheckStorageDuration(const clang::ReturnStmt *stmt);

  clang::Decl *GetLocalAddress(const clang::Stmt *stmt);

public:
  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckStorageDuration(stmt);
  }

  void VisitReturnStmt(const clang::ReturnStmt *stmt) {
    CheckStorageDuration(stmt);
  }


};

}
}

#endif //LABRADOR_CERT_STMT_RULE_H
