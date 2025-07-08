// SWIG file

%{
#include "persalys/DataAnalysisResult.hxx"
%}

%include DataAnalysisResult_doc.i

%copyctor PERSALYS::DataAnalysisResult;

%include persalys/DataAnalysisResult.hxx
