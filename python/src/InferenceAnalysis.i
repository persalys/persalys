// SWIG file

%{
#include "otgui/InferenceAnalysis.hxx"
%}

%include InferenceAnalysis_doc.i

%include otgui/InferenceAnalysis.hxx
namespace OTGUI {

%extend InferenceAnalysis { InferenceAnalysis(const InferenceAnalysis & other) { return new OTGUI::InferenceAnalysis(other); } 

} }
