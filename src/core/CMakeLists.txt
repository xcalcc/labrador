#  Copyright (C) 2021 Xcalibyte (Shenzhen) Limited.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

add_library (xscaCore STATIC
  decl_dump_handler.cpp
  pp_dump_handler.cpp
  stmt_dump_handler.cpp
  type_dump_handler.cpp
  xsca_checker_manager.cpp
  xsca_opts.cpp
  xsca_issue.cpp
  xsca_report.cpp
  scope_manager.cpp
)
