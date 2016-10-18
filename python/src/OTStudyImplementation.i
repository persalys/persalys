// SWIG file OTStudyImplementation.i

%{
#include "otgui/OTStudyImplementation.hxx"
%}

%include otgui/OTStudyImplementation.hxx
namespace OTGUI {

%extend OTStudyImplementation { OTStudyImplementation(const OTStudyImplementation & other) { return new OTGUI::OTStudyImplementation(other); } 

} }
