// SWIG file

%{
#include "persalys/InferenceResult.hxx"
%}

%include InferenceResult_doc.i

%copyctor PERSALYS::InferenceResult;

%include persalys/InferenceResult.hxx
