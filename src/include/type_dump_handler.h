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
// type_dump_handler.h
// ====================================================================
//
// type dump handler which dump clang Type for reference
//

#ifndef TYPE_DUMP_HANDLER_INCLUDED
#define TYPE_DUMP_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Type.h"

namespace xsca {

// class TypeDumpHandler
class TypeDumpHandler {

public:
  // generate function prototype from TypeNodes.inc
  #define TYPE(CLASS, BASE) \
      void Visit##CLASS(const clang::CLASS##Type *type);
  #define ABSTRACT_TYPE(CLASS, BASE)
  # include "clang/AST/TypeNodes.inc"
  #undef ABSTRACT_TYPE
  #undef TYPE

};  // TypeDumpHandler

}  // namespace xsca

#endif  // TYPE_DUMP_HANDLER_INCLUDED
