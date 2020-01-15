//                                               -*- C++ -*-
/**
 *  @brief ADAO Calibration backend
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/AdaoCalibration.hxx"
#include "persalys/InterpreterUnlocker.hxx"

#include <openturns/Dirac.hxx>
#include <openturns/Normal.hxx>
#include <openturns/ParametricEvaluation.hxx>

#include "AdaoExchangeLayer.hxx"
#include "AdaoExchangeLayerException.hxx"
#include "AdaoModelKeyVal.hxx"

#include "py2cpp/py2cpp.hxx"

using namespace OT;
using PERSALYS::InterpreterUnlocker;

CLASSNAMEINIT(PERSALYS::AdaoCalibration)

class MyDefaultNonPythonVisitor : public AdaoModel::RecursiveVisitor
{
public:
  MyDefaultNonPythonVisitor(AdaoModel::EnumAlgo algo, double observationErrorScalar):_algo(algo),_observationErrorScalar(observationErrorScalar) { }
  void visit(AdaoModel::GenericKeyVal *obj) override
  {
    AdaoModel::EnumAlgoKeyVal *objc(dynamic_cast<AdaoModel::EnumAlgoKeyVal *>(obj));
    if(objc)
      {
        objc->setVal(_algo);
        return ;
      }
    if( obj->getKey() == "ScalarSparseMatrix" && _subdir->getKey() == "ObservationError" )
      {
        AdaoModel::DoubleKeyVal *objc( dynamic_cast<AdaoModel::DoubleKeyVal *>(obj) );
        if(objc)
          objc->setVal(_observationErrorScalar);
      }
  }
  void enterSubDir(AdaoModel::DictKeyVal *subdir) override { _subdir = subdir ; }
  void exitSubDir(AdaoModel::DictKeyVal *subdir) override { }
private:
  AdaoModel::EnumAlgo _algo;
  double _observationErrorScalar;
  AdaoModel::DictKeyVal *_subdir = nullptr;
};

class MyDefaultVisitor : public AdaoModel::PythonLeafVisitor
{
public:
  MyDefaultVisitor(PyObject *context, Point thetaPrior, Sample modelObservations):_context(context)
  {
    std::vector< double > Xb(thetaPrior.begin(),thetaPrior.end());
    py2cpp::PyPtr XbPy(py2cpp::toPyPtr(Xb));
    _Xb = XbPy.get();
    Py_XINCREF(_Xb);
    Point modelObs(modelObservations.asPoint());
    std::vector<double> observation(modelObs.begin(),modelObs.end());
    py2cpp::PyPtr observationPy(py2cpp::toPyPtr(observation));
    _observation = observationPy.get();
    Py_XINCREF(_observation);
  }
  
  void visit(AdaoModel::MainModel *godFather, AdaoModel::PyObjKeyVal *obj) override
  {
    visitImpl(godFather,obj);
  }

  bool visitImpl(AdaoModel::MainModel *godFather, AdaoModel::PyObjKeyVal *obj)
  {
    if(godFather->findPathOf(obj)=="Background/Vector")
      {
        std::ostringstream oss; oss << "___" << _cnt++;
        std::string varname(oss.str());
        obj->setVal(_Xb);
        PyDict_SetItemString(_context,varname.c_str(),_Xb);
        obj->setVarName(varname);
        return true;
      }
    if(godFather->findPathOf(obj)=="Observation/Vector")
      {
        std::ostringstream oss; oss << "___" << _cnt++;
        std::string varname(oss.str());
        obj->setVal(_observation);
        PyDict_SetItemString(_context,varname.c_str(),_observation);
        obj->setVarName(varname);
        return true;
      }
    if(godFather->findPathOf(obj)=="ObservationError/Matrix")
      {
        std::ostringstream oss; oss << "___" << _cnt++;
        std::string varname(oss.str());
        PyObject *tmp(PyFloat_FromDouble(1.0));
        obj->setVal(tmp);
        PyDict_SetItemString(_context,varname.c_str(),tmp);
        obj->setVarName(varname);
        return true;
      }
    return false;
  }
protected:
  unsigned int _cnt = 0;
  PyObject *_context = nullptr;
private:
  PyObject *_Xb = nullptr;
  PyObject *_observation = nullptr;
};

class VisitorLinear : public MyDefaultVisitor
{
public:
  VisitorLinear(PyObject *context, const std::vector< std::vector<double> >& gradient, Point thetaPrior, Sample modelObservations):MyDefaultVisitor(context,thetaPrior,modelObservations)
  {
    py2cpp::PyPtr gradientPy(py2cpp::toPyPtr(gradient));
    _matrixObservationOperator = gradientPy.get();
    Py_XINCREF(_matrixObservationOperator);
  }
  void visit(AdaoModel::MainModel *godFather, AdaoModel::PyObjKeyVal *obj) override
  {
    if(this->visitImpl(godFather,obj))
      return ;
    if(godFather->findPathOf(obj)=="ObservationOperator/Matrix")
      {
        std::ostringstream oss; oss << "___" << _cnt++;
        std::string varname(oss.str());
        obj->setVal(_matrixObservationOperator);
        PyDict_SetItemString(_context,varname.c_str(),_matrixObservationOperator);
        obj->setVarName(varname);
      }
    if(godFather->findPathOf(obj)=="ObservationOperator/OneFunction")
      {
        obj->setVal(nullptr);
        obj->setVarName(std::string());
      }
  }
private:
  PyObject *_matrixObservationOperator = nullptr;
};
  
PyObjectRAII NumpyToListWaitingForPy2CppManagement(PyObject *npObj)
{
  PyObjectRAII func(PyObjectRAII::FromNew(PyObject_GetAttrString(npObj,"tolist")));
  if(func.isNull())
    throw AdaoExchangeLayerException("input pyobject does not have tolist attribute !");
  PyObjectRAII args(PyObjectRAII::FromNew(PyTuple_New(0)));
  PyObjectRAII ret(PyObjectRAII::FromNew(PyObject_CallObject(func,args)));
  return ret;
}

class LinearEvaluation2 : public EvaluationImplementation
{
 public:
  LinearEvaluation2(const Function &paramFunction, UnsignedInteger inputDimension, const Sample& outputObservations):paramFunction_(paramFunction),inputDimension_(inputDimension),outputObservations_(outputObservations) { }
  virtual LinearEvaluation2 * clone() const { return  new LinearEvaluation2(*this); }
  virtual Point operator() (const Point & inP) const
  {
  Point outputPredictions(paramFunction_(inP));
  Point delta( outputPredictions-this->outputObservations_.asPoint() );
  callsNumber_.increment();
  return delta;
  }
  virtual Sample operator() (const Sample & inS) const
  {
  Sample outputPredictions(paramFunction_(inS));
  Sample ret;
  for(UnsignedInteger i = 0; i< inS.getDimension() ; i++)
    {
      Point a = outputPredictions[i];
      Point delta = a - this->outputObservations_.asPoint();
      ret.add(delta);
    }
  return ret;
  }
  virtual UnsignedInteger getInputDimension() const override { return this->inputDimension_; }
  virtual UnsignedInteger getOutputDimension() const override { return this->outputObservations_.getDimension(); }
 private:
  LinearEvaluation2(const LinearEvaluation2& other) = default;
 private:
  Function paramFunction_;
  UnsignedInteger inputDimension_;
  Sample outputObservations_;
};

class LinearFunction2 : public Function
{
public:
  LinearFunction2(const Function &paramFunction, UnsignedInteger inputDimension, const Sample& outputObservations):Function(LinearEvaluation2(paramFunction,inputDimension,outputObservations)) { }
};

AdaoModel::EnumAlgo FromPersalysAlgoStrToADAOEnum(const std::string& persalysAlgo)
{
  if(persalysAlgo == "LeastSquaresLinear")
    return AdaoModel::EnumAlgo::LinearLeastSquares;
  if(persalysAlgo == "LeastSquaresNonlinear")
    return AdaoModel::EnumAlgo::NonLinearLeastSquares;
  if(persalysAlgo == "GaussianLinear")
    return AdaoModel::EnumAlgo::Blue;
  if(persalysAlgo == "GaussianNonlinear")
    return AdaoModel::EnumAlgo::ThreeDVar;
  throw InvalidArgumentException(HERE) << "Only LeastSquaresNonlinear, GaussianLinear, GaussianNonlinear are implemented for ADAO ! Not recognized \"" << persalysAlgo << "\"";
}

/*!
 * Function called inside iteration loop
 * 
 * \param [in] listOfParamsToCalibrate - iterable python object containing list of input parameter candidate list to calibrate
 */
