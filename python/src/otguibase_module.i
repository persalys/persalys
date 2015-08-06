// SWIG file

%module(docstring="otguibase module") otguibase
%feature("autodoc","1");

%{
#include "OT.hxx"
#include "PythonWrappingFunctions.hxx"
%}

// Prerequisites needed
%include typemaps.i
%include exception.i
%ignore *::load(OT::Advocate & adv);
%ignore *::save(OT::Advocate & adv) const;

%import base_module.i
%import uncertainty_module.i

// The new classes
%include OTGuiprivate.hxx
%include Observer.i
%include Variable.i
%include Input.i
%include Output.i
%include PhysicalModelImplementation.i
%include PhysicalModel.i
%include Calculus.i
%include ParametricCalculus.i
%include OTStudy.i


