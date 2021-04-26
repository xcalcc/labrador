add_subdirectory (GJB5369)
add_subdirectory (GJB8114)
add_subdirectory (MISRA)
add_subdirectory (SJT11682)
add_subdirectory (common)
add_subdirectory (example)

add_library (xscaRule STATIC
  rule_checker.cpp
  )

target_link_libraries (xscaRule
  xscaCore
  xscaGJB5369Rule
  xscaMISRARule
  )
