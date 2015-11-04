// SWIG file

%{
#include "otgui/SRCCalculusResult.hxx"
%}

%include SRCCalculusResult_doc.i

%include otgui/SRCCalculusResult.hxx
namespace OTGUI {

%extend SRCCalculusResult { SRCCalculusResult(const SRCCalculusResult & other) { return new OTGUI::SRCCalculusResult(other); } 

std::string __repr__() {
  return "SRCCalculusResult";
}

} }