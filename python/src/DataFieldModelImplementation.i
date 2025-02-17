// SWIG file

%{
#include "persalys/DataFieldModelImplementation.hxx"
%}

%include DataFieldModelImplementation_doc.i

%include persalys/DataFieldModelImplementation.hxx
namespace PERSALYS {

%extend DataFieldModelImplementation { DataFieldModelImplementation(const DataFieldModelImplementation & other) { return new PERSALYS::DataFieldModelImplementation(other); }

} }
