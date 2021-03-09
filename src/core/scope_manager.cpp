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
#include "scope_manager.h"
#include "xsca_checker_manager.h"

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

bool IdentifierManager::InFunctionRange(clang::SourceLocation Loc) const {
  clang::SourceLocation start_loc, end_loc;
  auto src_mgr = XcalCheckerManager::GetSourceManager();

  for (const auto &it : _id_to_func) {
    start_loc = it.second->getBeginLoc();
    end_loc = it.second->getEndLoc();
    if (Loc > start_loc && Loc < end_loc) {
      return true;
    }
  }
  return false;
}

bool LexicalScope::InFunctionRange(clang::SourceLocation Loc) const {
  return _identifiers->InFunctionRange(Loc);
}

}; // namespace xsca

