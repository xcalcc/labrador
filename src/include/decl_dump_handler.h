/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// decl_dump_handler.h
// ====================================================================
//
// decl dump handler which dump clang Decl for reference
//

#ifndef DECL_DUMP_HANDLER_INCLUDED
#define DECL_DUMP_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclFriend.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclOpenMP.h"

namespace xsca {

// class XcalDeclDumpHandler
class XcalDeclDumpHandler {

public:
  // finalize handler
  void Finalize();

  // generate function prototype from DeclNodes.inc
  #define DECL(DERIVED, BASE) \
      void Visit##DERIVED(const clang::DERIVED##Decl *decl);
  #define ABSTRACT_DECL(DECL)
  # include "clang/AST/DeclNodes.inc"
  #undef ABSTRACT_DECL
  #undef DECL

};  // XcalDeclDumpHandler

}  // namespace xsca

#endif  // DECL_DUMP_HANDLER_INCLUDED
