import os, platform

if platform.system() == "Windows":
    copt_home = os.environ.get("COPT_HOME", None)
    if not copt_home:
        raise ValueError("COPT_HOME is not set in environment")
    if not os.path.exists(copt_home):
        raise ValueError(f"COPT_HOME does not exist: {copt_home}")
    os.add_dll_directory(os.path.join(copt_home, "bin"))

try:
    from .copt_model_ext import RawModel, Env, Constants
except Exception as e:
    raise ImportError(
        f"Failed to import copt_model_ext. Please check if COPT_HOME is set correctly. Error: {e}"
    )

from .attributes import (
    VariableAttribute,
    ConstraintAttribute,
    ModelAttribute,
    ResultStatusCode,
    TerminationStatusCode,
)
from .core_ext import VariableDomain, ConstraintType, VariableIndex
from .solver_common import (
    _get_model_attribute,
    _set_model_attribute,
    _get_entity_attribute,
    _set_entity_attribute,
)
from .ctypes_helper import pycapsule_to_cvoidp

DEFAULT_ENV = None


def init_default_env():
    global DEFAULT_ENV
    if DEFAULT_ENV is None:
        DEFAULT_ENV = Env()


variable_attribute_get_func_map = {
    VariableAttribute.Value: lambda model, v: model.get_variable_info(v, "Value"),
    VariableAttribute.LowerBound: lambda model, v: model.get_variable_info(v, "LB"),
    VariableAttribute.UpperBound: lambda model, v: model.get_variable_info(v, "UB"),
    VariableAttribute.PrimalStart: lambda model, v: model.mip_start_values.get(v, None),
    VariableAttribute.Domain: lambda model, v: model.get_variable_type(v),
    VariableAttribute.Name: lambda model, v: model.get_variable_name(v),
}

variable_attribute_set_func_map = {
    VariableAttribute.LowerBound: lambda model, v, val: model.set_variable_lower_bound(
        v, val
    ),
    VariableAttribute.UpperBound: lambda model, v, val: model.set_variable_upper_bound(
        v, val
    ),
    VariableAttribute.PrimalStart: lambda model, v, val: model.mip_start_values.__setitem__(
        v, val
    ),
    VariableAttribute.Domain: lambda model, v, val: model.set_variable_type(v, val),
    VariableAttribute.Name: lambda model, v, val: model.set_variable_name(v, val),
}

constraint_type_attribute_name_map = {
    ConstraintType.Linear: "Rows",
    ConstraintType.Quadratic: "QConstrs",
}


def get_objsense(model):
    raw_sense = model.get_raw_attribute_int("ObjSense")
    if raw_sense == Constants.COPT_MINIMIZE:
        return ObjectiveSense.Minimize
    elif raw_sense == Constants.COPT_MAXIMIZE:
        return ObjectiveSense.Maximize
    else:
        raise ValueError(f"Unknown objective sense: {raw_sense}")


def get_primalstatus(model):
    if model._is_mip():
        attr_name = "HasMipSol"
    else:
        attr_name = "HasLpSol"
    has_sol = model.get_raw_attribute_int(attr_name)
    if has_sol != 0:
        return ResultStatusCode.FEASIBLE_POINT
    else:
        return ResultStatusCode.NO_SOLUTION


def get_dualstatus(model):
    if not model._is_mip():
        has_sol = model.get_raw_attribute_int("HasLpSol")
        if has_sol != 0:
            return ResultStatusCode.FEASIBLE_POINT
    return ResultStatusCode.NO_SOLUTION


