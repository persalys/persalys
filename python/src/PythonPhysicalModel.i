// SWIG file

%{
#include "persalys/PythonPhysicalModel.hxx"
%}

%include PythonPhysicalModel_doc.i

%ignore *::resetCallsNumber;

%include persalys/PythonPhysicalModel.hxx
namespace PERSALYS {

%extend PythonPhysicalModel { PythonPhysicalModel(const PythonPhysicalModel & other) { return new PERSALYS::PythonPhysicalModel(other); } 

} }
