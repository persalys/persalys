// SWIG file CalibrationAnalysis.i

%{
#include "persalys/CalibrationAnalysis.hxx"
%}

%include CalibrationAnalysis_doc.i

%ignore PERSALYS::CalibrationAnalysis::updateParameters;

%include persalys/CalibrationAnalysis.hxx
namespace PERSALYS {

%extend CalibrationAnalysis { CalibrationAnalysis(const CalibrationAnalysis & other) { return new PERSALYS::CalibrationAnalysis(other); } 

} }
