/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// GJB8114_stmt_rule.h
// ====================================================================
//
// implement all stmt related rules in GJB8114
//

#include <unordered_set>

#include "GJB8114_enum.inc"
#include "stmt_null_handler.h"
#include "xsca_checker_manager.h"

namespace xsca {
namespace rule {

class GJB8114StmtRule : public StmtNullHandler {
public:
  ~GJB8114StmtRule() {}

private:
  const clang::FunctionDecl *_current_function_decl;

  // Check if stmt contains bitwise operator
  bool HasBitwiseSubStmt(const clang::Stmt *stmt);

  bool HasPrefixOrPostfixSubStmt(const clang::Stmt *stmt);

  // check if this node is in cpp or hpp file
  bool IsInCPPFile(clang::SourceLocation location);

  // collect object types within try block
  std::vector<clang::QualType>
  RecordThrowObjectTypes(const clang::Stmt *stmt);

  const clang::FunctionDecl *GetCalleeDecl(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.2.1.1
   * Loop body should be enclosed with brace
   */
  void CheckLoopBodyWithBrace(const clang::Stmt *stmt);

  bool CheckStmtWithBrace(const clang::Stmt *stmt);

  /*
   * GJB8114: 5.3.1.3
   * Assigning to pointer parameters is forbidden
   */
  void CheckAssignToPointerParam(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.3.1.9
   * Using NULL to stand a nullptr instead of using 0
   */
  void CheckUsingNullWithPointer(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.4.1.8
   * Cases of switch should have the same hierarchy range
   */
  void CheckDifferentHierarchySwitchCase(const clang::SwitchStmt *stmt);

  /*
   * GJB8114: 5.4.2.1
   * Branches nested more than 7 level is forbidden
   */
  void CheckBranchNestedTooMuch(const clang::IfStmt *stmt);

  /*
   * GJB8114: 5.6.1.4
   * Bitwise operator within logic statement is forbidden
   */
  void CheckBitwiseOpInLogicStmt(const clang::IfStmt *stmt);

  /*
   * GJB8114: 5.6.1.5
   * Using ++ or -- in arithmetic statement or function parameters is forbidden
   */
  void CheckIncOrDecUnaryInStmt(const clang::UnaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.10
   * Performing logic-not on ingeter literal is forbidden
   */
  void CheckNotOperatorOnConstant(const clang::UnaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.11
   * Enum value used by non-enum variable is forbidden
   */
  void CheckUsingEnumByOtherTypeVar(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.6.1.18
   * Using gets function is forbidden
   */
  void CheckUsingGetsFunction(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.6.2.2
   * Be careful with the division of integer and integer
   */
  void CheckIntegerDivision(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.6.2.4
   * Avoid using strcpy function
   */
  void CheckUsingStrcpy(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.6.2.5
   * Avoid using strcat function
   */
  void CheckUsingStrcat(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.7.1.11
   * void is required as the function which has return value is called but the return value is not used
   */
  void CheckUnusedFunctionCast(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.7.1.12
   * Void is not required as the function which is void type is called
   */
  void CheckNotRequiredFunctionCast(const clang::CallExpr *stmt);

  /*
   * GJB8114: 5.8.1.5
   * Suffix of number must use upper case letters
   *
   * GJB8114: 5.8.2.4
   * Using suffix with number is recommended
   */
  void CheckLiteralSuffix(const clang::FloatingLiteral *stmt) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    auto data = src_mgr->getCharacterData(stmt->getBeginLoc());
    int res = CheckLiteralSuffic(data);
    switch (res) {
      case 1: {
        issue = report->ReportIssue(GJB8114, G5_8_1_5, stmt);
        std::string ref_msg = "Suffix of number must use upper case letters";
        issue->SetRefMsg(ref_msg);
        break;
      }
      case 2: {
        issue = report->ReportIssue(GJB8114, G5_8_2_4, stmt);
        std::string ref_msg = "Using suffix with number is recommended";
        issue->SetRefMsg(ref_msg);
      }
      default: break;
    }
  }

  void CheckLiteralSuffix(const clang::IntegerLiteral *stmt) {
    XcalIssue *issue = nullptr;
    XcalReport *report = XcalCheckerManager::GetReport();
    auto src_mgr = XcalCheckerManager::GetSourceManager();

    auto data = src_mgr->getCharacterData(stmt->getBeginLoc());
    int res = CheckLiteralSuffic(data);
    switch (res) {
      case 1: {
        issue = report->ReportIssue(GJB8114, G5_8_1_5, stmt);
        std::string ref_msg = "Suffix of number must use upper case letters";
        issue->SetRefMsg(ref_msg);
        break;
      }
      case 2: {
        issue = report->ReportIssue(GJB8114, G5_8_2_4, stmt);
        std::string ref_msg = "Using suffix with number is recommended";
        issue->SetRefMsg(ref_msg);
      }
      default: break;
    }
  }

  // return 0 if normal, return 1 if G5815, return 2 if G5824
  int CheckLiteralSuffic(const char *data) {
    char ch;
    do {
      ch = *data++;
      if (ch == '.') continue;
      if (std::isdigit(ch)) {
        continue;
      } else if (std::isalpha(ch)) {
        if (std::isupper(ch)) return 0;
        return 1;
      } else {
        return 2;
      }
    } while (true);
  }

  void CheckLiteralSuffix(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.9.1.4
   * Infinite loop must use while(1)
   */
  void CheckInfiniteForLoop(const clang::ForStmt *stmt);

  /*
   * GJB8114: 5.10.1.1
   * Explicit cast is required when assigning float value to int variable
   * GJB8114: 5.10.2.1
   * Rounding need to be taken in account when convert float to integer
   */
  void CheckFloatAssignToInt(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.10.1.1
   * Explicit cast is required when assigning int value to shorter int variable
   */
  void CheckTruncWithoutCastInAssign(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.10.1.3
   * Explicit cast is required when assigning double value to float variable
   */
  void CheckDoubleToFloatWithoutCast(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.10.1.5
   * Explicit cast is required by assignments between pointer type value and non-pointer type value
   */
  void CheckAssignPointerAndNonPointerWithoutCast(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.10.2.2
   * Convert double to float carefully
   */
  void CheckDoubleToFloat(const clang::CastExpr *stmt);

  /*
   * GJB8114: 5.10.2.3
   * Convert int to shorter int carefully
   */
  void CheckIntToShorter(const clang::CastExpr *stmt);

  /*
   * GJB8114: 5.12.1.1
   * Comparing logic values is forbidden
   */
  void CheckComparedLogicValue(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.12.1.4
   * Comparing(bigger or less) unsigned integers with zero is forbidden
   */
  void CheckCompareUnsignedWithZero(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.12.1.5
   * Comparing unsigned number with signed number is forbidden
   */
  void CheckCompareUnsignedWithSigned(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.12.2.1
   * Constant value should stay at left side of the compare operator
   */
  void CheckCompareConstantWithVariable(const clang::BinaryOperator *stmt);

  /*
   * GJB8114: 5.13.1.15
   * Assigning negative value to unsigned variables is forbidden
   */
  void CheckAssignNegToUnsignedVar(const clang::BinaryOperator *stmt);

 /*
  * GJB8114: 6.1.1.2
  * Virtual base class converting to derived class should use dynamic_cast
  */
  void CheckVirtualBaseClassCastToDerivedClass(const clang::CXXReinterpretCastExpr *stmt);

  /*
 * GJB8114: 6.2.1.1
 * Using global variables in construct function is forbidden
 */
  void CheckUsingGlobalVarInConstructor(const clang::DeclRefExpr *stmt);

  /*
   * GJB8114: 6.5.1.1
   * Converting unrelated pointers to object pointers is forbidden
   */
  void CheckUnrelatedCastToObject(const clang::CXXReinterpretCastExpr *stmt);

  /*
   * GJB8114: 6.5.1.2
   * Removing const or volatile from type conversion of pointer or reference
   */
  void CheckConstCastOnPointerOrReference(const clang::CXXConstCastExpr *stmt);

  /*
   * GJB8114: 6.5.2.1
   * Using C++ style type converting operator is recommended
   */
  void CheckCStyleCastInCPPFile(const clang::CStyleCastExpr *stmt);

  /*
   * GJB8114: 6.7.1.1
   * Using reference to pass a array whose size is constant
   */
  void CheckConstLenghtArrayPassToFunction(const clang::CallExpr *stmt);

  /*
   * GJB8114: 6.8.1.2
   * Each specified throw must have a matching catch
   */
  void CheckMissingCatchStmt(const clang::CXXTryStmt *stmt);

  /*
   * GJB8114: 6.8.1.3
   * Exception objects should be catched as reference
   */
  void CheckCatchTypeNotReference(const clang::CXXCatchStmt *stmt);

  /*
   * GJB8114: 6.8.1.5
   * Throwing NULL is forbidden
   * MOVED TO MISRA
   */

  /*
   * GJB8114: 6.8.2.1
   * Using default catch after other catches to avoid omitting
   * MOVED TO MISRA
   */

  /*
   * GJB8114: 6.8.2.2
   * Throwing pointer carefully
   * MOVED TO MISRA
   */

public:
  void VisitIfStmt(const clang::IfStmt *stmt) {
    CheckBranchNestedTooMuch(stmt);
    CheckBitwiseOpInLogicStmt(stmt);
  }

  void VisitForStmt(const clang::ForStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
    CheckInfiniteForLoop(stmt);
  }

  void VisitWhileStmt(const clang::WhileStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
  }

  void VisitDoStmt(const clang::DoStmt *stmt) {
    CheckLoopBodyWithBrace(stmt->getBody());
  }

  void VisitBinaryOperator(const clang::BinaryOperator *stmt) {
    CheckAssignToPointerParam(stmt);
    CheckUsingNullWithPointer(stmt);
    CheckUsingEnumByOtherTypeVar(stmt);
    CheckIntegerDivision(stmt);
    CheckLiteralSuffix(stmt);
    CheckFloatAssignToInt(stmt);
    CheckTruncWithoutCastInAssign(stmt);
    CheckDoubleToFloatWithoutCast(stmt);
    CheckAssignPointerAndNonPointerWithoutCast(stmt);
    CheckComparedLogicValue(stmt);
    CheckCompareUnsignedWithZero(stmt);
    CheckCompareUnsignedWithSigned(stmt);
    CheckCompareConstantWithVariable(stmt);
    CheckAssignNegToUnsignedVar(stmt);
  }

  void VisitUnaryOperator(const clang::UnaryOperator *stmt) {
    CheckIncOrDecUnaryInStmt(stmt);
    CheckNotOperatorOnConstant(stmt);
  }

  void VisitSwitchStmt(const clang::SwitchStmt *stmt) {
    CheckDifferentHierarchySwitchCase(stmt);
  }

  void VisitCallExpr(const clang::CallExpr *stmt) {
    CheckUsingGetsFunction(stmt);
    CheckUnusedFunctionCast(stmt);
    CheckNotRequiredFunctionCast(stmt);
    CheckConstLenghtArrayPassToFunction(stmt);
  }

  void VisitImplicitCastExpr(const clang::ImplicitCastExpr *stmt) {
    CheckDoubleToFloat(stmt);
    CheckIntToShorter(stmt);
  }

  void VisitCStyleCastExpr(const clang::CStyleCastExpr *stmt) {
    CheckDoubleToFloat(stmt);
    CheckIntToShorter(stmt);
    CheckCStyleCastInCPPFile(stmt);
  }

  void VisitCXXReinterpretCastExpr(const clang::CXXReinterpretCastExpr *stmt) {
    CheckVirtualBaseClassCastToDerivedClass(stmt);
  }

  void VisitAtFunctionExit(const clang::Stmt *stmt) {
    _current_function_decl = nullptr;
    XcalCheckerManager::SetCurrentFunction(nullptr);
  }

  void VisitDeclRefExpr(const clang::DeclRefExpr *stmt) {
    CheckUsingGlobalVarInConstructor(stmt);
  }

  void VisitCXXConstCastExpr(const clang::CXXConstCastExpr *stmt) {
    CheckConstCastOnPointerOrReference(stmt);
  }

  void VisitCXXTryStmt(const clang::CXXTryStmt *stmt) {
    CheckMissingCatchStmt(stmt);
  }

  void VisitCXXCatchStmt(const clang::CXXCatchStmt *stmt) {
    CheckCatchTypeNotReference(stmt);
  }

  void VisitCXXThrowExpr(const clang::CXXThrowExpr *stmt) {
  }

public:
  void SetCurrentFunctionDecl(const clang::FunctionDecl *decl) {
    _current_function_decl = decl;
  }

}; // GJB8114StmtRule
}
}
