// SWIG file

%{
#include "persalys/PythonPhysicalModel.hxx"
%}

%include PythonPhysicalModel_doc.i

%ignore *::resetCallsNumber;

%copyctor PERSALYS::PythonPhysicalModel;

%include persalys/PythonPhysicalModel.hxx
