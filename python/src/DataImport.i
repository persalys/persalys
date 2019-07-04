// SWIG file

%{
#include "persalys/DataImport.hxx"
%}

%include DataImport_doc.i

%include persalys/DataImport.hxx
namespace PERSALYS {

%extend DataImport { DataImport(const DataImport & other) { return new PERSALYS::DataImport(other); } 

} }
