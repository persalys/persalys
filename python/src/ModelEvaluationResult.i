// SWIG file

%{
#include "otgui/ModelEvaluationResult.hxx"
%}


%include otgui/ModelEvaluationResult.hxx
namespace OTGUI {

%extend ModelEvaluationResult { ModelEvaluationResult(const ModelEvaluationResult & other) { return new OTGUI::ModelEvaluationResult(other); } 

std::string __repr__() {
  return "ModelEvaluationResult";
}

} }