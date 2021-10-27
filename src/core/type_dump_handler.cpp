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
// type_dump_handler.cpp
// ====================================================================
//
// type dump handler which dump clang Type for reference
//

#include "type_dump_handler.h"

namespace xsca {

// generate function body from TypeNodes.inc
#define TYPE(CLASS, BASE) \
    void \
    TypeDumpHandler::Visit##CLASS(const clang::CLASS##Type *type) { \
      printf("==== %s ====\n", type->getTypeClassName()); \
      type->dump(); \
    }
#define ABSTRACT_TYPE(CLASS, BASE)
# include "clang/AST/TypeNodes.inc"
#undef ABSTRACT_TYPE
#undef TYPE

}  // namespace xsca
