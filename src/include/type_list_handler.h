/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// type_list_handler.h
// ====================================================================
//
// type list handler which call individual handlers on clang Type
//

#ifndef TYPE_LIST_HANDLER_INCLUDED
#define TYPE_LIST_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Type.h"

namespace xsca {

// class TypeListHandler with variadic template parameters as handlers
template<typename _First, typename... _Rest>
class TypeListHandler {
private:
  _First                    _first;
  TypeListHandler<_Rest...> _rest;

public:
  // generate Visit* function from TypeNodes.inc
  #define TYPE(CLASS, BASE) \
      void Visit##CLASS(const clang::CLASS##Type *type) { \
        _first.Visit##CLASS(type); \
        _rest.Visit##CLASS(type); \
      }
  #define ABSTRACT_TYPE(CLASS, BASE)
  # include "clang/AST/TypeNodes.inc"
  #undef ABSTRACT_TYPE
  #undef TYPE

};  // TypeListHandler

// class TypeListHandler with single template parameters as handlers
template<typename _First>
class TypeListHandler<_First> {
private:
  _First _first;

public:
  // generate Visit* function from TypeNodes.inc
  #define TYPE(CLASS, BASE) \
      void Visit##CLASS(const clang::CLASS##Type *type) { \
        _first.Visit##CLASS(type); \
      }
  #define ABSTRACT_TYPE(CLASS, BASE)
  # include "clang/AST/TypeNodes.inc"
  #undef ABSTRACT_TYPE
  #undef TYPE

};  // TypeListHandler

}  // namespace xsca

#endif  // TYPE_LIST_HANDLER_INCLUDED
