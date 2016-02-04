// SWIG file

%{
#include "otgui/TaylorExpansionsMomentsResult.hxx"
%}

%include TaylorExpansionsMomentsResult_doc.i

%include otgui/TaylorExpansionsMomentsResult.hxx
namespace OTGUI {

%extend TaylorExpansionsMomentsResult { TaylorExpansionsMomentsResult(const TaylorExpansionsMomentsResult & other) { return new OTGUI::TaylorExpansionsMomentsResult(other); } 

std::string __repr__() {
  return "TaylorExpansionsMomentsResult";
}

} }
