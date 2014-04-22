/*
 * CS3201 Simple Static Analyzer
 * Copyright (C) 2011 Soares Chen Ruo Fei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "simple/util/query_utils.h"
#include "simple/util/condition_utils.h"

#include "impl/parser/parser.h"
#include "impl/parser/pql_parser.h"
#include "impl/parser/iterator_tokenizer.h"

#include "impl/linker.h"
#include "impl/selector.h"
#include "impl/processor.h"
#include "impl/solver_table.h"
#include "impl/predicate_table.h"

namespace simple {
namespace impl {

using namespace simple;
using namespace simple::util;
using namespace simple::parser;

class SimplePqlFrontEnd {
  public:
    template <typename Iterator>
    SimplePqlFrontEnd(Iterator begin, Iterator end)
    {
        parse_source(begin, end);
        
        _solver_table = create_solver_table(_ast);

        populate_predicates();
    }

    template <typename Iterator>
    std::vector<std::string> process_query(Iterator begin, Iterator end)
    {

        SimplePqlParser parser(std::shared_ptr<SimpleTokenizer>(
                new IteratorTokenizer<Iterator>(begin, end)),
                _ast, _line_table, _solver_table, _pred_table);

        PqlQuerySet query = parser.parse_query();
        query.predicates["*"] = _wildcard_pred;

        std::shared_ptr<QueryLinker> linker(
            new SimpleQueryLinker(query.predicates, _wildcard_pred->global_set()));

        QueryProcessor processor(linker, query.predicates, _wildcard_pred);

        for(ClauseSet::iterator it = query.clauses.begin();
            it != query.clauses.end(); ++it)
        {
            processor.solve_clause(it->get());
        }

        return format_result(&query, linker.get());
    }
  
  protected:
    template <typename SourceIterator>
    void parse_source(SourceIterator begin, SourceIterator end) 
    {
        SimpleParser parser(
            new IteratorTokenizer<SourceIterator>(begin, end));

        _ast = parser.parse_program();
        _line_table = parser.get_statement_line_table();
    }

    void populate_predicates() {
        _pred_table = create_predicate_table(_ast);
        _wildcard_pred = _pred_table["wildcard"];
    }

  private:
    SimpleRoot      _ast;
    SolverTable     _solver_table;
    PredicateTable  _pred_table;
    LineTable       _line_table;
    PredicatePtr    _wildcard_pred;
};

}
}
