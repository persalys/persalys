// SWIG file PhysicalModelImplementation.i

%{
#include "persalys/PhysicalModelImplementation.hxx"
%}

%include PhysicalModelImplementation_doc.i

%ignore *::getHTMLDescription;
%ignore *::getCopulaCollection;
%ignore PERSALYS::PhysicalModelImplementation::canBeDetached;
%ignore PERSALYS::PhysicalModelImplementation::canBeInterrupted;
%ignore PERSALYS::PhysicalModelImplementation::interrupt;

%copyctor PERSALYS::PhysicalModelImplementation;

%include persalys/PhysicalModelImplementation.hxx
