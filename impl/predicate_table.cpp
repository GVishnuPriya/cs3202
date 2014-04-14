
#include "impl/predicate_table.h"
#include "impl/predicate.h"

namespace simple {
namespace impl {

using namespace simple;

PredicateTable create_predicate_table(SimpleRoot ast) {
    PredicateTable pred_table;

    pred_table["wildcard"] = PredicatePtr(new SimpleWildCardPredicate(ast));
    pred_table["procedure"] = PredicatePtr(new SimpleProcPredicate(ast));
    pred_table["statement"] = PredicatePtr(new SimpleStatementPredicate(ast));
    pred_table["assign"] = PredicatePtr(new SimpleAssignmentPredicate(ast));
    pred_table["while"] = PredicatePtr(new SimpleWhilePredicate(ast));
    pred_table["if"] = PredicatePtr(new SimpleIfPredicate(ast));
    pred_table["call"] = PredicatePtr(new SimpleCallPredicate(ast));
    pred_table["var"] = PredicatePtr(new SimpleVariablePredicate(ast));
    pred_table["const"] = PredicatePtr(new SimpleConstantPredicate(ast));
    pred_table["operator"] = PredicatePtr(new SimpleOperatorPredicate(ast));
    pred_table["minus"] = PredicatePtr(new SimpleMinusPredicate(ast));
    pred_table["plus"] = PredicatePtr(new SimplePlusPredicate(ast));
    pred_table["times"] = PredicatePtr(new SimpleTimesPredicate(ast));

    return pred_table;
}

}
}