# LP status codes. Descriptions taken from COPT user guide.
# Code : (TerminationStatus, RawStatusString)
_RAW_LPSTATUS_STRINGS = {
    Constants.COPT_LPSTATUS_UNSTARTED: (
        TerminationStatusCode.OPTIMIZE_NOT_CALLED,
        "The LP optimization is not started yet.",
    ),
    Constants.COPT_LPSTATUS_OPTIMAL: (
        TerminationStatusCode.OPTIMAL,
        "The LP problem is solved to optimality.",
    ),
    Constants.COPT_LPSTATUS_INFEASIBLE: (
        TerminationStatusCode.INFEASIBLE,
        "The LP problem is infeasible.",
    ),
    Constants.COPT_LPSTATUS_UNBOUNDED: (
        TerminationStatusCode.DUAL_INFEASIBLE,
        "The LP problem is unbounded.",
    ),
    Constants.COPT_LPSTATUS_NUMERICAL: (
        TerminationStatusCode.NUMERICAL_ERROR,
        "Numerical trouble encountered.",
    ),
    Constants.COPT_LPSTATUS_IMPRECISE: (
        TerminationStatusCode.ALMOST_OPTIMAL,
        "The LP problem is solved to optimality with relaxed tolerances.",
    ),
    Constants.COPT_LPSTATUS_TIMEOUT: (
        TerminationStatusCode.TIME_LIMIT,
        "The LP optimization is stopped because of time limit.",
    ),
    Constants.COPT_LPSTATUS_UNFINISHED: (
        TerminationStatusCode.NUMERICAL_ERROR,
        "The LP optimization is stopped but the solver cannot provide a solution because of numerical difficulties.",
    ),
    Constants.COPT_LPSTATUS_INTERRUPTED: (
        TerminationStatusCode.INTERRUPTED,
        "The LP optimization is stopped by user interrupt.",
    ),
}

# MIP status codes. Descriptions taken from COPT user guide.
# Code : (TerminationStatus, RawStatusString)
_RAW_MIPSTATUS_STRINGS = {
    Constants.COPT_MIPSTATUS_UNSTARTED: (
        TerminationStatusCode.OPTIMIZE_NOT_CALLED,
        "The MIP optimization is not started yet.",
    ),
    Constants.COPT_MIPSTATUS_OPTIMAL: (
        TerminationStatusCode.OPTIMAL,
        "The MIP problem is solved to optimality.",
    ),
    Constants.COPT_MIPSTATUS_INFEASIBLE: (
        TerminationStatusCode.INFEASIBLE,
        "The MIP problem is infeasible.",
    ),
    Constants.COPT_MIPSTATUS_UNBOUNDED: (
        TerminationStatusCode.DUAL_INFEASIBLE,
        "The MIP problem is unbounded.",
    ),
    Constants.COPT_MIPSTATUS_INF_OR_UNB: (
        TerminationStatusCode.INFEASIBLE_OR_UNBOUNDED,
        "The MIP problem is infeasible or unbounded.",
    ),
    Constants.COPT_MIPSTATUS_NODELIMIT: (
        TerminationStatusCode.NODE_LIMIT,
        "The MIP optimization is stopped because of node limit.",
    ),
    Constants.COPT_MIPSTATUS_TIMEOUT: (
        TerminationStatusCode.TIME_LIMIT,
        "The MIP optimization is stopped because of time limit.",
    ),
    Constants.COPT_MIPSTATUS_UNFINISHED: (
        TerminationStatusCode.NUMERICAL_ERROR,
        "The MIP optimization is stopped but the solver cannot provide a solution because of numerical difficulties.",
    ),
    Constants.COPT_MIPSTATUS_INTERRUPTED: (
        TerminationStatusCode.INTERRUPTED,
        "The MIP optimization is stopped by user interrupt.",
    ),
}


def get_terminationstatus(model):
    if model._is_mip():
        raw_status = model.get_raw_attribute_int("MipStatus")
        status_string_pair = _RAW_MIPSTATUS_STRINGS.get(raw_status, None)
    else:
        raw_status = model.get_raw_attribute_int("LpStatus")
        status_string_pair = _RAW_LPSTATUS_STRINGS.get(raw_status, None)
    if not status_string_pair:
        raise ValueError(f"Unknown termination status: {raw_status}")
    return status_string_pair[0]


def get_rawstatusstring(model):
    if model._is_mip():
        raw_status = model.get_raw_attribute_int("MipStatus")
        status_string_pair = _RAW_MIPSTATUS_STRINGS.get(raw_status, None)
    else:
        raw_status = model.get_raw_attribute_int("LpStatus")
        status_string_pair = _RAW_LPSTATUS_STRINGS.get(raw_status, None)
    if not status_string_pair:
        raise ValueError(f"Unknown termination status: {raw_status}")
    return status_string_pair[1]


def get_silent(model):
    return model.get_raw_attribute_int("LogToConsole") == 0


