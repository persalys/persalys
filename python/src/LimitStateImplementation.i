// SWIG file

%{
#include "persalys/LimitStateImplementation.hxx"

namespace OT {
  template <>
  struct traitsPythonType< OT::ComparisonOperator >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::ComparisonOperator >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::ComparisonOperator *"), 0))) {
      OT::ComparisonOperator * p_it = reinterpret_cast< OT::ComparisonOperator * >( ptr );
      return p_it != NULL;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ComparisonOperatorImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::ComparisonOperatorImplementation * p_impl = reinterpret_cast< OT::ComparisonOperatorImplementation * >( ptr );
      return p_impl != NULL;
    }
    return false;
  }

  template <>
  inline
  OT::ComparisonOperator
  convert< _PyObject_, OT::ComparisonOperator >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::ComparisonOperator *"), 0))) {
      OT::ComparisonOperator * p_it = reinterpret_cast< OT::ComparisonOperator * >( ptr );
      return *p_it;
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::ComparisonOperatorImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::ComparisonOperatorImplementation * p_impl = reinterpret_cast< OT::ComparisonOperatorImplementation * >( ptr );
      return OT::ComparisonOperator(*p_impl);
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a ComparisonOperator";
    }
    return OT::ComparisonOperator();
  }
}
%}

%include LimitStateImplementation_doc.i

%template(ComparisonOperatorCollection) OT::Collection<OT::ComparisonOperator>;

%typemap(in) const ComparisonOperatorCollection & (OT::Pointer<OT::Collection<OT::ComparisonOperator> > temp)
{
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL)))
  {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence<OT::ComparisonOperator>($input);
      $1 = temp.get();
    }
    catch (const OT::InvalidArgumentException &) {
       SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Collection of ComparisonOperator");
    }
  }
}

%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) const ComparisonOperatorCollection &
{
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence<OT::ComparisonOperator>($input);
}

%apply const ComparisonOperatorCollection & { const PERSALYS::LimitStateImplementation::ComparisonOperatorCollection & };

%copyctor PERSALYS::LimitStateImplementation;

%include persalys/LimitStateImplementation.hxx
