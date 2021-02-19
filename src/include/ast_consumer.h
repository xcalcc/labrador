/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// ast_consumer.h
// ====================================================================
//
// interface for clang ast consumer bridge
//

#ifndef AST_CONSUMER_INCLUDED
#define AST_CONSUMER_INCLUDED

#include "clang/AST/ASTContext.h"
#include "xsca_defs.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTConsumer.h"

namespace xsca {

// class XcalAstConsumer
template<typename _DeclVisitor>
class XcalAstConsumer : public clang::ASTConsumer {
private:
  _DeclVisitor            &_decl_visitor;
  clang::CompilerInstance *_CI;

public:
  XcalAstConsumer(_DeclVisitor &decl_handler,
                  clang::CompilerInstance *CI)
      : _decl_visitor(decl_handler), _CI(CI) {}

  ~XcalAstConsumer() {}

private:
  // Handle ASTConsumer interfaces
  void Initialize(clang::ASTContext &Context) override {
    // TODO: initialize handlers

    // initialize global scope
    ScopeManager *mgr = XcalCheckerManager::GetScopeManager();
    mgr->InitializeScope(_CI->getASTContext().getTranslationUnitDecl());
  }

  bool HandleTopLevelDecl(clang::DeclGroupRef D) override {
    printf("Top Level\n");
    for (clang::DeclGroupRef::iterator it = D.begin(), end = D.end();
         it != end; ++it) {
      _decl_visitor.Visit(*it);
    }
  }

  void HandleInlineFunctionDefinition(clang::FunctionDecl *D) override {
    //_decl_visitor.VisitFunction(D);
  }

  void HandleInterestingDecl(clang::DeclGroupRef D) override {
    //for (clang::DeclGroupRef::iterator it = D.begin(), end = D.end();
    //     it != end; ++it) {
    //  _decl_visitor.Visit(*it);
    //}
  }

  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    // TODO: finalize handlers
    clang::TranslationUnitDecl* decl = Ctx.getTranslationUnitDecl();
    ScopeManager *mgr = XcalCheckerManager::GetScopeManager();
    mgr->DumpAll();

    mgr->FinalizeScope(decl);
  }

  void HandleTagDeclDefinition(clang::TagDecl *D) override {
    //_decl_visitor.Visit(D);
  }

  void HandleTagDeclRequiredDefinition(const clang::TagDecl *D) override {
    //_decl_visitor.Visit(D);
  }

  void HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl *D) override {
    //_decl_visitor.VisitFunction(D);
  }

  void HandleTopLevelDeclInObjCContainer(clang::DeclGroupRef D) override {
    //for (clang::DeclGroupRef::iterator it = D.begin(), end = D.end();
    //     it != end; ++it) {
    //  _decl_visitor.Visit(*it);
    //}
  }

  void HandleImplicitImportDecl(clang::ImportDecl *D) override {
    //_decl_visitor.VisitImport(D);
  }

  void CompleteTentativeDefinition(clang::VarDecl *D) override {
    //_decl_visitor.VisitVar(D);
  }

  void CompleteExternalDeclaration(clang::VarDecl *D) override {
    //_decl_visitor.VisitVar(D);
  }

  void AssignInheritanceModel(clang::CXXRecordDecl *RD) override {
    //_decl_visitor.VisitCXXRecord(RD);
  }

  void HandleCXXStaticMemberVarInstantiation(clang::VarDecl *D) override {
    _decl_visitor.VisitVar(D);
  }

  void HandleVTable(clang::CXXRecordDecl *RD) override {
    //_decl_visitor.VisitCXXRecord(RD);
  }
};  // XcalAstConsumer

}  // name xsca


#endif  // AST_CONSUMER_INCLUDED
