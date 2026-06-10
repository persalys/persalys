//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#include "persalys/FileMemoizeEvaluation.hxx"
#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/Cache.hxx>
#include <openturns/BatchFailedException.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FileMemoizeEvaluation)

static const Factory<FileMemoizeEvaluation> Factory_FileMemoizeEvaluation;

/* Default constructor */
FileMemoizeEvaluation::FileMemoizeEvaluation()
  : EvaluationProxy()
  , isCacheEnabled_(true)
  , hits_(0)
{

}

/* Parameter constructor */
FileMemoizeEvaluation::FileMemoizeEvaluation(const Evaluation & evaluation, const FileName & inputFile, const FileName & outputFile)
  : EvaluationProxy(evaluation)
  , inputFile_(inputFile)
  , outputFile_(outputFile)
  , isCacheEnabled_(!inputFile_.empty() && !outputFile_.empty())
  , hits_(0)
{
  setEvaluation(evaluation);
}

/* Virtual constructor */
FileMemoizeEvaluation * FileMemoizeEvaluation::clone() const
{
  return new FileMemoizeEvaluation(*this);
}

/** Function implementation accessors */
void FileMemoizeEvaluation::setEvaluation(const Evaluation & evaluation)
{
  if (!inputFile_.empty() && !outputFile_.empty())
  {
    try
    {
      Sample inSample(Sample::ImportFromTextFile(inputFile_, ";"));
      if (inSample.getDimension() != evaluation.getInputDimension())
        throw InvalidDimensionException(HERE) << "Input dimension has changed. Try clearing cache.";
      Sample outSample(Sample::ImportFromTextFile(outputFile_, ";"));
      if (outSample.getDimension() != evaluation.getOutputDimension())
        throw InvalidDimensionException(HERE) << "Output dimension has changed. Try clearing cache.";
    }
    catch (const FileNotFoundException &)
    {
      // one of the files do not exist: write empty files
      clearCache();
    }
  }
  evaluation_ = evaluation;

  // disable when the inner evaluation is a FileMemoizeEvaluation too
  FileMemoizeEvaluation * p_FileMemoizeEvaluation = dynamic_cast<FileMemoizeEvaluation*>(evaluation.getImplementation().get());
  if (p_FileMemoizeEvaluation && p_FileMemoizeEvaluation->isCacheEnabled_)
  {
    isCacheEnabled_ = false;
  }
}


void FileMemoizeEvaluation::setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile)
{
  if (inputFile.empty() || outputFile.empty())
  {
    isCacheEnabled_ = false;
    inputFile_ = "";
    outputFile_ = "";
  }
  else
  {
    try
    {
      Sample inSample(Sample::ImportFromTextFile(inputFile, ";"));
      if (inSample.getDimension() != evaluation_.getInputDimension())
        throw InvalidDimensionException(HERE) << "Input dimension has changed. Try clearing cache.";
      Sample outSample(Sample::ImportFromTextFile(outputFile, ";"));
      if (outSample.getDimension() != evaluation_.getOutputDimension())
        throw InvalidDimensionException(HERE) << "Output dimension has changed. Try clearing cache.";
    }
    catch (const FileNotFoundException &)
    {
      // one of the files do not exist: write empty files
      clearCache();
    }
    inputFile_ = inputFile;
    outputFile_ = outputFile;
  }
}

/* Operator () */
Point FileMemoizeEvaluation::operator() (const Point & inPoint) const
{
  return operator()(Sample(1, inPoint))[0];
}

