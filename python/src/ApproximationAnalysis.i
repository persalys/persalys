// SWIG file

%{
#include "persalys/ApproximationAnalysis.hxx"
%}

%include ApproximationAnalysis_doc.i

%include persalys/ApproximationAnalysis.hxx
namespace PERSALYS {

%extend ApproximationAnalysis { ApproximationAnalysis(const ApproximationAnalysis & other) { return new PERSALYS::ApproximationAnalysis(other); } 

} }
