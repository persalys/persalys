// SWIG file

%{
#include "otgui/PythonPhysicalModel.hxx"
%}



%include otgui/PythonPhysicalModel.hxx
namespace OTGUI {

%extend PythonPhysicalModel { PythonPhysicalModel(const PythonPhysicalModel & other) { return new OTGUI::PythonPhysicalModel(other); } 

std::string __repr__() {
  return "PythonPhysicalModel";
}

} }
