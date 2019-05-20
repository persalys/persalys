// SWIG file

%{
#include "persalys/TaylorExpansionMomentsResult.hxx"
%}

%include TaylorExpansionMomentsResult_doc.i

%include persalys/TaylorExpansionMomentsResult.hxx
namespace PERSALYS {

%extend TaylorExpansionMomentsResult { TaylorExpansionMomentsResult(const TaylorExpansionMomentsResult & other) { return new PERSALYS::TaylorExpansionMomentsResult(other); } 

} }
