// SWIG file

%{
#include "otgui/Variable.hxx"
%}

%include Variable_doc.i

%include otgui/Variable.hxx
namespace OTGUI {

%extend Variable { Variable(const Variable & other) { return new OTGUI::Variable(other); } 

std::string __repr__() {
  return "Variable";
}

} }
