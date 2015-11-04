// SWIG file

%{
#include "otgui/QuadraticCumulResult.hxx"
%}

%include QuadraticCumulResult_doc.i

%include otgui/QuadraticCumulResult.hxx
namespace OTGUI {

%extend QuadraticCumulResult { QuadraticCumulResult(const QuadraticCumulResult & other) { return new OTGUI::QuadraticCumulResult(other); } 

std::string __repr__() {
  return "QuadraticCumulResult";
}

} }
