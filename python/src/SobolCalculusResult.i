// SWIG file

%{
#include "otgui/SobolCalculusResult.hxx"
%}

%include SobolCalculusResult_doc.i

%include otgui/SobolCalculusResult.hxx
namespace OTGUI {

%extend SobolCalculusResult { SobolCalculusResult(const SobolCalculusResult & other) { return new OTGUI::SobolCalculusResult(other); } 

std::string __repr__() {
  return "SobolCalculusResult";
}

} }