/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// decl_list_handler.h
// ====================================================================
//
// decl list handler which call individual handlers on clang Decl
//

#ifndef DECL_LIST_HANDLER_INCLUDED
#define DECL_LIST_HANDLER_INCLUDED

#include "xsca_defs.h"

namespace xsca {

// class DeclListHandler with variadic template parameters as handlers
template<typename _First, typename... _Rest>
class DeclListHandler {
private:
  _First                    _first;
  DeclListHandler<_Rest...> _rest;

public:
  // generate Visit* function from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
        _first.Visit##DERIVED(decl); \
        _rest.Visit##DERIVED(decl); \
      }
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // DeclListHandler

// class DeclListHandler with single template parameters as handlers
template<typename _First>
class DeclListHandler<_First> {
private:
  _First _first;

public:
  // generate Visit* function from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
        _first.Visit##DERIVED(decl); \
      }
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // DeclListHandler

}  // namespace xsca

#endif  // DECL_LIST_HANDLER_INCLUDED