model_attribute_get_func_map = {
    ModelAttribute.ObjectiveSense: get_objsense,
    ModelAttribute.DualStatus: get_dualstatus,
    ModelAttribute.PrimalStatus: get_primalstatus,
    ModelAttribute.RawStatusString: get_rawstatusstring,
    ModelAttribute.TerminationStatus: get_terminationstatus,
    ModelAttribute.Silent: get_silent,
    ModelAttribute.SolverName: lambda _: "COPT",
    ModelAttribute.SolverVersion: lambda model: model.version_string(),
}


def set_silent(model, value: bool):
    if value:
        model.set_raw_attribute_int("LogToConsole", 0)
    else:
        model.set_raw_attribute_int("LogToConsole", 1)


model_attribute_set_func_map = {
    ModelAttribute.Silent: set_silent,
}

constraint_attribute_get_func_map = {
    ConstraintAttribute.Name: lambda model, constraint: model.get_constraint_name(
        constraint
    ),
    ConstraintAttribute.Primal: lambda model, constraint: model.get_constraint_info(
        constraint, "Slack"
    ),
    ConstraintAttribute.Dual: lambda model, constraint: model.get_constraint_info(
        constraint, "Dual"
    ),
}

constraint_attribute_set_func_map = {
    ConstraintAttribute.Name: lambda model, constraint, value: model.set_constraint_name(
        constraint, value
    ),
}


class Model(RawModel):
    def __init__(self, env=None):
        if env is None:
            init_default_env()
            env = DEFAULT_ENV
        super().__init__(env)

        self.mip_start_values: dict[VariableIndex, float] = dict()

    @property
    def c_pointer(self):
        return pycapsule_to_cvoidp(self.get_raw_model())

    def supports_variable_attribute(self, attribute: VariableAttribute):
        return True

    def get_variable_attribute(self, variable, attribute: VariableAttribute):
        def e(attribute):
            raise ValueError(f"Unknown variable attribute to get: {attribute}")

        value = _get_entity_attribute(
            self,
            variable,
            attribute,
            variable_attribute_get_func_map,
            {},
            e,
        )
        return value

    def set_variable_attribute(self, variable, attribute: VariableAttribute, value):
        def e(attribute):
            raise ValueError(f"Unknown variable attribute to set: {attribute}")

        _set_entity_attribute(
            self,
            variable,
            attribute,
            value,
            variable_attribute_set_func_map,
            {},
            e,
        )

    def number_of_constraints(self, type: ConstraintType):
        attr_name = constraint_type_attribute_name_map.get(type, None)
        if not attr_name:
            raise ValueError(f"Unknown constraint type: {type}")
        return self.get_raw_attribute_int(attr_name)

    def number_of_variables(self):
        return self.get_raw_attribute_int("Cols")

    def _is_mip(self):
        ismip = self.get_raw_attribute_int("IsMIP")
        return ismip > 0

    def get_model_attribute(self, attribute: ModelAttribute):
        def e(attribute):
            raise ValueError(f"Unknown model attribute to get: {attribute}")

        value = _get_model_attribute(
            self,
            attribute,
            model_attribute_get_func_map,
            {},
            e,
        )
        return value

    def set_model_attribute(self, attribute: ModelAttribute, value):
        def e(attribute):
            raise ValueError(f"Unknown model attribute to set: {attribute}")

        _set_model_attribute(
            self,
            attribute,
            value,
            model_attribute_set_func_map,
            {},
            e,
        )

    def get_constraint_attribute(self, constraint, attribute: ConstraintAttribute):
        def e(attribute):
            raise ValueError(f"Unknown constraint attribute to get: {attribute}")

        value = _get_entity_attribute(
            self,
            constraint,
            attribute,
            constraint_attribute_get_func_map,
            {},
            e,
        )
        return value

    def set_constraint_attribute(
        self, constraint, attribute: ConstraintAttribute, value
    ):
        def e(attribute):
            raise ValueError(f"Unknown constraint attribute to set: {attribute}")

        _set_entity_attribute(
            self,
            constraint,
            attribute,
            value,
            constraint_attribute_set_func_map,
            {},
            e,
        )

    def optimize(self):
        if self._is_mip():
            mip_start = self.mip_start_values
            variables = list(mip_start.keys())
            values = list(mip_start.values())
            self.add_mip_start(variables, values)
            mip_start.clear()
        super().optimize()
