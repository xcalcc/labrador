/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_decl_rule.h
// ====================================================================
//
// implement Decl related rules for GJB8114
//


#include "scope_manager.h"
#include "decl_null_handler.h"
//#include <clang/AST/Decl.h>
//#include <vector>

namespace xsca {
namespace rule {

class GJB8114DeclRule : public DeclNullHandler {
public:
  ~GJB8114DeclRule() {}

private:

  /*
   * GJB8114: 5.1.1.8
   * identifier is a must in declaration of struct, enum and union
   */
  void CheckAnonymousRecord(const clang::RecordDecl *decl);

  void CheckAnonymousEnum(const clang::EnumDecl *decl);

  /*
   * GJB8114: 5.1.1.9
   * Anonymous struct in struct is forbidden
   */
  void CheckAnonymousStructInRecord(const clang::RecordDecl *decl);

  /*
   * GJB8114: 5.1.1.12
   * Bit-fields should be the same length and within the length of its origin type
   */
  void CheckUniformityOfBitFields(const clang::RecordDecl *decl);

  /*
   * GJB8114: 5.1.1.17
   * Using "extern" variable in function is forbidden
   */
  void CheckExternVariableInFunction(const clang::VarDecl *decl);

  void CheckExternVariableInFunction(const clang::FunctionDecl *decl);

public:
  void Finalize() {}

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckAnonymousRecord(decl);
    CheckAnonymousStructInRecord(decl);
    CheckUniformityOfBitFields(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckAnonymousRecord(decl);
    CheckAnonymousStructInRecord(decl);
    CheckUniformityOfBitFields(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    CheckAnonymousEnum(decl);
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckExternVariableInFunction(decl);
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckExternVariableInFunction(decl);
  }


}; // GJB8114DeclRule

} // rule
} // xsca
