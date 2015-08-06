// SWIG file

%{
#include "Variable.hxx"
%}

%include Variable_doc.i

%include Variable.hxx
namespace OTGUI {

%extend Variable { Variable(const Variable & other) { return new OTGUI::Variable(other); } 

std::string __repr__() {
  return "Variable";
}

} }
