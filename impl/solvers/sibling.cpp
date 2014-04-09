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

#include "impl/solvers/sibling.h"
#include "simple/util/statement_visitor_generator.h"

namespace simple {
namespace impl {

using namespace simple;

template <>
bool SiblingSolver::validate<StatementAst, StatementAst>(
        StatementAst *left, StatementAst *right)
{
    StatementVisitorGenerator<SiblingSolver, 
        PartialValidateVisitorTraits<SiblingSolver> > visitor(this, right);
    left->accept_statement_visitor(&visitor);
    return visitor.return_result();
  
  //Alternative: Soares please choose which one to take
  
}
  
  

}
}
