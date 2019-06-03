// SWIG file

%{
#include "persalys/PythonFieldModel.hxx"
%}

%include PythonFieldModel_doc.i

%include persalys/PythonFieldModel.hxx
namespace PERSALYS {

%extend PythonFieldModel { PythonFieldModel(const PythonFieldModel & other) { return new PERSALYS::PythonFieldModel(other); } 

} }
