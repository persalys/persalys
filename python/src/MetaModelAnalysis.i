// SWIG file

%{
#include "otgui/MetaModelAnalysis.hxx"
%}

%include MetaModelAnalysis_doc.i

%include otgui/MetaModelAnalysis.hxx
namespace OTGUI {

%extend MetaModelAnalysis { MetaModelAnalysis(const MetaModelAnalysis & other) { return new OTGUI::MetaModelAnalysis(other); } 

} }
