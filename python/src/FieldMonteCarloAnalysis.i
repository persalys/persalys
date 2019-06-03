// SWIG file

%{
#include "persalys/FieldMonteCarloAnalysis.hxx"
%}

%include FieldMonteCarloAnalysis_doc.i

%include persalys/FieldMonteCarloAnalysis.hxx
namespace PERSALYS {

%extend FieldMonteCarloAnalysis { FieldMonteCarloAnalysis(const FieldMonteCarloAnalysis & other) { return new PERSALYS::FieldMonteCarloAnalysis(other); } 

} }
