/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB5369_decl_rule.h
// ====================================================================
//
// implement Decl related rules for GJB5369
//

#include <set>
#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"
//#include <clang/AST/Decl.h>
//#include <clang/AST/ASTContext.h>

namespace xsca {
namespace rule {

class GJB5369DeclRule : public DeclNullHandler {
public:
  ~GJB5369DeclRule() {}

private:
  std::string GetTypeString(clang::QualType type);

  bool IsExplicitSign(std::string type_name);

  bool IsTypedefBasicType(clang::QualType &decl_type);

  void GetFunctionTokens(const clang::FunctionDecl *decl,
                         std::vector<std::string> &tokens);

  bool IsEmptyParamList(const clang::FunctionDecl *decl,
                        std::vector<std::string> &tokens);

  void CheckFunctionNameReuse();

  void CheckVariableNameReuse();

  bool IsPointerNestedMoreThanTwoLevel(clang::QualType decl_type);

  /** GJB5396
   * 4.1.1.3 struct with empty field is forbidden
   */
  void CheckStructEmptyField(const clang::RecordDecl *decl);

  /* GJB5396
   * TODO: 4.1.1.5 declaring the type of parameters is a must
   * TODO: 4.1.1.6 without the parameter declarations
   * in function declaration is forbidden
   * 4.1.1.8 ellipsis in the function parameter list is forbidden
   * 4.1.1.10 the empty function parameter list is forbidden
   * */
  void CheckParameterTypeDecl(const clang::FunctionDecl *decl);

  /* GJB5369: 4.1.1.7
   * Only type but no identifiers in function prototype.
   */
  void CheckParameterNoIdentifier(const clang::FunctionDecl *decl);

  /* GJB5369: 4.1.1.9
   * redefining the keywords of C/C++ is forbidden
   */
  void CheckKeywordRedefine();

  /*
   * GJB5369: 4.1.1.15
   * the sign of the char type should be explicit
   */
  void CheckExplicitCharType(const clang::FunctionDecl *decl);

  void checkExplicitCharType(const clang::RecordDecl *decl);

  void CheckExplicitCharType(const clang::VarDecl *decl);

  /*
   * GJB5369: 4.1.1.17
   * self-defined types(typedef) redefined as other types is forbidden
   */
  void CheckTypedefRedefine();

  /*
   * GJB5369: 4.1.1.19
   * arrays without boundary limitation is forbidden
   */
  void CheckArrayBoundary(const clang::VarDecl *decl);

  /*
   * GJB5369: 4.1.1.21
   * the incomplete declaration of struct is forbidden
   */
  void CheckIncompleteStruct(const clang::RecordDecl *decl);

  /*
   * GJB5369: 4.1.1.22
   * the forms of the parameter declarations in the parameter list
   * should keep in line
   */
  void CheckDifferentParamForms(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.1.2.1
   * Use typedef redefine the basic type
   */
  void CheckTypedefBasicType(const clang::FunctionDecl *decl);

  void CheckTypedefBasicType(const clang::RecordDecl *decl);

  void CheckTypedefBasicType(const clang::VarDecl *decl);

  /*
   * GJB5369: 4.1.2.2
   * avoid using the function as parameter
   */
  void CheckFunctionAsParameter(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.1.2.3
   * using too much parameters(more than 20) is forbidden
   */
  void CheckPlethoraParameters(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.1.2.4
   * using bit-field in struct should be carefully
   * GJB5369: 4.1.2.9
   * using non-named bit fields carefully
   */
  void CheckBitfieldInStruct(const clang::RecordDecl *decl);

  /*
   * GJB5369: 4.1.2.8
   * using "union" carefully
   */
  void CheckUnionDecl(const clang::RecordDecl *decl);

  /*
   * GJB5369: 4.2.1.1
   * procedure must be enclosed in braces
   */
  void CheckProcedureWithBraces(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.2.1.10
   * main function should be defined as:
   * 1. int main(void)
   * 2. int main(int, char*[])
   */
  void CheckMainFunctionDefine(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.2.2.2
   * the function length shouldn't exceed 200 lines
   */
  void CheckFunctionLength(const clang::FunctionDecl *decl);

  /*
   * GJB5369: 4.4.1.2
   * pointer's pointer nested more than two levels is forbidden
   */
  void CheckPointerNestedLevel(const clang::VarDecl *decl);

  void CheckPointerNestedLevel(const clang::FunctionDecl *decl);

  void CheckPointerNestedLevel(const clang::RecordDecl *decl);

  /*
   * GJB5369: 4.4.1.3
   * function pointer is forbidden
   */
  void CheckFunctionPointer(const clang::VarDecl *decl);

  /*
   * GJB5369: 4.6.1.6
   * signed-value must be longer than two bits
   */
  void CheckSingleBitSignedValue(const clang::RecordDecl *decl);

public:
  void Finalize() {
    CheckFunctionNameReuse();
    CheckVariableNameReuse();
    CheckKeywordRedefine();
    CheckTypedefRedefine();
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckParameterNoIdentifier(decl);
    CheckParameterTypeDecl(decl);
    CheckDifferentParamForms(decl);
    CheckFunctionAsParameter(decl);
    CheckPlethoraParameters(decl);
    CheckTypedefBasicType(decl);
    CheckExplicitCharType(decl);
    CheckMainFunctionDefine(decl);
    CheckProcedureWithBraces(decl);
    CheckFunctionLength(decl);
    CheckPointerNestedLevel(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckStructEmptyField(decl);
    CheckIncompleteStruct(decl);
    CheckBitfieldInStruct(decl);
    CheckUnionDecl(decl);
    CheckTypedefBasicType(decl);
    checkExplicitCharType(decl);
    CheckPointerNestedLevel(decl);
//    CheckSingleBitSignedValue(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckStructEmptyField(decl);
    CheckIncompleteStruct(decl);
    CheckBitfieldInStruct(decl);
    CheckUnionDecl(decl);
    CheckTypedefBasicType(decl);
    checkExplicitCharType(decl);
    CheckPointerNestedLevel(decl);
//    CheckSingleBitSignedValue(decl);
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckExplicitCharType(decl);
    CheckArrayBoundary(decl);
    CheckTypedefBasicType(decl);
    CheckPointerNestedLevel(decl);
    CheckFunctionPointer(decl);
  }

}; // GJB5369DeclRule
}  // rule
}  // xsca
