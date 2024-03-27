#pragma once

#include <memory>

#include "solvers/gurobi/gurobi_c.h"

#include "pyoptinterface/core.hpp"
#include "pyoptinterface/container.hpp"
#include "pyoptinterface/solver_common.hpp"

// define Gurobi C APIs
namespace gurobi
{
#define B(f) extern decltype(&::f) f

B(GRBnewmodel);
B(GRBfreemodel);
B(GRBgetenv);
B(GRBaddvar);
B(GRBdelvars);
B(GRBaddconstr);
B(GRBaddqconstr);
B(GRBaddsos);
B(GRBdelconstrs);
B(GRBdelqconstrs);
B(GRBdelsos);
B(GRBdelq);
B(GRBsetdblattrarray);
B(GRBaddqpterms);
B(GRBoptimize);
B(GRBupdatemodel);
B(GRBgetparamtype);
B(GRBsetintparam);
B(GRBsetdblparam);
B(GRBsetstrparam);
B(GRBgetintparam);
B(GRBgetdblparam);
B(GRBgetstrparam);
B(GRBgetattrinfo);
B(GRBsetintattr);
B(GRBsetdblattr);
B(GRBsetstrattr);
B(GRBgetintattr);
B(GRBgetdblattr);
B(GRBgetstrattr);
B(GRBgetdblattrarray);
B(GRBgetdblattrlist);
B(GRBsetintattrelement);
B(GRBsetcharattrelement);
B(GRBsetdblattrelement);
B(GRBsetstrattrelement);
B(GRBgetintattrelement);
B(GRBgetcharattrelement);
B(GRBgetdblattrelement);
B(GRBgetstrattrelement);
B(GRBgetcoeff);
B(GRBchgcoeffs);
B(GRBgeterrormsg);
B(GRBversion);
B(GRBemptyenv);
B(GRBloadenv);
B(GRBfreeenv);
B(GRBstartenv);

#undef B

bool is_library_loaded();

bool load_library(const std::string &path);
} // namespace gurobi

class GurobiEnv
{
  public:
	GurobiEnv(bool empty = false);
	~GurobiEnv();

	// parameter
	int raw_parameter_type(const char *param_name);
	void set_raw_parameter_int(const char *param_name, int value);
	void set_raw_parameter_double(const char *param_name, double value);
	void set_raw_parameter_string(const char *param_name, const char *value);

	void start();

	void check_error(int error);

  private:
	GRBenv *m_env;

	friend class GurobiModel;
};

struct GRBfreemodelT
{
	void operator()(GRBmodel *model) const
	{
		gurobi::GRBfreemodel(model);
	};
};

class GurobiModel
{
  public:
	GurobiModel() = default;
	GurobiModel(const GurobiEnv &env);
	void init(const GurobiEnv &env);

	VariableIndex add_variable(VariableDomain domain = VariableDomain::Continuous,
	                           double lb = -GRB_INFINITY, double ub = GRB_INFINITY,
	                           const char *name = nullptr);
	void delete_variable(const VariableIndex &variable);
	void delete_variables(const Vector<VariableIndex> &variables);
	bool is_variable_active(const VariableIndex &variable);
	double get_variable_value(const VariableIndex &variable);
	std::string pprint_variable(const VariableIndex &variable);

	void set_variable_name(const VariableIndex &variable, const char *name);
	void set_constraint_name(const ConstraintIndex &constraint, const char *name);

	ConstraintIndex add_linear_constraint(const ScalarAffineFunction &function,
	                                      ConstraintSense sense, CoeffT rhs,
	                                      const char *name = nullptr);
	ConstraintIndex add_quadratic_constraint(const ScalarQuadraticFunction &function,
	                                         ConstraintSense sense, CoeffT rhs,
	                                         const char *name = nullptr);
	ConstraintIndex add_sos_constraint(const Vector<VariableIndex> &variables, SOSType sos_type);
	ConstraintIndex add_sos_constraint(const Vector<VariableIndex> &variables, SOSType sos_type,
	                                   const Vector<CoeffT> &weights);

	void delete_constraint(const ConstraintIndex &constraint);
	bool is_constraint_active(const ConstraintIndex &constraint);

	void _set_affine_objective(const ScalarAffineFunction &function, ObjectiveSense sense,
	                           bool clear_quadratic);
	void set_objective(const ScalarAffineFunction &function, ObjectiveSense sense);
	void set_objective(const ScalarQuadraticFunction &function, ObjectiveSense sense);
	void set_objective(const ExprBuilder &function, ObjectiveSense sense);

	void optimize();
	void update();
	void *get_raw_model();
	std::string version_string();

	// parameter
	int raw_parameter_type(const char *param_name);
	void set_raw_parameter_int(const char *param_name, int value);
	void set_raw_parameter_double(const char *param_name, double value);
	void set_raw_parameter_string(const char *param_name, const char *value);
	int get_raw_parameter_int(const char *param_name);
	double get_raw_parameter_double(const char *param_name);
	std::string get_raw_parameter_string(const char *param_name);

