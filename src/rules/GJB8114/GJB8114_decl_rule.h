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

  /*
   * GJB8114: 5.1.1.8
   * identifier is a must in declaration of struct, enum and union
   */
  void CheckAnonymousRecord(const clang::RecordDecl *decl);
  void CheckAnonymousEnum(const clang::EnumDecl *decl);

private:

public:
  void Finalize() {
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckAnonymousRecord(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    CheckAnonymousEnum(decl);
  }

}; // GJB8114DeclRule

} // rule
} // xsca
