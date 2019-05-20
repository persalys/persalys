// SWIG file PhysicalModelImplementation.i

%{
#include "persalys/PhysicalModelImplementation.hxx"
%}
%ignore *::acceptLaunchParameters;

%include PhysicalModelImplementation_doc.i

%ignore *::getHtmlDescription;
%ignore *::getCopulaCollection;

%include persalys/PhysicalModelImplementation.hxx
namespace PERSALYS {

%extend PhysicalModelImplementation { PhysicalModelImplementation(const PhysicalModelImplementation & other) { return new PERSALYS::PhysicalModelImplementation(other); } 

} }
