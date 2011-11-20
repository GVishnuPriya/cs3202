SUBDIRS = test

bin_PROGRAMS = main

main_SOURCES = \
  simple/ast.cpp \
  simple/condition_set.cpp \
  simple/tuple.cpp \
  simple/query.cpp \
  simple/util/condition_utils.cpp \
  simple/util/ast_utils.cpp \
  simple/util/query_utils.cpp \
  impl/linker.cpp \
  impl/predicate.cpp \
  impl/processor.cpp \
  impl/solvers/follows.cpp \
  impl/solvers/ifollows.cpp \
  impl/solvers/parent.cpp \
  impl/solvers/iparent.cpp \
  impl/solvers/modifies.cpp \
  impl/solvers/next.cpp \
  impl/solvers/inext.cpp \
  impl/solvers/call.cpp \
  impl/solvers/icall.cpp \
  impl/solvers/uses.cpp \
  impl/solvers/same_name.cpp \
  impl/parser/token.cpp \
  impl/parser/parser.cpp \
  impl/parser/pql_parser.cpp \
  impl/command_line.cpp


LIBS = -pthread
AM_CPPFLAGS = -std=c++0x -Wall
