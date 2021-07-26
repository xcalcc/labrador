/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// MISRA_decl_rule.h
// ====================================================================
//
// implement Decl related rules for MISRA-C-2012
//

#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class MISRADeclRule : public DeclNullHandler {
public:
  ~MISRADeclRule() = default;

private:
  std::set<const clang::TypedefNameDecl *> _used_typedef;

  std::string GetTypeString(clang::QualType type);

  bool IsExplicitSign(const std::string &type_name);

    /* MISRA
     * Rule: 2.3
     * A project should not contain unused type declarations
     */
  void CheckUnusedTypedef(const clang::VarDecl *decl);

  void CheckUnusedTypedef(const clang::TypedefDecl *decl);

  void CheckUnusedTypedef();

  /* MISRA
   * Rule: 2.6
   * A function should not contain unused label declarations
   */
  void CheckUnusedLabelInFunction();

  /* MISRA
   * Rule: 2.7
   * There should be no unused parameters in functions
   */
  void CheckUnusedParameters(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 5.1
   * External identifiers shall be distinct
   */
  void CheckUndistinctExternalIdent();

  /* MISRA
   * Rule: 5.3
   * An identifier declared in an inner scope shall not hide an identifier declared in an outer scope
   */
  void CheckIdentifierNameConflict();

  /* MISRA
   * Rule: 5.6
   * A typedef name shall be a unique identifier
   */
  void CheckTypedefUnique();

  /* MISRA
   * Rule 6.1
   * Bit-fields shall only be declared with an appropriate type
   * Note: This assumes that the "int" type is 32 bit
   */
  void CheckInappropriateBitField(const clang::RecordDecl *);

  /* MISRA
   * Rule: 7.4
   * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
   */
  void CheckStringLiteralToNonConstChar(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 8.8
   * The static storage class specifier shall be used in all declarations of objects and functions that have internal linkage
   */
  void CheckStaticSpecifier(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 8.10
   * An inline function shall be declared with the static storage class
   */
  void CheckInlineFunctionWithExternalLinkage(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 8.11
   * When an array with external linkage is declared, its size should be explicitly specified
   */
  void CheckImplicitSizeWithExternalArray(const clang::VarDecl *decl);

public:
  void Finalize() {
    CheckUnusedTypedef();
    CheckUnusedLabelInFunction();
    CheckUndistinctExternalIdent();
    CheckIdentifierNameConflict();
    CheckTypedefUnique();
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckUnusedTypedef(decl);
    CheckStringLiteralToNonConstChar(decl);
    CheckImplicitSizeWithExternalArray(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckUnusedTypedef(decl);
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckUnusedParameters(decl);
    CheckStaticSpecifier(decl);
    CheckInlineFunctionWithExternalLinkage(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckInappropriateBitField(decl);
  }


}; // MISRADeclRule
} // rule
} // xsca

