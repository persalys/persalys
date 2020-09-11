// SWIG file DataCleaningTools.i

%{
#include "persalys/DataCleaningTools.hxx"
%}

%include DataCleaningTools_doc.i

%include persalys/DataCleaningTools.hxx
namespace PERSALYS {

%extend DataCleaningTools { DataCleaningTools(const DataCleaningTools & other) { return new PERSALYS::DataCleaningTools(other); }

} }
