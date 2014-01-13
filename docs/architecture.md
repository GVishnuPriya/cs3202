
Architecture Overview
=====================

## Simple AST

The AST is consist of two sets of API. The first set of API has [read-only methods](../simple/ast.h), while the second set of API contain the [implementation details](../impl/ast.h) and allow mutation to the AST.

The implementation API is only needed for parser implementation. Implementor of query solver and all other parts only need to understand the read-only API.


## Solver

[Solver](../impl/solvers) is the core part of this program and all the query logic only need to implement as solvers. The solvers need to implement code logic to solve three kinds of queries: validate, solve right, and solve left.

### Validate

The solver's validate method is used to solve PQL clause with two fixed terms, such as `Modifies(s1, "a")`. The solver return a boolean to indicate whether the given condition is true in the program.

### Solve Right

The solver is given the left part of a PQL clause and need to solve the right part, such as `Modifies(s1, V)`. The solver would return the right result as a condition set.

### Solve Left

Similar to solve right, the solver is given the right part of a PQL clause, like `Modifies(S, "a")`, and need to return the left result.

### Solve Both

There is no need for the solver to implement solve both. The query engine would instead permutate through all possible AST combinations and pass it to the solver's validate method to filter out valid results.


## Condition

[Condition](../simple/condition.h) is used as a union type for all types of conditions in a query. There are currently 5 types of conditions:

  - Statement condition
  - Procedure condition
  - Variable condition
  - Constant condition
  - Pattern (expression) condition

What it really mean is that for example given a query clause `Modifies(s1, "a")`, it will generate two condition objects. The left side would be a statement condition and the right side would be a variable condition. If we make another query with different types like `Follows(s1, s2)`, then the first and second arguments would be both statement condition.

Note that clauses with PQL variables would only generate condition on the non-variable part. i.e. for a query clause `Modifies(s1, V)`, We'd only have one statement condition object at the left side, while the right side we would get a PQL variable term.


## PQL Term

Condition itself is actually a subset of the union type `PQL Term`. A [PQL term](../simple/query.h) represents one of the term inside a PQL clause. There are three types of PQL terms:

  - Condition term
  - Variable term
  - Wildcard term

As an example, a query clause `Modifies(s1, V)` would have two PQL terms. The first term is a condition term, which in turn is of type statement condition. The second term is a PQL variable term containing the PQL variable name. There is also a wildcard term to represent clauses like `Modifies(S, _)`, which basically mean get all statements that modifies _some_ variable.


## PQL Clause

PQL clause is the root class that hold the PQL terms together. Like we have seen earlier, a [PQL clause](../simple/query.h) contains two PQL terms which are positioned as left term and right term. A PQL query is consist of a list of PQL clauses and PQL predicates.

## Predicate

A [PQL predicate](../simple/predicate.h) is basically a one-term PQL clause. It is classified separately from a two-term PQL clause because the logic is different but also much simpler. The current only a few types of predicates with relatively simple logic:

  - Procedure predicate
  - Statement predicate
  - Assign predicate
  - Condition predicate
  - While predicate
  - Call predicate
  - Variable predicate
  - Constant predicate
  - Wildcard predicate

A predicate is much like a solver except that they operate only on single clause. Each predicate has to implement three solving methods: validate, filter, and global set.

### Validate

The predicate validates whether a _condition_ satisfies a predicate. For example a while predicate would only return true for a statement condition of the type while statement.

### Filter

The predicate filter accepts a list of conditions and return a filtered list of conditions that satisfies the predicate. For example a statement predicate that filters a condition set containing both statement and variable conditions will return a result condition set that only contain statement conditions.

### Global Set

Each predicate has a global set of conditions that satisfy a given predicate. For example a statement predicate would have all statement conditions stored in its global set.


## PQL Selector

The [PQL selector](../simple/query.h) represents the `Select` clause in a PQL query. There are three types of PQL selectors:

  - Boolean selector
  - Single var selector
  - Tuple selector

The query processor will return different results based on the selector type. For example the boolean selector simply return true if there exist at least one solution in a PQL query.

## PQL Query Set

The [PQL query set](../simple/query.h) is essentially the AST root for the PQL query language. It holds all the elements of a PQL query as follow:

  - One PQL selector
  - Table of PQL predicates
  - Set of PQL clauses

A PQL parser parses the query string and construct a PQL query set from it. The query set is then passed to the qery processor to solve the PQL query.


## Query Processor

A [query processor](../impl/) is initialized with a list of predicates. It has a solve clause method which solves one PQL clause and store the result to the query linker. After each PQL clauses is solved with the query processor, they can be retrieved from the query linker.


## Query Linker

The [query linker](../impl/linker.h) is used to keep track of all the states of the PQL query variables as the query processor solve new clauses. The main power of the query linker is that it has a link method to link together arbitrary pair of query variables with a set of conditions. The query linker would then eliminate the condition results in a query variable that do not satisfy the linkage.

The query linker is able to loop through indirect chains of query variable links to construct result tuple that do not violate the indirect linkage. For example given a PQL query `Select X, Z from Clause(X, Z) and Clause(X, Y) and Clause(Y, Z)`, the query linker needs to intelligently detect that `X` and `Z` has an indirect linkage via `Y`. Therefore the linker may not return just the raw result from `Clause(X, Z)` but also have to go through `Clause(X, Y)` and `Clause(Y, Z)`.


## Front End

The [front-end engine](../impl/front-end.h) can accept arbitrary byte stream from sources like file or socket and parse them into AST and PQL queries. It is the glue for all the static analyzer components together and make them work as a complete application.