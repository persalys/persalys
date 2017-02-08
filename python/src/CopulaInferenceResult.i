// SWIG file

%{
#include "otgui/CopulaInferenceResult.hxx"
%}

%include CopulaInferenceResult_doc.i

%include otgui/CopulaInferenceResult.hxx
namespace OTGUI {

%extend CopulaInferenceResult { CopulaInferenceResult(const CopulaInferenceResult & other) { return new OTGUI::CopulaInferenceResult(other); } 

} }