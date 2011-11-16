bin_PROGRAMS = unit_tests

unit_tests_SOURCES = \
  test_ast.cpp \
  test_solver.cpp \
  test_call.cpp \
  test_icall.cpp \
  test_follows.cpp \
  test_ifollows.cpp \
  test_parent.cpp \
  test_iparent.cpp \
  test_modifies.cpp \
  test_condition.cpp \
  test_next.cpp \
  test_inext.cpp \
  test_matcher.cpp \
  test_parser.cpp \
  test_predicate.cpp \
  test_tokenizer.cpp \
  ../simple/ast.cpp \
  ../simple/condition_set.cpp \
  ../simple/util/condition_utils.cpp \
  ../simple/util/ast_utils.cpp \
  ../impl/matcher.cpp \
  ../impl/predicate.cpp \
  ../impl/solvers/modifies.cpp \
  ../impl/solvers/next.cpp \
  ../impl/solvers/inext.cpp \
  ../impl/solvers/call.cpp \
  ../impl/solvers/icall.cpp \
  ../impl/parser/token.cpp \
  gtest/gtest-all.cc \
  test_main.cpp

LIBS = -pthread
AM_CPPFLAGS = -std=c++0x -Wall
