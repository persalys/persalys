// SWIG file

%{
#include "otgui/DataModel.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::DataModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::DataModel
  convert< _PyObject_, OTGUI::DataModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::DataModel *"), 0))) {
      OTGUI::DataModel * p_it = reinterpret_cast< OTGUI::DataModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a DataModel";
    }
    return OTGUI::DataModel();
  }
}
%}

%include DataModel_doc.i

OTGUITypedInterfaceObjectHelper(DataModel)
OTGUITypedCollectionInterfaceObjectHelper(DataModel)

%include otgui/DataModel.hxx
namespace OTGUI {

%extend DataModel { DataModel(const DataModel & other) { return new OTGUI::DataModel(other); } 

std::string __repr__() {
  return "DataModel";
}

} }
