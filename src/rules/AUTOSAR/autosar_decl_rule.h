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

#ifndef LABRADOR_AUTOSAR_DECL_RULE_H
#define LABRADOR_AUTOSAR_DECL_RULE_H

#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {
class AUTOSARDeclRule : public DeclNullHandler {
public:
  ~AUTOSARDeclRule() = default;

  AUTOSARDeclRule() {
    _enable = XcalCheckerManager::GetEnableOption().getValue().find("AUTOSAR") != std::string::npos;
  }

private:

  bool IsAssign(clang::OverloadedOperatorKind kind) const;

  bool IsCmp(clang::OverloadedOperatorKind kind) const;
  bool IsCmp(clang::NamedDecl *decl) const;

  /*
   * AUTOSAR: A7-1-6
   * The typedef specifier shall not be used.
   */
  void CheckTypedefDecl(const clang::TypedefDecl *decl);

  /*
   * AUTOSAR: A7-2-2
   * Enumeration underlying base type shall be explicitly defined.
   */
  void CheckEnumUnderlyingType(const clang::EnumDecl *decl);

  /*
   * AUTOSAR: A7-2-3
   * Enumerations shall be declared as scoped enum classes.
   */
  void CheckEnumScoped(const clang::EnumDecl *decl);

  /* AUTOSAR
   * Rule: 7-2-4
   * Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique
   */
  void CheckUniqueImplicitEnumerator(const clang::EnumDecl *decl);

  /*
   * AUTOSAR: A7-3-3
   * There shall be no unnamed namespaces in header files.
   */
  void CheckUnnamedNamespaceInHeaderFile(const clang::NamespaceDecl *decl);

  /*
   * AUTOSAR: A7-3-4
   * Using-directives shall not be used.
   */
  void CheckUsingDirective(const clang::UsingDirectiveDecl *decl);

  /*
   * AUTOSAR: A7-3-6
   * Using-directives and using-declarations
   * (excluding class scope or function scope using-declarations)
   * shall not be used in header files.
   */
  void CheckUsingDeclInHeaderFile(const clang::UsingDecl *decl);
  void CheckUsingDirectiveInHeaderFile(const clang::UsingDirectiveDecl *decl);

  /*
   * AUTOSAR: A8-2-1
   * When declaring function templates, the trailing return type syntax shall
   * be used if the return type depends on the type of parameters.
   */
  void CheckTrailingReturnWhenDependTypeParameter(const clang::FunctionTemplateDecl *decl);

  /*
   * AUTOSAR: A10-1-1
   * Class shall not be derived from more than one base class which is not an interface class.
   */
  void CheckMultiNonAbstractBaseClass(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A10-3-1
   * Virtual function declaration shall contain exactly one of the three specifiers:
   * (1) virtual, (2) override, (3) final.
   */
  void CheckMethodSpecifier(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A10-3-2
   * Each overriding virtual function shall be declared with the override or final specifier.
   */
  void CheckExplictOverriddenFunction(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A10-3-3
   * Virtual functions shall not be introduced in a final class.
   */
  void CheckVirtualFunctionsInFinalClass(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A10-3-5
   * A user-defined assignment operator shall not be virtual.
   */
  void CheckVirtualUserDefinedAssignmentOperator(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A11-0-1
   * A non-POD type should be defined as class.
   */
  void CheckNonPODStruct(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A11-0-2
   * A type defined as struct shall:
   * (1) provide only public data members,
   * (2) not provide any special member functions or methods,
   * (3) not be a base of another struct or class,
   * (4) not inherit from another struct or class.
   */
  void CheckStruct(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A11-3-1
   * Friend declarations shall not be used.
   * It is allowed to declare comparison operators as friend functions
   */
  void CheckFriendDeclarations(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A12-1-6
   * Derived classes that do not need further explicit initialization
   * and require all the constructors from the base class shall use inheriting constructors.
   */
  void CheckUnnecessaryCTor(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A12-4-1
   * Destructor of a base class shall be public virtual, public override or protected non-virtual.
   */
  void CheckNonVirtualDestructor(const clang::CXXRecordDecl *decl);
  void CheckNonVirtualDestructor(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A12-4-2
   * If a public destructor of a class is non-virtual, then the class should be declared final.
   */
  void CheckNonVirtualDestructorInNonFinalClass(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A12-8-7
   * Assignment operators should be declared with the ref-qualifier &.
   */
  void CheckAssignmentWithoutRefQualifier(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A13-1-2
   * User defined suffixes of the user defined literal operators shall start with
   * underscore followed by one or more letters.
   */
  void CheckUserDefinedSuffixes(const clang::FunctionDecl *decl);

  /*
   * AUTOSAR: A13-2-1
   * An assignment operator shall return a reference to “this”.
   */
  void CheckAssignmentOperatorReturnThisRef(const clang::CXXMethodDecl *decl);

  /*
   * AUTOSAR: A13-5-1
   * If “operator[]” is to be overloaded with a non-const version,
   * const version shall also be implemented.
   */
  void CheckBracketOpOverloadedWithOnlyNonConstVersion(const clang::CXXRecordDecl *decl);

  /*
   * AUTOSAR: A13-5-2
   * All user-defined conversion operators shall be defined explicit.
   */
  void CheckExplictUserDefinedConversionOp(const clang::FunctionDecl *decl);

  /*
   * AUTOSAR: A13-5-5
   * Comparison operators shall be non-member functions with identical
   * parameter types and noexcept.
   */
  void CheckComparisonOpDecl(const clang::CXXMethodDecl *decl);
  void CheckComparisonOpDecl(const clang::FriendDecl *decl);

public:
  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckUserDefinedSuffixes(decl);
    CheckExplictUserDefinedConversionOp(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    CheckEnumUnderlyingType(decl);
    CheckEnumScoped(decl);
    CheckUniqueImplicitEnumerator(decl);
  }

  void VisitUsingDirective(const clang::UsingDirectiveDecl *decl) {
    CheckUsingDirective(decl);
    CheckUsingDirectiveInHeaderFile(decl);
  }

  void VisitUsing(const clang::UsingDecl *decl) {
    CheckUsingDeclInHeaderFile(decl);
  }

  void VisitNamespace(const clang::NamespaceDecl *decl) {
    CheckUnnamedNamespaceInHeaderFile(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckMultiNonAbstractBaseClass(decl);
    CheckMethodSpecifier(decl);
    CheckVirtualFunctionsInFinalClass(decl);
    CheckNonPODStruct(decl);
    CheckStruct(decl);
    CheckFriendDeclarations(decl);
    CheckUnnecessaryCTor(decl);
    CheckNonVirtualDestructor(decl);
    CheckNonVirtualDestructorInNonFinalClass(decl);
    CheckBracketOpOverloadedWithOnlyNonConstVersion(decl);
  }

  void VisitCXXMethod(const clang::CXXMethodDecl *decl) {
    CheckExplictOverriddenFunction(decl);
    CheckVirtualUserDefinedAssignmentOperator(decl);
    CheckAssignmentWithoutRefQualifier(decl);
    CheckAssignmentOperatorReturnThisRef(decl);
    CheckComparisonOpDecl(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckTypedefDecl(decl);
  }

  void VisitFunctionTemplate(const clang::FunctionTemplateDecl *decl) {
    CheckTrailingReturnWhenDependTypeParameter(decl);
  }

  void VisitFriend(const clang::FriendDecl *decl) {
    CheckComparisonOpDecl(decl);
  }
};
}
}

#endif //LABRADOR_AUTOSAR_DECL_RULE_H
