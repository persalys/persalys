// SWIG file

%{
#include "persalys/ModelEvaluation.hxx"
%}

%include ModelEvaluation_doc.i

%ignore PERSALYS::ModelEvaluation::updateParameters;

%include persalys/ModelEvaluation.hxx
namespace PERSALYS {

%extend ModelEvaluation { ModelEvaluation(const ModelEvaluation & other) { return new PERSALYS::ModelEvaluation(other); } 

} }
