//                                               -*- C++ -*-
/**
 * @brief Class for identity evaluation
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

#ifndef PERSALYS_FILEMEMOIZEEVALUATION_HXX
#define PERSALYS_FILEMEMOIZEEVALUATION_HXX

#include "openturns/EvaluationProxy.hxx"
#include "openturns/Evaluation.hxx"

#include "persalys/PersalysPrivate.hxx"
namespace PERSALYS
{

/**
 * @class FileMemoizeEvaluation
 */
class PERSALYS_BASE_API FileMemoizeEvaluation
  : public OT::EvaluationProxy
{
  CLASSNAME
public:

  /** Default constructor */
  FileMemoizeEvaluation();

  /** Parameter constructor */
  FileMemoizeEvaluation(const OT::Evaluation & evaluation, const OT::FileName & inputFile, const OT::FileName & outputFile);

  /** Virtual constructor */
  FileMemoizeEvaluation * clone() const override;

  /** Function implementation accessors */
  void setEvaluation(const OT::Evaluation & evaluation);
  OT::Evaluation getEvaluation() const;

  /** Comparison operator */
  OT::Bool operator ==(const FileMemoizeEvaluation & other) const;

  /** String converter */
  OT::String __repr__() const override;
  OT::String __str__(const OT::String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  OT::Point operator() (const OT::Point & inPoint) const override;

  /** Operator () */
  OT::Sample operator() (const OT::Sample & inSample) const override;

  /** Enable or disable the internal cache */
  void enableCache();
  void disableCache();

  /** @brief Test the internal cache activity
   * @see enableCache()
   */
  OT::Bool isCacheEnabled() const;

  /** @brief Returns the number of successful hits in the cache
   */
  OT::UnsignedInteger getCacheHits() const;

  /** @brief Add some content to the cache
   */
  void addCacheContent(const OT::Sample & inSample, const OT::Sample & outSample) const;

  /** @brief Returns the cache input
   */
  OT::Sample getCacheInput() const;

  /** @brief Returns the cache output
   */
  OT::Sample getCacheOutput() const;

  /** @brief Empty the cache
   */
  void clearCache() const;

  /** Cache files accessors */
  void setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile);
  OT::FileName getCacheInputFile() const;
  OT::FileName getCacheOutputFile() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

private:

  OT::FileName inputFile_;
  OT::FileName outputFile_;
  OT::Bool isCacheEnabled_;
  mutable OT::UnsignedInteger hits_;

}; /* class FileMemoizeEvaluation */


}

#endif /* PERSALYS_FILEMEMOIZEEVALUATION_HXX */
