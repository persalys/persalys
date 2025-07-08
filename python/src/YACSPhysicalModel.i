// SWIG file YACSPhysicalModel.i

%{
#include "persalys/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i


namespace PERSALYS {

%extend YACSPhysicalModel {
YACSPhysicalModel(const YACSPhysicalModel & other)
{ return new PERSALYS::YACSPhysicalModel(other); }

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore PERSALYS::YACSPhysicalModel::jobParameters;

%include persalys/YACSPhysicalModel.hxx
