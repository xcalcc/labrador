/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_report.h
// ====================================================================
//
// interface for XSCA report
//

#ifndef XSCA_REPORT_INCLUDED
#define XSCA_REPORT_INCLUDED

#include "xsca_defs.h"
#include "xsca_issue.h"
#include <vector>
#include <memory>

namespace xsca {

// class XcalReport
class XcalReport {
private:
  std::vector< std::unique_ptr<XcalIssue> > _issue_vec;
  const clang::SourceManager *_source_mgr;  // clang source manager
  FILE *_vtxt_file;                         // .vtxt file
  unsigned _line_table_offset;              // file id offset for files from line table
  bool  _vtxt_enabled;                      // write to vtxt file?
  bool  _stdout_enabled;                    // write to stdout?

protected:
  // get file id from line table
  unsigned GetFileIdFromLineTable(const char *fname);

  // print file list for vtxt
  void PrintVtxtFileList();

  // print issue into vtxt file
  void PrintVtxtIssue(const XcalIssue *issue);

  // print issue into stdout
  void PrintStdoutIssue(const XcalIssue *issue);


public:
  // cconstructor
  XcalReport() : _source_mgr(NULL), _vtxt_file(NULL), _line_table_offset(1),
                 _vtxt_enabled(true), _stdout_enabled(true) {
  }

  ~XcalReport() {
    if (_vtxt_file)
      fclose(_vtxt_file);
  }

  // initialize report with clang SourceManager and vtxt file name
  void Initialize(const clang::SourceManager *mgr, const char* vtxt) {
    _source_mgr = mgr;
    if (vtxt) {
      DBG_ASSERT(_vtxt_file == NULL, "vtxt file already initialized");
      _vtxt_file = fopen(vtxt, "w");
    }
    if (_vtxt_file == nullptr)
      _vtxt_enabled = false;
  }

  // finalize vtxt file
  void Finalize() {
    if (_vtxt_enabled) {
      PrintVtxtFileList();
    }
    for (auto &it : _issue_vec) {
      if (_vtxt_enabled) {
        PrintVtxtIssue(it.get());
      }
      if (_stdout_enabled) {
        PrintStdoutIssue(it.get());
      }
    }
  }

  // check if source is std library
  static bool IsStdLibrary(clang::SourceLocation location);

  XcalIssue *ReportIssue(const char *std, const char *rule, const clang::Decl *decl);

  XcalIssue *ReportIssue(const char *std, const char *rule, const clang::Stmt *stmt);

  XcalIssue *ReportIssue(const char *std, const char *rule, const clang::SourceLocation location) {
    auto issue = std::make_unique<XcalIssue>(std, rule, location);

    // ignore this issue if it is std source
    if (IsStdLibrary(location)) issue->SetIgnore(true);

    XcalIssue *issue_ptr = issue.get();
    _issue_vec.push_back(std::move(issue));
    return issue_ptr;
  }

};  // XcalReport

#define REPORT(fmt, ...) printf(fmt, ##__VA_ARGS__);

}  // namespace xsca


#endif  // XSCA_REPORT_INCLUDED
