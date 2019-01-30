// SWIG file

%{
#include "otgui/EvaluationResult.hxx"
%}

%include EvaluationResult_doc.i

%include otgui/EvaluationResult.hxx
namespace OTGUI {

%extend EvaluationResult { EvaluationResult(const EvaluationResult & other) { return new OTGUI::EvaluationResult(other); } 

} }
