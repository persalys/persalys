// SWIG file

%{
#include "otgui/SORMAnalysis.hxx"
%}

%include SORMAnalysis_doc.i

%include otgui/SORMAnalysis.hxx
namespace OTGUI {

%extend SORMAnalysis { SORMAnalysis(const SORMAnalysis & other) { return new OTGUI::SORMAnalysis(other); } 

} }