/* Operator () */
Sample FileMemoizeEvaluation::operator() (const Sample & inSample) const
{
  const UnsignedInteger size = inSample.getSize();
  const UnsignedInteger outDim = getOutputDimension();

  Sample outSample;
  if (isCacheEnabled_)
  {
    // read content of cache
    Sample inputCache(Sample::ImportFromTextFile(inputFile_, ";"));
    Sample outputCache(Sample::ImportFromTextFile(outputFile_, ";"));
    typedef PersistentCollection<Scalar>               CacheKeyType;
    typedef PersistentCollection<Scalar>               CacheValueType;
    typedef Cache<CacheKeyType, CacheValueType>        CacheType;
    CacheType initCache(inputCache.getSize());
    for (UnsignedInteger i = 0; i < inputCache.getSize(); ++ i)
      initCache.add(inputCache[i], outputCache[i]);

    outSample = Sample(size, outDim);
    std::set<Point> uniqueValues;
    Indices toDoIndices;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      CacheKeyType inKey(inSample[i].getCollection());
      if (initCache.hasKey(inKey))
      {
        outSample[i] = Point::ImplementationType(initCache.find(inKey));
        ++ hits_;
      }
      else if (!uniqueValues.count(inSample[i]))
      {
        toDoIndices.add(i);
        uniqueValues.insert(inSample[i]);
      }
    }
    Sample toDo(inSample.select(toDoIndices));
    const UnsignedInteger toDoSize = toDo.getSize();

    CacheType tempCache(toDoSize);
    tempCache.enable();
    if (toDoSize > 0)
    {
      try
      {
        const Sample result(evaluation_.operator()(toDo));
        callsNumber_.fetchAndAdd(toDoSize);
        addCacheContent(toDo, result);
        for (UnsignedInteger i = 0; i < toDoSize; ++ i)
          tempCache.add(toDo[i], result[i]);
      }
      catch (const BatchFailedException & exc)
      {
        // rethrow another BatchFailedException, but with updated indices
        // as the original evaluation is called on a subset of the input sample
        callsNumber_.fetchAndAdd(toDoSize);
        const Indices localOkIndices(exc.getSucceededIndices());
        const Indices localFailedIndices(exc.getFailedIndices());
        const Sample localOkSample(exc.getOutputSample());
        const Description localErrorDescription(exc.getErrorDescription());
        Indices failedIndices;
        Description errorDescription;
        Sample okSample(0, outDim);
        okSample.setDescription(evaluation_.getOutputDescription());
        for (UnsignedInteger i = 0; i < localOkIndices.getSize(); ++ i)
          tempCache.add(toDo[localOkIndices[i]], localOkSample[i]);
        std::map<Point, String> failCache;
        for (UnsignedInteger i = 0; i < localFailedIndices.getSize(); ++ i)
          failCache[toDo[localFailedIndices[i]]] = localErrorDescription[i];
        for (UnsignedInteger i = 0; i < size; ++ i)
        {
          const CacheKeyType inKey(inSample[i].getCollection());
          if (initCache.hasKey(inKey))
            okSample.add(outSample[i]);// already retrieved
          else if (tempCache.hasKey(inKey))
            okSample.add(Point::ImplementationType(tempCache.find(inKey)));
          else
          {
            failedIndices.add(i);
            errorDescription.add(failCache[inSample[i]]);
          }
        }
        addCacheContent(toDo.select(localOkIndices), localOkSample);
        const Indices okIndices(failedIndices.complement(size));
        throw BatchFailedException(HERE, failedIndices, errorDescription, okIndices, okSample) << exc.what();
      }
    }
    // Fill all the output values
    for(UnsignedInteger i = 0; i < size; ++ i)
    {
      CacheKeyType inKey(inSample[i].getCollection());
      if (tempCache.hasKey(inKey))
      {
        outSample[i] = Point::ImplementationType(tempCache.find(inKey));
      }
    }
  }
  else
  {
    // Cache not enabled
    outSample = evaluation_.operator()(inSample);
    callsNumber_.fetchAndAdd(size);
  }

  outSample.setDescription(evaluation_.getOutputDescription());
  return outSample;
}

/* Enable or disable the internal cache */
void FileMemoizeEvaluation::enableCache()
{
  isCacheEnabled_ = true;
}

