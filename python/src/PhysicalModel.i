// SWIG file

%{
#include "persalys/PhysicalModel.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::PhysicalModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::PhysicalModel
  convert< _PyObject_, PERSALYS::PhysicalModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::PhysicalModel *"), 0))) {
      PERSALYS::PhysicalModel * p_it = reinterpret_cast< PERSALYS::PhysicalModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a PhysicalModel";
    }
    return PERSALYS::PhysicalModel();
  }
}
%}

%ignore *::addObserver;
%ignore *::blockNotification;
%ignore *::acceptLaunchParameters;
%ignore *::getHtmlDescription;
%ignore *::getCopulaCollection;

%include PhysicalModel_doc.i

PERSALYSTypedInterfaceObjectHelper(PhysicalModel)
PERSALYSTypedCollectionInterfaceObjectHelper(PhysicalModel)

%include persalys/PhysicalModel.hxx
namespace PERSALYS {

%extend PhysicalModel { PhysicalModel(const PhysicalModel & other) { return new PERSALYS::PhysicalModel(other); } 

std::string __repr__() {
  return "PhysicalModel";
}

} }
