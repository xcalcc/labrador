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
// decl_dump_handler.h
// ====================================================================
//
// decl dump handler which dump clang Decl for reference
//

#ifndef DECL_DUMP_HANDLER_INCLUDED
#define DECL_DUMP_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"

namespace xsca {

// class XcalDeclDumpHandler
class XcalDeclDumpHandler {

public:
  // finalize handler
  void Finalize();

  // generate function prototype from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl);
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // XcalDeclDumpHandler

}  // namespace xsca

#endif  // DECL_DUMP_HANDLER_INCLUDED
