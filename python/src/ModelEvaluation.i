// SWIG file

%{
#include "persalys/ModelEvaluation.hxx"
%}

%include ModelEvaluation_doc.i

%ignore PERSALYS::ModelEvaluation::updateParameters;

%copyctor PERSALYS::ModelEvaluation;

%include persalys/ModelEvaluation.hxx
