/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// pp_null_handler.h
// ====================================================================
//
// preprocessor null handler which do nothing with preprocessor
//  callbacks
//

#ifndef PP_NULL_HANDLER_INCLUDED
#define PP_NULL_HANDLER_INCLUDED

#include "xsca_defs.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/FileManager.h"

namespace xsca {

// class PPNullHandler
class PPNullHandler {
protected:
  bool _disabled;

public:
  inline bool Disabled() const { return _disabled; }

  void FileChanged(clang::SourceLocation Loc,
           clang::PPCallbacks::FileChangeReason Reason,
           clang::SrcMgr::CharacteristicKind FileType,
           clang::FileID PrevFID) {}

  void InclusionDirective(clang::SourceLocation DirectiveLoc,
           const clang::Token &IncludeToken,
           llvm::StringRef IncludedFilename,
           bool IsAngled,
           clang::CharSourceRange FilenameRange,
           const clang::FileEntry *IncludedFile,
           llvm::StringRef SearchPath,
           llvm::StringRef RelativePath,
           const clang::Module *Imported,
           clang::SrcMgr::CharacteristicKind FileType) {}

  void EndOfMainFile()
           {}

  void Ident(clang::SourceLocation Loc,
           llvm::StringRef Str) {}

  void PragmaDirective(clang::SourceLocation Loc,
           clang::PragmaIntroducerKind Introducer) {}

  void PragmaComment(clang::SourceLocation Loc,
           const clang::IdentifierInfo *Kind,
           llvm::StringRef Str) {}

  void PragmaDetectMismatch(clang::SourceLocation Loc,
           llvm::StringRef Name,
           llvm::StringRef Value) {}

  void PragmaDebug(clang::SourceLocation Loc,
           llvm::StringRef DebugType) {}

  void PragmaMessage(clang::SourceLocation Loc,
           llvm::StringRef Namespace,
           clang::PPCallbacks::PragmaMessageKind Kind,
           llvm::StringRef Str) {}

  void PragmaDiagnosticPush(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {}

  void PragmaDiagnosticPop(clang::SourceLocation Loc,
           llvm::StringRef Namespace) {}

  void PragmaDiagnostic(clang::SourceLocation Loc,
           llvm::StringRef,
           clang::diag::Severity Mapping,
           llvm::StringRef Str) {}

  void HasInclude(clang::SourceLocation Loc,
           llvm::StringRef FileName,
           bool IsAngled,
           llvm::Optional<clang::FileEntryRef> File,
           clang::SrcMgr::CharacteristicKind FileType) {}

  void PragmaOpenCLExtension(clang::SourceLocation NameLoc,
           const clang::IdentifierInfo *Name,
           clang::SourceLocation StateLoc,
           unsigned State) {}

  void PragmaWarning(clang::SourceLocation Loc,
           llvm::StringRef WarningSpec,
           llvm::ArrayRef<int> Ids) {}

  void PragmaWarningPush(clang::SourceLocation Loc,
           int Level) {}

  void PragmaWarningPop(clang::SourceLocation Loc)
           {}

  void PragmaAssumeNonNullBegin(clang::SourceLocation Loc)
           {}

  void PragmaAssumeNonNullEnd(clang::SourceLocation Loc)
           {}

  void MacroExpands(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range,
           const clang::MacroArgs *Args) {}

  void MacroDefined(const clang::Token &MacroNameTok,
           const clang::MacroDirective *MD) {}

  void MacroUndefined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           const clang::MacroDirective *Undef) {}

  void Defined(const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD,
           clang::SourceRange Range) {}

  void SourceRangeSkipped(clang::SourceRange Range,
           clang::SourceLocation EndifLoc) {}

  void If(clang::SourceLocation Loc,
           clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue) {}

  void Elif(clang::SourceLocation Loc,
           clang::SourceRange ConditionalRange,
           clang::PPCallbacks::ConditionValueKind ConditionalValue,
           clang::SourceLocation IfLoc) {}

  void Ifdef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {}

  void Ifndef(clang::SourceLocation Loc,
           const clang::Token &MacroNameTok,
           const clang::MacroDefinition &MD) {}

  void Else(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {}

  void Endif(clang::SourceLocation Loc,
           clang::SourceLocation IfLoc) {}

};  // PPNullHandler

}  // namespace xsca

#endif  // PP_NULL_HANDLER_INCLUDED
