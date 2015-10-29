// SWIG file

%{
#include "SobolCalculusResult.hxx"
%}

%include SobolCalculusResult_doc.i

%include SobolCalculusResult.hxx
namespace OTGUI {

%extend SobolCalculusResult { SobolCalculusResult(const SobolCalculusResult & other) { return new OTGUI::SobolCalculusResult(other); } 

std::string __repr__() {
  return "SobolCalculusResult";
}

} }