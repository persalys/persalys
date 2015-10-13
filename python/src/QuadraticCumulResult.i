// SWIG file

%{
#include "QuadraticCumulResult.hxx"
%}

%include QuadraticCumulResult_doc.i

%include QuadraticCumulResult.hxx
namespace OTGUI {

%extend QuadraticCumulResult { QuadraticCumulResult(const QuadraticCumulResult & other) { return new OTGUI::QuadraticCumulResult(other); } 

std::string __repr__() {
  return "QuadraticCumulResult";
}

} }
