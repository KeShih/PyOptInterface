from pyoptinterface._src.core_ext import (
    VariableIndex,
    ConstraintIndex,
    ExprBuilder,
    VariableDomain,
    ConstraintSense,
    ConstraintType,
    SOSType,
    ObjectiveSense,
    ScalarAffineFunction,
    ScalarQuadraticFunction,
)

from pyoptinterface._src.attributes import (
    VariableAttribute,
    ModelAttribute,
    TerminationStatusCode,
    ResultStatusCode,
    ConstraintAttribute,
)

from pyoptinterface._src.tupledict import (
    tupledict,
    make_tupledict,
)

from pyoptinterface._src.aml import make_nd_variable, quicksum, quicksum_

# Alias of ConstraintSense
Eq = ConstraintSense.Equal
"""Alias of `ConstraintSense.Equal` for equality constraints.
"""
Leq = ConstraintSense.LessEqual
"""Alias of `ConstraintSense.LessEqual` for less-than-or-equal-to constraints.
"""
Geq = ConstraintSense.GreaterEqual
"""Alias of `ConstraintSense.GreaterEqual` for greater-than-or-equal-to constraints.
"""

__all__ = [
    "VariableIndex",
    "ConstraintIndex",
    "ExprBuilder",
    "VariableDomain",
    "ConstraintSense",
    "ConstraintType",
    "SOSType",
    "ObjectiveSense",
    "ScalarAffineFunction",
    "ScalarQuadraticFunction",
    "VariableAttribute",
    "ModelAttribute",
    "TerminationStatusCode",
    "ResultStatusCode",
    "ConstraintAttribute",
    "tupledict",
    "make_tupledict",
    "make_nd_variable",
    "quicksum",
    "Eq",
    "Leq",
    "Geq",
]
