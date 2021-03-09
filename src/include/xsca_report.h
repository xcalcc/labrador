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

namespace xsca {

// class XcalReport
class XcalReport {
public:
  XcalReport() {}

  ~XcalReport() {}

private:

};  // XcalReport

#define REPORT(fmt, ...) printf(fmt, ##__VA_ARGS__);

}  // namespace xsca


#endif  // XSCA_REPORT_INCLUDED
