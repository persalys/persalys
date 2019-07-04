// SWIG file CalibrationAnalysisResult.i

%{
#include "persalys/CalibrationAnalysisResult.hxx"
%}

%include CalibrationAnalysisResult_doc.i

%include persalys/CalibrationAnalysisResult.hxx
namespace PERSALYS {

%extend CalibrationAnalysisResult { CalibrationAnalysisResult(const CalibrationAnalysisResult & other) { return new PERSALYS::CalibrationAnalysisResult(other); } 

} }
