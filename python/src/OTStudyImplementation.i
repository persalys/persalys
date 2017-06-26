// SWIG file OTStudyImplementation.i

%{
#include "otgui/OTStudyImplementation.hxx"
%}

%ignore OTGUI::OTStudyImplementation::setFileName;

%include otgui/OTStudyImplementation.hxx
namespace OTGUI {

%extend OTStudyImplementation { OTStudyImplementation(const OTStudyImplementation & other) { return new OTGUI::OTStudyImplementation(other); } 

} }
