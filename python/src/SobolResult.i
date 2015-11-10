// SWIG file

%{
#include "otgui/SobolResult.hxx"
%}

%include SobolResult_doc.i

%include otgui/SobolResult.hxx
namespace OTGUI {

%extend SobolResult { SobolResult(const SobolResult & other) { return new OTGUI::SobolResult(other); } 

std::string __repr__() {
  return "SobolResult";
}

} }