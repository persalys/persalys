// SWIG file

%{
#include "persalys/AnalysisResult.hxx"
%}

%include AnalysisResult_doc.i

%copyctor PERSALYS::AnalysisResult;

%include persalys/AnalysisResult.hxx
