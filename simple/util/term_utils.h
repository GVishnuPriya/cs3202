
#pragma once

#include "simple/query.h"

namespace simple {
namespace util {

using namespace simple;

enum PqlTermType {
    ConditionTT,
    VariableTT,
    WildcardTT
};

PqlTermType get_term_type(PqlTerm *term);

PqlTerm* clone_term(PqlTerm *term);

}
}