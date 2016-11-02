// SWIG file

%{
#include "otgui/FittingTestResult.hxx"
%}

%include FittingTestResult_doc.i

%include otgui/FittingTestResult.hxx
namespace OTGUI {

%extend FittingTestResult { FittingTestResult(const FittingTestResult & other) { return new OTGUI::FittingTestResult(other); } 

} }
