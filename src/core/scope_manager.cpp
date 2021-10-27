/*
   Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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

bool IdentifierManager::IsKeyword(const std::string &var_name) const {
  auto conf_mgr = XcalCheckerManager::GetConfigureManager();
  return conf_mgr->FindCXXKeyword(var_name);
}

bool IdentifierManager::InFunctionRange(clang::SourceLocation Loc) const {
  clang::SourceLocation start_loc, end_loc;

  for (const auto &it : _id_to_func) {
    start_loc = it.second->getBeginLoc();
    end_loc = it.second->getEndLoc();
    if (Loc > start_loc && Loc < end_loc) {
      return true;
    }
  }
  return false;
}

/* Get outer-scope variable name and decl pair  */
void IdentifierManager::GetOuterVariables(const std::string &var_name,
                                          std::vector<const clang::VarDecl *> &variables) const {
  _scope->GetOutterVariables(var_name, variables);
}

bool LexicalScope::InFunctionRange(clang::SourceLocation Loc) const {
  return _identifiers->InFunctionRange(Loc);
}

/* Add exception specifications of functions */
void LexicalScope::AddExceptionSpec(const clang::FunctionDecl *decl, clang::QualType type) {
  auto func = decl;
  if (decl->isThisDeclarationADefinition()) func = decl->getCanonicalDecl();
  _except_map[func].push_back(type);
}

/* Add thrown types of functions */
void LexicalScope::AddThrowType(const clang::FunctionDecl *decl, clang::QualType type) {
  auto func = decl;
  if (decl->isThisDeclarationADefinition()) func = decl->getCanonicalDecl();
  _throw_tp_map[func].push_back(type);
}

std::vector<clang::QualType> LexicalScope::GetExceptionSpec(const clang::FunctionDecl *decl) {
  auto func = decl;
  if (decl->isThisDeclarationADefinition()) func = decl;
  return _except_map[func];
}

std::vector<clang::QualType> LexicalScope::GetThrowType(const clang::FunctionDecl *decl) {
  auto func = decl;
  if (decl->isThisDeclarationADefinition()) func = decl;
  return _throw_tp_map[func];
}


}; // namespace xsca

