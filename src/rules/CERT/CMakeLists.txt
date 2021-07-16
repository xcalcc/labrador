# remove comments below if there are real cpp files used
add_library (xscaCERTRule STATIC
        cert_decl_rule.cpp
        cert_stmt_rule.cpp
        cert_pp_rule.cpp
        diagnostic.cpp
        )
#
target_link_libraries (xscaCERTRule
        xscaRule
        )
