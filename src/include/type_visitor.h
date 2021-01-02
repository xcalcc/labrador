/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
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
