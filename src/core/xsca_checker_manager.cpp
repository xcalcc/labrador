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
// xsca_checker_manager.cpp
// ====================================================================
//
// implementation for XSCA checker manager which is used to create checker
// instances from checker factories
//

#include "xsca_null_checker.h"
#include "xsca_checker_manager.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include <libgen.h>

namespace xsca {

XcalCheckerManager XcalCheckerManager::_instance;

llvm::cl::opt<std::string> XcalCheckerManager::_enable_opt(
    "enable", llvm::cl::init("MISRA"), llvm::cl::Hidden,
    llvm::cl::desc("switch of rule sets"));

llvm::cl::opt<std::string> _disable_opt(
    "disable", llvm::cl::init(""), llvm::cl::Hidden,
    llvm::cl::desc("switch of rule sets"));

std::unique_ptr<clang::ASTConsumer>
XcalCheckerManager::InitCheckers(clang::CompilerInstance &CI,
                                 llvm::StringRef InFile) {
  DBG_ASSERT(_checkers.size() == 0, "checkers initialized.\n");

  // get ast context and source manager from CI
  _ast_context = &CI.getASTContext();
  _source_mgr = &CI.getASTContext().getSourceManager();

  // initialize report
  _report = std::make_unique<XcalReport>();
  auto file = CI.getSourceManager().fileinfo_begin()->getFirst()->tryGetRealPathName()
      .rtrim(".i").rtrim(".ii");
  _report->Initialize(_source_mgr, basename(const_cast<char *>((file+".fe.vtxt").str().c_str())));

  // initializer consumers and ppcallbacks
  std::vector<std::unique_ptr<clang::ASTConsumer> > consumers;
  clang::Preprocessor *pp = &CI.getPreprocessor();

  auto diag_client = &CI.getDiagnosticClient();
  auto client = static_cast<XscaDiagnosticConsumer *>(diag_client);
  
  for (auto &factory : _factories) {
    std::unique_ptr<XcalChecker> checker = factory->CreateChecker(this);
    DBG_ASSERT(checker, "failed to create checker.\n");
    checker->Initialize(&CI, InFile);
    std::unique_ptr<clang::ASTConsumer> ast_consumer
        = checker->GetAstConsumer();
    if (ast_consumer)
      consumers.push_back(std::move(ast_consumer));
    std::unique_ptr<clang::PPCallbacks> pp_callbacks
        = checker->GetPPCallbacks();
    if (pp_callbacks)
      pp->addPPCallbacks(std::move(pp_callbacks));

    _diagnostic_mgr = checker->GetDiagnosticConsumer();
    if (_diagnostic_mgr != nullptr)
      client->AddConsumer(std::move(_diagnostic_mgr));

    AddChecker(std::move(checker));
  }


  if (consumers.size() == 0) {
    return XcalNullChecker(this).GetAstConsumer();
  } else if (consumers.size() == 1) {
    return std::move(consumers.front());
  } else {
    return std::make_unique<clang::MultiplexConsumer>(std::move(consumers));
  }
}

void
XcalCheckerManager::FiniCheckers() {
  _checkers.clear();
}

}  // namespace xsca
