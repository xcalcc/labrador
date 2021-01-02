/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// type_null_handler.h
// ====================================================================
//
// type null handler which does nothing on clang Type
//

#ifndef TYPE_NULL_HANDLER_INCLUDED
#define TYPE_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Type.h"

namespace xsca {

// class TypeNullHandler
class TypeNullHandler {

public:
  // generate function prototype from TypeNodes.inc
  #define TYPE(CLASS, BASE) \
      void Visit##CLASS(const clang::CLASS##Type *type) { \
      }
  #define ABSTRACT_TYPE(CLASS, BASE)
  # include "clang/AST/TypeNodes.inc"
  #undef ABSTRACT_TYPE
  #undef TYPE

};  // TypeNullHandler

}  // namespace xsca

#endif  // TYPE_NULL_HANDLER_INCLUDED
