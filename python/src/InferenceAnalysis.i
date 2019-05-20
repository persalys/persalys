// SWIG file

%{
#include "persalys/InferenceAnalysis.hxx"
%}

%include InferenceAnalysis_doc.i

%include persalys/InferenceAnalysis.hxx
namespace PERSALYS {

%extend InferenceAnalysis { InferenceAnalysis(const InferenceAnalysis & other) { return new PERSALYS::InferenceAnalysis(other); } 

} }