	// attribute
	int raw_attribute_type(const char *attr_name);

	// model attribute
	void set_model_raw_attribute_int(const char *attr_name, int value);
	void set_model_raw_attribute_double(const char *attr_name, double value);
	void set_model_raw_attribute_string(const char *attr_name, const char *value);
	int get_model_raw_attribute_int(const char *attr_name);
	double get_model_raw_attribute_double(const char *attr_name);
	std::string get_model_raw_attribute_string(const char *attr_name);

	std::vector<double> get_model_raw_attribute_vector_double(const char *attr_name, int start,
	                                                          int len);
	std::vector<double> get_model_raw_attribute_list_double(const char *attr_name,
	                                                        const std::vector<int> &ind);

	// variable attribute
	void set_variable_raw_attribute_int(const VariableIndex &variable, const char *attr_name,
	                                    int value);
	void set_variable_raw_attribute_char(const VariableIndex &variable, const char *attr_name,
	                                     char value);
	void set_variable_raw_attribute_double(const VariableIndex &variable, const char *attr_name,
	                                       double value);
	void set_variable_raw_attribute_string(const VariableIndex &variable, const char *attr_name,
	                                       const char *value);
	int get_variable_raw_attribute_int(const VariableIndex &variable, const char *attr_name);
	char get_variable_raw_attribute_char(const VariableIndex &variable, const char *attr_name);
	double get_variable_raw_attribute_double(const VariableIndex &variable, const char *attr_name);
	std::string get_variable_raw_attribute_string(const VariableIndex &variable,
	                                              const char *attr_name);

	int _variable_index(const VariableIndex &variable);
	int _checked_variable_index(const VariableIndex &variable);

	// constraint attribute
	void set_constraint_raw_attribute_int(const ConstraintIndex &constraint, const char *attr_name,
	                                      int value);
	void set_constraint_raw_attribute_char(const ConstraintIndex &constraint, const char *attr_name,
	                                       char value);
	void set_constraint_raw_attribute_double(const ConstraintIndex &constraint,
	                                         const char *attr_name, double value);
	void set_constraint_raw_attribute_string(const ConstraintIndex &constraint,
	                                         const char *attr_name, const char *value);
	int get_constraint_raw_attribute_int(const ConstraintIndex &constraint, const char *attr_name);
	char get_constraint_raw_attribute_char(const ConstraintIndex &constraint,
	                                       const char *attr_name);
	double get_constraint_raw_attribute_double(const ConstraintIndex &constraint,
	                                           const char *attr_name);
	std::string get_constraint_raw_attribute_string(const ConstraintIndex &constraint,
	                                                const char *attr_name);

	int _constraint_index(const ConstraintIndex &constraint);
	int _checked_constraint_index(const ConstraintIndex &constraint);

	// Modifications of model
	// 1. set/get RHS of a constraint
	double get_normalized_rhs(const ConstraintIndex &constraint);
	void set_normalized_rhs(const ConstraintIndex &constraint, double value);
	// 2. set/get coefficient of variable in constraint
	double get_normalized_coefficient(const ConstraintIndex &constraint,
	                                  const VariableIndex &variable);
	void set_normalized_coefficient(const ConstraintIndex &constraint,
	                                const VariableIndex &variable, double value);
	// 3. set/get linear coefficient of variable in objective
	double get_objective_coefficient(const VariableIndex &variable);
	void set_objective_coefficient(const VariableIndex &variable, double value);

	// Non-exported functions
	void check_error(int error);

  private:
	MonotoneIndexer<int> m_variable_index;

	MonotoneIndexer<int> m_linear_constraint_index;

	MonotoneIndexer<int> m_quadratic_constraint_index;

	MonotoneIndexer<int> m_sos_constraint_index;

	/* flag to indicate whether the model needs update */
	enum : std::uint64_t
	{
		m_variable_creation = 1,
		m_variable_deletion = 1 << 1,
		m_linear_constraint_creation = 1 << 2,
		m_linear_constraint_deletion = 1 << 3,
		m_quadratic_constraint_creation = 1 << 4,
		m_quadratic_constraint_deletion = 1 << 5,
		m_sos_constraint_creation = 1 << 6,
		m_sos_constraint_deletion = 1 << 7,
		m_general_constraint_creation = 1 << 8,
		m_general_constraint_deletion = 1 << 9,
		m_objective_update = 1 << 10,
		m_attribute_update = 1 << 11,
		m_constraint_coefficient_update = 1 << 12,
	};
	std::uint64_t m_update_flag = 0;
	void _update_for_information();
	void _update_for_variable_index();
	void _update_for_constraint_index(ConstraintType type);

	/* Gurobi part */
	GRBenv *m_env = nullptr;
	std::unique_ptr<GRBmodel, GRBfreemodelT> m_model;
};

using GurobiModelMixin = CommercialSolverMixin<GurobiModel>;