// SWIG file

%{
#include "otgui/TaylorExpansionMomentsResult.hxx"
%}

%include TaylorExpansionMomentsResult_doc.i

%include otgui/TaylorExpansionMomentsResult.hxx
namespace OTGUI {

%extend TaylorExpansionMomentsResult { TaylorExpansionMomentsResult(const TaylorExpansionMomentsResult & other) { return new OTGUI::TaylorExpansionMomentsResult(other); } 

std::string __repr__() {
  return "TaylorExpansionMomentsResult";
}

} }
