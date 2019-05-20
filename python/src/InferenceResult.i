// SWIG file

%{
#include "persalys/InferenceResult.hxx"
%}

%include InferenceResult_doc.i

%include persalys/InferenceResult.hxx
namespace PERSALYS {

%extend InferenceResult { InferenceResult(const InferenceResult & other) { return new PERSALYS::InferenceResult(other); } 

} }
