// SWIG file

%{
#include "otgui/PythonFieldModel.hxx"
%}

%include PythonFieldModel_doc.i

%include otgui/PythonFieldModel.hxx
namespace OTGUI {

%extend PythonFieldModel { PythonFieldModel(const PythonFieldModel & other) { return new OTGUI::PythonFieldModel(other); } 

} }
