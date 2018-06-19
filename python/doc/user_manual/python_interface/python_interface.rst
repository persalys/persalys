====================================
User manual for the Python interface
====================================

Model creation
--------------

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

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

Model evaluation
----------------

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ModelEvaluation
    FieldModelEvaluation

Optimization
------------

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OptimizationAnalysis

.. only:: otmorris

    Screening
    ---------

    .. currentmodule:: otguibase

    .. autosummary::
        :toctree: _generated/
        :template: class.rst_t

        MorrisAnalysis
        MorrisResult

Probabilistic studies
---------------------

Central tendency
~~~~~~~~~~~~~~~~

.. currentmodule:: otguibase

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

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SobolAnalysis
    SobolResult
    SRCAnalysis
    SRCResult

Reliability model
-----------------

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LimitState

Reliability analysis
--------------------

.. currentmodule:: otguibase

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

.. currentmodule:: otguibase

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

.. currentmodule:: otguibase

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

.. currentmodule:: otguibase

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

.. currentmodule:: otguibase

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Study
