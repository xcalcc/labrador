add_library (xscaMISRARule STATIC
  MISRA_pp_rule.cpp
  MISRA_decl_rule.cpp
  MISRA_stmt_rule.cpp
  MISRA_type_rule.cpp
  )

target_link_libraries (xscaMISRARule
  xscaRule)
