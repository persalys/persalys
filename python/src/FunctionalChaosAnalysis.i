// SWIG file

%{
#include "otgui/FunctionalChaosAnalysis.hxx"
%}

%include FunctionalChaosAnalysis_doc.i

%include otgui/FunctionalChaosAnalysis.hxx
namespace OTGUI {

%extend FunctionalChaosAnalysis { FunctionalChaosAnalysis(const FunctionalChaosAnalysis & other) { return new OTGUI::FunctionalChaosAnalysis(other); } 

} }
