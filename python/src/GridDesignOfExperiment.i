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
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_NO_NULL))) {
      Point * p_it = reinterpret_cast< Point * >(ptr);
      return p_it != NULL;
    } else {
      return OT::isAPythonSequenceOf<OT::_PyFloat_>( pyObj );
    }
  }
}
%}

%include GridDesignOfExperiment_doc.i

%ignore PERSALYS::GridDesignOfExperiment::updateParameters;
%ignore PERSALYS::GridDesignOfExperiment::setDesignOfExperiment;

%typemap(in) const OT::Collection<OT::Point> & (OT::Pointer<OT::Collection<OT::Point> > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence<OT::Point>($input);
      $1 = temp.get();
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Point");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_INT64_ARRAY) const OT::Collection<OT::Point> & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence<OT::Point>($input);
}

%copyctor PERSALYS::GridDesignOfExperiment;

%include persalys/GridDesignOfExperiment.hxx
