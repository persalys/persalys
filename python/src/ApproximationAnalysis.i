// SWIG file

%{
#include "otgui/ApproximationAnalysis.hxx"
%}

%include ApproximationAnalysis_doc.i

%include otgui/ApproximationAnalysis.hxx
namespace OTGUI {

%extend ApproximationAnalysis { ApproximationAnalysis(const ApproximationAnalysis & other) { return new OTGUI::ApproximationAnalysis(other); } 

} }
