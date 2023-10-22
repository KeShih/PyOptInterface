#pragma once

#include <stdint.h>
#include <vector>
#include <optional>
#include "ankerl/unordered_dense.h"

using IndexT = uint32_t;
using CoeffT = double;

constexpr CoeffT COEFTHRESHOLD = 1e-12;

template <typename K, typename V>
using Hashmap = ankerl::unordered_dense::map<K, V>;

template <typename K>
using Hashset = ankerl::unordered_dense::set<K>;

template <typename V>
using Vector = std::vector<V>;

struct VariableIndex;
struct ScalarAffineFunction;
struct ScalarQuadraticFunction;
struct ExprBuilder;

enum class VariableDomain
{
	Continuous,
	Integer,
	Binary,
	SemiContinuous,
};

struct VariableIndex
{
	IndexT index;

	VariableIndex() = default;
	VariableIndex(IndexT v);
};

struct ScalarAffineFunction
{
	Vector<CoeffT> coefficients;
	Vector<IndexT> variables;
	std::optional<CoeffT> constant;

	ScalarAffineFunction() = default;
	ScalarAffineFunction(CoeffT v);
	ScalarAffineFunction(const VariableIndex &v);
	// v * c
	ScalarAffineFunction(const VariableIndex &v, CoeffT c);
	// v * c1 + c2
	ScalarAffineFunction(const VariableIndex &v, CoeffT c1, CoeffT c2);
	ScalarAffineFunction(const Vector<CoeffT> &, const Vector<IndexT> &);
	ScalarAffineFunction(const Vector<CoeffT> &, const Vector<IndexT> &,
	                     const std::optional<CoeffT> &);

	ScalarAffineFunction(const ExprBuilder &t);

	size_t size() const;
	void canonicalize(CoeffT threshold = COEFTHRESHOLD);
};

struct ScalarQuadraticFunction
{
	Vector<CoeffT> coefficients;
	Vector<IndexT> variable_1s;
	Vector<IndexT> variable_2s;
	std::optional<ScalarAffineFunction> affine_part;

	ScalarQuadraticFunction() = default;
	ScalarQuadraticFunction(const Vector<CoeffT> &, const Vector<IndexT> &, const Vector<IndexT> &);
	ScalarQuadraticFunction(const Vector<CoeffT> &, const Vector<IndexT> &, const Vector<IndexT> &,
	                        const std::optional<ScalarAffineFunction> &);
	ScalarQuadraticFunction(const ExprBuilder &t);

	size_t size() const;
	void canonicalize(CoeffT threshold = COEFTHRESHOLD);
};

struct VariablePair
{
	IndexT var_1;
	IndexT var_2;

	bool operator==(const VariablePair &x) const;
	bool operator<(const VariablePair &x) const;
};

template <>
struct ankerl::unordered_dense::hash<VariablePair>
{
	using is_avalanching = void;

	[[nodiscard]] auto operator()(VariablePair const &x) const noexcept -> uint64_t
	{
		static_assert(std::has_unique_object_representations_v<VariablePair>);
		return detail::wyhash::hash(&x, sizeof(x));
	}
};

struct ExprBuilder
{
	Hashmap<VariablePair, CoeffT> quadratic_terms;
	Hashmap<IndexT, CoeffT> affine_terms;
	std::optional<CoeffT> constant_term;

	ExprBuilder() = default;
	ExprBuilder(const VariableIndex &v);
	ExprBuilder(const ScalarAffineFunction &a);
	ExprBuilder(const ScalarQuadraticFunction &q);

	ExprBuilder &operator+=(CoeffT c);
	ExprBuilder &operator+=(const VariableIndex &v);
	ExprBuilder &operator+=(const ScalarAffineFunction &a);
	ExprBuilder &operator+=(const ScalarQuadraticFunction &q);
	ExprBuilder &operator+=(const ExprBuilder &t);

	ExprBuilder &operator-=(CoeffT c);
	ExprBuilder &operator-=(const VariableIndex &v);
	ExprBuilder &operator-=(const ScalarAffineFunction &a);
	ExprBuilder &operator-=(const ScalarQuadraticFunction &q);
	ExprBuilder &operator-=(const ExprBuilder &t);

	ExprBuilder &operator*=(CoeffT c);
	ExprBuilder &operator*=(const VariableIndex &v);
	ExprBuilder &operator*=(const ScalarAffineFunction &a);
	ExprBuilder &operator*=(const ScalarQuadraticFunction &q);
	ExprBuilder &operator*=(const ExprBuilder &t);

	bool empty() const;
	int degree() const;

	void clear();
	void clean_nearzero_terms(CoeffT threshold = COEFTHRESHOLD);
	void _add_quadratic_term(IndexT i, IndexT j, CoeffT coeff);
	void add_quadratic_term(const VariableIndex &i, const VariableIndex &j, CoeffT coeff);
	void _add_affine_term(IndexT i, CoeffT coeff);
	void add_affine_term(const VariableIndex &i, CoeffT coeff);
};

