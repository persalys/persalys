// SWIG file

%{
#include "otgui/FORMAnalysis.hxx"
%}

%include FORMAnalysis_doc.i

%include otgui/FORMAnalysis.hxx
namespace OTGUI {

%extend FORMAnalysis { FORMAnalysis(const FORMAnalysis & other) { return new OTGUI::FORMAnalysis(other); } 

} }