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
// example_checker.cpp
// ====================================================================
//
// implementation for example checker
//

#include "xsca_checker_manager.h"
#include "example_decl_rule.h"
#include "example_stmt_rule.h"
#include "example_type_rule.h"
#include "example_pp_rule.h"
#include "xsca_checker_tmpl.h"

// used by xsca_link.cpp to make sure this library is linked correctly
int __ExampleRuleLinked__;

namespace xsca {
namespace example {

using ExampleChecker = xsca::XcalCheckerTmpl<ExampleDeclHandler, ExampleStmtHandler,
                           ExampleTypeHandler, ExamplePPHandler>;

// ExampleCheckerFactory
class ExampleCheckerFactory : public XcalCheckerFactory {
public:
  std::unique_ptr<XcalChecker>
  CreateChecker(XcalCheckerManager *mgr) {
    return std::make_unique<ExampleChecker>(mgr);
  }
};

static XcalCheckerFactoryRegister<ExampleCheckerFactory> registry;

}  // namespace example
}  // namespace xsca
