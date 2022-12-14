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

//
// ====================================================================
// GJB8114_decl_rule.h
// ====================================================================
//
// implement Decl related rules for GJB8114
//

#include "xsca_report.h"
#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"
//#include <clang/AST/Decl.h>
//#include <vector>

namespace xsca {
namespace rule {

class GJB8114DeclRule : public DeclNullHandler {
public:
  ~GJB8114DeclRule() {}

  GJB8114DeclRule() {
    _enable = XcalCheckerManager::GetEnableOption().getValue().find("GJB8114") != std::string::npos;
  }

private:
  // check if function has new expr
  bool HasNewExpr(const clang::Stmt *decl);

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

  /*
   * GJB8114: 5.1.2.4
   * Variables should be declared at the beginning of function body
   */
  void CheckVariableDeclPosition(const clang::FunctionDecl *decl);

  /*
   * GJB8114: 5.1.2.5
   * Struct should not nest more than three levels
   */
  void CheckNestedStructure(const clang::RecordDecl *decl);

  /*
   * GJB8114: 5.3.1.7
   * Pointers should be initialized as NULL
   */
  void CheckPointerInitWithNull(const clang::VarDecl *decl);

  /*
   * GJB8114: 5.3.2.2
   * Using void pointer carefully
   */
  void CheckVoidPointer(const clang::VarDecl *decl);

  void CheckVoidPointer(const clang::FunctionDecl *decl);

  /*
   * GJB8114: 5.7.1.13
   * static function must be used
   */
  void CheckUnusedStaticFunction();

  /*
   * GJB8114: 5.7.2.1
   * Avoid using too much parameters in function
   */
  void CheckTooManyParams(const clang::FunctionDecl *decl);

  /*
   * GJB8114: 5.8.1.5
   * Suffix of number must use upper case letters
   *
   * GJB8114: 5.8.2.4
   * Using suffix with number is recommended
   */
  void CheckLiteralSuffixInit(const clang::VarDecl *decl);

  /*
   * GJB8114: 5.11.1.2
   * Omitting init value which depends on the system is forbidden
   */
  void CheckOmitInitValueDependOnSystem();

  /*
   * GJB8114: 5.11.2.1
   * Init the variable at its declaration
   */
  void CheckInitWithDecl();

  /*
   * GJB8114: 5.13.1.3
   * Variable names conflicting with function names is forbidden
   */
  void CheckVariableConflictWithFunction();

  /*
   * GJB8114: 5.13.1.4
   * Variable names conflicting with identifiers in forbidden
   */
  void CheckVariableConflictWithIdentifiers();

  /*
   * GJB8114: 5.13.1.5
   * Variable names conflicting with enum elements is forbidden
   */
  void CheckVariableConflictWithEnumElem();

  /*
   * GJB8114: 5.13.1.6
   * Variable names conflict with typedef value is forbidden
   */
  void CheckVariableConflictWithTypeDef();

