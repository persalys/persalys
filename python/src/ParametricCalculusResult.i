// SWIG file

%{
#include "otgui/ParametricCalculusResult.hxx"
%}

%include ParametricCalculusResult_doc.i

%include otgui/ParametricCalculusResult.hxx
namespace OTGUI {

%extend ParametricCalculusResult { ParametricCalculusResult(const ParametricCalculusResult & other) { return new OTGUI::ParametricCalculusResult(other); } 

std::string __repr__() {
  return "ParametricCalculusResult";
}

} }
