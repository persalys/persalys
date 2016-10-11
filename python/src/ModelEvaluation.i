// SWIG file

%{
#include "otgui/ModelEvaluation.hxx"
%}

%include ModelEvaluation_doc.i

%ignore OTGUI::ModelEvaluation::updateParameters;

%include otgui/ModelEvaluation.hxx
namespace OTGUI {

%extend ModelEvaluation { ModelEvaluation(const ModelEvaluation & other) { return new OTGUI::ModelEvaluation(other); } 

} }
