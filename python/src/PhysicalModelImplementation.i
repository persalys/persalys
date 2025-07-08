// SWIG file PhysicalModelImplementation.i

%{
#include "persalys/PhysicalModelImplementation.hxx"
%}
%ignore *::acceptLaunchParameters;

%include PhysicalModelImplementation_doc.i

%ignore *::getHtmlDescription;
%ignore *::getCopulaCollection;

%copyctor PERSALYS::PhysicalModelImplementation;

%include persalys/PhysicalModelImplementation.hxx
