// SWIG file

%{
#include "otgui/SobolAnalysis.hxx"
%}

%include SobolAnalysis_doc.i

%include otgui/SobolAnalysis.hxx
namespace OTGUI {

%extend SobolAnalysis { SobolAnalysis(const SobolAnalysis & other) { return new OTGUI::SobolAnalysis(other); } 

} }
