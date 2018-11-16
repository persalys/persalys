// SWIG file StudyImplementation.i

%{
#include "otgui/StudyImplementation.hxx"
%}

%ignore OTGUI::StudyImplementation::setFileName;

%include otgui/StudyImplementation.hxx
namespace OTGUI {

%extend StudyImplementation { StudyImplementation(const StudyImplementation & other) { return new OTGUI::StudyImplementation(other); } 

} }
