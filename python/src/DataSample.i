// SWIG file

%{
#include "persalys/DataSample.hxx"
%}

%include DataSample_doc.i

%include persalys/DataSample.hxx
namespace PERSALYS {

%extend DataSample { DataSample(const DataSample & other) { return new PERSALYS::DataSample(other); } 

} }
