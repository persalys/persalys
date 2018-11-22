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
#include <QPainter>

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
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(new TitleLabel(tr("Model creation")));

  // spacer
  mainLayout->addSpacing(10);

  // buttons
  QGridLayout * layout = new QGridLayout;

  // left side

  QPushButton * button = new DiagramPushButton(tr("Symbolic model"));
  button->setStatusTip(tr("Create a physical model with outputs defined by analytical formulae"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createSymbolicModel()));
  layout->addWidget(button, 0, 0);

  button = new DiagramPushButton(tr("Python model"));
  button->setStatusTip(tr("Create a physical model defined with a Python script"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createPythonModel()));
  layout->addWidget(button, 1, 0);

  int row = 2;
#ifdef OTGUI_HAVE_YACS
  button = new DiagramPushButton(tr("YACS model"));
  button->setStatusTip(tr("Create a physical model defined with an YACS file"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createYACSModel()));
  layout->addWidget(button, row, 0);
  ++row;
#endif

#ifdef OTGUI_HAVE_OTFMI
  button = new DiagramPushButton(tr("FMI model"));
  button->setStatusTip(tr("Create a physical model defined with a FMU file"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createFMIModel()));
  layout->addWidget(button, row, 0);
  ++ row;
#endif

  QLabel * textEdit = new QLabel;
  textEdit->setText(tr("Available functionalities :"));
  textEdit->setStyleSheet("font: bold;");
  layout->addWidget(textEdit, row, 0);

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
  layout->addWidget(textEdit, row + 1, 0, Qt::AlignHCenter | Qt::AlignTop);

  // vertical line

  QFrame * buttonsFrame = new QFrame;
  buttonsFrame->setFrameShape(QFrame::VLine);
  layout->addWidget(buttonsFrame, 0, 1, row + 2, 1);

  // right side

  button = new DiagramPushButton(tr("Data model"));
  button->setStatusTip(tr("Import a sample to create a model"));
  connect(button, SIGNAL(clicked()), studyItem_, SLOT(createDataModel()));
  layout->addWidget(button, 0, 2);

  textEdit = new QLabel;
  textEdit->setText(tr("Available functionalities :"));
  textEdit->setStyleSheet("font: bold;");
  layout->addWidget(textEdit, row, 2);

  textEdit = new QLabel;
  text = QString("- %1\n- %2\n- %3\n- %4")
         .arg(tr("Data analysis"))
         .arg(tr("Marginals inference"))
         .arg(tr("Dependence inference"))
         .arg(tr("Metamodel"));
  textEdit->setText(text);
  layout->addWidget(textEdit, row + 1, 2, Qt::AlignHCenter | Qt::AlignTop);

  // set widget
  mainLayout->addLayout(layout);
  mainLayout->addStretch();

  // OT image
  QLabel * imageLabel = new QLabel;
  QPixmap pixmap(":/images/OT_image256x256.png");
  QImage image(pixmap.size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  QPainter p(&image);
  p.setOpacity(0.4);
  p.drawPixmap(0, 0, pixmap);
  p.end();
  QPixmap imagePixmap = QPixmap::fromImage(image);
  imageLabel->setPixmap(imagePixmap);

  mainLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
}
}
