
#include "simple/util/term_utils.h"
#include "impl/query.h"

namespace simple {
namespace util {

using namespace simple;
using namespace impl;

class TermTypeVisitor : public PqlTermVisitor {
  public:
  	void visit_condition_term(PqlConditionTerm *term) {
  		result = ConditionTT;
  	}

  	void visit_variable_term(PqlVariableTerm *term) {
  		result = VariableTT;
  	}

  	void visit_wildcard_term(PqlWildcardTerm *term) {
  		result = WildcardTT;
  	}

  	PqlTermType result;
};

PqlTermType get_term_type(PqlTerm *term) {
	TermTypeVisitor visitor;

	term->accept_pql_term_visitor(&visitor);
	return visitor.result;
}

class CloneTermVisitor : public PqlTermVisitor {
  public:
    void visit_condition_term(PqlConditionTerm *term) {
      result = new SimplePqlConditionTerm(term->get_condition());
    }

    void visit_variable_term(PqlVariableTerm *term) {
      result = new SimplePqlVariableTerm(term->get_query_variable());
    }

    void visit_wildcard_term(PqlWildcardTerm *term) {
      result = new SimplePqlWildcardTerm();
    }

    PqlTerm *result;
};

PqlTerm* clone_term(PqlTerm *term) {
  CloneTermVisitor visitor;

  term->accept_pql_term_visitor(&visitor);
  return visitor.result;
}

}
}