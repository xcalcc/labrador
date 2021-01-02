/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
