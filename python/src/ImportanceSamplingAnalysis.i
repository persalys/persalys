// SWIG file ImportanceSamplingAnalysis.i

%{
#include "otgui/ImportanceSamplingAnalysis.hxx"
%}

%include ImportanceSamplingAnalysis_doc.i

%include otgui/ImportanceSamplingAnalysis.hxx
namespace OTGUI {

%extend ImportanceSamplingAnalysis { ImportanceSamplingAnalysis(const ImportanceSamplingAnalysis & other) { return new OTGUI::ImportanceSamplingAnalysis(other); } 

} }
