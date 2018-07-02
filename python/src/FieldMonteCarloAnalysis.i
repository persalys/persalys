// SWIG file

%{
#include "otgui/FieldMonteCarloAnalysis.hxx"
%}

%include FieldMonteCarloAnalysis_doc.i

%include otgui/FieldMonteCarloAnalysis.hxx
namespace OTGUI {

%extend FieldMonteCarloAnalysis { FieldMonteCarloAnalysis(const FieldMonteCarloAnalysis & other) { return new OTGUI::FieldMonteCarloAnalysis(other); } 

} }
