/*****************************************************************************
 *   Copyright (C) 2004-2009 The PaGMO development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://apps.sourceforge.net/mediawiki/pagmo                             *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Developers  *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Credits     *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 3 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/make_function.hpp>
#include <boost/python/module.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/pure_virtual.hpp>
#include <string>

#include "../../src/problem/base.h"
#include "../../src/problem/paraboloid.h"
#include "../../src/problem/python.h"
#include "../../src/types.h"
#include "../exceptions.h"
#include "../utils.h"

using namespace boost::python;
using namespace pagmo;

template <class Problem>
static inline class_<Problem,bases<problem::base> > problem_wrapper(const char *name, const char *descr)
{
	class_<Problem,bases<problem::base> > retval(name,descr,init<const Problem &>());
	retval.def("__copy__", &Problem::clone);
	return retval;
}

struct python_problem: problem::python, wrapper<problem::python>
{
	python_problem(int n, int ni = 0, int nf = 1, int nc = 0, int nic = 0):
		problem::python(n,ni,nf,nc,nic),wrapper<problem::python>() {}
	python_problem(const double &lb, const double &ub, int n, int ni = 0, int nf = 1, int nc = 0, int nic = 0):
		problem::python(lb,ub,n,ni,nf,nc,nic),wrapper<problem::python>() {}
	python_problem(const decision_vector &lb, const decision_vector &ub, int ni = 0, int nf = 1, int nc = 0, int nic = 0):
		problem::python(lb,ub,ni,nf,nc,nic),wrapper<problem::python>() {}
	problem::base_ptr clone() const
	{
		return this->get_override("__copy__")();
	}
	fitness_vector py_objfun(const decision_vector &x) const
	{
		return this->get_override("py_objfun")(x);
	}
	std::string human_readable_extra() const
	{
		if (override f = this->get_override("py_repr_extra")) {
			return f();
		}
		return problem::python::human_readable_extra();
	}
	std::string default_human_readable_extra() const
	{
		return this->problem::python::human_readable_extra();
	}
};

BOOST_PYTHON_MODULE(_problem) {
	// Translate exceptions for this module.
	translate_exceptions();

	// Expose base problem class, including the non-virtual methods.
	typedef void (problem::base::*bounds_setter)(const decision_vector &);
	typedef void (problem::base::*bounds_setter_value)(const double &, const double &);
	typedef void (problem::base::*bounds_setter_vectors)(const decision_vector &, const decision_vector &);
	typedef constraint_vector (problem::base::*return_constraints)(const decision_vector &) const;
	typedef fitness_vector (problem::base::*return_fitness)(const decision_vector &) const;
	class_<problem::base,boost::noncopyable>("__base", no_init)
		.def("__repr__", &problem::base::human_readable)
		// Dimensions.
		.add_property("dimension", &problem::base::get_dimension, "Global dimension.")
		.add_property("i_dimension", &problem::base::get_i_dimension, "Integer dimension.")
		.add_property("c_dimension", &problem::base::get_c_dimension, "Global constraints dimension.")
		.add_property("ic_dimension", &problem::base::get_ic_dimension, "Inequality constraints dimension.")
		// Bounds.
		.add_property("lb",make_function(&problem::base::get_lb,return_value_policy<copy_const_reference>()), bounds_setter(&problem::base::set_lb), "Lower bounds.")
		.add_property("ub",make_function(&problem::base::get_ub,return_value_policy<copy_const_reference>()), bounds_setter(&problem::base::set_ub), "Upper bounds.")
		.def("set_bounds",bounds_setter_value(&problem::base::set_bounds),"Set all bounds to the input values.")
		.def("set_bounds",bounds_setter_vectors(&problem::base::set_bounds),"Set bounds to the input vectors.")
		.add_property("diameter",&problem::base::get_diameter, "Problem's diameter.")
		// Useful operators.
		.def(self == self)
		.def(self != self)
		.def("is_compatible",&problem::base::is_compatible,"Check compatibility with other problem.")
		// Comparisons.
		.def("compare_x",&problem::base::compare_x,"Compare decision vectors.")
		.def("verify_x",&problem::base::verify_x,"Check if decision vector is compatible with problem.")
		.def("compare_fc",&problem::base::compare_fc,"Simultaneous fitness-constraint comparison.")
		// Constraints.
		.def("compare_constraints",&problem::base::compare_constraints,"Compare constraint vectors.")
		.def("compute_constraints",return_constraints(&problem::base::compute_constraints),"Compute and return constraint vector.")
		.def("feasibility_x",&problem::base::feasibility_x,"Determine feasibility of decision vector.")
		.def("feasibility_c",&problem::base::feasibility_c,"Determine feasibility of constraint vector.")
		// Fitness.
		.def("objfun",return_fitness(&problem::base::objfun),"Compute and return fitness vector.")
		.def("compare_fitness",&problem::base::compare_fitness,"Compare fitness vectors.");

	// Special handling for the Python problem.
	class_<python_problem,boost::noncopyable,bases<problem::base> >("base","Base Python problem.",init<int,optional<int,int,int,int> >())
		.def(init<const double &, const double &, int, optional<int,int,int,int> >())
		.def(init<const decision_vector &, const decision_vector &, optional<int,int,int,int> >())
		.def(init<const python_problem &>())
		.def("__copy__",pure_virtual(&problem::python::clone))
		.def("py_objfun",pure_virtual(&problem::python::py_objfun))
		.def("py_repr_extra",&problem::python::human_readable_extra,&python_problem::default_human_readable_extra);

	// Paraboloid problem.
	problem_wrapper<problem::paraboloid>("paraboloid","Multi-dimensional paraboloid miminisation.")
		.def(init<>())
		.def(init<const decision_vector &, const decision_vector &>());
}
