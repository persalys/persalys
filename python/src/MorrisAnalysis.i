// SWIG file

%{
#include "otgui/MorrisAnalysis.hxx"
%}

%include MorrisAnalysis_doc.i

%ignore OTGUI::MorrisAnalysis::updateParameters;

%include otgui/MorrisAnalysis.hxx
namespace OTGUI {

%extend MorrisAnalysis { MorrisAnalysis(const MorrisAnalysis & other) { return new OTGUI::MorrisAnalysis(other); } 

} }
