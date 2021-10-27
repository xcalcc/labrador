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
// type_visitor.h
// ====================================================================
//
// type visitor which visit clang Type and invoke type handler
//

#ifndef TYPE_VISITOR_INCLUDED
#define TYPE_VISITOR_INCLUDED

namespace xsca {

// class XcalTypeVisitor
template<typename _TypeHandler>
class XcalTypeVisitor {
private:
  _TypeHandler &_type_handler;

public:
  XcalTypeVisitor(_TypeHandler &type_handler) : _type_handler(type_handler) {}

  #define TYPE(CLASS, BASE) \
  void Visit##CLASS(const clang::CLASS##Type *type) { \
    _type_handler.Visit##CLASS(type); \
  }
  #define ABSTRACT_TYPE(CLASS, BASE)
  # include "clang/AST/TypeNodes.inc"
  #undef ABSTRACT_TYPE

  // general Visit method
  void Visit(const clang::Type *type) {
  switch (type->getTypeClass()) {
    #define TYPE(CLASS, BASE) \
    case clang::Type::CLASS: \
      Visit##CLASS(clang::cast<clang::CLASS##Type>(type)); \
      break;
    #define ABSTRACT_TYPE(CLASS, BASE)
    # include "clang/AST/TypeNodes.inc"
    #undef ABSTRACT_TYPE
    #undef TYPE
    default:
      TRACE("TODO: handle %s\n", type->getTypeClassName());
      break;
    }
  }
};  // XcalTypeVisitor

}  // namespace xsca

#endif  // TYPE_VISITOR_INCLUDED
