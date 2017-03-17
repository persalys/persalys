// SWIG file FORMImportanceSamplingAnalysis.i

%{
#include "otgui/FORMImportanceSamplingAnalysis.hxx"
%}

%include FORMImportanceSamplingAnalysis_doc.i

%include otgui/FORMImportanceSamplingAnalysis.hxx
namespace OTGUI {

%extend FORMImportanceSamplingAnalysis { FORMImportanceSamplingAnalysis(const FORMImportanceSamplingAnalysis & other) { return new OTGUI::FORMImportanceSamplingAnalysis(other); } 

} }
