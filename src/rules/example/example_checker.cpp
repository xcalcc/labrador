/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// example_checker.cpp
// ====================================================================
//
// implementation for example checker
//

#include "xsca_checker_manager.h"
#include "example_decl_rule.h"
#include "example_stmt_rule.h"
#include "example_type_rule.h"
#include "example_pp_rule.h"
#include "xsca_checker_tmpl.h"
#include "check_rules.hpp"

// used by xsca_link.cpp to make sure this library is linked correctly
int __ExampleRuleLinked__;

namespace xsca {
namespace example {

using ExampleChecker = xsca::XcalCheckerTmpl<ExampleDeclHandler, ExampleStmtHandler,
                           ExampleTypeHandler, ExamplePPHandler>;

// ExampleCheckerFactory
class ExampleCheckerFactory : public XcalCheckerFactory {
public:
  std::unique_ptr<XcalChecker>
  CreateChecker(XcalCheckerManager *mgr) {
    auto checker = std::make_unique<ExampleChecker>(mgr);
    checker->SetCheckCallBack(RunCheck);
    return checker;
  }
};

static XcalCheckerFactoryRegister<ExampleCheckerFactory> registry;

}  // namespace example
}  // namespace xsca
