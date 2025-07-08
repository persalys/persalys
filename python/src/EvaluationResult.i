// SWIG file

%{
#include "persalys/EvaluationResult.hxx"
%}

%include EvaluationResult_doc.i

%copyctor PERSALYS::EvaluationResult;

%include persalys/EvaluationResult.hxx
