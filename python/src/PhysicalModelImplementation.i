// SWIG file PhysicalModelImplementation.i

%{
#include "otgui/PhysicalModelImplementation.hxx"
%}
%ignore *::acceptLaunchParameters;

%include PhysicalModelImplementation_doc.i

%ignore OTGUI::PhysicalModelImplementation::getRestrictedFunction;

%include otgui/PhysicalModelImplementation.hxx
namespace OTGUI {

%extend PhysicalModelImplementation { PhysicalModelImplementation(const PhysicalModelImplementation & other) { return new OTGUI::PhysicalModelImplementation(other); } 

} }
