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

add_subdirectory (GJB5369)
add_subdirectory (GJB8114)
add_subdirectory (MISRA)
add_subdirectory (SJT11682)
add_subdirectory (CERT)
add_subdirectory (AUTOSAR)
add_subdirectory (common)
add_subdirectory (example)

add_library (xscaRule STATIC
  rule_checker.cpp
  )

target_link_libraries (xscaRule
  xscaCore
  xscaGJB5369Rule
  xscaGJB8114Rule
  xscaMISRARule
  xscaCERTRule
  xscaAUTOSARRule
  )
