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
// xsca_checker.h
// ====================================================================
//
// interface for XSCA checker
//

#ifndef XSCA_CHECKER_INCLUDED
#define XSCA_CHECKER_INCLUDED

#include "xsca_defs.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include <memory>

namespace xsca {

class XcalCheckerManager;

// class XcalChecker
class XcalChecker {
protected:
  XcalCheckerManager *_manager;

public:
  XcalChecker(XcalCheckerManager *mgr) : _manager(mgr) {}

  virtual ~XcalChecker() {}

public:
  virtual void Initialize(clang::CompilerInstance *CI,
                          llvm::StringRef InFile) = 0;
  virtual std::unique_ptr<clang::ASTConsumer> GetAstConsumer() = 0;
  virtual std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() = 0;

  virtual std::unique_ptr<clang::DiagnosticConsumer>
  GetDiagnosticConsumer() = 0;

};  // XcalChecker

}  // name xsca


#endif  // XSCA_CHECKER_INCLUDED