PyObject *EvaluateSamples(const std::vector<double>& inputObservation, PyObject *listOfParamsToCalibrate, const Function &paramFunction)
{
  // Gather convolution of inputObservation and listOfParamsToCalibrate into a unique Sample object for evaluation to maximize parallelism
  std::vector< std::vector<double> > vects;
  {
    InterpreterUnlocker agil;
    PyObjectRAII iterator(PyObjectRAII::FromNew(PyObject_GetIter(listOfParamsToCalibrate)));
    if(iterator.isNull())
      throw AdaoExchangeLayerException("Input object is not iterable !");
    PyObject *item(nullptr);
    PyObjectRAII numpyModule(PyObjectRAII::FromNew(PyImport_ImportModule("numpy")));
    if(numpyModule.isNull())
      throw AdaoExchangeLayerException("Failed to load numpy");
    PyObjectRAII ravelFunc(PyObjectRAII::FromNew(PyObject_GetAttrString(numpyModule,"ravel")));
    while ((item = PyIter_Next(iterator)))
      {
        PyObjectRAII item2(PyObjectRAII::FromNew(item));
        {
          PyObjectRAII args(PyObjectRAII::FromNew(PyTuple_New(1)));
          PyTuple_SetItem(args,0,item2.retn());
          PyObjectRAII npyArray(PyObjectRAII::FromNew(PyObject_CallObject(ravelFunc,args)));
          // Waiting management of npy arrays into py2cpp
          PyObjectRAII lolistFunc(PyObjectRAII::FromNew(PyObject_GetAttrString(npyArray,"tolist")));
          PyObjectRAII listPy;
          {
            PyObjectRAII args2(PyObjectRAII::FromNew(PyTuple_New(0)));
            listPy=PyObjectRAII::FromNew(PyObject_CallObject(lolistFunc,args2));
          }
          std::vector<double> vect;
          {
            py2cpp::PyPtr listPy2(listPy.retn());
            py2cpp::fromPyPtr(listPy2,vect);
          }
          vects.emplace_back( std::move(vect) );
        }
      }
  }
  // explode paramFunction to get access of all arguments whatever their nature : parameter/variable
  Evaluation p0(paramFunction.getEvaluation());
  ParametricEvaluation *p0c( dynamic_cast<ParametricEvaluation * >(p0.getImplementation().get()) );
  Function zeFunc(p0c->getFunction());
  Sample samplesToEvaluate(vects.size()*inputObservation.size(),zeFunc.getInputDimension());
  Indices inputPositions(p0c->getInputPositions()),paramPositions(p0c->getParametersPositions());
  UnsignedInteger paramPositionsLen(paramPositions.getSize());
  if( inputPositions.getSize()!=1 )
    throw InvalidArgumentException(HERE) << " input position size is expected to be of size 1 !";
  std::size_t i(0);
  for(const auto& vect : vects)
    {
      if(paramPositionsLen!=vect.size())
        throw InvalidArgumentException(HERE) << " param positions in Persalys mismatch those in ADAO !";
      for(const auto& inpo : inputObservation)
        {
          samplesToEvaluate(i,inputPositions[0]) = inpo;
          for(UnsignedInteger k = 0 ; k < paramPositionsLen ; k++)
            samplesToEvaluate(i,paramPositions[k]) = vect[k];
          i++;
        }
    }
  ///// Evaluation is here !
  Sample resuEvaluated( zeFunc(samplesToEvaluate) );
  ///// Sample -> PyObject *
  PyObject *ret(nullptr);
  {
    InterpreterUnlocker agil;
    std::size_t len(vects.size());
    ret = PyList_New(len);
    std::size_t k(0);
    for(std::size_t i=0;i<len;++i)
      {
        PyObjectRAII row(PyObjectRAII::FromNew(PyList_New(inputObservation.size())));
        for(std::size_t j=0;j<inputObservation.size();j++)
          {
            PyList_SetItem(row,j,PyFloat_FromDouble( resuEvaluated(k++,0) ));
          }
        PyList_SetItem(ret,i,row.retn());
      }
  }
  return ret;
}

