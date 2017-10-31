// SWIG file

%{
#include "otgui/MorrisResult.hxx"
%}



%include otgui/MorrisResult.hxx
namespace OTGUI {

%extend MorrisResult { MorrisResult(const MorrisResult & other) { return new OTGUI::MorrisResult(other); } 

} }
