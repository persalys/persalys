// SWIG file

%{
#include "persalys/CopulaInferenceResult.hxx"
%}

%include CopulaInferenceResult_doc.i

%include persalys/CopulaInferenceResult.hxx
namespace PERSALYS {

%extend CopulaInferenceResult { CopulaInferenceResult(const CopulaInferenceResult & other) { return new PERSALYS::CopulaInferenceResult(other); } 

} }