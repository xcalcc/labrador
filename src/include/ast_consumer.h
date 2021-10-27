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
// ast_consumer.h
// ====================================================================
//
// interface for clang ast consumer bridge
//

#ifndef AST_CONSUMER_INCLUDED
#define AST_CONSUMER_INCLUDED

#include "xsca_defs.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "xsca_checker_manager.h"
#include "clang/Frontend/CompilerInstance.h"

namespace xsca {

// class XcalAstConsumer
template<typename DeclVisitor>
class XcalAstConsumer : public clang::ASTConsumer {
private:
  DeclVisitor            &_decl_visitor;
  clang::CompilerInstance *_CI;

public:
  XcalAstConsumer(DeclVisitor &decl_handler,
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
    for (clang::DeclGroupRef::iterator it = D.begin(), end = D.end();
         it != end; ++it) {
      _decl_visitor.Visit(*it);
    }
    return true;
//    XcalCheckerManager::Finalize();
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
//    mgr->DumpAll();

    mgr->FinalizeScope(decl);
    // finalize decl visitor
    _decl_visitor.Finalize();
    // finalize XcalCheckerManager
    XcalCheckerManager::Finalize();
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
