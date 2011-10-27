bin_PROGRAMS = unit_tests

unit_tests_SOURCES = \
  test_ast.cpp \
  test_solver.cpp \
  test_modifies.cpp \
  condition_test.cpp \
  ../simple/condition.cpp \
  ../simple/condition_utils.cpp \
  ../simple/ast_utils.cpp \
  ../impl/solvers/modifies.cpp \
  ../impl/solvers/next.cpp \
  gtest/gtest-all.cc \
  test_main.cpp

AM_CPPFLAGS = -lpthread -std=c++0x
AM_LDFLAGS = -lpthread
