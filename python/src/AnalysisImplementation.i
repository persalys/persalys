// SWIG file AnalysisImplementation.i

%{
#include "otgui/AnalysisImplementation.hxx"
%}

%ignore *::getInformationMessage;
%ignore *::getProgressValue;
%ignore *::stop;
%ignore *::acceptLaunchParameters;

%include AnalysisImplementation_doc.i

%include otgui/AnalysisImplementation.hxx
namespace OTGUI {

%extend AnalysisImplementation { AnalysisImplementation(const AnalysisImplementation & other) { return new OTGUI::AnalysisImplementation(other); } 

std::string __repr__() {
  return "AnalysisImplementation";
}

} }
