/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// check_rules.hpp
// ====================================================================
//
// The implement of the inspection rules.
//
#include "xsca_checker_manager.h"
#include <bitset>
#include <set>
namespace xsca {

void RunCheck();

class RulesImplement {
public:
  using TraverseExclude = IdentifierManager::TraverseExclude;

  void CheckFunctionNameReuse() {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    auto top_scope = scope_mgr->GlobalScope();
    top_scope->TraverseAll(
        [](const std::string &x, IdentifierManager *id_mgr) -> void {
          if (id_mgr->HasFunctionName(x)) {
            printf("Function name reused: %s\n", x.c_str());
          }
        },
        std::bitset<6>(static_cast<unsigned long>(TraverseExclude::FUNCTION)));
  }
};

void RunCheck() {
  TRACE0();
  RulesImplement rules_impl;
  rules_impl.CheckFunctionNameReuse();
}

}; // namespace xsca
