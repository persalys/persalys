// SWIG file

%{
#include "otgui/SRCAnalysis.hxx"
%}

%include SRCAnalysis_doc.i

%include otgui/SRCAnalysis.hxx
namespace OTGUI {

%extend SRCAnalysis { SRCAnalysis(const SRCAnalysis & other) { return new OTGUI::SRCAnalysis(other); } 

} }
