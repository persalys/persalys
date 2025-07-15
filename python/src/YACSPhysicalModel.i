// SWIG file YACSPhysicalModel.i

%{
#include "persalys/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i


namespace PERSALYS {

%extend YACSPhysicalModel {

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore PERSALYS::YACSPhysicalModel::jobParameters;

%copyctor PERSALYS::YACSPhysicalModel;

%include persalys/YACSPhysicalModel.hxx
