// SWIG file ImportanceSamplingAnalysis.i

%{
#include "persalys/ImportanceSamplingAnalysis.hxx"
%}

%include ImportanceSamplingAnalysis_doc.i

%include persalys/ImportanceSamplingAnalysis.hxx
namespace PERSALYS {

%extend ImportanceSamplingAnalysis { ImportanceSamplingAnalysis(const ImportanceSamplingAnalysis & other) { return new PERSALYS::ImportanceSamplingAnalysis(other); } 

} }
