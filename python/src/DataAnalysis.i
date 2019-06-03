// SWIG file

%{
#include "persalys/DataAnalysis.hxx"
%}

%include DataAnalysis_doc.i

%include persalys/DataAnalysis.hxx
namespace PERSALYS {

%extend DataAnalysis { DataAnalysis(const DataAnalysis & other) { return new PERSALYS::DataAnalysis(other); } 

} }
