//                                               -*- C++ -*-
/**
 *  @brief Multi-objective optimization of a physical model
 *
 *  Copyright 2015-2022 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PERSALYS_MULTIOBJECTIVEOPTIMIZATIONANALYSIS_HXX
#define PERSALYS_MULTIOBJECTIVEOPTIMIZATIONANALYSIS_HXX

#include "OptimizationAnalysis.hxx"
#include "openturns/Pagmo.hxx"

namespace PERSALYS
{
  class PERSALYS_BASE_API MultiObjectiveOptimizationAnalysisResult : public OT::PersistentObject
  {
    CLASSNAME
      public:
    /** Default constructor */
    MultiObjectiveOptimizationAnalysisResult();

    /** Virtual constructor */
    MultiObjectiveOptimizationAnalysisResult * clone() const override;

    OT::Collection<OT::Sample> getFronts() const {return fronts_;};
    void setFronts(const OT::Collection<OT::Sample>& fronts) {fronts_=fronts;};

    OT::Sample getFinalPop() const {return finalPop_;};
    void setFinalPop(const OT::Sample& finalPop) {finalPop_=finalPop;};

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const override;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv) override;

    /** String converter */
    OT::String __repr__() const override;

  private:
    OT::PersistentCollection<OT::Sample> fronts_;
    OT::Sample finalPop_;

  };

  class PERSALYS_BASE_API MultiObjectiveOptimizationAnalysis : public OptimizationAnalysis
  {
    CLASSNAME
      public:
    /** Default constructor */
    MultiObjectiveOptimizationAnalysis();

    /** Constructor with parameters */
    MultiObjectiveOptimizationAnalysis(const OT::String & name,
                                       const PhysicalModel & physicalModel,
                                       const OT::String & solverName = "nsga2");

    /** Virtual constructor */
    MultiObjectiveOptimizationAnalysis * clone() const override;

    void setInterestVariables(const OT::Description& variablesNames) override;
    void setSolverName(const OT::String& name) override;

    void initialize() override;
    void updateParameters() override;
    bool canBeLaunched(OT::String &errorMessage) const override;

    static OT::Description GetSolverNames(const OT::Interval& bounds,
                                          const OT::Indices& types = OT::Indices());

    /** Initial population size */
    void setPopulationSize(const OT::UnsignedInteger popSize);
    OT::UnsignedInteger getPopulationSize() const;

    /** Number of generations to evolve */
    void setGenerationNumber(const OT::UnsignedInteger generationNumber);
    OT::UnsignedInteger getGenerationNumber() const;

    /** Objectives */
    OT::Description getMinimization() const;
    void setMinimization(const OT::Description& varNames);

    /** Seed */
    void setSeed(const OT::UnsignedInteger & seed) {seed_=seed;};
    OT::UnsignedInteger getSeed() const {return seed_;};

    /** Result **/
    bool hasValidResult() const override;
    MultiObjectiveOptimizationAnalysisResult getResult() const;

    /** Parameters **/
    Parameters getParameters() const override;

    OT::String getPythonScript() const override;

    /** String converter */
    OT::String __repr__() const override;

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const override;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv) override;

  protected:
    void launch() override;

  private:
    OT::UnsignedInteger startingPopSize_ = 50;
    OT::UnsignedInteger generationNumber_ = 10;
    OT::UnsignedInteger seed_ = 0;
    OT::Description areMinimization_;
    MultiObjectiveOptimizationAnalysisResult moResult_;
  };
}

#endif
