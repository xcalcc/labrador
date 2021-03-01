/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// scope_manager.cpp
// ====================================================================
//
// interface for scope manager
//

#include <set>
#include <scope_manager.h>

namespace xsca {
std::set<std::string> keywords{
    "public", "private",  "protected", "operator",
    "new",    "template", "virtual",   "delete",
    "friend", "cout",     "cin",       "endl"};

bool IdentifierManager::IsKeyword(const std::string &var_name) const {
  auto res = keywords.find(var_name);
  if (res != keywords.end()) {
    return true;
  }
  return false;
}

}; // namespace xsca

