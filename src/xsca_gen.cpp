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
// xsca_gen.cxx
// ====================================================================
//
// inject preprocess callback, ast match finder and ast consumer
//

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

// dummy implementation
#include "clang/CodeGen/BackendUtil.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"

#include "xsca_checker_manager.h"

// helper function to dump decl
extern "C" void dump_decl(void* decl) {
  ((clang::Decl*)decl)->dump();
}

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

