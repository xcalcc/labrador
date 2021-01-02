/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_gen.cxx
// ====================================================================
//
// inject preprocess callback, ast match finder and ast consumer
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

// dummy implementation
#include "clang/CodeGen/BackendUtil.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"

#include "xsca_checker_manager.h"

namespace clang {

CodeGenAction::CodeGenAction(unsigned _Act, llvm::LLVMContext *_VMContext)
  : Act(_Act),
    VMContext(_VMContext ? _VMContext : new llvm::LLVMContext),
    OwnsVMContext(!_VMContext) {}

CodeGenAction::~CodeGenAction() {
  TheModule.reset();
  if (OwnsVMContext)
    delete VMContext;
}

bool CodeGenAction::hasIRSupport() const {
  return true;
}

void
CodeGenAction::EndSourceFileAction() {
}

std::unique_ptr<llvm::Module>
CodeGenAction::takeModule() {
  return std::unique_ptr<llvm::Module>(nullptr);
}

llvm::LLVMContext *
CodeGenAction::takeLLVMContext() {
  OwnsVMContext = false;
  return VMContext;
}

std::unique_ptr<ASTConsumer>
CodeGenAction::CreateASTConsumer(CompilerInstance &CI,
                                 StringRef InFile) {

  return xsca::XcalCheckerManager::Initialize(CI, InFile);
}

void CodeGenAction::ExecuteAction() {
  this->ASTFrontendAction::ExecuteAction();
}

EmitAssemblyAction::EmitAssemblyAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitAssembly, _VMContext) {}

void EmitAssemblyAction::anchor() {}

EmitBCAction::EmitBCAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitBC, _VMContext) {}

void EmitBCAction::anchor() {}

EmitLLVMAction::EmitLLVMAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitLL, _VMContext) {}

void EmitLLVMAction::anchor() {}

EmitLLVMOnlyAction::EmitLLVMOnlyAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitNothing, _VMContext) {}

void EmitLLVMOnlyAction::anchor() {}

EmitCodeGenOnlyAction::EmitCodeGenOnlyAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitMCNull, _VMContext) {}

void EmitCodeGenOnlyAction::anchor() {}

EmitObjAction::EmitObjAction(llvm::LLVMContext *_VMContext)
  : CodeGenAction(Backend_EmitObj, _VMContext) {}

void EmitObjAction::anchor() {}

#if 0
// dummy implementation for Rewrite
bool FixItRecompile::BeginInvocation(CompilerInstance &CI) { return false; }

bool RewriteIncludesAction::BeginSourceFileAction(CompilerInstance &CI) { return false; }
void RewriteIncludesAction::ExecuteAction() { }

void RewriteMacrosAction::ExecuteAction() { }

void RewriteTestAction::ExecuteAction() { }
#endif
}

