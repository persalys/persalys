// SWIG file

%{
#include "persalys/FunctionalChaosAnalysis.hxx"
%}

%include FunctionalChaosAnalysis_doc.i

%include persalys/FunctionalChaosAnalysis.hxx
namespace PERSALYS {

%extend FunctionalChaosAnalysis { FunctionalChaosAnalysis(const FunctionalChaosAnalysis & other) { return new PERSALYS::FunctionalChaosAnalysis(other); } 

} }
