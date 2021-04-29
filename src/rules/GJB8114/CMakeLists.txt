add_library (xscaGJB8114Rule STATIC
  GJB8114_pp_rule.cpp
  GJB8114_decl_rule.cpp
  GJB8114_stmt_rule.cpp
  GJB8114_type_rule.cpp
  )

target_link_libraries (xscaGJB8114Rule
  xscaRule
  )
