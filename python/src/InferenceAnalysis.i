// SWIG file

%{
#include "persalys/InferenceAnalysis.hxx"
%}

%include InferenceAnalysis_doc.i

%copyctor PERSALYS::InferenceAnalysis;

%include persalys/InferenceAnalysis.hxx
