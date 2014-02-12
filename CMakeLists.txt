cmake_minimum_required (VERSION 2.6)

project (Simple3201 CXX)

set(CMAKE_BUILD_TYPE Debug)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x -pthread")
endif()

include_directories("${PROJECT_SOURCE_DIR}" , "${PROJECT_SOURCE_DIR}/test")

set(SOURCE
  simple/ast.cpp 
  simple/condition_set.cpp 
  simple/spa.cpp 
  simple/tuple.cpp 
  simple/query.cpp 
  simple/util/ast_utils.cpp 
  simple/util/condition_utils.cpp 
  simple/util/expr.cpp
  simple/util/query_utils.cpp 
  impl/linker.cpp 
  impl/predicate.cpp 
  impl/processor.cpp 
  impl/selector.cpp 
  impl/solver_table.cpp 
  impl/solvers/affects.cpp 
  impl/solvers/iaffects.cpp 
  impl/solvers/assign.cpp 
  impl/solvers/follows.cpp 
  impl/solvers/ifollows.cpp 
  impl/solvers/parent.cpp 
  impl/solvers/iparent.cpp 
  impl/solvers/modifies.cpp 
  impl/solvers/next.cpp 
  impl/solvers/inext.cpp 
  impl/solvers/call.cpp 
  impl/solvers/icall.cpp 
  impl/solvers/uses.cpp 
  impl/solvers/same_name.cpp 
  impl/parser/token.cpp 
  impl/parser/parser.cpp 
  impl/parser/pql_parser.cpp 
  spa/ast.cpp
  spa/cfg.cpp
  spa/follows.cpp
  spa/pkb.cpp
  spa/statement.cpp
  spa/uses.cpp
  spa/var_table.cpp
  )

add_library(spa ${SOURCE})

add_executable(main 
  impl/command_line.cpp 
  ${SOURCE}
  )

add_executable(unit_tests 
  test/test_main.cpp
  test/gtest/gtest-all.cpp
  ${SOURCE}
  test/test_ast.cpp 
  test/test_solver.cpp 
  test/test_call.cpp 
  test/test_icall.cpp 
  test/test_follows.cpp 
  test/test_ifollows.cpp 
  test/test_parent.cpp 
  test/test_iparent.cpp 
  test/test_modifies.cpp 
  test/test_uses.cpp 
  test/test_condition.cpp 
  test/test_next.cpp 
  test/test_inext.cpp 
  test/test_frontend.cpp 
  test/test_linker.cpp 
  test/test_parser.cpp 
  test/test_pql_parser.cpp 
  test/test_predicate.cpp 
  test/test_processor.cpp 
  test/test_query.cpp 
  test/test_tokenizer.cpp 
  )