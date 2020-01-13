// SWIG file CouplingResourceFile.i

%{
#include "persalys/CouplingResourceFile.hxx"
%}

%include CouplingResourceFile_doc.i

%include persalys/CouplingResourceFile.hxx
namespace PERSALYS {

%extend CouplingResourceFile { CouplingResourceFile(const CouplingResourceFile & other) { return new PERSALYS::CouplingResourceFile(other); } 

} }
