// SWIG file AnalysisImplementation.i

%{
#include "persalys/AnalysisImplementation.hxx"
%}

%ignore PERSALYS::AnalysisImplementation::getInformationMessage;
%ignore PERSALYS::AnalysisImplementation::getProgressValue;
%ignore PERSALYS::AnalysisImplementation::stop;
%ignore PERSALYS::AnalysisImplementation::detach;
%ignore PERSALYS::AnalysisImplementation::canBeLaunched;
%ignore PERSALYS::AnalysisImplementation::canBeDetached;
%ignore PERSALYS::AnalysisImplementation::getElapsedTime;
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
