// SWIG file

%{
#include "persalys/SobolAnalysis.hxx"
%}

%include SobolAnalysis_doc.i

%copyctor PERSALYS::SobolAnalysis;

%include persalys/SobolAnalysis.hxx
