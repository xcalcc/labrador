/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// rule_checker.cpp
// ====================================================================
//
// implementation for rule checker
//

#include "xsca_checker_manager.h"
#include "xsca_decl_rule.h"
#include "xsca_stmt_rule.h"
#include "xsca_type_rule.h"
#include "xsca_pp_rule.h"
#include "xsca_checker_tmpl.h"
#include "xsca_diagnostics.h"

int __RuleLinked__;


namespace xsca {
namespace rule {

using RuleChecker = xsca::XcalCheckerTmpl<RuleDeclHandler, RuleStmtHandler,
                                          RuleTypeHandler, RulePPHandler, RuleDiagnosticManager>;

// RuleCheckerFactory
class RuleCheckerFactory : public XcalCheckerFactory {
public:
  std::unique_ptr<XcalChecker>
  CreateChecker(XcalCheckerManager *mgr) override {
    return std::make_unique<RuleChecker>(mgr);
  }
};

static XcalCheckerFactoryRegister<RuleCheckerFactory> registry;

}  // namespace rule
}  // namespace xsca

