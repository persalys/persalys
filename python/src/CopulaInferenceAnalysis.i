// SWIG file

%{
#include "persalys/CopulaInferenceAnalysis.hxx"
%}

%include CopulaInferenceAnalysis_doc.i

%include persalys/CopulaInferenceAnalysis.hxx
namespace PERSALYS {

%extend CopulaInferenceAnalysis { CopulaInferenceAnalysis(const CopulaInferenceAnalysis & other) { return new PERSALYS::CopulaInferenceAnalysis(other); } 

} }
