// SWIG file CouplingInputFile.i

%{
#include "persalys/CouplingInputFile.hxx"
%}

%include CouplingInputFile_doc.i

%include persalys/CouplingInputFile.hxx
namespace PERSALYS {

%extend CouplingInputFile { CouplingInputFile(const CouplingInputFile & other) { return new PERSALYS::CouplingInputFile(other); } 

} }
