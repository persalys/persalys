// SWIG file CalibrationAnalysis.i

%{
#include "persalys/CalibrationAnalysis.hxx"
%}

%include CalibrationAnalysis_doc.i

%ignore PERSALYS::CalibrationAnalysis::updateParameters;

%copyctor PERSALYS::CalibrationAnalysis;

%include persalys/CalibrationAnalysis.hxx
