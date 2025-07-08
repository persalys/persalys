// SWIG file

%{
#include "persalys/MorrisAnalysis.hxx"
%}

%include MorrisAnalysis_doc.i

%ignore PERSALYS::MorrisAnalysis::updateParameters;

%copyctor PERSALYS::MorrisAnalysis;

%include persalys/MorrisAnalysis.hxx
