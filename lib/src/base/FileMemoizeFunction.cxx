//                                               -*- C++ -*-
/**
 *  @brief This class is a Function with history of input and output.
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
#include "persalys/FileMemoizeFunction.hxx"
#include "persalys/FileMemoizeEvaluation.hxx"
#include <openturns/Function.hxx>
#include <openturns/NoGradient.hxx>
#include <openturns/NoHessian.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(FileMemoizeFunction)

static const Factory<FileMemoizeFunction> Factory_FileMemoizeFunction;

/* Default constructor */
FileMemoizeFunction::FileMemoizeFunction ()
  : FunctionImplementation()
{
  // Nothing to do
}


/* Parameter constructor */
FileMemoizeFunction::FileMemoizeFunction (const Function & function, const FileName & inputFile, const FileName & outputFile)
  : FunctionImplementation(new FileMemoizeEvaluation(function.getEvaluation(), inputFile, outputFile),
                           function.getGradient(),
                           function.getHessian())
{
  setUseDefaultGradientImplementation(function.getUseDefaultGradientImplementation());
  setUseDefaultHessianImplementation(function.getUseDefaultHessianImplementation());
}


/* Virtual constructor */
FileMemoizeFunction * FileMemoizeFunction::clone() const
{
  return new FileMemoizeFunction(*this);
}


/* String converter */
String FileMemoizeFunction::__repr__() const
{
  return FunctionImplementation::__repr__();
}

/* Function implementation accessors */
void FileMemoizeFunction::setEvaluation(const Evaluation & evaluation)
{
  // Ensure that p_evaluation_ always points to an FileMemoizeEvaluation
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->setEvaluation(evaluation);
}

/* Enable or disable the internal cache */
void FileMemoizeFunction::enableCache() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->enableCache();
}

void FileMemoizeFunction::disableCache() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->disableCache();
}

Bool FileMemoizeFunction::isCacheEnabled() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->isCacheEnabled();
}

UnsignedInteger FileMemoizeFunction::getCacheHits() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheHits();
}

void FileMemoizeFunction::addCacheContent(const Sample & inSample, const Sample & outSample)
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->addCacheContent(inSample, outSample);
}

Sample FileMemoizeFunction::getCacheInput() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheInput();
}

Sample FileMemoizeFunction::getCacheOutput() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheOutput();
}

void FileMemoizeFunction::clearCache() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->clearCache();
}

/* Comparison operator */
Bool FileMemoizeFunction::operator ==(const FileMemoizeFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

void FileMemoizeFunction::setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile)
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  p_evaluation->setCacheFiles(inputFile, outputFile);
}

OT::FileName FileMemoizeFunction::getCacheInputFile() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheInputFile();
}

OT::FileName FileMemoizeFunction::getCacheOutputFile() const
{
  FileMemoizeEvaluation* p_evaluation = dynamic_cast<FileMemoizeEvaluation*>(getEvaluation().getImplementation().get());
  return p_evaluation->getCacheOutputFile();
}

}
