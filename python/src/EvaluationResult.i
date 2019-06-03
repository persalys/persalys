// SWIG file

%{
#include "persalys/EvaluationResult.hxx"
%}

%include EvaluationResult_doc.i

%include persalys/EvaluationResult.hxx
namespace PERSALYS {

%extend EvaluationResult { EvaluationResult(const EvaluationResult & other) { return new PERSALYS::EvaluationResult(other); } 

} }
