// SWIG file

%{
#include "otgui/MorrisAnalysis.hxx"
%}



%include otgui/MorrisAnalysis.hxx
namespace OTGUI {

%extend MorrisAnalysis { MorrisAnalysis(const MorrisAnalysis & other) { return new OTGUI::MorrisAnalysis(other); } 

} }
