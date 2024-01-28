PyOptInterface
=======

## Overview

### What is PyOptInterface?

**PyOptInterface** is an open-source Python library to provide a unified API to construct and solve optimization models with various optimizers.

### Why another optimization modeling library after we have Pyomo/cvxpy/JuMP.jl?
It is designed as a very thin wrapper of native C API of optimizers and attempts to provide common abstractions of an algebraic modelling environment including model, variable, constraint and expression with the least overhead of performance.

The key features of PyOptInterface include:
- Very fast speed to construct optimization model (10-20x faster than Pyomo, comparable with JuMP.jl and some official Python bindings provided by vendor of optimizer)
- Low overhead to modify and re-solve the problem incrementally
- Unified API to cover common usages, write once and the code works for all optimizers
- You still have escape hatch to query or modify solver-specific parameter/attribute/information for different optimizers directly like the vendored Python binding of optimizer

### What kind of problems can PyOptInterface solve? 
It currently supports the following problem types:
- Linear Programming (LP)
- Mixed-Integer Linear Programming (MILP)
- Quadratic Programming (QP)
- Mixed-Integer Quadratic Programming (MIQP)
- Quadratically Constrained Quadratic Programming (QCQP)
- Mixed-Integer Quadratically Constrained Quadratic Programming (MIQCQP)

(supported-optimizers)=
### What optimizers does PyOptInterface support?
It currently supports the following optimizers:
- [Gurobi](https://www.gurobi.com/)
- [COPT](https://shanshu.ai/copt)
- [MOSEK](https://www.mosek.com/)

## Contents
```{toctree}
:maxdepth: 2
:titlesonly:
:caption: Usage Guide
:glob:

getting_started.md
model.md
variable.md
expression.md
constraint.md
```

```{toctree}
:maxdepth: 1
:titlesonly:
:caption: API docs

api/pyoptinterface.rst
```

## Indices and tables

- {ref}`genindex`
- {ref}`modindex`
- {ref}`search`