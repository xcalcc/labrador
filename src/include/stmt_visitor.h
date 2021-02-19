/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// stmt_visitor.h
// ====================================================================
//
// stmt visitor which visit clang Stmt and invoke stmt handler
//

#ifndef STMT_VISITOR_INCLUDED
#define STMT_VISITOR_INCLUDED

namespace xsca {

// class XcalStmtVisitor
template<typename _StmtHandler, typename _DeclVisitor>
class XcalStmtVisitor {
private:
  _StmtHandler &_stmt_handler;
  _DeclVisitor &_decl_visitor;  // For Stmt::DeclStmtClass

public:
  XcalStmtVisitor(_StmtHandler &stmt_handler, _DeclVisitor &decl_visitor)
      : _stmt_handler(stmt_handler),
        _decl_visitor(decl_visitor) {}

  // special handling of DeclStmt
  void VisitDeclStmt(const clang::DeclStmt *stmt) {
    _stmt_handler.VisitDeclStmt(stmt);
    for (const auto *decl : stmt->decls()) {
      _decl_visitor.Visit(decl);
    }
  }

  // special handling of LabelStmt
  void VisitLabelStmt(const clang::LabelStmt *stmt) {
    _stmt_handler.VisitLabelStmt(stmt);
    _decl_visitor.VisitLabel(stmt->getDecl());
    for (auto child : stmt->clang::Stmt::children()) {
      Visit(child);
    }
  }

  void VisitCompoundStmt(const clang::CompoundStmt *stmt) {
    auto scope_mgr = XcalCheckerManager::GetScopeManager();
    ScopeHelper<clang::CompoundStmt> scope(scope_mgr, stmt);

    _stmt_handler.VisitCompoundStmt(stmt);
    for (auto child : stmt->clang::Stmt::children()) {
      Visit(child);
    }
  }

  // generate individual Visit##CLASS method
  #define STMT(CLASS, BASE) \
  void Visit##CLASS(const clang::CLASS *stmt) { \
    _stmt_handler.Visit##CLASS(stmt); \
    for (auto child : stmt->clang::Stmt::children()) { \
      Visit(child); \
    } \
  }
  #define ABSTRACT_STMT(CLASS)
  #define DECLSTMT(CLASS, BASE)   // already handled above
  #define LABELSTMT(CLASS, BASE)
  #define COMPOUNDSTMT(CLASS, BASE)
  # include "clang/AST/StmtNodes.inc"

  // general Visit method
  void Visit(const clang::Stmt *stmt) {
    switch (stmt->getStmtClass()) {
    #define STMT(CLASS, BASE) \
    case clang::Stmt::CLASS##Class: \
      this->Visit##CLASS(clang::cast<clang::CLASS>(stmt)); \
      break;
    #define ABSTRACT_STMT(CLASS)
    # include "clang/AST/StmtNodes.inc"

    default:
      TRACE("TODO: handle %s\n", stmt->getStmtClassName());
      break;
    }
  }
};  // XcalStmtVisitor

}  // namespace xsca

#endif  // STMT_VISITOR_INCLUDED
