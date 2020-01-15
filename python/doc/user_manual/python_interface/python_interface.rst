====================================
User manual for the Python interface
====================================

Model creation
--------------

.. currentmodule:: persalys

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
    MetaModel

Coupling model
--------------

.. currentmodule:: persalys

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

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ModelEvaluation
    FieldModelEvaluation

Optimization
------------

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OptimizationAnalysis

.. only:: otmorris

    Screening
    ---------

    .. currentmodule:: persalys

    .. autosummary::
        :toctree: _generated/
        :template: class.rst_t

        MorrisAnalysis
        MorrisResult

Calibration
-----------

.. currentmodule:: persalys

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

.. currentmodule:: persalys

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

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SobolAnalysis
    SobolResult
    SRCAnalysis
    SRCResult

Reliability model
-----------------

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LimitState

Reliability analysis
--------------------

.. currentmodule:: persalys

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

.. currentmodule:: persalys

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

.. currentmodule:: persalys

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

.. currentmodule:: persalys

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

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Study

Function caching
----------------

.. currentmodule:: persalys

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FileMemoizeFunction
