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
// xsca_dump_checker.h
// ====================================================================
//
// interface for XSCA dump checker which only dumps information
//

#ifndef XSCA_DUMP_CHECKER_INCLUDED
#define XSCA_DUMP_CHECKER_INCLUDED

#include "xsca_defs.h"
#include "decl_dump_handler.h"
#include "pp_dump_handler.h"
#include "xsca_checker.h"

namespace xsca {

class XcalCheckerManager;

// class XcalDumpChecker
class XcalDumpChecker : public XcalChecker {
private:
  clang::CompilerInstance *_CI;

  XcalDeclDumpHandler      _decl_handler;
  using XcalDumpConsumer = XcalAstConsumer<XcalDeclDumpHandler>;

  PPDumpHandler            _pp_handler;
  using XcalDumpCallback = XcalPPCallback<PPDumpHandler>;

public:
  XcalDumpChecker(XcalCheckerManager *mgr)
      : XcalChecker(mgr), _CI(NULL) {}
  ~XcalDumpChecker();

private:
  // Handle XcalChecker interfaces
  void Initialize(clang::CompilerInstance *CI,
                  llvm::StringRef InFile) override {
    DBG_ASSERT(!_CI && CI, "XcalDumpChecker initialized.\n");
    _CI = CI;
  }

  std::unique_ptr<clang::ASTConsumer> GetAstConsumer() override {
    return std::make_unique<XcalDumpConsumer>(_decl_handler, _CI);
  }

  std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() override {
    return std::make_unique<XcalDumpCallback>(_pp_handler, _CI);
  }

};  // XcalDumpChecker

}  // name xsca


#endif  // XSCA_DUMP_CHECKER_INCLUDED
