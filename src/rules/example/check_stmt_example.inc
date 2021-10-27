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
// check_stmt_example.inc
// ====================================================================
//
// example to describe how to write checker which checks clang Stmt/Expr
//

class CheckStmtExample : public StmtNullHandler {
public:
  ~CheckStmtExample() {}

private:
  void CheckConsecutiveLabels(const clang::LabelStmt *stmt) {
    /*
     * GJB5369 4.1.1.4
     * Check multiple consecutive labels.
     */
    if (clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())) {
      printf("GJB5396:4.1.1.4: Multiple consecutive labels: %s and %s\n",
             stmt->getDecl()->getNameAsString().c_str(),
             clang::dyn_cast<clang::LabelStmt>(stmt->getSubStmt())
                 ->getDecl()
                 ->getNameAsString()
                 .c_str());
    }
  }

public:
  void VisitGotoStmt(const clang::GotoStmt *stmt) {}

  void VisitCompoundStmt(const clang::CompoundStmt *stmt) {}

  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    CheckConsecutiveLabels(stmt);
  }
}; // CheckStmtExample
