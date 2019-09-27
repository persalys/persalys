// SWIG file

%{
#include "persalys/GridDesignOfExperiment.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
namespace OT {
  template <>
  inline
  bool
  canConvert< _PySequence_, Point >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Point, 0))) {
      Point * p_it = reinterpret_cast< Point * >(ptr);
      return p_it != NULL;
    } else {
      return OT::isAPythonSequenceOf<OT::_PyFloat_>( pyObj );
    }
    return false;
  }
}
%}

%include GridDesignOfExperiment_doc.i

%ignore PERSALYS::GridDesignOfExperiment::updateParameters;
%ignore PERSALYS::GridDesignOfExperiment::setDesignOfExperiment;

%typemap(in) const OT::Collection<OT::Point> & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence<OT::Point>($input);
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Point");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_INT64_ARRAY) const OT::Collection<OT::Point> & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence<OT::Point>($input);
}

%include persalys/GridDesignOfExperiment.hxx
namespace PERSALYS {

%extend GridDesignOfExperiment { GridDesignOfExperiment(const GridDesignOfExperiment & other) { return new PERSALYS::GridDesignOfExperiment(other); } 

} }
