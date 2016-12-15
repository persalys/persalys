// SWIG file

%{
#include "otgui/KrigingAnalysis.hxx"
%}

%include KrigingAnalysis_doc.i

%include otgui/KrigingAnalysis.hxx
namespace OTGUI {

%extend KrigingAnalysis { KrigingAnalysis(const KrigingAnalysis & other) { return new OTGUI::KrigingAnalysis(other); } 

} }
