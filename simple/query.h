
#include "simple/condition.h"
#include "simple/qvar.h"
#include "simple/matcher.h"
#include <memory>
#include <string>
#include <list>

namespace simple {

class PqlQuery {
  public:
    virtual QueryMatcher* get_matcher() = 0;
    virtual accept_pql_query_visitor(PqlQueryVisitor *visitor) = 0;

    virtual ~PqlQuery() { }
};

class PqlSolveLeftQuery : public PqlQuery {
  public:
    virtual QueryVariable* get_left_query_variable() = 0;
    virtual SimpleCondition* get_right_criteria() = 0;

    virtual ~PqlSolverLeftQuery() { }
};

class PqlSolveRightQuery : public PqlQuery {
  public:
    virtual SimpleCondition* get_left_criteria() = 0;
    virtual QueryVariable* get_right_query_variable() = 0

    virtual ~PqlSolveRightQuery() { }
};

class PqlSolveBothQuery : public PqlQuery {
  public:
    virtual QueryVariable* get_left_query_variable() = 0;
    virtual QueryVariable* get_right_query_variable() = 0

    virtual ~PqlSolveBothQuery() { }
};

class PqlValidateQuery : public PqlQuery {
  public:
    virtual SimpleCondition* get_left_criteria() = 0;
    virtual SimpleCondition* get_right_criteria() = 0;

    virtual ~PqlValidateQuery() { }
};

struct PqlQuerySet {
  public:
    PqlQuerySet() : boolean_mode(false) { }

    std::list< std::unique_ptr<PqlQuery> >  
    queries;
    
    std::shared_ptr<QVarTable> 
    qvars;
    
    std::list< std::shared_ptr<QueryVariable> >
    selected_qvars;

    bool boolean_mode;
  private:
    PqlQuerySet(const PqlQuerySet& other); // non-copyable
};

}
