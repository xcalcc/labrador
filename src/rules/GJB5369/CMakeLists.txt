# remove comments below if there are real cpp files used
add_library (xscaGJB5369Rule STATIC
        GJB5369_decl_rule.cpp
        GJB5369_stmt_rule.cpp
        GJB5369_pp_rule.cpp
  )
#
target_link_libraries (xscaGJB5369Rule
  xscaRule
  )