void FileMemoizeEvaluation::disableCache()
{
  isCacheEnabled_ = false;
}

Bool FileMemoizeEvaluation::isCacheEnabled() const
{
  return isCacheEnabled_;
}

UnsignedInteger FileMemoizeEvaluation::getCacheHits() const
{
  return hits_;
}

void FileMemoizeEvaluation::addCacheContent(const Sample& inSample, const Sample& outSample) const
{
  const UnsignedInteger inDim = inSample.getDimension();
  const UnsignedInteger outDim = getOutputDimension();
  const UnsignedInteger size = inSample.getSize();

  // append input points
  const String csvSeparator(";");
  std::ofstream csvFile(inputFile_.c_str(), std::ios_base::app);
  if (csvFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << inputFile_;
  csvFile.imbue(std::locale("C"));
  csvFile.precision(16);
  csvFile << std::scientific;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    csvFile << inSample(i, 0);
    for(UnsignedInteger j = 1; j < inDim; ++ j)
    {
      csvFile << csvSeparator << inSample(i, j);
    } // j
    csvFile << "\n";
  }
  csvFile.close();

  // append output point
  csvFile.open(outputFile_.c_str(), std::ios_base::app);
  if (csvFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << outputFile_;
  csvFile.imbue(std::locale("C"));
  csvFile.precision(16);
  csvFile << std::scientific;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    csvFile << outSample(i, 0);
    for(UnsignedInteger j = 1; j < outDim; ++ j)
    {
      csvFile << csvSeparator << outSample(i, j);
    } // j
    csvFile << "\n";
  }
  csvFile.close();
}

Sample FileMemoizeEvaluation::getCacheInput() const
{
  return Sample::ImportFromTextFile(inputFile_, ";");
}

Sample FileMemoizeEvaluation::getCacheOutput() const
{
  return Sample::ImportFromTextFile(outputFile_, ";");
}

void FileMemoizeEvaluation::clearCache() const
{
  if (isCacheEnabled_)
  {
    Sample input(0, evaluation_.getInputDimension());
    input.setDescription(getInputDescription());
    input.exportToCSVFile(inputFile_);

    Sample output(0, evaluation_.getOutputDimension());
    output.setDescription(getOutputDescription());
    output.exportToCSVFile(outputFile_);
  }
  hits_ = 0;
}

Evaluation FileMemoizeEvaluation::getEvaluation() const
{
  return evaluation_;
}

/* Comparison operator */
Bool FileMemoizeEvaluation::operator ==(const FileMemoizeEvaluation & other) const
{
  return evaluation_ == other.evaluation_;
}

/* String converter */
String FileMemoizeEvaluation::__repr__() const
{
  return OSS(true) << evaluation_.getImplementation()->__repr__();
}

String FileMemoizeEvaluation::__str__(const String & offset) const
{
  return OSS(false) << evaluation_.getImplementation()->__str__(offset);
}

OT::FileName FileMemoizeEvaluation::getCacheInputFile() const
{
  return inputFile_;
}

OT::FileName FileMemoizeEvaluation::getCacheOutputFile() const
{
  return outputFile_;
}


/* Method save() stores the object through the StorageManager */
void FileMemoizeEvaluation::save(Advocate & adv) const
{
  EvaluationProxy::save(adv);
  adv.saveAttribute("inputFile_", inputFile_);
  adv.saveAttribute("outputFile_", outputFile_);
  adv.saveAttribute("isCacheEnabled_", isCacheEnabled_);
}

/* Method load() reloads the object from the StorageManager */
void FileMemoizeEvaluation::load(Advocate & adv)
{
  EvaluationProxy::load(adv);
  adv.loadAttribute("inputFile_", inputFile_);
  adv.loadAttribute("outputFile_", outputFile_);
  adv.loadAttribute("isCacheEnabled_", isCacheEnabled_);
}

}
