// SWIG file

%{
#include "persalys/TaylorExpansionMomentsAnalysis.hxx"
%}

%include TaylorExpansionMomentsAnalysis_doc.i

%include persalys/TaylorExpansionMomentsAnalysis.hxx
namespace PERSALYS {

%extend TaylorExpansionMomentsAnalysis { TaylorExpansionMomentsAnalysis(const TaylorExpansionMomentsAnalysis & other) { return new PERSALYS::TaylorExpansionMomentsAnalysis(other); } 

} }
