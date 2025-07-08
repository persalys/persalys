// SWIG file AnalysisImplementation.i

%{
#include "persalys/AnalysisImplementation.hxx"
%}

%ignore *::getInformationMessage;
%ignore *::getProgressValue;
%ignore *::stop;
%ignore PERSALYS::AnalysisImplementation::getParameters;

%include AnalysisImplementation_doc.i

%copyctor PERSALYS::AnalysisImplementation;

%include persalys/AnalysisImplementation.hxx
namespace PERSALYS {

%extend AnalysisImplementation {

std::string __repr__() {
  return "AnalysisImplementation";
}

} }
