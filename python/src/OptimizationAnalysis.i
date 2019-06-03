// SWIG file OptimizationAnalysis.i

%{
#include "persalys/OptimizationAnalysis.hxx"
%}

%include OptimizationAnalysis_doc.i

%ignore PERSALYS::OptimizationAnalysis::updateParameters;

%include persalys/OptimizationAnalysis.hxx
namespace PERSALYS {

%extend OptimizationAnalysis { OptimizationAnalysis(const OptimizationAnalysis & other) { return new PERSALYS::OptimizationAnalysis(other); } 

} }
