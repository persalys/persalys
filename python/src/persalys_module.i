// SWIG file

%module(docstring="persalys module") persalys

%{
#include "openturns/OT.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
%}

// Prerequisites needed
%include typemaps.i
%include exception.i
%ignore *::load(OT::Advocate & adv);
%ignore *::save(OT::Advocate & adv) const;
%ignore *::GetClassName();
%ignore *::getParentObserver() const;


%import base_module.i
%import uncertainty_module.i

OTDefaultCollectionConvertFunctions(DistributionFactory)


// define PERSALYSTypedInterfaceObjectHelper
%define PERSALYSTypedInterfaceObjectImplementationHelper(Interface,Implementation)

OTgetImplementationHelper(PERSALYS, Interface, Implementation)

%template(_ ## Implementation ## TypedInterfaceObject) OT::TypedInterfaceObject<PERSALYS::Implementation>;

%typemap(in) const PERSALYS::Interface &
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, SWIG_TypeQuery("PERSALYS::Interface *"), SWIG_POINTER_NO_NULL)))
  {
    // From interface class, ok
  }
  else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("PERSALYS::Implementation *"), SWIG_POINTER_NO_NULL)))
  {
    // From Implementation*: use raw-pointer constructor to preserve object identity
    // (Persalys equality is pointer-identity based; the const-ref constructor would clone)
    PERSALYS::Implementation * p_impl = reinterpret_cast< PERSALYS::Implementation * >(ptr);
    $1 = new PERSALYS::Interface( p_impl );
  }
  else
  {
    SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a " # Interface);
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PERSALYS::Interface &
{
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("PERSALYS::Interface *"), SWIG_POINTER_NO_NULL))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("PERSALYS::Implementation *"), SWIG_POINTER_NO_NULL));
}
%enddef

%define PERSALYSTypedInterfaceObjectHelper(Interface)
PERSALYSTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef


// define PERSALYSTypedCollectionInterfaceObjectHelper
%define PERSALYSTypedCollectionInterfaceObjectMisnamedHelper(Interface,CollectionType)

%{
namespace OT {
  template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::Interface >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Interface *"), SWIG_POINTER_NO_NULL))) {
      PERSALYS::Interface * p_it = reinterpret_cast< PERSALYS::Interface * >(ptr);
      return p_it != NULL;
    }
    return false;
  }
} /* namespace OT */
%}

%template(CollectionType)           OT::Collection<PERSALYS::Interface>;

%typemap(in) const CollectionType & (OT::Pointer<OT::Collection<PERSALYS::Interface> > temp) {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    // From interface class, ok
  } else {
    try {
      temp = OT::buildCollectionFromPySequence< PERSALYS::Interface >($input);
      $1 = temp.get();
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of " # Interface);
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CollectionType & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::Interface >($input);
}

%apply const CollectionType & { const OT::Collection<PERSALYS::Interface> & };

%enddef

%define PERSALYSTypedCollectionInterfaceObjectHelper(Interface)
PERSALYSTypedCollectionInterfaceObjectMisnamedHelper(Interface,Interface ## Collection)
%enddef



// The new classes
%include persalys/PersalysPrivate.hxx
%include persalys/BaseTools.hxx
%include Observer.i
%include Observable.i

%include Variable.i
%include Input.i
%include Output.i
%include DataSample.i
%include ImportedDataset.i
%include MeshModelImplementation.i
%include MeshModel.i
%include GridMeshModel.i
%include ImportedMeshModel.i
%include PhysicalModelImplementation.i
%include SymbolicPhysicalModel.i
%include PythonPhysicalModel.i
%include MetaModel.i
%include PhysicalModel.i
%include DataModel.i
%include DesignOfExperiment.i
%include FMUInfo.i
%include FMIPhysicalModel.i
%include SymbolicFieldModel.i
%include PythonFieldModel.i
%include LimitStateImplementation.i
%include LimitState.i
%include AnalysisResult.i
%include AnalysisImplementation.i
%include PhysicalModelAnalysis.i
%include Analysis.i
%include WithStopCriteriaAnalysis.i
%include DataFieldModelImplementation.i
%include DataFieldModel.i
%include Observations.i
%include EvaluationResult.i
%include DataAnalysisResult.i
%include QuantileAnalysisResult.i
%include DesignOfExperimentAnalysis.i
%include SimulationAnalysis.i
%include DesignOfExperimentEvaluation.i
%include FixedDesignOfExperiment.i
%include GridDesignOfExperiment.i
%include ImportedDesignOfExperiment.i
%include ProbabilisticDesignOfExperiment.i
%include ModelEvaluation.i
%include FieldModelEvaluation.i
%include DataAnalysis.i
%include QuantileAnalysis.i
%include TaylorExpansionMomentsResult.i
%include MonteCarloAnalysis.i
%include FieldMonteCarloResult.i
%include FieldKarhunenLoeveAnalysis.i
%include FieldMonteCarloAnalysis.i
%include TaylorExpansionMomentsAnalysis.i
%include SobolResult.i
%include SobolAnalysis.i
%include SRCResult.i
%include SRCAnalysis.i
%include MorrisResult.i
%include MorrisAnalysis.i
%include ReliabilityAnalysis.i
%include SimulationReliabilityResult.i
%include SimulationReliabilityAnalysis.i
%include MonteCarloReliabilityAnalysis.i
%include ImportanceSamplingAnalysis.i
%include ApproximationAnalysis.i
%include FORMAnalysisResult.i
%include FORMAnalysis.i
%include SORMAnalysisResult.i
%include SORMAnalysis.i
%include FORMImportanceSamplingAnalysis.i
%include MetaModelValidationResult.i
%include MetaModelAnalysisResult.i
%include MetaModelAnalysis.i
%include FunctionalChaosAnalysisResult.i
%include FunctionalChaosAnalysis.i
%include KrigingAnalysisResult.i
%include KrigingAnalysis.i
%include PolynomialRegressionAnalysisResult.i
%include PolynomialRegressionAnalysis.i
%include FittingTestResult.i
%include InferenceResult.i
%include InferenceAnalysis.i
%include CopulaInferenceSetResult.i
%include CopulaInferenceResult.i
%include CopulaInferenceAnalysis.i
%include OptimizationAnalysis.i
%include MultiObjectiveOptimizationAnalysis.i
%include CalibrationAnalysisResult.i
%include CalibrationAnalysis.i
%include StudyImplementation.i
%include Study.i
%include CouplingInputFile.i
%include CouplingResourceFile.i
%include CouplingOutputFile.i
%include CouplingStep.i
%include CouplingPhysicalModel.i
%include DataCleaning.i
#ifdef PERSALYS_HAVE_YDEFX
%include YACSPhysicalModel.i
%include YACSCouplingPhysicalModel.i
#endif
%include FileMemoizeEvaluation.i
%include FileMemoizeFunction.i
%include AnsysParser.i
%include DataSensitivityAnalysisResult.i
%include DataSensitivityAnalysis.i
