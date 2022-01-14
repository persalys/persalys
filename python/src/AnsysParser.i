// SWIG file

%{
#include "persalys/AnsysParser.hxx"
%}

%include AnsysParser_doc.i

%include persalys/AnsysParser.hxx
namespace PERSALYS {

%extend AnsysParser { AnsysParser(const AnsysParser & other) { return new PERSALYS::AnsysParser(other); }

} }
