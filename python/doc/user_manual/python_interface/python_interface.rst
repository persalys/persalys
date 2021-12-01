====================================
User manual for the Python interface
====================================

.. currentmodule:: persalys

Model creation
--------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Variable
    Input
    Output
    MeshModel
    GridMeshModel
    ImportedMeshModel
    PhysicalModel
    SymbolicPhysicalModel
    SymbolicFieldModel
    PythonPhysicalModel
    PythonFieldModel
    DataModel
    DataCleaning
    MetaModel

.. only:: otfmi

    .. autosummary::
        :toctree: _generated/
        :template: class.rst_t

        FMIPhysicalModel

Coupling model
--------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    CouplingInputFile
    CouplingResourceFile
    CouplingOutputFile
    CouplingStep
    CouplingPhysicalModel

Model evaluation
----------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ModelEvaluation
    FieldModelEvaluation

Optimization
------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OptimizationAnalysis

.. only:: otmorris

    Screening
    ---------

    .. autosummary::
        :toctree: _generated/
        :template: class.rst_t

        MorrisAnalysis
        MorrisResult

Calibration
-----------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Observations
    CalibrationAnalysis
    CalibrationAnalysisResult


Probabilistic studies
---------------------

Central tendency
~~~~~~~~~~~~~~~~

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    MonteCarloAnalysis
    FieldMonteCarloAnalysis
    FieldMonteCarloResult
    TaylorExpansionMomentsAnalysis
    TaylorExpansionMomentsResult

Sensitivity analysis
~~~~~~~~~~~~~~~~~~~~

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SobolAnalysis
    SobolResult
    SRCAnalysis
    SRCResult

Reliability model
-----------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LimitState

Reliability analysis
--------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    MonteCarloReliabilityAnalysis
    FORMImportanceSamplingAnalysis
    ImportanceSamplingAnalysis
    FORMAnalysis
    SORMAnalysis
    SimulationReliabilityResult

Design of experiments
---------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DesignOfExperiment
    FixedDesignOfExperiment
    GridDesignOfExperiment
    ImportedDesignOfExperiment
    ProbabilisticDesignOfExperiment

Data analysis
-------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DataAnalysis
    DataAnalysisResult
    InferenceAnalysis
    InferenceResult
    FittingTestResult
    CopulaInferenceAnalysis
    CopulaInferenceResult
    CopulaInferenceSetResult

Metamodel
---------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    MetaModelAnalysis
    FunctionalChaosAnalysis
    KrigingAnalysis
    MetaModelAnalysisResult
    FunctionalChaosAnalysisResult
    KrigingAnalysisResult
    MetaModelValidationResult

Study storage
-------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Study

Function caching
----------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FileMemoizeFunction
