/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_checker_manager.cpp
// ====================================================================
//
// implementation for XSCA checker manager which is used to create checker
// instances from checker factories
//

#include "xsca_checker_manager.h"
#include "xsca_null_checker.h"
#include "clang/Frontend/MultiplexConsumer.h"

namespace xsca {

XcalCheckerManager XcalCheckerManager::_instance;

std::unique_ptr<clang::ASTConsumer>
XcalCheckerManager::InitCheckers(clang::CompilerInstance &CI,
    llvm::StringRef InFile)
{
  DBG_ASSERT(_checkers.size() == 0, "checkers initialized.\n");

  std::vector< std::unique_ptr<clang::ASTConsumer> > consumers;
  clang::Preprocessor *pp = &CI.getPreprocessor();

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

    AddChecker(std::move(checker));
  }

  if (consumers.size() == 0) {
    return XcalNullChecker(this).GetAstConsumer();
  }
  else if (consumers.size() == 1) {
    return std::move(consumers.front());
  }
  else {
    return std::make_unique<clang::MultiplexConsumer>(std::move(consumers));
  }
}

void
XcalCheckerManager::FiniCheckers()
{
  _checkers.clear();
}

}  // namespace xsca
