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
  // finalize handler
  void Finalize() {
    _first.Finalize();
    _rest.Finalize();
  }

  // generate Visit* function from DeclNodes.inc
  #define DECL(DERIVED, BASE)                                 \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
        if (!_first.Disabled()) _first.Visit##DERIVED(decl);  \
        _rest.Visit##DERIVED(decl);                           \
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
  // finalize handler
  void Finalize() {
    _first.Finalize();
  }

  // generate Visit* function from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl) { \
        if (!_first.Disabled()) _first.Visit##DERIVED(decl);  \
      }
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL


};  // DeclListHandler

}  // namespace xsca

#endif  // DECL_LIST_HANDLER_INCLUDED
