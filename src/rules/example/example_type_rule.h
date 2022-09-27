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
// example_type_rule.h
// ====================================================================
//
// this is an example to describe how to write a type match based
// checker for type related rules.
//

#include "xsca_defs.h"
#include "type_null_handler.h"
#include "type_list_handler.h"

// CHANGE HERE: add all type rules into macro below
#define ALL_TYPE_RULES(R) \
  R(CheckTypeExample, "CheckTypeExample")

namespace xsca {
namespace example {

// CHANGE HERE: include type rules file below
#include "check_type_example.inc"

#define GetClass(CLASS, ...) CLASS
using ExampleTypeHandler = TypeListHandler<ALL_TYPE_RULES(GetClass)>;

}  // namespace example
}  // namespace xsca
