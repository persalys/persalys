// SWIG file

%{
#include "persalys/DataModel.hxx"
%}

%include DataModel_doc.i

%include persalys/DataModel.hxx
namespace PERSALYS {

%extend DataModel { DataModel(const DataModel & other) { return new PERSALYS::DataModel(other); } 

} }