// SWIG file CouplingOutputFile.i

%{
#include "persalys/CouplingOutputFile.hxx"
%}

%include CouplingOutputFile_doc.i

%include persalys/CouplingOutputFile.hxx
namespace PERSALYS {

%extend CouplingOutputFile { CouplingOutputFile(const CouplingOutputFile & other) { return new PERSALYS::CouplingOutputFile(other); } 

} }