/* Constructor with parameters */
PERSALYS::AdaoCalibration::AdaoCalibration(const String & methodName,
                                           const Function & model,
                                           const Sample & inputObservations,
                                           const Sample & outputObservations,
                                           const Point & candidate,
                                           const CovarianceMatrix & parameterCovariance,
                                           const CovarianceMatrix & errorCovariance)
  : CalibrationAlgorithmImplementation(outputObservations, Normal(candidate, parameterCovariance))
  , methodName_(methodName)
  , candidate_(candidate)
  , model_(model)
  , inputObservations_(inputObservations)
  , outputObservations_(outputObservations)
  , errorCovariance_(errorCovariance)
{
}

void PERSALYS::AdaoCalibration::run()
{
  const Point thetaPrior(candidate_);
  
  Point inputSample(inputObservations_.asPoint());
  std::vector<double> inputObservation(inputSample.begin(),inputSample.end());
  AdaoModel::MainModel mm;
  AdaoExchangeLayer adao;
  adao.init();
  // Put into mm ADAO model the callback function
  adao.setFunctionCallbackInModel(&mm);
  // Put into mm ADAO model Background/Vector, Observation/Vector...
  this->feedADAOModel(model_,adao,mm);
  // Put into mm ADAO model algorithm type and observation error
  MyDefaultNonPythonVisitor visitor( FromPersalysAlgoStrToADAOEnum(methodName_), getObservationErrorScalar() );
  mm.visitAll(&visitor);
  // At this point mm ADAO model is ready to be dumped into python and evaluated
  /*{
    InterpreterUnlocker agil;
    std::string sciptPyOfModelMaker(mm.pyStr());
    std::cerr << adao.printContext() << std::endl;
    std::cerr << sciptPyOfModelMaker << std::endl;
    }*/
  adao.loadTemplate(&mm);
  adao.execute();
  PyObject *listOfElts( nullptr );
  while( adao.next(listOfElts) )
    {
      PyObject *resultOfChunk(EvaluateSamples(inputObservation,listOfElts,model_));
      adao.setResult(resultOfChunk);
    }
  std::vector<double> vect;
  PyObject *res(adao.getResult());
  {
    InterpreterUnlocker agil;
    PyObjectRAII optimum(PyObjectRAII::FromNew(res));
    PyObjectRAII optimum_4_py2cpp(NumpyToListWaitingForPy2CppManagement(optimum));
    {
      py2cpp::PyPtr obj(optimum_4_py2cpp);
      py2cpp::fromPyPtr(obj,vect);
    }
  }
  Point parameterMap( Collection<double>(vect.begin(),vect.end()) );
  parameterMap = this->postProcessResult(parameterMap);
  std::for_each(parameterMap.begin(),parameterMap.end(),[](const double& elt) { std::cerr << elt << ", "; });
  //End of first
  Dirac parameterPriorDistribution( thetaPrior );
  parameterPriorDistribution.setDescription( model_.getParameterDescription() );
  Dirac parameterPosteriorDistribution( Point( Collection<double>(vect.begin(),vect.end() ) ) );
  parameterPosteriorDistribution.setDescription( model_.getParameterDescription() );
  Normal observationsError = this->getObservationsError();
  Sample outputObservations( outputObservations_ );
  LinearFunction2 residualFunction( model_, parameterMap.getDimension(), outputObservations );
  result_ = CalibrationResult(parameterPriorDistribution, parameterPosteriorDistribution, parameterMap, observationsError, outputObservations, residualFunction);
}

