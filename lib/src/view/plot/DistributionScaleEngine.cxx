#include "otgui/DistributionScaleEngine.hxx"
#include <cmath>
#include "qwt_math.h"

namespace OTGUI {

DistributionScaleEngine::DistributionScaleEngine()
  : QwtLinearScaleEngine()
{
}

void DistributionScaleEngine::autoScale(int maxNumSteps, double & x1, double & x2, double & stepSize) const
{
  QwtInterval interval(x1, x2);
  interval = interval.normalized();

  // qwt version problem
#if (QWT_VERSION >= 0x050200)
  interval.setMinValue(interval.minValue() - lowerMargin());
  interval.setMaxValue(interval.maxValue() + upperMargin());
#else
  interval.setMinValue(interval.minValue() - loMargin());
  interval.setMaxValue(interval.maxValue() + hiMargin());
#endif

  if (testAttribute(QwtScaleEngine::Symmetric))
    interval = interval.symmetrize(reference());

  if (testAttribute(QwtScaleEngine::IncludeReference))
    interval = interval.extend(reference());

  if (interval.width() == 0.0)
    interval = buildInterval(interval.minValue());

  stepSize = divideInterval(interval.width(), maxNumSteps);

  if (!testAttribute(QwtScaleEngine::Floating))
    interval = aligne(interval, stepSize, maxNumSteps);

  x1 = interval.minValue();
  x2 = interval.maxValue();

  if (testAttribute(QwtScaleEngine::Inverted))
  {
    qSwap(x1, x2);
    stepSize = -stepSize;
  }
}


static double ceil125(double x)
{
  if ( x == 0.0 )
    return 0.0;

  const double sign = ( x > 0 ) ? 1.0 : -1.0;
  const double lx = log10( fabs( x ) );
  const double p10 = floor( lx );

  double fr = qPow( 10.0, lx - p10 );
  if ( fr <= 1.0 )
    fr = 1.0;
  else if ( fr <= 2.0 )
    fr = 2.0;
  else if ( fr <= 5.0 )
    fr = 5.0;
  else
    fr = 10.0;

  return sign * fr * qPow( 10.0, p10 );
}


QwtInterval DistributionScaleEngine::aligne(const QwtInterval & interval, const double & stepSize, const int & maxNumSteps) const
{
  double max, min;
  double width = interval.width();
  if (testAttribute(QwtScaleEngine::Symmetric))
  {
    double delta = maxNumSteps * stepSize - width;
    min = interval.minValue() - delta / 2;
    max = min + maxNumSteps * stepSize;
  }
  else
  {
    double delta = maxNumSteps * stepSize - width;
    max = ceil125(interval.maxValue());
    min = interval.minValue() - (delta - (max - interval.maxValue()));
  }
  return QwtInterval(min, max);
}

}
