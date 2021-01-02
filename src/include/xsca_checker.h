/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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

  ~XcalChecker() {}

public:
  virtual void Initialize(clang::CompilerInstance *CI,
                          llvm::StringRef InFile) = 0;
  virtual std::unique_ptr<clang::ASTConsumer> GetAstConsumer() = 0;
  virtual std::unique_ptr<clang::PPCallbacks> GetPPCallbacks() = 0;

};  // XcalChecker

}  // name xsca


#endif  // XSCA_CHECKER_INCLUDED