OT::Point PERSALYS::AdaoCalibration::postProcessResult(const OT::Point &resu)
{
  if( methodName_ != "LeastSquaresLinear")
    return resu;
  else
    {
      const Point thetaPrior(candidate_);
      return thetaPrior+resu;
    }
}
  
void PERSALYS::AdaoCalibration::feedADAOModel(const OT::Function &paramFunction, AdaoExchangeLayer& adao, AdaoModel::MainModel& mm)
{
  InterpreterUnlocker agil;
  const Point thetaPrior(candidate_);
  if( methodName_ != "LeastSquaresLinear")
    {
      MyDefaultVisitor visitorPythonObj(adao.getPythonContext(),thetaPrior,outputObservations_);
      mm.visitPythonLeaves(&visitorPythonObj);
    }
  else
    {
      Function parametrizedModel(paramFunction);
      parametrizedModel.setParameter(thetaPrior);
      const UnsignedInteger parameterDimension = thetaPrior.getDimension();
      const UnsignedInteger outputDimension = outputObservations_.getDimension();
      const UnsignedInteger size = outputObservations_.getSize();
      Matrix gradientObservations = MatrixImplementation(parameterDimension, size * outputDimension);
      UnsignedInteger shift = 0;
      UnsignedInteger skip = parameterDimension * outputDimension;
      for(UnsignedInteger i = 0; i < size; ++i)
        {
          const Matrix parameterGradient(parametrizedModel.parameterGradient( inputObservations_[i] ));
          std::copy(parameterGradient.getImplementation()->begin(), parameterGradient.getImplementation()->end(), gradientObservations.getImplementation()->begin() + shift);
          shift += skip;
        }
      gradientObservations = gradientObservations.transpose();
      std::vector< std::vector<double> > gradientObservationsBase(size);
      for(UnsignedInteger i = 0; i < size; ++i)
        {
          std::vector<double>& row(gradientObservationsBase[i]);
          row.resize(parameterDimension);
          for(UnsignedInteger j = 0; j < parameterDimension; ++j)
            row[j]=gradientObservations(i,j);
        }
      // overload output samples

      Sample outputSample( outputObservations_ );
      Sample outputWithDft(parametrizedModel( inputObservations_ ));
      Sample outputResidual( outputSample-outputWithDft );
      //
      VisitorLinear visitorPythonObj(adao.getPythonContext(),gradientObservationsBase,thetaPrior,outputResidual);
      mm.visitPythonLeaves(&visitorPythonObj);
    }
}
  
