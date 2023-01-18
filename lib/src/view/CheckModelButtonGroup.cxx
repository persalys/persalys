//                                               -*- C++ -*-
/**
 *  @brief Buttons group to check a physical model
 *
 *  Copyright 2015-2023 EDF-Phimeca
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

  CheckModelButtonGroup::CheckModelButtonGroup(QWidget * parent)
    : QWidget(parent)
  {
    // button Evaluate outputs -------------------------------------------
    QPushButton * evaluateOutputsButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Evaluate model"));
    evaluateOutputsButton->setToolTip(tr("Evaluate the outputs"));
    connect(evaluateOutputsButton, SIGNAL(clicked()), this, SIGNAL(evaluateOutputsRequested()));

    // button Evaluate gradient -------------------------------------------
    QPushButton * evaluateGradientButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Evaluate gradient"));
    evaluateGradientButton->setToolTip(tr("Evaluate the gradient"));
    connect(evaluateGradientButton, SIGNAL(clicked()), this, SIGNAL(evaluateGradientRequested()));

    QHBoxLayout * buttonLayout = new QHBoxLayout(this);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addStretch();
    buttonLayout->addWidget(evaluateOutputsButton);
    buttonLayout->addWidget(evaluateGradientButton);
  }

}
