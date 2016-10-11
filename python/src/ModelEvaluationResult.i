// SWIG file

%{
#include "otgui/ModelEvaluationResult.hxx"
%}

%include ModelEvaluationResult_doc.i

%include otgui/ModelEvaluationResult.hxx
namespace OTGUI {

%extend ModelEvaluationResult { ModelEvaluationResult(const ModelEvaluationResult & other) { return new OTGUI::ModelEvaluationResult(other); } 

} }