OT::Normal PERSALYS::AdaoCalibration::getObservationsError()
{
  if ( methodName_ == "LeastSquaresNonlinear" || methodName_ == "LeastSquaresLinear" )
    {
      return OT::Normal(1);
    }
  if ( methodName_ == "GaussianLinear" || methodName_ == "GaussianNonlinear" )
    {
      UnsignedInteger dimError = errorCovariance_.getDimension();
      OT::Normal observationsError = OT::Normal( Point(dimError), errorCovariance_ );
      return observationsError;
    }
  throw InvalidArgumentException(HERE) << "Only LeastSquaresNonlinear, LeastSquaresLinear, GaussianLinear, GaussianNonlinear are implemented for ADAO !";
}

double PERSALYS::AdaoCalibration::getObservationErrorScalar()
{
  if ( methodName_ == "LeastSquaresNonlinear" || methodName_ == "LeastSquaresLinear" )
    {
      return 1.;
    }
  if ( methodName_ == "GaussianLinear" || methodName_ == "GaussianNonlinear" )
    {
      return errorCovariance_(0,0);
    }
  throw InvalidArgumentException(HERE) << "Only LeastSquaresNonlinear, LeastSquaresLinear, GaussianLinear, GaussianNonlinear are implemented for ADAO !";
}
