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

#include <scope_manager.h>

namespace xsca {

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

bool LexicalScope::HasVariableName(const std::string &var_name,
                                   bool recursive) const {
  bool res = _identifiers->HasVariableName(var_name, recursive);
  return res;
}
}; // namespace xsca