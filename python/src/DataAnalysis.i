// SWIG file

%{
#include "persalys/DataAnalysis.hxx"
%}

%include DataAnalysis_doc.i

%copyctor PERSALYS::DataAnalysis;

%include persalys/DataAnalysis.hxx
