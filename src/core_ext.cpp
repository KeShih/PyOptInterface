#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/bind_vector.h>
#include <nanobind/stl/bind_map.h>
#include <nanobind/stl/optional.h>

#include "pyoptinterface/core.hpp"

namespace nb = nanobind;

NB_MODULE(core_ext, m)
{
	nb::bind_vector<Vector<IndexT>>(m, "IndexVector");
	nb::bind_vector<Vector<CoeffT>>(m, "CoefVector");

	// VariableDomain
	nb::enum_<VariableDomain>(m, "VariableDomain")
	    .value("Continuous", VariableDomain::Continuous)
	    .value("Integer", VariableDomain::Integer)
	    .value("Binary", VariableDomain::Binary)
	    .value("SemiContinuous", VariableDomain::SemiContinuous);

	// ConstraintSense
	nb::enum_<ConstraintSense>(m, "ConstraintSense")
	    .value("LessEqual", ConstraintSense::LessEqual)
	    .value("Equal", ConstraintSense::Equal)
	    .value("GreaterEqual", ConstraintSense::GreaterEqual);

	// ConstraintType
	nb::enum_<ConstraintType>(m, "ConstraintType")
	    .value("Linear", ConstraintType::Linear)
	    .value("Quadratic", ConstraintType::Quadratic)
	    .value("SOS1", ConstraintType::SOS1)
	    .value("SOS2", ConstraintType::SOS2);

	// ObjectiveSense
	nb::enum_<ObjectiveSense>(m, "ObjectiveSense")
	    .value("Minimize", ObjectiveSense::Minimize)
	    .value("Maximize", ObjectiveSense::Maximize);

	nb::class_<VariableIndex>(m, "VariableIndex")
	    .def(nb::init<IndexT>())
	    .def_ro("index", &VariableIndex::index)
	    .def(nb::self + CoeffT())
	    .def(CoeffT() + nb::self)
	    .def(nb::self + VariableIndex())
	    .def(nb::self + ScalarAffineFunction())
	    .def(nb::self + ScalarQuadraticFunction())
	    .def(nb::self - CoeffT())
	    .def(nb::self - VariableIndex())
	    .def(nb::self - ScalarAffineFunction())
	    .def(nb::self - ScalarQuadraticFunction())
	    .def(nb::self * CoeffT())
	    .def(CoeffT() * nb::self)
	    .def(nb::self * VariableIndex())
	    .def(nb::self * ScalarAffineFunction());

	nb::class_<ConstraintIndex>(m, "ConstraintIndex")
	    .def_ro("type", &ConstraintIndex::type)
	    .def_ro("index", &ConstraintIndex::index);

	nb::class_<ScalarAffineFunction>(m, "ScalarAffineFunction")
	    .def(nb::init<CoeffT>())
	    .def(nb::init<const VariableIndex &>())
	    .def(nb::init<const VariableIndex &, CoeffT>())
	    .def(nb::init<const VariableIndex &, CoeffT, CoeffT>())
	    .def(nb::init<const Vector<CoeffT> &, const Vector<IndexT> &>(), nb::arg("coefficients"),
	         nb::arg("variables"))
	    .def(nb::init<const Vector<CoeffT> &, const Vector<IndexT> &, CoeffT>(),
	         nb::arg("coefficients"), nb::arg("variables"), nb::arg("constant"))
	    .def(nb::init<const TermsTable &>())
	    .def_ro("coefficients", &ScalarAffineFunction::coefficients)
	    .def_ro("variables", &ScalarAffineFunction::variables)
	    .def_ro("constant", &ScalarAffineFunction::constant)
	    .def("canonicalize", &ScalarAffineFunction::canonicalize,
	         nb::arg("threshold") = COEFTHRESHOLD)
	    .def(nb::self + CoeffT())
	    .def(CoeffT() + nb::self)
	    .def(nb::self + VariableIndex())
	    .def(nb::self + ScalarAffineFunction())
	    .def(nb::self + ScalarQuadraticFunction())
	    .def(nb::self - CoeffT())
	    .def(CoeffT() - nb::self)
	    .def(nb::self - VariableIndex())
	    .def(nb::self - ScalarAffineFunction())
	    .def(nb::self - ScalarQuadraticFunction())
	    .def(nb::self * CoeffT())
	    .def(CoeffT() * nb::self)
	    .def(nb::self * VariableIndex())
	    .def(nb::self * ScalarAffineFunction());

	nb::class_<ScalarQuadraticFunction>(m, "ScalarQuadraticFunction")
	    .def(nb::init<const Vector<CoeffT> &, const Vector<IndexT> &, const Vector<IndexT> &>(),
	         nb::arg("coefficients"), nb::arg("var1s"), nb::arg("var2s"))
	    .def(nb::init<const Vector<CoeffT> &, const Vector<IndexT> &, const Vector<IndexT> &,
	                  const ScalarAffineFunction &>(),
	         nb::arg("coefficients"), nb::arg("var1s"), nb::arg("var2s"), nb::arg("affine_part"))
	    .def(nb::init<const TermsTable &>())
	    .def_ro("coefficients", &ScalarQuadraticFunction::coefficients)
	    .def_ro("variable_1s", &ScalarQuadraticFunction::variable_1s)
	    .def_ro("variable_2s", &ScalarQuadraticFunction::variable_2s)
	    .def_ro("affine_part", &ScalarQuadraticFunction::affine_part)
	    .def("canonicalize", &ScalarQuadraticFunction::canonicalize,
	         nb::arg("threshold") = COEFTHRESHOLD)
	    .def(nb::self + CoeffT())
	    .def(CoeffT() + nb::self)
	    .def(nb::self + VariableIndex())
	    .def(nb::self + ScalarAffineFunction())
	    .def(nb::self + ScalarQuadraticFunction())
	    .def(nb::self - CoeffT())
	    .def(CoeffT() - nb::self)
	    .def(nb::self - VariableIndex())
	    .def(nb::self - ScalarAffineFunction())
	    .def(nb::self - ScalarQuadraticFunction())
	    .def(nb::self * CoeffT())
	    .def(CoeffT() * nb::self);

	nb::class_<VariablePair>(m, "VariablePair").def(nb::init<IndexT, IndexT>());

	nb::class_<TermsTable>(m, "TermsTable")
	    .def(nb::init<>())
	    .def(nb::init<const VariableIndex &>())
	    .def(nb::init<const ScalarAffineFunction &>())
	    .def(nb::init<const ScalarQuadraticFunction &>())
	    .def_ro("quadratic_terms", &TermsTable::quadratic_terms)
	    .def_ro("affine_terms", &TermsTable::affine_terms)
	    .def_ro("constant_term", &TermsTable::constant_term)
	    .def("empty", &TermsTable::empty)
	    .def("degree", &TermsTable::degree)
	    .def("clear", &TermsTable::clear)
	    .def("clean_nearzero_terms", &TermsTable::clean_nearzero_terms,
	         nb::arg("threshold") = COEFTHRESHOLD)
	    .def("add_quadratic_term", &TermsTable::add_quadratic_term)
	    .def("add_affine_term", &TermsTable::add_affine_term)
	    .def(nb::self += CoeffT())
	    .def(nb::self += VariableIndex())
	    .def(nb::self += ScalarAffineFunction())
	    .def(nb::self += ScalarQuadraticFunction())
	    .def(nb::self += TermsTable())
	    .def(nb::self -= CoeffT())
	    .def(nb::self -= VariableIndex())
	    .def(nb::self -= ScalarAffineFunction())
	    .def(nb::self -= ScalarQuadraticFunction())
	    .def(nb::self -= TermsTable())
	    .def(nb::self *= CoeffT())
	    .def(nb::self *= VariableIndex())
	    .def(nb::self *= ScalarAffineFunction())
	    .def(nb::self *= ScalarQuadraticFunction())
	    .def(nb::self *= TermsTable());
}
