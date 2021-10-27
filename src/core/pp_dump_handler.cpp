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
// pp_dump_handler.cpp
// ====================================================================
//
// clang preprocessor dump handler which dumps preprocess callback
// parameters for reference
//

#include "pp_dump_handler.h"

namespace xsca {

void
PPDumpHandler::FileChanged(clang::SourceLocation Loc,
    clang::PPCallbacks::FileChangeReason Reason,
    clang::SrcMgr::CharacteristicKind FileType,
    clang::FileID PrevFID)
{
  TRACE0();
}


void
PPDumpHandler::InclusionDirective(clang::SourceLocation DirectiveLoc,
    const clang::Token &IncludeToken, llvm::StringRef IncludedFilename,
    bool IsAngled, clang::CharSourceRange FilenameRange,
    const clang::FileEntry *IncludedFile, llvm::StringRef SearchPath,
    llvm::StringRef RelativePath, const clang::Module *Imported,
    clang::SrcMgr::CharacteristicKind FileType)
{
  printf("Preprocess InclusionDirective:\n");
}


void
PPDumpHandler::EndOfMainFile()
{
  TRACE0();
}


void
PPDumpHandler::Ident(clang::SourceLocation Loc, llvm::StringRef Str)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDirective(clang::SourceLocation Loc,
    clang::PragmaIntroducerKind Introducer)
{
  TRACE0();
}

void
PPDumpHandler::PragmaComment(clang::SourceLocation Loc,
    const clang::IdentifierInfo *Kind, llvm::StringRef Str)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDetectMismatch(clang::SourceLocation Loc,
    llvm::StringRef Name, llvm::StringRef Value)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDebug(clang::SourceLocation Loc,
    llvm::StringRef DebugType)
{
  TRACE0();
}

void
PPDumpHandler::PragmaMessage(clang::SourceLocation Loc,
    llvm::StringRef Namespace, clang::PPCallbacks::PragmaMessageKind Kind,
    llvm::StringRef Str)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDiagnosticPush(clang::SourceLocation Loc,
    llvm::StringRef Namespace)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDiagnosticPop(clang::SourceLocation Loc,
    llvm::StringRef Namespace)
{
  TRACE0();
}

void
PPDumpHandler::PragmaDiagnostic(clang::SourceLocation Loc,
    llvm::StringRef, clang::diag::Severity Mapping, llvm::StringRef Str)
{
  TRACE0();
}

void
PPDumpHandler::HasInclude(clang::SourceLocation Loc,
    llvm::StringRef FileName, bool IsAngled,
    llvm::Optional<clang::FileEntryRef> File,
    clang::SrcMgr::CharacteristicKind FileType)
{
  TRACE0();
}

void
PPDumpHandler::PragmaOpenCLExtension(clang::SourceLocation NameLoc,
    const clang::IdentifierInfo *Name, clang::SourceLocation StateLoc,
    unsigned State)
{
  TRACE0();
}

void
PPDumpHandler::PragmaWarning(clang::SourceLocation Loc,
    llvm::StringRef WarningSpec, llvm::ArrayRef<int> Ids)
{
  TRACE0();
}

void
PPDumpHandler::PragmaWarningPush(clang::SourceLocation Loc, int Level)
{
  TRACE0();
}

void
PPDumpHandler::PragmaWarningPop(clang::SourceLocation Loc)
{
  TRACE0();
}

void
PPDumpHandler::PragmaAssumeNonNullBegin(clang::SourceLocation Loc)
{
  TRACE0();
}

void
PPDumpHandler::PragmaAssumeNonNullEnd(clang::SourceLocation Loc)
{
  TRACE0();
}

void
PPDumpHandler::MacroExpands(const clang::Token &MacroNameTok,
    const clang::MacroDefinition &MD, clang::SourceRange Range,
    const clang::MacroArgs *Args)
{
  TRACE0();
}

void
PPDumpHandler::MacroDefined(const clang::Token &MacroNameTok,
    const clang::MacroDirective *MD)
{
  TRACE0();
}

void
PPDumpHandler::MacroUndefined(const clang::Token &MacroNameTok,
    const clang::MacroDefinition &MD, const clang::MacroDirective *Undef)
{
  TRACE0();
}

void
PPDumpHandler::Defined(const clang::Token &MacroNameTok,
    const clang::MacroDefinition &MD, clang::SourceRange Range)
{
  TRACE0();
}

void
PPDumpHandler::SourceRangeSkipped(clang::SourceRange Range,
    clang::SourceLocation EndifLoc)
{
  TRACE0();
}

void
PPDumpHandler::If(clang::SourceLocation Loc,
    clang::SourceRange ConditionalRange,
    clang::PPCallbacks::ConditionValueKind ConditionalValue)
{
  TRACE0();
}

void
PPDumpHandler::Elif(clang::SourceLocation Loc,
    clang::SourceRange ConditionalRange,
    clang::PPCallbacks::ConditionValueKind ConditionalValue,
    clang::SourceLocation IfLoc)
{
  TRACE0();
}

void
PPDumpHandler::Ifdef(clang::SourceLocation Loc,
    const clang::Token &MacroNameTok, const clang::MacroDefinition &MD)
{
  TRACE0();
}

void
PPDumpHandler::Ifndef(clang::SourceLocation Loc,
    const clang::Token &MacroNameTok, const clang::MacroDefinition &MD)
{
  TRACE0();
}

void
PPDumpHandler::Else(clang::SourceLocation Loc, clang::SourceLocation IfLoc)
{
  TRACE0();
}

void
PPDumpHandler::Endif(clang::SourceLocation Loc,
    clang::SourceLocation IfLoc)
{
  TRACE0();
}

}  // namespace xsca