  /*
   * GJB8114: 6.1.1.1
   * Copy construct function is a must for classes which has dynamic allocated memory members
   */
  void CheckCopyConstructor(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.1.1.3
   * "virtual" is needed when inheriting from base class in derivative design of diamond structure.
   */
  void CheckDiamondDerivativeWithoutVirtual(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.1.1.4
   * Overloaded assigment operator in abstract classes should be private or protect.
   */
  void CheckAssignOperatorOverload(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.1.2.1
   * Deriving from virtual base class should be carefully
   */
  void CheckDerivedFromAbstractClass(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.1.2.2
   * Using inline functions carefully
   */
  void CheckInlineFunction(const clang::FunctionDecl *decl);

  /*
   * GJB8114: 6.2.1.2
   * Default construct functions should be defined explicitly in class
   */
  void CheckExplictConstructor(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.2.1.3
   * Construct functions which contains only one parameter should be note by "explicit"
   * MOVED TO MISRA
   */

  /*
   * GJB8114: 6.2.1.4
   * All class members should be initialized in constructor
   */
  void CheckInitFieldsInConstructor(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.2.1.5
   * Derived class should contain constructor of base class
   * MOVED TO MISRA
   */

  /*
   * GJB8114: 6.3.1.1
   * Destruct function of classes which contain the virtual functions should be virtual
   */
  void CheckVirtualDestructor(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.4.1.1
   * Default parameters in virtual function of base shouldn't be changed by derived classes
   */
  void CheckDefaultParamChangedInDerivedClassVirtualMethod(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.4.1.2
   * Overridden virtual functions in derived class should be noted with virtual
   * MOVEd to MISRA-2008
   */

  /*
   * GJB8114: 6.4.1.3
   * Non-pure virtual function being overridden as pure virtual function is forbidden
   */
  void CheckNonVirtualMethodOverriddenAsPure(const clang::CXXMethodDecl *decl);

  /*
   * GJB8114: 6.7.1.2
   * Returning non-const value from const member functions is forbidden
   */
  void CheckReturnNonConstPointerOrReferenceFromConstMethod(const clang::CXXMethodDecl *decl);

  /*
   * GJB8114: 6.7.2.1
   * Implement of member functions shouldn't in class definition
   */
  void CheckLocationOfMethodsDefination(const clang::CXXRecordDecl *decl);

  /*
   * GJB8114: 6.9.1.1
   * Declaration, defination and implement of template should be in the same file
   */
  void CheckTemplateDeclaration(const clang::FunctionTemplateDecl *decl);

  /*
   * GJB8114: 6.9.2.1
   * Using typename in template's parameter list is recommended
   */
  void CheckClassUsedInTemplateParameters(const clang::FunctionTemplateDecl *decl);

  /*
   * GJB8114: 6.9.2.2
   * It is recommended that const be in the outermost layer of the description except for the constant pointer
   */
  void CheckPositionOfConst(const clang::VarDecl *decl);

  /*
   * GJB8114: 6.9.2.3
   * Overloading && or || is forbidden
   */
  void CheckOverloadingLogicOperator(const clang::FunctionDecl *decl);

public:
  void Finalize() {
    CheckUnusedStaticFunction();
    CheckOmitInitValueDependOnSystem();
    CheckInitWithDecl();
    CheckVariableConflictWithFunction();
    CheckVariableConflictWithIdentifiers();
    CheckVariableConflictWithEnumElem();
    CheckVariableConflictWithTypeDef();
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckAnonymousRecord(decl);
    CheckAnonymousStructInRecord(decl);
    CheckUniformityOfBitFields(decl);
    CheckNestedStructure(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckAnonymousRecord(decl);
    CheckAnonymousStructInRecord(decl);
    CheckUniformityOfBitFields(decl);
    CheckNestedStructure(decl);
    CheckCopyConstructor(decl);
    CheckDiamondDerivativeWithoutVirtual(decl);
    CheckAssignOperatorOverload(decl);
    CheckDerivedFromAbstractClass(decl);
    CheckExplictConstructor(decl);
    CheckInitFieldsInConstructor(decl);
    CheckVirtualDestructor(decl);
    CheckDefaultParamChangedInDerivedClassVirtualMethod(decl);
    CheckLocationOfMethodsDefination(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    CheckAnonymousEnum(decl);
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckExternVariableInFunction(decl);
    CheckVariableDeclPosition(decl);
    CheckVoidPointer(decl);
    CheckTooManyParams(decl);
    CheckInlineFunction(decl);
    CheckOverloadingLogicOperator(decl);
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckExternVariableInFunction(decl);
    CheckPointerInitWithNull(decl);
    CheckVoidPointer(decl);
    CheckLiteralSuffixInit(decl);
    CheckPositionOfConst(decl);
  }

  void VisitCXXMethod(const clang::CXXMethodDecl *decl) {
    CheckNonVirtualMethodOverriddenAsPure(decl);
    CheckReturnNonConstPointerOrReferenceFromConstMethod(decl);
  }

  void VisitFunctionTemplate(const clang::FunctionTemplateDecl *decl) {
    CheckTemplateDeclaration(decl);
    CheckClassUsedInTemplateParameters(decl);
  }


}; // GJB8114DeclRule

} // rule
} // xsca
