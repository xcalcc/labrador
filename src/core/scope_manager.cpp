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

/* Check if the identifier is in the variable map. */
bool IdentifierManager::HasVariableName(const std::string &var_name,
                                        bool recursive) const {
  int num = _id_to_var.count(var_name);
  if (!recursive)
    return num;
  if (num == 0) {
    for (const auto &it : _scope->Children()) {
      num = it->HasVariableName(var_name, recursive);
    }
  }
  return num;
}

bool IdentifierManager::IsKeyword(const std::string &var_name) const {
  auto res = keywords.find(var_name);
  if (res != keywords.end()) {
    return true;
  }
  return false;
}

bool LexicalScope::HasVariableName(const std::string &var_name,
                                   bool recursive) const {
  bool res = _identifiers->HasVariableName(var_name, recursive);
  return res;
}

}; // namespace xsca