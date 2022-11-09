// SWIG file MultiObjectiveOptimizationAnalysis.i

%{
#include "persalys/MultiObjectiveOptimizationAnalysis.hxx"
%}

%include MultiObjectiveOptimizationAnalysis_doc.i

%ignore PERSALYS::MultiObjectiveOptimizationAnalysis::updateParameters;

%include persalys/MultiObjectiveOptimizationAnalysis.hxx
namespace PERSALYS {

%extend MultiObjectiveOptimizationAnalysis { MultiObjectiveOptimizationAnalysis(const MultiObjectiveOptimizationAnalysis & other) { return new PERSALYS::MultiObjectiveOptimizationAnalysis(other); }

}

}
