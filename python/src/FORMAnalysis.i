// SWIG file

%{
#include "persalys/FORMAnalysis.hxx"
%}

%include FORMAnalysis_doc.i

%include persalys/FORMAnalysis.hxx
namespace PERSALYS {

%extend FORMAnalysis { FORMAnalysis(const FORMAnalysis & other) { return new PERSALYS::FORMAnalysis(other); } 

} }