auto operator+(const VariableIndex &a, CoeffT b) -> ScalarAffineFunction;
auto operator+(CoeffT b, const VariableIndex &a) -> ScalarAffineFunction;
auto operator+(const VariableIndex &a, const VariableIndex &b) -> ScalarAffineFunction;
auto operator+(const ScalarAffineFunction &a, CoeffT b) -> ScalarAffineFunction;
auto operator+(CoeffT b, const ScalarAffineFunction &a) -> ScalarAffineFunction;
auto operator+(const ScalarAffineFunction &a, const VariableIndex &b) -> ScalarAffineFunction;
auto operator+(const VariableIndex &b, const ScalarAffineFunction &a) -> ScalarAffineFunction;
auto operator+(const ScalarAffineFunction &a, const ScalarAffineFunction &b)
    -> ScalarAffineFunction;
auto operator+(const ScalarQuadraticFunction &a, CoeffT b) -> ScalarQuadraticFunction;
auto operator+(CoeffT b, const ScalarQuadraticFunction &a) -> ScalarQuadraticFunction;
auto operator+(const ScalarQuadraticFunction &a, const VariableIndex &b) -> ScalarQuadraticFunction;
auto operator+(const VariableIndex &b, const ScalarQuadraticFunction &a) -> ScalarQuadraticFunction;
auto operator+(const ScalarQuadraticFunction &a, const ScalarAffineFunction &b)
    -> ScalarQuadraticFunction;
auto operator+(const ScalarAffineFunction &b, const ScalarQuadraticFunction &a)
    -> ScalarQuadraticFunction;
auto operator+(const ScalarQuadraticFunction &a, const ScalarQuadraticFunction &b)
    -> ScalarQuadraticFunction;

auto operator-(const VariableIndex &a, CoeffT b) -> ScalarAffineFunction;
auto operator-(CoeffT b, const VariableIndex &a) -> ScalarAffineFunction;
auto operator-(const VariableIndex &a, const VariableIndex &b) -> ScalarAffineFunction;
auto operator-(const ScalarAffineFunction &a, CoeffT b) -> ScalarAffineFunction;
auto operator-(CoeffT b, const ScalarAffineFunction &a) -> ScalarAffineFunction;
auto operator-(const ScalarAffineFunction &a, const VariableIndex &b) -> ScalarAffineFunction;
auto operator-(const VariableIndex &b, const ScalarAffineFunction &a) -> ScalarAffineFunction;
auto operator-(const ScalarAffineFunction &a, const ScalarAffineFunction &b)
    -> ScalarAffineFunction;
auto operator-(const ScalarQuadraticFunction &a, CoeffT b) -> ScalarQuadraticFunction;
auto operator-(CoeffT b, const ScalarQuadraticFunction &a) -> ScalarQuadraticFunction;
auto operator-(const ScalarQuadraticFunction &a, const VariableIndex &b) -> ScalarQuadraticFunction;
auto operator-(const VariableIndex &b, const ScalarQuadraticFunction &a) -> ScalarQuadraticFunction;
auto operator-(const ScalarQuadraticFunction &a, const ScalarAffineFunction &b)
    -> ScalarQuadraticFunction;
auto operator-(const ScalarAffineFunction &b, const ScalarQuadraticFunction &a)
    -> ScalarQuadraticFunction;
auto operator-(const ScalarQuadraticFunction &a, const ScalarQuadraticFunction &b)
    -> ScalarQuadraticFunction;

auto operator*(const VariableIndex &a, CoeffT b) -> ScalarAffineFunction;
auto operator*(CoeffT b, const VariableIndex &a) -> ScalarAffineFunction;
auto operator*(const VariableIndex &a, const VariableIndex &b) -> ScalarQuadraticFunction;
auto operator*(const ScalarAffineFunction &a, CoeffT b) -> ScalarAffineFunction;
auto operator*(CoeffT b, const ScalarAffineFunction &a) -> ScalarAffineFunction;
auto operator*(const ScalarAffineFunction &a, const VariableIndex &b) -> ScalarQuadraticFunction;
auto operator*(const VariableIndex &b, const ScalarAffineFunction &a) -> ScalarQuadraticFunction;
auto operator*(const ScalarAffineFunction &a, const ScalarAffineFunction &b)
    -> ScalarQuadraticFunction;
auto operator*(const ScalarQuadraticFunction &a, CoeffT b) -> ScalarQuadraticFunction;
auto operator*(CoeffT b, const ScalarQuadraticFunction &a) -> ScalarQuadraticFunction;

enum class ConstraintType
{
	Linear,
	Quadratic,
	SOS1,
	SOS2
};

enum class ConstraintSense
{
	LessEqual,
	GreaterEqual,
	Equal
};

struct ConstraintIndex
{
	ConstraintType type;
	IndexT index;
};

// struct LinearConstraint
//{
//	ScalarAffineFunction function;
//	ConstraintSense sense;
//	CoeffT rhs;
// };
//
// struct QuadraticConstraint
//{
//	ScalarQuadraticFunction function;
//	ConstraintSense sense;
//	CoeffT rhs;
// };
//
// struct SOSConstraint
//{
//	Vector<VariableIndex> variables;
//	Vector<CoeffT> weights;
// };

enum class ObjectiveSense
{
	Minimize,
	Maximize
};
