// SWIG file AnalysisImplementation.i

%{
#include "persalys/AnalysisImplementation.hxx"
%}

%ignore *::getInformationMessage;
%ignore *::getProgressValue;
%ignore *::stop;
%ignore *::acceptLaunchParameters;
%ignore PERSALYS::AnalysisImplementation::getParameters;

%include AnalysisImplementation_doc.i

%include persalys/AnalysisImplementation.hxx
namespace PERSALYS {

%extend AnalysisImplementation { AnalysisImplementation(const AnalysisImplementation & other) { return new PERSALYS::AnalysisImplementation(other); } 

std::string __repr__() {
  return "AnalysisImplementation";
}

} }
