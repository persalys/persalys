// SWIG file

%{
#include "otgui/SRCResult.hxx"
%}

%include SRCResult_doc.i

%include otgui/SRCResult.hxx
namespace OTGUI {

%extend SRCResult { SRCResult(const SRCResult & other) { return new OTGUI::SRCResult(other); } 

} }