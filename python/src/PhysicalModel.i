// SWIG file

%{
#include "otgui/PhysicalModel.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::PhysicalModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::PhysicalModel
  convert< _PyObject_, OTGUI::PhysicalModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::PhysicalModel *"), 0))) {
      OTGUI::PhysicalModel * p_it = reinterpret_cast< OTGUI::PhysicalModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a PhysicalModel";
    }
    return OTGUI::PhysicalModel();
  }
}
%}

%ignore *::addObserver;
%ignore *::blockNotification;
%ignore *::acceptLaunchParameters;
%ignore *::getHtmlDescription;
%ignore *::getCopulaCollection;

%include PhysicalModel_doc.i

OTGUITypedInterfaceObjectHelper(PhysicalModel)
OTGUITypedCollectionInterfaceObjectHelper(PhysicalModel)

%include otgui/PhysicalModel.hxx
namespace OTGUI {

%extend PhysicalModel { PhysicalModel(const PhysicalModel & other) { return new OTGUI::PhysicalModel(other); } 

std::string __repr__() {
  return "PhysicalModel";
}

} }
