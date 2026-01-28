// SWIG file

%{
#include "persalys/DesignOfExperiment.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::DesignOfExperiment >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::DesignOfExperiment
  convert< _PyObject_, PERSALYS::DesignOfExperiment >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::DesignOfExperiment *"), 0))) {
      PERSALYS::DesignOfExperiment * p_it = reinterpret_cast< PERSALYS::DesignOfExperiment * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a DesignOfExperiment";
    }
    return PERSALYS::DesignOfExperiment();
  }
}
%}

%ignore PERSALYS::DesignOfExperiment::setImplementationAsPersistentObject;

%include DesignOfExperiment_doc.i

PERSALYSTypedInterfaceObjectImplementationHelper(DesignOfExperiment,DataModel)
PERSALYSTypedCollectionInterfaceObjectHelper(DesignOfExperiment)

%copyctor PERSALYS::DesignOfExperiment;

%include persalys/DesignOfExperiment.hxx
namespace PERSALYS {

%extend DesignOfExperiment {

std::string __repr__() {
  return "DesignOfExperiment";
}

} }
