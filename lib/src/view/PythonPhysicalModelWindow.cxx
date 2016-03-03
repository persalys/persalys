//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2016 EDF
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/PythonPhysicalModelWindow.hxx"

#include "otgui/CodeDelegate.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"
#include "otgui/PythonPhysicalModel.hxx"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QSplitter>

using namespace OT;

namespace OTGUI {

CodeView::CodeView(QWidget * parent)
  : QTableView(parent)
{
}


bool CodeView::event(QEvent * event)
{
  if (event->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Tab)
    {
      return true;
    }
    return QTableView::event(event);
  }
  return QTableView::event(event);
}


PythonPhysicalModelWindow::PythonPhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , codeModel_(0)
{
  connect(item, SIGNAL(codeChanged()), this, SLOT(updateCodeModel()));
  setFocusPolicy(Qt::ClickFocus);

  QSplitter * horizontalSplitter = new QSplitter(Qt::Horizontal);

  codeView_ = new CodeView;
  codeView_->setEditTriggers(QTableView::AllEditTriggers);
  codeView_->horizontalHeader()->setStretchLastSection(true);
  codeView_->verticalHeader()->setStretchLastSection(true);
  codeView_->horizontalHeader()->hide();
  codeView_->verticalHeader()->hide();
  codeView_->setItemDelegate(new CodeDelegate);
  horizontalSplitter->addWidget(codeView_);
  updateCodeModel();

  QWidget * rightSideWidget = new QWidget;
  QVBoxLayout * vBoxLayout = new QVBoxLayout(rightSideWidget);
  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  connect(widget, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  vBoxLayout->addWidget(widget);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  vBoxLayout->addWidget(errorMessageLabel_);

  horizontalSplitter->addWidget(rightSideWidget);

  ////////////////
  setWidget(horizontalSplitter);
}


void PythonPhysicalModelWindow::updateCodeModel()
{
  if (codeModel_)
    delete codeModel_;
  codeModel_ = new CodeModel(physicalModel_);
  connect(codeModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(parseVariables()));
  codeView_->setModel(codeModel_);
}


void PythonPhysicalModelWindow::parseVariables()
{
  QString code = QString::fromStdString(dynamic_cast<PythonPhysicalModel*>(physicalModel_.getImplementation().get())->getCode());
  QStringList lines = code.split("\n");
  Description inputVariables;
  Description outputVariables;

  QRegExp variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  foreach (QString line, lines)
  {
    QRegExp defFunction("f[ ]+\\_exec[ ]*\\(([_a-zA-Z0-9, ]+)\\)[ ]*:");
    if (defFunction.indexIn(line) > 0)
    {
      QString inputList = defFunction.cap(1);
      int pos = 0;
      while ((pos = variable.indexIn(inputList, pos)) != -1)
      {
        inputVariables.add(variable.cap(1).toStdString());
        pos += variable.matchedLength();
      }
    }

    QRegExp returnOutput("return[ ]+\\[([_a-zA-Z0-9, ]+)\\]");
    if (returnOutput.indexIn(line, 4) > 0)
    {
      QString outputList = returnOutput.cap(1);
      int pos = 0;
      while ((pos = variable.indexIn(outputList, pos)) != -1)
      {
        outputVariables.add(variable.cap(1).toStdString());
        pos += variable.matchedLength();
      }
    }
  }

  Description existingInputVariables(physicalModel_.getInputNames());
  for (unsigned int i = 0; i < inputVariables.getSize(); ++ i)
  {
    if (!existingInputVariables.__contains__(inputVariables[i]))
    {
      physicalModel_.addInput(Input(inputVariables[i]));
    }
  }
  for (unsigned int i = 0; i < existingInputVariables.getSize(); ++ i)
  {
    if (!inputVariables.__contains__(existingInputVariables[i]))
    {
      physicalModel_.removeInput(existingInputVariables[i]);
    }
  }

  Description existingOutputVariables(physicalModel_.getOutputNames());
  for (unsigned int i = 0; i < outputVariables.getSize(); ++ i)
  {
    if (!existingOutputVariables.__contains__(outputVariables[i]))
    {
      physicalModel_.addOutput(Output(outputVariables[i]));
    }
  }
  for (unsigned int i = 0; i < existingOutputVariables.getSize(); ++ i)
  {
    if (!outputVariables.__contains__(existingOutputVariables[i]))
    {
      physicalModel_.removeOutput(existingOutputVariables[i]);
    }
  }
}
}