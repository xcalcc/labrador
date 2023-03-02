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
// decl_null_handler.h
// ====================================================================
//
// decl null handler which does nothing on clang Decl
//

#ifndef DECL_NULL_HANDLER_INCLUDED
#define DECL_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"

namespace xsca {

// class DeclNullHandler
class DeclNullHandler {
protected:
  bool _enable;

public:
  DeclNullHandler() : _enable(false) {}

  // finalize handler
  void Finalize() {}

  inline bool Enable() const { return _enable; }

  // generate function prototype from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
    void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
    }
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // DeclNullHandler

}  // namespace xsca

#endif  // DECL_NULL_HANDLER_INCLUDED
