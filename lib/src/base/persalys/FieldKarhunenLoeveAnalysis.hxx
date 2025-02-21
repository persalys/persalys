//                                               -*- C++ -*-
/**
 *  @brief Compute the KL decomposition of a field model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_FIELDKARHUNENLOEVEANALYSIS_HXX
#define PERSALYS_FIELDKARHUNENLOEVEANALYSIS_HXX

#include "AnalysisImplementation.hxx"
#include "FieldMonteCarloResult.hxx"
#include "DataFieldModel.hxx"

namespace PERSALYS
{
  class PERSALYS_BASE_API FieldKarhunenLoeveAnalysis : public AnalysisImplementation
  {
    CLASSNAME
      friend class FieldMonteCarloAnalysis;

  public:
    /** Default constructor */
    FieldKarhunenLoeveAnalysis();

    /** Constructor with parameters */
    FieldKarhunenLoeveAnalysis(const OT::String & name);
    FieldKarhunenLoeveAnalysis(const OT::String & name, const DataFieldModel& dataModel);
    /** Virtual constructor */
    FieldKarhunenLoeveAnalysis * clone() const override;

    Observer * getParentObserver() const override;

    void launch() override;

    DataFieldModel getDataFieldModel() const {return model_;};

    /* Threshold accessors */
    OT::Scalar getKarhunenLoeveThreshold() const;
    void setKarhunenLoeveThreshold(const OT::Scalar threshold);

    /* Quantile probability accessors */
    OT::Scalar getQuantileLevel() const;
    void setQuantileLevel(const OT::Scalar proba);

    FieldMonteCarloResult getResult() const;
    bool hasValidResult() const override;

    OT::String getPythonScript() const override;

    /** String converter */
    OT::String __repr__() const override;

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const override;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv) override;

  private:
    DataFieldModel model_;
    OT::Scalar karhunenLoeveThreshold_ = 1.e-5;
    OT::Scalar quantileLevel_ = 0.05;
    FieldMonteCarloResult result_;

  };
}
#endif
