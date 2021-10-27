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
// decl_dump_handler.cpp
// ====================================================================
//
// decl dump handler which dump clang Decl for reference
//

#include "decl_dump_handler.h"

namespace xsca {

// finalize dump handler
void
XcalDeclDumpHandler::Finalize() {
}

// generate function body from DeclNodes.inc
#define DECL(DERIVED, BASE) \
    void \
    XcalDeclDumpHandler::Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
      printf("==== %s ====\n", decl->clang::Decl::getDeclKindName()); \
      decl->dump(); \
    }
#define ABSTRACT_DECL(DECL)
# include "clang/AST/DeclNodes.inc"
#undef ABSTRACT_DECL
#undef DECL

}  // namespace xsca

