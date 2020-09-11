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

%import base_module.i
%import uncertainty_module.i


OTDefaultCollectionConvertFunctions(DistributionFactory)


// define PERSALYSTypedInterfaceObjectHelper
%define PERSALYSTypedInterfaceObjectImplementationHelper(Interface,Implementation)

%template(Implementation ## TypedInterfaceObject)           OT::TypedInterfaceObject<PERSALYS::Implementation>;

%typemap(in) const PERSALYS::Interface &
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0)))
  {
    // From interface class, ok
  }
  else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIGTYPE_p_PERSALYS__ ## Implementation, 0)))
  {
    // From Implementation*
    PERSALYS::Implementation * p_impl = reinterpret_cast< PERSALYS::Implementation * >( ptr );
    $1 = new PERSALYS::Interface( &*p_impl );
  }
  else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Pointer<PERSALYS::Implementation> *"), 0)))
  {
    // From Pointer<Implementation>
    OT::Pointer<PERSALYS::Implementation> * p_impl = reinterpret_cast< OT::Pointer<PERSALYS::Implementation> * >( ptr );
    $1 = new PERSALYS::Interface( **p_impl );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PERSALYS::Interface &
{
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIGTYPE_p_PERSALYS__ ## Implementation, 0))
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Pointer<PERSALYS::Implementation> *"), 0));
}
%enddef

%define PERSALYSTypedInterfaceObjectHelper(Interface)
PERSALYSTypedInterfaceObjectImplementationHelper(Interface,Interface ## Implementation)
%enddef


// define PERSALYSTypedCollectionInterfaceObjectHelper
%define PERSALYSTypedCollectionInterfaceObjectMisnamedHelper(Interface,CollectionType)

%template(CollectionType)           OT::Collection<PERSALYS::Interface>;

%typemap(in) const CollectionType & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    $1 = OT::buildCollectionFromPySequence< PERSALYS::Interface >( $input );
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CollectionType & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::Interface >( $input );
}

%apply const CollectionType & { const OT::Collection<PERSALYS::Interface> & };

%enddef

%define PERSALYSTypedCollectionInterfaceObjectHelper(Interface)
PERSALYSTypedCollectionInterfaceObjectMisnamedHelper(Interface,Interface ## Collection)
%enddef



// The new classes
%include persalys/PersalysPrivate.hxx
%include Observer.i
%include Observable.i

%include Variable.i
%include Input.i
%include Output.i
%include DataSample.i
%include DataImport.i
%include MeshModelImplementation.i
%include MeshModel.i
%include GridMeshModel.i
%include ImportedMeshModel.i
%include PhysicalModelImplementation.i
%include SymbolicPhysicalModel.i
%include PythonPhysicalModel.i
%include MetaModel.i
%include PhysicalModel.i
#ifdef PERSALYS_HAVE_OTFMI
%include FMUInfo.i
%include FMIPhysicalModel.i
#endif
%include SymbolicFieldModel.i
%include PythonFieldModel.i
%include LimitStateImplementation.i
%include LimitState.i
%include AnalysisResult.i
%include AnalysisImplementation.i
%include PhysicalModelAnalysis.i
%include Analysis.i
%include WithStopCriteriaAnalysis.i
%include DesignOfExperimentImplementation.i
%include DesignOfExperiment.i
%include DataModel.i
%include Observations.i
%include EvaluationResult.i
%include DataAnalysisResult.i
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
%include TaylorExpansionMomentsResult.i
%include MonteCarloAnalysis.i
%include FieldMonteCarloResult.i
%include FieldMonteCarloAnalysis.i
%include TaylorExpansionMomentsAnalysis.i
%include SobolResult.i
%include SobolAnalysis.i
%include SRCResult.i
%include SRCAnalysis.i
#ifdef PERSALYS_HAVE_OTMORRIS
%include MorrisResult.i
%include MorrisAnalysis.i
#endif
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
%include FittingTestResult.i
%include InferenceResult.i
%include InferenceAnalysis.i
%include CopulaInferenceSetResult.i
%include CopulaInferenceResult.i
%include CopulaInferenceAnalysis.i
%include OptimizationAnalysis.i
%include CalibrationAnalysisResult.i
%include CalibrationAnalysis.i
%include StudyImplementation.i
%include Study.i
%include CouplingInputFile.i
%include CouplingResourceFile.i
%include CouplingOutputFile.i
%include CouplingStep.i
%include CouplingPhysicalModel.i
%include DataCleaningTools.i
#ifdef PERSALYS_HAVE_YACS
%include YACSPhysicalModel.i
%include YACSCouplingPhysicalModel.i
#endif
%include FileMemoizeEvaluation.i
%include FileMemoizeFunction.i
