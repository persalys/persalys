//                                               -*- C++ -*-
/**
 *  @brief Buttons group to check a physical model
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

#include "persalys/CheckModelButtonGroup.hxx"

#include <QHBoxLayout>
#include <QPushButton>

namespace PERSALYS
{

CheckModelButtonGroup::CheckModelButtonGroup(QWidget * parent, bool isDifferentiable)
  : QWidget(parent)
  , isDifferentiable_(isDifferentiable)
{
  auto * buttonLayout = new QHBoxLayout(this);
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  errorMessageWidget_ = new ErrorWidget;
  errorMessageWidget_->forceVisible(true);
  errorMessageWidget_->usePadding(false);
  buttonLayout->addWidget(errorMessageWidget_);
  buttonLayout->setStretch(0, 1);

  // button Evaluate outputs -------------------------------------------
  auto * evaluateOutputsButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Evaluate model"));
  evaluateOutputsButton->setToolTip(tr("Evaluate the outputs"));
  connect(evaluateOutputsButton, &QPushButton::clicked, this, &CheckModelButtonGroup::evaluateOutputsRequested);
  buttonLayout->addWidget(evaluateOutputsButton);

  // button Evaluate gradient -------------------------------------------
  if (isDifferentiable)
  {
    auto * evaluateGradientButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Evaluate gradient"));
    evaluateGradientButton->setToolTip(tr("Evaluate the gradient"));
    connect(evaluateGradientButton, &QPushButton::clicked, this, &CheckModelButtonGroup::evaluateGradientRequested);
    buttonLayout->addWidget(evaluateGradientButton);

    connect(this, &CheckModelButtonGroup::evaluateGradientRequested, errorMessageWidget_, &ErrorWidget::reset);
  }

  connect(this, &CheckModelButtonGroup::evaluateOutputsRequested, errorMessageWidget_, &ErrorWidget::reset);
}

}
