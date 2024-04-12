//                                               -*- C++ -*-
/**
 *  @brief This class is a Function with history of input and output.
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_FILEMEMOIZEFUNCTION_HXX
#define PERSALYS_FILEMEMOIZEFUNCTION_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/FunctionImplementation.hxx>


namespace PERSALYS
{

/**
 * @class FileMemoizeFunction
 *
 * This class is a Function with history of input and output.
 */
class PERSALYS_BASE_API FileMemoizeFunction
  : public OT::FunctionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FileMemoizeFunction();

  /** Constructor with parameters */
  FileMemoizeFunction(const OT::Function & function, const OT::FileName & inputFile, const OT::FileName & outputFile);

  /** Virtual copy constructor */
  FileMemoizeFunction * clone() const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Function implementation accessors */
  void setEvaluation(const OT::Evaluation & evaluation) override;

  /** Comparison operator */
  OT::Bool operator ==(const FileMemoizeFunction & other) const;

  /** Operator () */
  OT::Point operator() (const OT::Point & inPoint) const override;

  /** Operator () */
  OT::Sample operator() (const OT::Sample & inSample) const override;

  /** @brief Enable the internal cache
   *
   * The cache stores previously computed output values, so calling the cache before processing the %Function
   * can save much time and avoid useless computations. However, calling the cache can eat time if the computation is
   * very short. So cache is disabled by default, except when the underlying implementation uses a wrapper.
   *
   * The reason is that building and linking to a wrapper is an extra burden that is valuable only if the computation
   * code is long enough to justify it. Calling the cache in this case will save time for sure.
   */
  void enableCache() const;

  /** @brief Disable the internal cache
   * @see enableCache()
   */
  void disableCache() const;

  /** @brief Test the internal cache activity
   * @see enableCache()
   */
  OT::Bool isCacheEnabled() const;

  /** @brief Returns the number of successful hits in the cache
   */
  OT::UnsignedInteger getCacheHits() const;

  /** @brief Add some content to the cache
   */
  void addCacheContent(const OT::Sample & inSample, const OT::Sample & outSample);

  /** @brief Returns the cache input
   */
  OT::Sample getCacheInput() const;

  /** @brief Returns the cache output
   */
  OT::Sample getCacheOutput() const;

  /** @brief Empty the cache
   */
  void clearCache() const;

  void setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile);
  OT::FileName getCacheInputFile() const;
  OT::FileName getCacheOutputFile() const;
}; /* class FileMemoizeFunction */


}

#endif /* PERSALYS_FILEMEMOIZEFUNCTION_HXX */
