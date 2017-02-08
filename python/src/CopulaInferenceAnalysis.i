// SWIG file

%{
#include "otgui/CopulaInferenceAnalysis.hxx"
%}

%include CopulaInferenceAnalysis_doc.i

%include otgui/CopulaInferenceAnalysis.hxx
namespace OTGUI {

%extend CopulaInferenceAnalysis { CopulaInferenceAnalysis(const CopulaInferenceAnalysis & other) { return new OTGUI::CopulaInferenceAnalysis(other); } 

} }
