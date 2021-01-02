/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_opts.cpp
// ====================================================================
//
// implementation for options control to enable/disable rulesets and/or
// individual rules
//

#include "llvm/Support/CommandLine.h"

namespace llvm {

cl::opt<bool> DryRun(
  "dry-run", cl::init(false),
  cl::desc("Use this option to dump information available in preprocessor"
           " and abstract syntax tree (AST)"));

}  // namespace llvm

namespace xsca {

bool isDryRun() {
  return llvm::DryRun;
}

}  // namespace xsca
