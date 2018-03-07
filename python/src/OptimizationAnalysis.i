// SWIG file OptimizationAnalysis.i

%{
#include "otgui/OptimizationAnalysis.hxx"
%}

%include OptimizationAnalysis_doc.i

%ignore OTGUI::OptimizationAnalysis::updateParameters;

%include otgui/OptimizationAnalysis.hxx
namespace OTGUI {

%extend OptimizationAnalysis { OptimizationAnalysis(const OptimizationAnalysis & other) { return new OTGUI::OptimizationAnalysis(other); } 

} }
