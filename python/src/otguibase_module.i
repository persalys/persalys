// SWIG file

%module(docstring="otguibase module") otguibase

%{
#include "openturns/OT.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
%}

// Prerequisites needed
%include typemaps.i
%include exception.i
%ignore *::load(OT::Advocate & adv);
%ignore *::save(OT::Advocate & adv) const;

%import base_module.i
%import uncertainty_module.i



// define OTGUITypedInterfaceObjectHelper
%define OTGUITypedInterfaceObjectImplementationHelper(Interface,Implementation)

%template(Implementation ## TypedInterfaceObject)           OT::TypedInterfaceObject<OTGUI::Implementation>;

%typemap(in) const OTGUI::Interface &
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0)))
  {
    // From interface class, ok
  }
  else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_OTGUI__ ## Implementation, 0)))
  {
    // From Implementation*
    OTGUI::Implementation * p_impl = reinterpret_cast< OTGUI::Implementation * >( ptr );
    $1 = new OTGUI::Interface( &*p_impl );
  }
  else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Pointer<OTGUI::Implementation> *"), 0)))
  {
    // From Pointer<Implementation>
    OT::Pointer<OTGUI::Implementation> * p_impl = reinterpret_cast< OT::Pointer<OTGUI::Implementation> * >( ptr );
    $1 = new OTGUI::Interface( **p_impl );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OTGUI::Interface &
{
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_OTGUI__ ## Implementation, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Pointer<OTGUI::Implementation> *"), 0));
}
%enddef

%define OTGUITypedInterfaceObjectHelper(Interface)
OTGUITypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef


// define OTGUITypedCollectionInterfaceObjectHelper
%define OTGUITypedCollectionInterfaceObjectMisnamedHelper(Interface,CollectionType)

%template(CollectionType)           OT::Collection<OTGUI::Interface>;

%typemap(in) const CollectionType & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    $1 = OT::buildCollectionFromPySequence< OTGUI::Interface >( $input );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CollectionType & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OTGUI::Interface >( $input );
}

%apply const CollectionType & { const OT::Collection<OTGUI::Interface> & };

%enddef

%define OTGUITypedCollectionInterfaceObjectHelper(Interface)
OTGUITypedCollectionInterfaceObjectMisnamedHelper(Interface,Interface ## Collection)
%enddef



// The new classes
%include otgui/OTGuiprivate.hxx
%include Observer.i
%include Observable.i

%include Variable.i
%include Input.i
%include Output.i
%include DataSample.i
%include ImportedSample.i
%include DataModelImplementation.i
%include DataModel.i
%include PhysicalModelImplementation.i
%include AnalyticalPhysicalModel.i
%include PythonPhysicalModel.i
%include PhysicalModel.i
#ifdef OTGUI_HAVE_YACS
%include YACSPhysicalModel.i
#endif
%include LimitStateImplementation.i
%include LimitState.i
%include AnalysisImplementation.i
%include PhysicalModelAnalysis.i
%include DataModelAnalysis.i
%include Analysis.i
%include DataAnalysisResult.i
%include DataAnalysis.i
%include WithStopCriteriaAnalysis.i
%include ModelEvaluationResult.i
%include ModelEvaluation.i
%include SimulationAnalysis.i
%include SimulationAnalysisResult.i
%include DesignOfExperimentImplementation.i
%include DesignOfExperiment.i
%include FixedDesignOfExperiment.i
%include FromFileDesignOfExperiment.i
%include MonteCarloResult.i
%include TaylorExpansionMomentsResult.i
%include MonteCarloAnalysis.i
%include TaylorExpansionMomentsAnalysis.i
%include SobolResult.i
%include SobolAnalysis.i
%include SRCResult.i
%include SRCAnalysis.i
%include MonteCarloReliabilityResult.i
%include ReliabilityAnalysis.i
%include MonteCarloReliabilityAnalysis.i
%include OTStudy.i

