//                                               -*- C++ -*-
/**
 *  @brief Study
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/StudyWindow.hxx"
#include "otgui/DiagramPushButton.hxx"

#include <QVBoxLayout>
#include <QScrollArea>

namespace OTGUI
{

StudyWindow::StudyWindow(StudyItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , studyItem_(item)
{
  buildInterface();
}


void StudyWindow::buildInterface()
{
  QVBoxLayout * subWinowLayout = new QVBoxLayout(this);
  subWinowLayout->addWidget(new TitleLabel(tr("Model creation"), "user_manual/graphical_interface/getting_started/user_manual_getting_started.html#modelsdefonstudywindow"));

  // buttons
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  subWinowLayout->addWidget(scrollArea);

  QWidget * mainWidget = new QWidget;
  QGridLayout * layout = new QGridLayout(mainWidget);
  scrollArea->setWidget(mainWidget);

  int row = 0;

  // row - titles
  QLabel * textEdit = new QLabel;
  textEdit->setText(tr("Available models"));
  textEdit->setStyleSheet("QLabel {font: bold;}");
  layout->addWidget(textEdit, row, 0);

  textEdit = new QLabel;
  textEdit->setText(tr("Available functionalities"));
  textEdit->setStyleSheet("QLabel {font: bold;}");
  layout->addWidget(textEdit, row, 1);

  // row - HLine
  QHBoxLayout * hLayout = new QHBoxLayout;
  QFrame * hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);

  QLabel * title = new QLabel;
  title->setText(tr("Vector function"));
  title->setStyleSheet("border: 1px solid black");
  hLayout->addWidget(title);

  hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);

  layout->addLayout(hLayout, ++row, 0, 1, 2);

  // row - buttons
  int nbModels = 2;
  QPushButton * button = new DiagramPushButton(tr("Symbolic model"));
  button->setStatusTip(tr("Create a physical model with outputs defined by analytical formulae"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createSymbolicModel()));
  layout->addWidget(button, ++row, 0, Qt::AlignTop);

  button = new DiagramPushButton(tr("Python model"));
  button->setStatusTip(tr("Create a physical model defined with a Python script"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createPythonModel()));
  layout->addWidget(button, ++row, 0, Qt::AlignTop);

#ifdef OTGUI_HAVE_YACS
  button = new DiagramPushButton(tr("YACS model"));
  button->setStatusTip(tr("Create a physical model defined with an YACS file"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createYACSModel()));
  layout->addWidget(button, ++row, 0, Qt::AlignTop);
  ++nbModels;
#endif

#ifdef OTGUI_HAVE_OTFMI
  button = new DiagramPushButton(tr("FMI model"));
  button->setStatusTip(tr("Create a physical model defined with a FMU file"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createFMIModel()));
  layout->addWidget(button, ++row, 0, Qt::AlignTop);
  ++nbModels;
#endif
  layout->setRowStretch(row, 1);

  // functionalities list
  textEdit = new QLabel;
  QString text = QString("- %1\n- %2\n- %3\n- %4\n- %5\n- %6\n- %7")
                 .arg(tr("Evaluation"))
                 .arg(tr("Optimization"))
                 .arg(tr("Design of experiments"))
                 .arg(tr("Central tendency"))
                 .arg(tr("Sensitivity"))
                 .arg(tr("Reliability"))
                 .arg(tr("Metamodel"));
#ifdef OTGUI_HAVE_OTMORRIS
  text.append(QString("\n- %1").arg(tr("Screening")));
#endif
  textEdit->setText(text);
  layout->addWidget(textEdit, 2, 1, nbModels, 1, Qt::AlignLeft | Qt::AlignTop);

  // row - HLine
  hLayout = new QHBoxLayout;
  hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);
  title = new QLabel;
  title->setText(tr("Field function"));
  title->setStyleSheet("border: 1px solid black");
  hLayout->addWidget(title);

  hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);
  layout->addLayout(hLayout, ++row, 0, 1, 2);

  // row - buttons
  button = new DiagramPushButton(tr("Symbolic Field model"));
  button->setStatusTip(tr("Create a physical model with outputs defined by analytical formulae"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createSymbolicFieldModel()));
  layout->addWidget(button, ++row, 0);

  textEdit = new QLabel;
  text = QString("- %1\n- %2")
         .arg(tr("Evaluation"))
         .arg(tr("Central tendency"));
  textEdit->setText(text);
  layout->addWidget(textEdit, row, 1, Qt::AlignLeft | Qt::AlignTop);

  button = new DiagramPushButton(tr("Python Field model"));
  button->setStatusTip(tr("Create a physical model defined with a Python script"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createPythonFieldModel()));
  layout->addWidget(button, ++row, 0);

  // row - HLine
  hLayout = new QHBoxLayout;
  hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);
  title = new QLabel;
  title->setText(tr("Data sample"));
  title->setStyleSheet("border: 1px solid black");
  hLayout->addWidget(title);

  hLineFrame = new QFrame;
  hLineFrame->setFrameShape(QFrame::HLine);
  hLayout->addWidget(hLineFrame, 1);
  layout->addLayout(hLayout, ++row, 0, 1, 2);

  button = new DiagramPushButton(tr("Data model"));
  button->setStatusTip(tr("Import a sample to create a model"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createDataModel()));
  layout->addWidget(button, ++row, 0, Qt::AlignTop);

  layout->setRowStretch(row, 1);

  // functionalities list
  textEdit = new QLabel;
  text = QString("- %1\n- %2\n- %3\n- %4")
         .arg(tr("Data analysis"))
         .arg(tr("Marginals inference"))
         .arg(tr("Dependence inference"))
         .arg(tr("Metamodel"));
  textEdit->setText(text);
  layout->addWidget(textEdit, row, 1, Qt::AlignLeft | Qt::AlignTop);

  layout->setRowStretch(++row, 4);
}
}
