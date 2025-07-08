// SWIG file YACSCouplingPhysicalModel.i

%{
#include "persalys/YACSCouplingPhysicalModel.hxx"
%}

%include YACSCouplingPhysicalModel_doc.i

namespace PERSALYS {

%extend YACSCouplingPhysicalModel {

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore PERSALYS::YACSCouplingPhysicalModel::jobParameters;

%copyctor PERSALYS::YACSCouplingPhysicalModel;

%include persalys/YACSCouplingPhysicalModel.hxx
