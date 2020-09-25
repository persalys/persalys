// SWIG file DataCleaning.i

%{
#include "persalys/DataCleaning.hxx"
%}

%include DataCleaning_doc.i

%include persalys/DataCleaning.hxx
namespace PERSALYS {

%extend DataCleaning { DataCleaning(const DataCleaning & other) { return new PERSALYS::DataCleaning(other); }

} }
