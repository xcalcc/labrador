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
// MISRA_decl_rule.h
// ====================================================================
//
// implement Decl related rules for MISRA-C-2012
//
#include "MISRA_enum.inc"
#include "scope_manager.h"
#include "decl_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class MISRADeclRule : public DeclNullHandler {
public:
  ~MISRADeclRule() = default;

  MISRADeclRule() {
    _enable = true;
  }

private:
  std::set<const clang::TypedefNameDecl *> _used_typedef;

  std::set<const clang::VarDecl *> _tentative_definitions;

  std::string GetTypeString(clang::QualType type);

  bool IsExplicitSign(const std::string &type_name);

  const clang::CXXRecordDecl *GetBaseDecl(const clang::CXXBaseSpecifier &BS);

  // check if a function has a single parameter whose type is template type
  bool IsSingleTemplateTypeParamFunction(const clang::Decl *decl);

  // check if pointer nested more than tow levels
  bool IsPointerNestedMoreThanTwoLevel(clang::QualType decl_type);

  // check if the expr is an IntegerLiteral expression
  bool IsIntegerLiteralExpr(const clang::Expr *expr, uint64_t *res);

  // get builtin type of typedef
  clang::QualType GetUnderlyingType(clang::QualType type);
  clang::QualType GetUnderlyingType(clang::QualType *type);

  /* MISRA
   * Directive: 1.1
   *
   * Any implementation-defined behaviour on which the output of the
   * program depends shall be documented and understood
   */
  void ReportImplementationDefinedBehaviour(const clang::Decl *decl);

  /* MISRA
   * Directive: 4.5
   * Identifiers in the same namespace with overlapping visibility should be
   * typographically unambiguous
   */
  void StringReplaceAll(std::string &base, std::string src, std::string des);

  uint16_t getLineNumber(clang::SourceLocation loc);

  void CheckUnambiguousIdentifier();

  /* MISRA
   * Directive: 4.6
   * typedefs that indicate size and signedness should be used in place of the
   * basic numerical types
   */
  void ReportTypeOfBasicNumericalType(const clang::Decl *decl);

  bool IsBasicNumericalType(const clang::QualType type);

  void CheckTypedefOfBasicNumericalType(const clang::TypedefDecl *decl);

  void CheckTypeOfBasicNumericalType(const clang::VarDecl *decl);

  void CheckTypeOfBasicNumericalType(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 1.2
   * Language extensions should not be used
   */
  void CheckLanguageExtension(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 2.3
   * A project should not contain unused type declarations
   */
  void CheckUnusedTypedef(clang::QualType type);
  void CheckUnusedTypedef(const clang::FunctionDecl *decl);
  void CheckUnusedTypedef(const clang::TypedefDecl *decl);
  void CheckUnusedTypedef();
  template<typename TYPE>
  void CheckUnusedTypedef(const TYPE *decl) {
    auto type = decl->getType();
    CheckUnusedTypedef(type);
  }

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
   * Rule: 5.2
   * Identifiers declared in the same scope and name space shall be distinct
   */
  void CheckIdentifierSameScopeUndistinct();

  /* MISRA
   * Rule: 5.3
   * An identifier declared in an inner scope shall not hide an identifier declared in an outer scope
   */
  void CheckIdentifierNameConflict();

  /* MISRA
   * Rule: 5.4
   * Macro identifiers shall be distinct
   */
  void CheckMacroIdentifierDistinct();

  /* MISRA
   * Rule: 5.5
   * Identifiers shall be distinct from macro names
   */
  void CheckIdentifiedMacro();

  /* MISRA
   * Rule: 5.6
   * A typedef name shall be a unique identifier
   */
  void CheckTypedefUnique();

  /* MISRA
   * Rule: 5.7
   * A tag name shall be a unique identifier
   */
  void CheckTageUnique();

  /* MISRA
   * Rule: 5.8
   * Identifiers that define objects or functions with external linkage shall be
   * unique
   */
  void CheckExternalIdentifierUnique();

  // Note: write out symbol info into pdb for post processing
  void WriteFuncDeclToPDB(const clang::FunctionDecl *decl);
  void WriteParmVarDeclToPDB(const clang::ParmVarDecl *decl);
  void WriteVarDeclToPDB(const clang::VarDecl *decl);
  void WriteTypedefDeclToPDB(const clang::TypedefDecl *decl);
  void WriteRecordDeclToPDB(const clang::RecordDecl *decl);
  void WriteFieldDeclToPDB(const clang::FieldDecl *decl);

  /* MISRA
   * Rule: 5.9
   * Identifiers that define objects or functions with internal linkage shall be unique
   */
  void CheckInternalIdentifierUnique();

  /* MISRA
   * Rule 6.1
   * Bit-fields shall only be declared with an appropriate type
   * Note: This assumes that the "int" type is 32 bit
   */
  void CheckInappropriateBitField(const clang::RecordDecl *);

  /* MISRA
   * Rule: 6.2
   * A typedef name shall be a unique identifier
   */
  void CheckSingleBitSignedValue(const clang::RecordDecl *decl);

  /* MISRA
   * Rule: 7.4
   * A string literal shall not be assigned to an object unless the object’s type is “pointer to const-qualified char”
   */
  void CheckStringLiteralToNonConstChar(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 8.2
   * Function types shall be in prototype form with named parameters
   */
  void ReportParameterNoIdentifier(const clang::Decl *decl);

  bool IsFunctionNoProtoType(clang::QualType type);

  void CheckParameterNoIdentifier(const clang::VarDecl *decl);

  void CheckParameterNoIdentifier(const clang::TypedefDecl *decl);

  void CheckParameterNoIdentifier(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 8.3
   * All declarations of an object or function shall use the same names and type qualifiers
   */
  void ReportDeclWithDifferentNameOrType(const clang::Decl *decl, const clang::Decl *prev);

  void CheckParameterNameAndType(const clang::FunctionDecl *decl);

  void CheckTypeOfPrevVarDecl(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 8.4
   * A compatible declaration shall be visible when an object or function with
   * external linkage is defined
   */
  void ReportNonVisibleDeclaration(const clang::Decl *decl);

  void CheckDeclarationWithExternalLinkage(const clang::VarDecl *decl);

  void CheckDeclarationWithExternalLinkage(const clang::FunctionDecl *decl);

  void CheckTentativeDefinition();

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

  /* MISRA
   * Rule: 8.12
   * Within an enumerator list, the value of an implicitly-specified enumeration constant shall be unique
   */
  void CheckNonUniqueValueOfEnum(const clang::EnumDecl *decl);

  /* MISRA
   * Rule: 8.14
   * The restrict type qualifier shall not be used
   */
  template<typename TYPE>
  void CheckRestrict(const TYPE *decl) {
    if (decl->getType().isRestrictQualified()) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();
      issue = report->ReportIssue(MISRA, M_R_8_14, decl);
      std::string ref_msg = "The restrict type qualifier shall not be used";
      issue->SetRefMsg(ref_msg);
    }
  }

  /* MISRA
   * Rule: 9.3
   * Arrays shall not be partially initialized
   */
  void CheckArrayPartialInitialized(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 9.5
   * Where designated initializers are used to initialize an array object the size of the array shall be specified explicitly
   */
  void CheckDesignatedInitWithImplicitSizeArray(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 17.1
   * The features of <stdarg.h> shall not be used
   * Rule: 21.5
   * The standard header file <signal.h> shall not be used
   */
  void CheckForbiddenHeaderFile();

  /* MISRA
   * Rule: 17.6
   * The declaration of an array parameter shall not contain the static keyword between the [ ]
   */
  void CheckStaticBetweenBracket(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 18.5
   * Declarations should contain no more than two levels of pointer nesting
   */
  XcalIssue *ReportNestedTwoLevel(const clang::Decl *decl);
  template<typename TYPE>
  void CheckPointerNestedMoreThanTwoLevel(const TYPE * decl) {
    if (IsPointerNestedMoreThanTwoLevel(decl->getType())) {
      ReportNestedTwoLevel(decl);
    }
  }
  void CheckPointerNestedMoreThanTwoLevel(const clang::FunctionDecl *decl);

  /* MISRA
   * Rule: 18.7
   * Flexible array members shall not be declared
   */
  void CheckFlexibleArray(const clang::RecordDecl *decl);

  /* MISRA
   * Rule: 18.8
   * Variable-length array types shall not be used
   */
  void CheckVariableAsArrayLength(const clang::VarDecl *decl);

  void CheckVariableAsArrayLength(const clang::FieldDecl *decl);

  /* MISRA
   * Rule: 19.2
   * The union keyword should not be used
   */
  void CheckUnionKeyword(const clang::RecordDecl *decl);

  void CheckUnionKeyword(const clang::TypedefDecl *decl);

  /* MISRA
   * Rule: 2-10-6
   * If an identifier refers to a type, it shall not also refer to an
   * object or a function in the same scope.
   */
  void CheckObjectOrFunctionConflictWithType();

  /* MISRA
   * Rule: 3-3-1
   * Objects or functions with external linkage shall be declared in a header file.
   */
  template<typename TYPE>
  void CheckExternObjInHeaderFile(const TYPE *decl) {
    if (decl->getStorageClass() != clang::SC_Extern) return;
    auto src_mgr = XcalCheckerManager::GetSourceManager();
    auto location = decl->getBeginLoc();

    if (!src_mgr->getFilename(location).endswith(".h")) return;
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    issue = report->ReportIssue(MISRA, M_R_3_3_1, decl);
    std::string ref_msg = "Objects or functions with external linkage shall be declared in a header file.";
    issue->SetRefMsg(ref_msg);
  }

  /* MISRA
   * Rule: 5-0-19
   * The declaration of objects shall contain no more than two levels of pointer indirection.
   */
  template<typename T>
  void CheckPointerNestedLevel(const T *decl) {
    auto decl_type = decl->getType();
    if (IsPointerNestedMoreThanTwoLevel(decl_type)) {
      XcalIssue *issue = nullptr;
      XcalReport *report = XcalCheckerManager::GetReport();

      issue = report->ReportIssue("MISRA", M_R_5_0_19, decl);
      std::string ref_msg = "The declaration of objects shall contain no more than two levels of pointer indirection.";
      issue->SetRefMsg(ref_msg);
    }
  }


  /* MISRA
   * Rule: 8-3-1
   * Parameters in an overriding virtual function shall either use the
   * same default arguments as the function they override, or else
   * shall not specify any default arguments.
   */
  void CheckOverriddenVirtualFuncHasDiffParam(const clang::CXXRecordDecl *decl);

  /*
 * MISRA: 8-4-4
 * A function identifier shall either be used to call the function or it shall be preceded by &.
 */
  void CheckUseFunctionNotCallOrDereference(const clang::VarDecl *decl);

  /* MISRA
   * Rule: 8-5-3
   * initial value is a must for the enum
   * INFO: MOVED TO AUTOSAR 7-2-4
   */

  /* MISRA
   * Rule: 10-1-3
   * base class should not be both virtual and non-virtual in the same hierarchy
   */
  void CheckDifferentVirtualInSameHierarchy(const clang::CXXRecordDecl *decl);

  /* MISRA
   * Rule: 10-2-1
   * all visible names within a inheritance hierarchy must be unique
   */
  void CheckUniqueNameInHierarchy(const clang::CXXRecordDecl *decl);

  /* MISRA
   * Rule: 10-3-2
   * Overridden virtual functions in derived class should be noted with virtual
   */
  void CheckOverriddenVirtualFunction(const clang::CXXRecordDecl *decl);

  /* MISRA
   * Rule: 11-0-1
   * Member data in non-POD class types shall be private.
   */
  void CheckNonPrivateFieldsInNormalClass(const clang::CXXRecordDecl *decl);

  /*
   * MISRA: 12-1-2
   * Derived class should contain constructor of base class
   */
  void CheckDerivedClassContainConstructorOfBaseClass(const clang::CXXRecordDecl *decl);

  /*
   * MISRA: 12-1-3
   * All constructors that are callable with a single argument of fundamental type shall be declared explicit.
   */
  void CheckExplicitConstructorWithSingleParam(const clang::FunctionDecl *decl);

  /*
   * MISRA: 12-8-2
   * The copy assignment operator shall be declared protected or private in an abstract class.
   */
  void CheckUnPrivateCopyAssigmentOpOfAbstractClass(const clang::CXXRecordDecl *decl);

  /*
   * MISRA: 14-5-2
   * A copy constructor shall be declared when there is a template constructor with
   * a single parameter that is a generic parameter.
   */
  void CheckCTorWithTemplateWithoutCopyCtor(const clang::CXXRecordDecl *decl);

  /*
   * MISRA: 14-5-3
   * A copy assignment operator shall be declared when there is a template
   * assignment operator with a parameter that is a generic parameter.
   */
  void CheckCopyAssignmentWithTemplate(const clang::CXXRecordDecl *decl);

  /*
   * MISRA: 15-4-1
   * If a function is declared with an exception-specification, then all declarations of the same
   * function (in other translation units) shall be declared with the same set of type-ids.
   */
  void CheckExceptionSpecification(const clang::FunctionDecl *decl);

  /*
   * MISRA: 15-5-2
   * Where a function’s declaration includes an exception- specification, the function
   * shall only be capable of throwing exceptions of the indicated type(s).
   */
  void CheckThrownUnSpecifiedType();

public:
  void Finalize() {
//    CheckUnusedTypedef();
    CheckUnusedLabelInFunction();
    CheckUndistinctExternalIdent();
    CheckIdentifierNameConflict();
//    CheckTypedefUnique();
//    CheckTageUnique();
    CheckThrownUnSpecifiedType();
    CheckObjectOrFunctionConflictWithType();
    CheckMacroIdentifierDistinct();
    CheckIdentifiedMacro();
    CheckForbiddenHeaderFile();
    CheckInternalIdentifierUnique();
    CheckIdentifierSameScopeUndistinct();
    CheckTentativeDefinition();
    CheckUnambiguousIdentifier();
    CheckExternalIdentifierUnique();
  }

  void VisitVar(const clang::VarDecl *decl) {
    CheckUnusedTypedef<clang::VarDecl>(decl);
    CheckStringLiteralToNonConstChar(decl);
    CheckImplicitSizeWithExternalArray(decl);
    CheckRestrict<clang::VarDecl>(decl);
    CheckArrayPartialInitialized(decl);
    CheckDesignatedInitWithImplicitSizeArray(decl);
    CheckVariableAsArrayLength(decl);
    CheckPointerNestedLevel(decl);
    CheckExternObjInHeaderFile(decl);
    CheckPointerNestedMoreThanTwoLevel(decl);
    CheckUseFunctionNotCallOrDereference(decl);
    CheckDeclarationWithExternalLinkage(decl);
    CheckTypeOfPrevVarDecl(decl);
    CheckTypeOfBasicNumericalType(decl);
    CheckParameterNoIdentifier(decl);
    WriteVarDeclToPDB(decl);
  }

  void VisitParmVar(const clang::ParmVarDecl *decl) {
    CheckRestrict<clang::ParmVarDecl>(decl);
    CheckPointerNestedLevel(decl);
    WriteParmVarDeclToPDB(decl);
  }

  void VisitEnum(const clang::EnumDecl *decl) {
    CheckNonUniqueValueOfEnum(decl);
  }

  void VisitTypedef(const clang::TypedefDecl *decl) {
    CheckUnusedTypedef(decl);
    CheckUnionKeyword(decl);
    CheckTypedefOfBasicNumericalType(decl);
    CheckParameterNoIdentifier(decl);
    WriteTypedefDeclToPDB(decl);
  }

  void VisitFunction(const clang::FunctionDecl *decl) {
    CheckUnusedTypedef(decl);
//    CheckUnusedParameters(decl);
    CheckStaticSpecifier(decl);
    CheckInlineFunctionWithExternalLinkage(decl);
    CheckStaticBetweenBracket(decl);
    CheckExplicitConstructorWithSingleParam(decl);
    CheckExceptionSpecification(decl);
    CheckExternObjInHeaderFile(decl);
    CheckParameterNoIdentifier(decl);
    CheckPointerNestedMoreThanTwoLevel(decl);
    CheckDeclarationWithExternalLinkage(decl);
    CheckParameterNameAndType(decl);
    CheckTypeOfBasicNumericalType(decl);
    CheckLanguageExtension(decl);
    WriteFuncDeclToPDB(decl);
  }

  void VisitField(const clang::FieldDecl *decl) {
    CheckUnusedTypedef<clang::FieldDecl>(decl);
    CheckVariableAsArrayLength(decl);
    CheckPointerNestedLevel(decl);
    CheckPointerNestedMoreThanTwoLevel(decl);
    WriteFieldDeclToPDB(decl);
  }

  void VisitRecord(const clang::RecordDecl *decl) {
    CheckInappropriateBitField(decl);
    CheckFlexibleArray(decl);
    CheckUnionKeyword(decl);
    CheckSingleBitSignedValue(decl);
    WriteRecordDeclToPDB(decl);
  }

  void VisitCXXRecord(const clang::CXXRecordDecl *decl) {
    CheckDifferentVirtualInSameHierarchy(decl);
    CheckOverriddenVirtualFuncHasDiffParam(decl);
    CheckUniqueNameInHierarchy(decl);
    CheckOverriddenVirtualFunction(decl);
    CheckNonPrivateFieldsInNormalClass(decl);
    CheckDerivedClassContainConstructorOfBaseClass(decl);
    CheckUnPrivateCopyAssigmentOpOfAbstractClass(decl);
    CheckCTorWithTemplateWithoutCopyCtor(decl);
    CheckCopyAssignmentWithTemplate(decl);
    CheckInappropriateBitField(decl);
    CheckFlexibleArray(decl);
    CheckUnionKeyword(decl);
    CheckSingleBitSignedValue(decl);
  }


}; // MISRADeclRule
} // rule
} // xsca

