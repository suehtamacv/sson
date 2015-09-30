#include "MainWindow.h"
#include "Def.h"
#include "AuxiliarMain.h"
#include "SaveLoadInfo.h"
#include "Regenerators.h"
#include <QMenuBar>
#include <QCoreApplication>
#include <QApplication>
#include <ios>
#include <iostream>
#include <fstream>
#include <thread>
#include <stdio.h>

void exec_Interface();

int MainWindow::ProgressBarCont = 0;

MainWindow::MainWindow() {
    resize(800, 600);

    opening_page = new QWidget;
    Screen1 = new QWidget;
    Screen2 = new QWidget;
    Screen3 = new QWidget;
    Screen4 = new QWidget;
    about_page = new AboutPage;
    addparam_page = new QWidget;

    gnuplot_TempPlot = new QTemporaryFile;
    gnuplot_TempPlot->open();

    menubar = new QMenuBar(this);
    menubar->setObjectName(QStringLiteral("menubar"));
    setMenuBar(menubar);

    QMetaObject::connectSlotsByName(this);

    main_pages = new QStackedWidget(this);
    main_pages->addWidget(opening_page);
    main_pages->addWidget(Screen1);
    main_pages->addWidget(Screen2);
    main_pages->addWidget(Screen3);
    main_pages->addWidget(Screen4);
    main_pages->setFixedSize(800, 600);

    Container = new QWidget;
    HContainerLayout = new QHBoxLayout(Container);
    HContainerLayout->addStretch();
    HContainerLayout->addWidget(main_pages);
    HContainerLayout->addStretch();
    setCentralWidget(Container);

    setWindowTitle(tr("Simulator for Slice Optical Networks"));
    main_pages->setCurrentWidget(opening_page);

    connect(this, SIGNAL(trigger_Refresh()), AuxiliarMain::TheGraphicThread,
            SLOT(trigger_GraphicRefresh()));
    connect(this, SIGNAL(trigger_SimulationEndedActions()),
            AuxiliarMain::TheGraphicThread, SLOT(trigger_SimulationEndedActions()));

    Set_AddParamPage();
    Set_OpeningPage();
    Set_Screen1();
    Set_Screen2();
    Set_Screen3();
    Set_Screen4();

    Create_Actions();
    Create_Menus();
}

MainWindow::~MainWindow() {
    about_page->AboutPage_Close();
    addparam_page->close();

    delete file_new_sim;
    delete file_open_sim;
    delete file_save_sim;
    delete file_exit;
    delete help_about;

    delete opening_page;
    delete Screen1;
    delete Screen2;
    delete Screen3;
    delete Screen4;
    delete about_page;
    delete addparam_page;

    delete Screen1_Topology_Image;
    delete Screen1_Topology_GrapScene;

    AuxiliarMain::SupremeFlag = false;
}

void MainWindow::ChangeTopologyPic() {
    switch (Screen1_Topology_ComboBox->currentIndex()) {
        case Top_PacificBell:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/PacificBell.png");
            break;

        case Top_NSFNet:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/NSFNet.png");
            break;

        case Top_NFSNetMod:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/NSFNetNormalizada.png");
            break;

        case Top_PointToPoint4:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/PontoAPonto4.png");
            break;

        case Top_PointToPoint8:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/PontoAPonto8.png");
            break;

        case Top_Top1:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/Topologia1.png");
            break;

        case Top_Top2:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/Topologia2.png");
            break;

        case Top_European:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/European.png");
            break;

        case Top_German:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/German.png");
            break;

        case Top_USBackbone:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/USBackbone.png");
            break;

        default:
            Screen1_Topology_Image->load(QString::fromStdString(
                                             AuxiliarMain::Installed_Path) + "Figures/Blank.png");
            break;
    }

    Screen1_Topology_ImagePixMap = QPixmap::fromImage(
                                       Screen1_Topology_Image->scaled(580, 280, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
    Screen1_Topology_GrapScene->clear();
    Screen1_Topology_GrapScene->addPixmap(Screen1_Topology_ImagePixMap);
    Screen1_Topology_GrapScene->setSceneRect(
        Screen1_Topology_ImagePixMap.rect());
    Screen1_Topology_GraphicsView->setScene(Screen1_Topology_GrapScene);
}

void MainWindow::ClearSimulation() {
    //First Screen information
    Screen1_SimulationType_ComboBox->setCurrentIndex(0);

    if (Screen1_NetworkType_TL_RadioButton->isChecked() ||
            Screen1_NetworkType_TP_RadioButton->isChecked()) {
        Screen1_NetworkType_ButtonGroup->setExclusive(false);
        Screen1_NetworkType_ButtonGroup->checkedButton()->setChecked(false);
        Screen1_NetworkType_ButtonGroup->setExclusive(true);
    }

    Screen1_Topology_ComboBox->setCurrentIndex(0);

    //Second Screen information
    Screen2_RoutAlg_ComboBox->setCurrentIndex(0);
    Screen2_SpecAssAlg_ComboBox->setCurrentIndex(0);
    Screen2_RAAlg_ComboBox->setCurrentIndex(0);
    Screen2_RPAlg_ComboBox->setCurrentIndex(0);
    Screen2_RepSim_LineEdit->setText("");
    Screen2_BandWidth_LineEdit->Text->setText("");
    Screen2_TotalNumReg_LineEdit->setText("");
    Screen2_NumRegNode_LineEdit->setText("");
    Screen2_mu_LineEdit->setText("");
    Screen2_MinNumRegNode_LineEdit->setText("");
    Screen2_MaxNumRegNode_LineEdit->setText("");

    //Third Screen Information
    Screen3_InLineAmpDist_LineEdit->setText("");
    Screen3_MinLaNet_LineEdit->setText("");
    Screen3_MaxLaNet_LineEdit->setText("");
    Screen3_PointResLaNet_LineEdit->setText("");
    Screen3_MinOSNR_LineEdit->setText("");
    Screen3_MaxOSNR_LineEdit->setText("");
    Screen3_PointResOSNR_LineEdit->setText("");
    Screen3_LaNet_LineEdit->setText("");
    Screen3_MinSPL_LineEdit->setText("");
    Screen3_MaxSPL_LineEdit->setText("");
    Screen3_MinInLineAmpDist_LineEdit->setText("");
    Screen3_MaxInLineAmpDist_LineEdit->setText("");
    Screen3_PointResInLineAmpDist_LineEdit->setText("");
    Screen3_InputPower_LineEdit->setText("");
    Screen3_RefPower_LineEdit->setText("");

    if (Screen3_NodeArc_BS_RadioButton->isChecked() ||
            Screen3_NodeArc_SS_RadioButton->isChecked()) {
        Screen3_NodeArc_ButtonGroup->setExclusive(false);
        Screen3_NodeArc_ButtonGroup->checkedButton()->setChecked(false);
        Screen3_NodeArc_ButtonGroup->setExclusive(true);
    }

    Screen3_MinNumBloqReq_LineEdit->setText("");
}

void MainWindow::closeEvent(QCloseEvent *) {
    about_page->close();
    addparam_page->close();

    AuxiliarMain::SupremeFlag = false;
}

void MainWindow::Close_AddParametersPage() {
    addparam_page->close();
}

void MainWindow::Close_Program() {
    about_page->close();
    addparam_page->close();
    this->close();
    this->destroy(true, true);
}

void MainWindow::CorrectDouble(QLineEdit *Object) {
    int Count;
    QString Text = Object->text();

    for (Count = 0; Count < Text.length(); Count++) {
        if (Text[Count] == ',') {
            Text[Count] = '.';
        }
    }

    Object->setText(Text);
}

void MainWindow::CorrectDouble(UnitLineEdit *Object) {
    CorrectDouble(Object->Text);
}

void MainWindow::Create_Actions() {
    file_open_sim = new QAction(tr("&Open Simulation"), this);
    file_saveas_sim = new QAction(tr("&Save Simulation"), this);
    file_exit = new QAction(tr("&Exit"), this);

    edit_load_default = new QAction(tr("Open &Default Values"), this);
    edit_set_parameters = new QAction(tr("&Set Additional Parameters"), this);

    help_about = new QAction(tr("&About"), this);

    for (map<The_Translations, SSON_Thread::Translator *>::iterator iter =
                SSON_Thread::Translators.begin(); iter != SSON_Thread::Translators.end();
            ++iter) {
        edit_Languages[iter->first] = new QAction(iter->second->Language_Name, this);
        connect(edit_Languages[iter->first], SIGNAL(triggered(bool)), this,
                SLOT(Translate_SSON()));
    }

    connect(file_saveas_sim, SIGNAL(triggered(bool)), this,
            SLOT(Slot_SaveAsSimulation()));
    connect(file_open_sim, SIGNAL(triggered(bool)), this,
            SLOT(Slot_LoadSimulation()));
    connect(file_exit, SIGNAL(triggered()), this, SLOT(Close_Program()));
    connect(edit_load_default, SIGNAL(triggered(bool)), this,
            SLOT(Slot_LoadDefaultValues()));
    connect(edit_set_parameters, SIGNAL(triggered(bool)), this,
            SLOT(Open_AddParametersPage()));
    connect(help_about, SIGNAL(triggered()), this, SLOT(Open_AboutPage()));
}

void MainWindow::Create_Menus() {
    file_menu = new QMenu(tr("&File"));
    edit_menu = new QMenu(tr("&Edit"));
    help_menu = new QMenu(tr("&Help"));
    choose_lang_menu = new QMenu(tr("&Choose Language"));

    menuBar()->addMenu(file_menu);
    file_menu->addAction(file_open_sim);
    file_menu->addAction(file_saveas_sim);
    file_menu->addSeparator();
    file_menu->addAction(file_exit);

    menuBar()->addMenu(edit_menu);
    edit_menu->addAction(edit_load_default);
    edit_menu->addMenu(choose_lang_menu);
    edit_menu->addAction(edit_set_parameters);

    for (map<The_Translations, QAction *>::iterator iter = edit_Languages.begin();
            iter != edit_Languages.end(); ++iter) {
        choose_lang_menu->addAction(iter->second);
    }

    menuBar()->addMenu(help_menu);
    help_menu->addAction(help_about);
}

void MainWindow::Delay(int Num_MilliSeconds) {
    QTime dieTime = QTime::currentTime().addMSecs(Num_MilliSeconds);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


bool MainWindow::InputCheck_Screen1() {
    int FailureCount = 0;

    if (Screen1_SimulationType_ComboBox->currentIndex() == _Sim_Null) {
        Screen1_SimulationType_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
        FailureCount++;
    } else {
        Screen1_SimulationType_ComboBox->setStyleSheet("QComboBox");
    }

    if ((Screen1_SimulationType_ComboBox->currentIndex() != Sim_RegenNum &&
            Screen1_SimulationType_ComboBox->currentIndex() != _Sim_Null)) {
        if ((!Screen1_NetworkType_TL_RadioButton->isChecked()) &&
                (!Screen1_NetworkType_TP_RadioButton->isChecked())) {
            Screen1_NetworkType_TL_RadioButton->setStyleSheet("QRadioButton { border: 3px solid red; }");
            Screen1_NetworkType_TP_RadioButton->setStyleSheet("QRadioButton { border: 3px solid red; }");
            FailureCount++;
        } else {
            Screen1_NetworkType_TL_RadioButton->setStyleSheet("QRadioButton");
            Screen1_NetworkType_TP_RadioButton->setStyleSheet("QRadioButton");
        }
    }

    if (Screen1_Topology_ComboBox->currentIndex() == _Top_Null) {
        Screen1_Topology_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
        FailureCount++;
    } else {
        Screen1_Topology_ComboBox->setStyleSheet("QComboBox");
    }

    if (FailureCount != 0) {
        return false;
    } else {
        Screen1_SimulationType_ComboBox->setStyleSheet("QComboBox");
        Screen1_NetworkType_TL_RadioButton->setStyleSheet("QRadioButton");
        Screen1_NetworkType_TP_RadioButton->setStyleSheet("QRadioButton");
        Screen1_Topology_ComboBox->setStyleSheet("QComboBox");

        return true;
    }
}

bool MainWindow::InputCheck_Screen2() {
    int ErrorCount = 0;

    if (Screen2_SpecAssAlg_ComboBox->currentIndex() == 0) {
        Screen2_SpecAssAlg_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
        ErrorCount++;
    } else {
        Screen2_SpecAssAlg_ComboBox->setStyleSheet("QComboBox");
    }

    if (TextCheck_Double(Screen2_BandWidth_LineEdit->text())) {
        Screen2_BandWidth_LineEdit->Text->setStyleSheet("QLineEdit { border: 3px solid red; }");
        ErrorCount++;
    } else {
        Screen2_BandWidth_LineEdit->Text->setStyleSheet("QLineEdit");
    }

    if (TextCheck_Double(Screen2_mu_LineEdit->text())) {
        Screen2_mu_LineEdit->setStyleSheet("QLineEdit { border: 3px solid red; }");
        ErrorCount++;
    } else {
        Screen2_mu_LineEdit->setStyleSheet("QLineEdit");
    }

    if (Screen2_RoutAlg_ComboBox->isHidden() == false) {
        if (Screen2_RoutAlg_ComboBox->currentIndex() == _Routing_Null) {
            Screen2_RoutAlg_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
            ErrorCount++;
        } else {
            Screen2_RoutAlg_ComboBox->setStyleSheet("QComboBox");
        }
    }

    if (Screen2_RPAlg_ComboBox->isHidden() == false) {
        if (Screen2_RPAlg_ComboBox->currentIndex() == _RP_Null) {
            Screen2_RPAlg_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
            ErrorCount++;
        } else {
            Screen2_RPAlg_ComboBox->setStyleSheet("QComboBox");
        }
    }


    if (Screen2_RAAlg_ComboBox->isHidden() == false) {
        if (Screen2_RAAlg_ComboBox->currentIndex() == _RA_Null) {
            Screen2_RAAlg_ComboBox->setStyleSheet("QComboBox { border: 3px solid red; }");
            ErrorCount++;
        } else {
            Screen2_RAAlg_ComboBox->setStyleSheet("QComboBox");
        }
    }


    if (Screen2_TotalNumReg_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen2_TotalNumReg_LineEdit->text())) {
            Screen2_TotalNumReg_LineEdit->setStyleSheet("QLineEdit { border: 3px solid red; }");
            ErrorCount++;
        } else {
            Screen2_TotalNumReg_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen2_NumRegNode_LineEdit->isHidden() == false &&
            Screen2_NumRegNode_LineEdit->isEnabled()) {
        if (TextCheck_Int(Screen2_NumRegNode_LineEdit->text())) {
            Screen2_NumRegNode_LineEdit->setStyleSheet("QLineEdit { border: 3px solid red; }");
            ErrorCount++;
        } else {
            if (Screen2_NumRegNode_LineEdit->text().toInt() >
                    Screen2_TotalNumReg_LineEdit->text().toInt()) {
                Screen2_NumRegNode_LineEdit->setStyleSheet("QLineEdit { border: 3px solid red; }");
                ErrorCount++;
            } else {
                Screen2_NumRegNode_LineEdit->setStyleSheet("QLineEdit");
            }
        }
    }

    if (Screen2_RepSim_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen2_RepSim_LineEdit->text())) {
            Screen2_RepSim_LineEdit->setStyleSheet("QLineEdit { border: 3px solid red; }");
            ErrorCount++;
        } else {
            Screen2_RepSim_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen2_MinNumRegNode_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen2_MinNumRegNode_LineEdit->text())) {
            Screen2_MinNumRegNode_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen2_MinNumRegNode_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen2_MaxNumRegNode_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen2_MaxNumRegNode_LineEdit->text())) {
            Screen2_MaxNumRegNode_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            if (Screen2_MaxNumRegNode_LineEdit->text().toDouble() <
                    Screen2_MinNumRegNode_LineEdit->text().toDouble()) {
                Screen2_MaxNumRegNode_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
                ErrorCount++;
            } else {
                Screen2_MaxNumRegNode_LineEdit->setStyleSheet("QLineEdit");
            }
        }
    }

    CorrectDouble(Screen2_BandWidth_LineEdit->Text);
    CorrectDouble(Screen2_mu_LineEdit);
    CorrectDouble(Screen2_MinNumRegNode_LineEdit);
    CorrectDouble(Screen2_MaxNumRegNode_LineEdit);

    if (ErrorCount != 0) {
        return false;
    } else {
        Screen2_SpecAssAlg_ComboBox->setStyleSheet("QComboBox");
        Screen2_BandWidth_LineEdit->Text->setStyleSheet("QLineEdit");
        Screen2_mu_LineEdit->setStyleSheet("QLineEdit");
        Screen2_RoutAlg_ComboBox->setStyleSheet("QComboBox");
        Screen2_RPAlg_ComboBox->setStyleSheet("QComboBox");
        Screen2_RAAlg_ComboBox->setStyleSheet("QComboBox");
        Screen2_TotalNumReg_LineEdit->setStyleSheet("QLineEdit");
        Screen2_NumRegNode_LineEdit->setStyleSheet("QLineEdit");
        Screen2_RepSim_LineEdit->setStyleSheet("QLineEdit");
        Screen2_MinNumRegNode_LineEdit->setStyleSheet("QLineEdit");
        Screen2_MaxNumRegNode_LineEdit->setStyleSheet("QLineEdit");

        return true;
    }
}

bool MainWindow::InputCheck_Screen3() {
    int ErrorCount = 0;

    if (TextCheck_Double(Screen3_InputPower_LineEdit->text())) {
        Screen3_InputPower_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
        ErrorCount++;
    } else {
        Screen3_InputPower_LineEdit->setStyleSheet("QLineEdit");
    }

    if (TextCheck_Double(Screen3_RefPower_LineEdit->text())) {
        Screen3_RefPower_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
        ErrorCount++;
    } else {
        Screen3_RefPower_LineEdit->setStyleSheet("QLineEdit");
    }

    if ((!Screen3_NodeArc_BS_RadioButton->isChecked()) &&
            (!Screen3_NodeArc_SS_RadioButton->isChecked())) {
        Screen3_NodeArc_BS_RadioButton->setStyleSheet("QRadioButton {border: 3px solid red}");
        Screen3_NodeArc_SS_RadioButton->setStyleSheet("QRadioButton {border: 3px solid red}");
        ErrorCount++;
    } else {
        Screen3_NodeArc_BS_RadioButton->setStyleSheet("QRadioButton");
        Screen3_NodeArc_SS_RadioButton->setStyleSheet("QRadioButton");
    }

    if (Screen3_InLineAmpDist_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_InLineAmpDist_LineEdit->text())) {
            Screen3_InLineAmpDist_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_InLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MinLaNet_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MinLaNet_LineEdit->text())) {
            Screen3_MinLaNet_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MinLaNet_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MaxLaNet_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MaxLaNet_LineEdit->text())) {
            Screen3_MaxLaNet_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MaxLaNet_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_PointResLaNet_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen3_PointResLaNet_LineEdit->text())) {
            Screen3_PointResLaNet_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            if (Screen3_PointResLaNet_LineEdit->text().toInt() <= 1) {
                Screen3_PointResLaNet_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
                ErrorCount++;
            } else {
                Screen3_PointResLaNet_LineEdit->setStyleSheet("QLineEdit");
            }
        }
    }

    if (Screen3_MinOSNR_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MinOSNR_LineEdit->text())) {
            Screen3_MinOSNR_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MinOSNR_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MaxOSNR_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MaxOSNR_LineEdit->text())) {
            Screen3_MaxOSNR_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MaxOSNR_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_PointResOSNR_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen3_PointResOSNR_LineEdit->text())) {
            Screen3_PointResOSNR_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            if (Screen3_PointResOSNR_LineEdit->text().toInt() <= 1) {
                Screen3_PointResOSNR_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
                ErrorCount++;
            } else {
                Screen3_PointResOSNR_LineEdit->setStyleSheet("QLineEdit");
            }
        }
    }

    if (Screen3_RABothDecRatio_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_RABothDecRatio_LineEdit->text())) {
            Screen3_RABothDecRatio_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_RABothDecRatio_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_LaNet_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_LaNet_LineEdit->text())) {
            Screen3_LaNet_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_LaNet_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MinSPL_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen3_MinSPL_LineEdit->text())) {
            Screen3_MinSPL_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MinSPL_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MaxSPL_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen3_MaxSPL_LineEdit->text())) {
            Screen3_MaxSPL_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MaxSPL_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MinInLineAmpDist_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MinInLineAmpDist_LineEdit->text())) {
            Screen3_MinInLineAmpDist_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MinInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_MaxInLineAmpDist_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MaxInLineAmpDist_LineEdit->text())) {
            Screen3_MaxInLineAmpDist_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MaxInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    if (Screen3_PointResInLineAmpDist_LineEdit->isHidden() == false) {
        if (TextCheck_Int(Screen3_PointResInLineAmpDist_LineEdit->text())) {
            Screen3_PointResInLineAmpDist_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            if (Screen3_PointResInLineAmpDist_LineEdit->text().toInt() <= 1) {
                Screen3_PointResInLineAmpDist_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
                ErrorCount++;
            } else {
                Screen3_PointResInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
            }
        }
    }

    if (Screen3_MinNumBloqReq_LineEdit->isHidden() == false) {
        if (TextCheck_Double(Screen3_MinNumBloqReq_LineEdit->text())) {
            Screen3_MinNumBloqReq_LineEdit->setStyleSheet("QLineEdit {border: 3px solid red;}");
            ErrorCount++;
        } else {
            Screen3_MinNumBloqReq_LineEdit->setStyleSheet("QLineEdit");
        }
    }

    CorrectDouble(Screen3_InputPower_LineEdit);
    CorrectDouble(Screen3_RefPower_LineEdit);
    CorrectDouble(Screen3_InLineAmpDist_LineEdit);
    CorrectDouble(Screen3_MinLaNet_LineEdit);
    CorrectDouble(Screen3_MaxLaNet_LineEdit);
    CorrectDouble(Screen3_MinOSNR_LineEdit);
    CorrectDouble(Screen3_MaxOSNR_LineEdit);
    CorrectDouble(Screen3_LaNet_LineEdit);
    CorrectDouble(Screen3_MinInLineAmpDist_LineEdit->Text);
    CorrectDouble(Screen3_MaxInLineAmpDist_LineEdit);
    CorrectDouble(Screen3_MinNumBloqReq_LineEdit);
    CorrectDouble(Screen3_RABothDecRatio_LineEdit);

    if (ErrorCount != 0) {
        return false;
    } else {
        Screen3_InputPower_LineEdit->setStyleSheet("QLineEdit");
        Screen3_RefPower_LineEdit->setStyleSheet("QLineEdit");
        Screen3_NodeArc_BS_RadioButton->setStyleSheet("QRadioButton");
        Screen3_NodeArc_SS_RadioButton->setStyleSheet("QRadioButton");
        Screen3_InLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MinLaNet_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MaxLaNet_LineEdit->setStyleSheet("QLineEdit");
        Screen3_PointResLaNet_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MinOSNR_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MaxOSNR_LineEdit->setStyleSheet("QLineEdit");
        Screen3_PointResOSNR_LineEdit->setStyleSheet("QLineEdit");
        Screen3_LaNet_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MinSPL_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MaxSPL_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MinInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MaxInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        Screen3_PointResInLineAmpDist_LineEdit->setStyleSheet("QLineEdit");
        Screen3_MinNumBloqReq_LineEdit->setStyleSheet("QLineEdit");
        Screen3_RABothDecRatio_LineEdit->setStyleSheet("QLineEdit");

        return true;
    }
}

/**
 * @brief MainWindow::GraphicInterface_LoadValues loads the values inserted in the Graphic Interface onto the simulation.
 */
void MainWindow::GraphicInterface_LoadValues(AuxiliarMain *Aux) {
    int aux;

    AuxiliarMain::choose_Simulation = (Simulations)
                                      Screen1_SimulationType_ComboBox->currentIndex();

    if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_Whiskers) {
        Def::maxSim_Whiskers = Screen2_RepSim_LineEdit->text().toInt();
    }

    AuxiliarMain::choose_Topology = (Topologies)
                                    Screen1_Topology_ComboBox->currentIndex();

    switch (AuxiliarMain::choose_Topology) {
        case Top_PacificBell:
            AuxiliarMain::TopolPBell >> aux;
            break;

        case Top_NSFNet:
            AuxiliarMain::TopolNFSNet >> aux;
            break;

        case Top_NFSNetMod:
            AuxiliarMain::TopolNSFNetMod >> aux;
            break;

        case Top_PointToPoint4:
            AuxiliarMain::TopolReduced4 >> aux;
            break;

        case Top_PointToPoint8:
            AuxiliarMain::TopolReduced8 >> aux;
            break;

        case Top_Top1:
            AuxiliarMain::TopolTopology1 >> aux;
            break;

        case Top_Top2:
            AuxiliarMain::TopolTopology2 >> aux;
            break;

        case Top_European:
            AuxiliarMain::TopolEuropean >> aux;
            break;

        case Top_German:
            AuxiliarMain::TopolGerman >> aux;
            break;

        case Top_USBackbone:
            AuxiliarMain::TopolUSBackbone >> aux;
            break;

        case _Top_Null:
            //assert(("Invalid topology.", 0 != 0));
            break;
    }

    Def::set_NumNodes(aux);

    AuxiliarMain::consider_PhyImpair = _YES;

    Def::set_BSlot(Screen2_BandWidth_LineEdit->text().toDouble());
    Def::set_BRef(12.5);

    switch (AuxiliarMain::choose_Topology) {
        case Top_PacificBell:
            AuxiliarMain::TopolPBell >> aux;
            break;

        case Top_NSFNet:
            AuxiliarMain::TopolNFSNet >> aux;
            break;

        case Top_NFSNetMod:
            AuxiliarMain::TopolNSFNetMod >> aux;
            break;

        case Top_PointToPoint4:
            AuxiliarMain::TopolReduced4 >> aux;
            break;

        case Top_PointToPoint8:
            AuxiliarMain::TopolReduced8 >> aux;
            break;

        case Top_Top1:
            AuxiliarMain::TopolTopology1 >> aux;
            break;

        case Top_Top2:
            AuxiliarMain::TopolTopology2 >> aux;
            break;

        case Top_European:
            AuxiliarMain::TopolEuropean >> aux;
            break;

        case Top_German:
            AuxiliarMain::TopolGerman >> aux;
            break;

        case Top_USBackbone:
            AuxiliarMain::TopolUSBackbone >> aux;
            break;

        case _Top_Null:
            assert((0 != 0));
            break;
    }

    Def::set_SL(aux);
    Def::set_SR(Def::SL);

    //redefines Config to reallocate Topology_S
    delete AuxiliarMain::Config;
    AuxiliarMain::Config = new Def();
    delete Aux;
    Aux = new AuxiliarMain();


    if (AuxiliarMain::choose_Simulation != Sim_RegenNum) {

        if (Screen1_NetworkType_TL_RadioButton->isChecked()) {
            AuxiliarMain::choose_NetworkType = TranslucentNetwork;
        } else if (Screen1_NetworkType_TP_RadioButton->isChecked()) {
            AuxiliarMain::choose_NetworkType = TransparentNetwork;
        }

        if (AuxiliarMain::choose_NetworkType == TranslucentNetwork) {
            AuxiliarMain::choose_RPAlgorithm = (RP_Algorithm)
                                               Screen2_RPAlg_ComboBox->currentIndex();
            AuxiliarMain::RegNumber_Total =
                Screen2_TotalNumReg_LineEdit->text().toInt();

            if (AuxiliarMain::choose_RPAlgorithm != RP_MSU) {
                AuxiliarMain::RegNumber_PerNode =
                    Screen2_NumRegNode_LineEdit->text().toInt();
            }

            AuxiliarMain::choose_RAAlgorithm = (RA_Algorithm)
                                               Screen2_RAAlg_ComboBox->currentIndex();

            if (AuxiliarMain::choose_RAAlgorithm == RA_Both) {
                Regenerators::RA_Both_DecisionRatio =
                    Screen3_RABothDecRatio_LineEdit->text().toDouble();
            }
        }
    } else {
        AuxiliarMain::choose_NetworkType = TranslucentNetwork;
        AuxiliarMain::RegNumber_Total = 0;
        AuxiliarMain::RegNumber_PerNode = 0;

        AuxiliarMain::choose_RAAlgorithm = (RA_Algorithm)
                                           Screen2_RAAlg_ComboBox->currentIndex();

        if (AuxiliarMain::choose_RAAlgorithm == RA_Both) {
            Regenerators::RA_Both_DecisionRatio =
                Screen3_RABothDecRatio_LineEdit->text().toDouble();
        }
    }

    AuxiliarMain::choose_RoutingAlgorithm =
        (Routing_Algorithm) Screen2_RoutAlg_ComboBox->currentIndex();

    AuxiliarMain::choose_SpectrumAssignmentAlgorithm =
        (SpectrumAssignment_Algorithm) Screen2_SpecAssAlg_ComboBox->currentIndex();

    AuxiliarMain::mu =
        Screen2_mu_LineEdit->text().toDouble(); //Conversão para double, não para long double !!!

    if (AuxiliarMain::choose_Simulation == Sim_OSNR ||
            AuxiliarMain::choose_Simulation == Sim_AmpDist) {
        AuxiliarMain::OSNR_Min = Screen3_MinOSNR_LineEdit->text().toDouble();
        AuxiliarMain::OSNR_Max = Screen3_MaxOSNR_LineEdit->text().toDouble();
        AuxiliarMain::OSNR_Step = (AuxiliarMain::OSNR_Max - AuxiliarMain::OSNR_Min) /
                                  (Screen3_PointResOSNR_LineEdit->text().toDouble() - 1);
    }

    if (AuxiliarMain::choose_Simulation == Sim_RegenNum) {
        AuxiliarMain::RegPerNode_Min =
            Screen2_MinNumRegNode_LineEdit->text().toDouble();
        AuxiliarMain::RegPerNode_Max =
            Screen2_MaxNumRegNode_LineEdit->text().toDouble();
    }

    if (AuxiliarMain::choose_Simulation == Sim_PbReq) {
        AuxiliarMain::LaNet_Min = Screen3_MinLaNet_LineEdit->text().toDouble();
        AuxiliarMain::LaNet_Max = Screen3_MaxLaNet_LineEdit->text().toDouble();
        AuxiliarMain::LaNet_Step = (AuxiliarMain::LaNet_Max -
                                    AuxiliarMain::LaNet_Min) /
                                   (Screen3_PointResLaNet_LineEdit->text().toDouble() - 1);
    }

    if (AuxiliarMain::choose_Simulation == Sim_Whiskers ||
            AuxiliarMain::choose_Simulation == Sim_RegenNum ||
            AuxiliarMain::choose_Simulation == Sim_OSNR ||
            AuxiliarMain::choose_Simulation == Sim_SlotsNum) {
        AuxiliarMain::laNet = Screen3_LaNet_LineEdit->text().toDouble();
    }

    if (AuxiliarMain::choose_Simulation == Sim_SlotsNum) {
        AuxiliarMain::SlotsNum_Min =
            Screen3_MinSPL_LineEdit->text().toInt();
        AuxiliarMain::SlotsNum_Max =
            Screen3_MaxSPL_LineEdit->text().toInt();
    }

    if (AuxiliarMain::choose_Simulation == Sim_AmpDist) {
        AuxiliarMain::AmpDist_Min =
            Screen3_MinInLineAmpDist_LineEdit->text().toDouble();
        AuxiliarMain::AmpDist_Max =
            Screen3_MaxInLineAmpDist_LineEdit->text().toDouble();
        AuxiliarMain::AmpDist_Step = (AuxiliarMain::AmpDist_Max -
                                      AuxiliarMain::AmpDist_Min) /
                                     (Screen3_PointResInLineAmpDist_LineEdit->text().toDouble() - 1);
    }

    if (AuxiliarMain::consider_PhyImpair == _YES) {
        AuxiliarMain::Config->set_Pin(
            Screen3_InputPower_LineEdit->text().toDouble());

        AuxiliarMain::Config->set_PRef(
            Screen3_RefPower_LineEdit->text().toDouble());

        if (AuxiliarMain::choose_Simulation != Sim_AmpDist) {
            AuxiliarMain::Config->set_AmpDist(
                Screen3_InLineAmpDist_LineEdit->text().toDouble());
        }

        if (Screen3_NodeArc_BS_RadioButton->isChecked()) {
            Def::set_Architeture(Arch_BS);
        } else if (Screen3_NodeArc_SS_RadioButton->isChecked()) {
            Def::set_Architeture(Arch_SS);
        }
    }

    if (AuxiliarMain::choose_Simulation != Sim_AmpDist) {
        Def::set_NumReqBloqMin(Screen3_MinNumBloqReq_LineEdit->text().toDouble());
    }
}

void MainWindow::Open_AboutPage() {
    about_page->setModal(true);
    about_page->exec();
}

void MainWindow::Open_AddParametersPage() {
    if (main_pages->currentIndex() == 0) {
        QMessageBox::information(this, tr("SSON - Information"),
                                 tr("You have not started a simulation."));
    } else if (main_pages->currentIndex() == 4) {
        QMessageBox::information(this, tr("SSON - Information"),
                                 tr("There is a simulation running, wait for it to finish."));
    } else {
        ShowParam();
        addparam_page->show();
    }
}

void MainWindow::OpeningScreen_NextPage() {
    main_pages->setCurrentIndex(main_pages->currentIndex() + 1);
}

void MainWindow::Povoate_SimulationType() {
    Screen1_SimulationType_ComboBox->setEnabled(true);
    int Selected = Screen1_SimulationType_ComboBox->currentIndex();
    bool SetSelected = true;
    Screen1_SimulationType_ComboBox->clear();

    Screen1_SimulationType_ComboBox->addItem(tr(""));
    Screen1_SimulationType_ComboBox->addItem(tr("Blocking Probability"));
    Screen1_SimulationType_ComboBox->addItem(tr("OSNR"));

    Screen1_SimulationType_ComboBox->addItem(tr("Inline Amplifiers Distance"));

    if (Screen1_NetworkType_TL_RadioButton->isChecked()) {
        QModelIndex Index = Screen1_SimulationType_ComboBox->model()->index(
                                Screen1_SimulationType_ComboBox->count() - 1, 0);
        Screen1_SimulationType_ComboBox->model()->setData(Index,
                QVariant(Qt::NoItemFlags), Qt::UserRole - 1);

        if (Selected == Screen1_SimulationType_ComboBox->count() - 1) {
            SetSelected = false;
        }
    }

    Screen1_SimulationType_ComboBox->addItem(tr("Slots per Link Number"));
    Screen1_SimulationType_ComboBox->addItem(tr("Whiskers"));

    if (Screen1_NetworkType_TP_RadioButton->isChecked()) {
        QModelIndex Index = Screen1_SimulationType_ComboBox->model()->index(
                                Screen1_SimulationType_ComboBox->count() - 1, 0);
        Screen1_SimulationType_ComboBox->model()->setData(Index,
                QVariant(Qt::NoItemFlags), Qt::UserRole - 1);

        if (Selected == Screen1_SimulationType_ComboBox->count() - 1) {
            SetSelected = false;
        }
    }

    Screen1_SimulationType_ComboBox->addItem(tr("Regenerator Number"));

    if (Screen1_NetworkType_TP_RadioButton->isChecked()) {
        QModelIndex Index = Screen1_SimulationType_ComboBox->model()->index(
                                Screen1_SimulationType_ComboBox->count() - 1, 0);
        Screen1_SimulationType_ComboBox->model()->setData(Index,
                QVariant(Qt::NoItemFlags), Qt::UserRole - 1);

        if (Selected == Screen1_SimulationType_ComboBox->count() - 1) {
            SetSelected = false;
        }
    }

    Screen1_SimulationType_ComboBox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    Screen1_SimulationType_ComboBox->adjustSize();

    if (SetSelected) {
        Screen1_SimulationType_ComboBox->setCurrentIndex(Selected);
    }
}

void MainWindow::PreviousPage() {
    main_pages->setCurrentIndex(main_pages->currentIndex() - 1);
}

void MainWindow::Refresh() {
    this->trigger_Refresh();
}

void MainWindow::retranslateUi() {
    int Selected;

    setWindowTitle(tr("Simulator for Slice Optical Networks"));

    file_menu->setTitle(tr("&File"));
    edit_menu->setTitle(tr("&Edit"));
    edit_set_parameters->setText(tr("&Set Additional Parameters"));
    help_menu->setTitle(tr("&Help"));
    file_open_sim->setText(tr("&Open Simulation"));
    file_saveas_sim->setText(tr("&Save Simulation"));
    file_exit->setText(tr("&Exit"));
    edit_load_default->setText(tr("Open &Default Values"));
    help_about->setText(tr("&About"));
    choose_lang_menu->setTitle(tr("&Choose Language"));

    AddParamPage_Model->setHorizontalHeaderItem(0,
            new QStandardItem(tr("Parameter")));
    AddParamPage_Model->setHorizontalHeaderItem(1,
            new QStandardItem(tr("Content")));

    AddParamPage_numReqMax_Param->setText(tr("Maximum Number of Requisitons"));
    AddParamPage_L_SSS_Param->setText(tr("Losses in the SSS elements"));
    AddParamPage_freq_Param->setText(tr("Frequency"));
    AddParamPage_BRef_Param->setText(tr("Reference Bandwidth"));
    AddParamPage_FAmp_Param->setText(tr("Amplifier's Noise Factor"));
    AddParamPage_OSNRin_Param->setText(tr("Input OSNR"));
    AddParamPage_BR_Param->setText(tr("Bitrate of each Regenerator"));

    Screen4_ReplotSaveButton->setText(tr("Replot"));
    Screen4_ButtonToggleToTable->setText(tr("Table"));
    Screen4_ButtonToggleToGraph->setText(tr("Graph"));

    OpeningPage_NewSimulation_Button->setText(tr("New Simulation"));
    OpeningPage_Exit_Button->setText(tr("Exit"));
    OpeningPage_OpenSimulation_Button->setText(tr("Open Simulation"));

    Screen1_SimulationType_Label->setText(tr("Choose a simulation:"));
    Screen1_Topology_Label->setText(tr("Choose a topology:"));
    Screen1_Topology_Button->setText(tr("Open Topology"));

    Selected = Screen1_Topology_ComboBox->currentIndex();
    Screen1_Topology_ComboBox->clear();
    Screen1_Topology_ComboBox->addItem(tr(""));
    Screen1_Topology_ComboBox->addItem(tr("Pacific Bell"));
    Screen1_Topology_ComboBox->addItem(tr("NSFNet"));
    Screen1_Topology_ComboBox->addItem(tr("NSFNet Modified (Reduced)"));
    Screen1_Topology_ComboBox->addItem(tr("Point-To-Point with 4 Nodes"));
    Screen1_Topology_ComboBox->addItem(tr("Point-To-Point with 8 Nodes"));
    Screen1_Topology_ComboBox->addItem(tr("Top1"));
    Screen1_Topology_ComboBox->addItem(tr("Top2"));
    Screen1_Topology_ComboBox->addItem(tr("European"));
    Screen1_Topology_ComboBox->addItem(tr("German"));
    Screen1_Topology_ComboBox->addItem(tr("US Backbone"));
    Screen1_Topology_ComboBox->setCurrentIndex(Selected);
    Screen1_Topology_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    Screen1_Topology_ComboBox->adjustSize();

    Povoate_SimulationType();

    Screen1_NetworkType_Label->setText(tr("Choose the network type:"));
    Screen1_NetworkType_TL_RadioButton->setText(tr("Translucent"));
    Screen1_NetworkType_TP_RadioButton->setText(tr("Transparent"));
    Screen1_NextPage_Button->setText(tr("Next"));

    Screen2_NextPage_Button->setText(tr("Next"));
    Screen2_PreviousPage_Button->setText(tr("Back"));
    Screen2_RoutAlg_Label->setText(tr("Choose a routing algorithm:"));

    Selected = Screen2_RoutAlg_ComboBox->currentIndex();
    Screen2_RoutAlg_ComboBox->clear();
    Screen2_RoutAlg_ComboBox->addItem(tr(""));
    Screen2_RoutAlg_ComboBox->addItem(tr("Minimum Hops"));
    Screen2_RoutAlg_ComboBox->addItem(tr("Shortest Path"));
    Screen2_RoutAlg_ComboBox->addItem(tr("LOR Allocation Possibilities"));
    Screen2_RoutAlg_ComboBox->addItem(tr("LOR Availability"));
    Screen2_RoutAlg_ComboBox->addItem(tr("OSNR-R"));
    Screen2_RoutAlg_ComboBox->setCurrentIndex(Selected);
    Screen2_RoutAlg_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    Screen2_RoutAlg_ComboBox->adjustSize();

    Screen2_SpecAssAlg_Label->setText(
        tr("Choose the spectrum assigment algorithm:"));
    Selected = Screen2_SpecAssAlg_ComboBox->currentIndex();
    Screen2_SpecAssAlg_ComboBox->clear();
    Screen2_SpecAssAlg_ComboBox->addItem(tr(""));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("Random"));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("FirstFit"));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("Most Used"));
    Screen2_SpecAssAlg_ComboBox->setCurrentIndex(Selected);
    Screen2_SpecAssAlg_ComboBox->setSizeAdjustPolicy(
        QComboBox::AdjustToContents);
    Screen2_SpecAssAlg_ComboBox->adjustSize();

    Screen2_RAAlg_Label->setText(tr("Choose the RA algorithm:"));
    Selected = Screen2_RAAlg_ComboBox->currentIndex();
    Screen2_RAAlg_ComboBox->clear();
    Screen2_RAAlg_ComboBox->addItem(tr(""));
    Screen2_RAAlg_ComboBox->addItem(tr("FLR"));
    Screen2_RAAlg_ComboBox->addItem(tr("FNS"));
    Screen2_RAAlg_ComboBox->addItem(tr("Both"));
    Screen2_RAAlg_ComboBox->setCurrentIndex(Selected);
    Screen2_RAAlg_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    Screen2_RAAlg_ComboBox->adjustSize();

    Screen2_RPAlg_Label->setText(tr("Choose the RP algorithm:"));
    Selected = Screen2_RPAlg_ComboBox->currentIndex();
    Screen2_RPAlg_ComboBox->clear();
    Screen2_RPAlg_ComboBox->addItem(tr(""));
    Screen2_RPAlg_ComboBox->addItem(tr("NDF"));
    Screen2_RPAlg_ComboBox->addItem(tr("CNF"));
    Screen2_RPAlg_ComboBox->addItem(tr("TLP"));
    Screen2_RPAlg_ComboBox->addItem(tr("SQP"));
    Screen2_RPAlg_ComboBox->addItem(tr("MU"));
    Screen2_RPAlg_ComboBox->addItem(tr("MSU"));
    Screen2_RPAlg_ComboBox->setCurrentIndex(Selected);
    Screen2_RPAlg_ComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    Screen2_RPAlg_ComboBox->adjustSize();

    Screen2_RepSim_Label->setText(
        tr("How many repetitions of the simulation?"));
    Screen2_BandWidth_Label->setText(tr("Enter Slots Bandwidth:"));
    Screen2_TotalNumReg_Label->setText(
        tr("Enter the total number of regenerators:"));
    Screen2_NumRegNode_Label->setText(
        tr("Enter the number of regenerators per node:"));
    Screen2_mu_Label->setText(tr("Enter the connections deactivation rate:"));
    Screen2_MinNumRegNode_Label->setText(
        tr("Enter the minimum number of regenerators per node:"));
    Screen2_MaxNumRegNode_Label->setText(
        tr("Enter the maximum number of regenerators per node:"));
    Screen2_RAAlg_Label->setToolTip(tr("Regenerator Allocation Algorithm"));
    Screen2_RPAlg_Label->setToolTip(tr("Regenerator Placement Algorithm"));
    Screen2_BandWidth_Label->setToolTip(
        tr("Common values are 1.5625, 3.125, 6.25 or 12.5"));

    Screen3_Simulate_Button->setText(tr("Simulate"));
    Screen3_PreviousPage_Button->setText(tr("Back"));
    Screen3_InLineAmpDist_Label->setText(
        tr("Enter the distance between InLine Amplifiers:"));
    Screen3_MinLaNet_Label->setText(tr("Enter the minimum load:"));
    Screen3_MaxLaNet_Label->setText(tr("Enter the maximum load:"));
    Screen3_PointResLaNet_Label->setText(tr("Points in the result (LaNet):"));
    Screen3_MinOSNR_Label->setText(tr("Enter the minimum OSNR:"));
    Screen3_MaxOSNR_Label->setText(tr("Enter the maximum OSNR:"));
    Screen3_PointResOSNR_Label->setText(tr("Points in the result (OSNR):"));
    Screen3_LaNet_Label->setText(tr("Enter the network load:"));
    Screen3_MinSPL_Label->setText(
        tr("Enter the minimum Slots per Link number:"));
    Screen3_MaxSPL_Label->setText(
        tr("Enter the maximum Slots per Link number:"));
    Screen3_MinInLineAmpDist_Label->setText(
        tr("Enter the minimum distance between inline amplifiers:"));
    Screen3_MaxInLineAmpDist_Label->setText(
        tr("Enter the maximum distance between inline amplifiers:"));
    Screen3_PointResInLineAmpDist_Label->setText(
        tr("Points in the result (Inline Amplifiers Distance):"));
    Screen3_InputPower_Label->setText(tr("Enter the input power:"));
    Screen3_RefPower_Label->setText(tr("Enter the fiber's reference power:"));
    Screen3_NodeArc_Label->setText(tr("Select the node architecture:"));
    Screen3_NodeArc_BS_RadioButton->setText(tr("Broadcast and Select"));
    Screen3_NodeArc_SS_RadioButton->setText(tr("Spectrum Switching"));
    Screen3_MinNumBloqReq_Label->setText(
        tr("Enter the minimum number of blocked requisitions before ending:"));
    Screen3_RABothDecRatio_Label->setText(
        tr("Enter the threshold ratio between FNS and FLR:"));

    Screen3_MinLaNet_Label->setToolTip(
        tr("La is the Connections Arrival Rate"));
    Screen3_MaxLaNet_Label->setToolTip(
        tr("La is the Connections Arrival Rate"));
    Screen3_LaNet_Label->setToolTip(tr("La is the Connections Arrival Rate"));
}

void MainWindow::RPAlg_Change() {
    if (Screen2_RPAlg_ComboBox->currentIndex() != _RP_Null &&
            Screen2_RPAlg_ComboBox->currentIndex() != RP_MSU) {
        Screen2_NumRegNode_Label->setEnabled(true);
        Screen2_NumRegNode_LineEdit->setEnabled(true);
    } else {
        Screen2_NumRegNode_Label->setEnabled(false);
        Screen2_NumRegNode_LineEdit->setEnabled(false);
        Screen2_NumRegNode_LineEdit->setText("");
    }
}

void MainWindow::Screen1_NextPage() {
    if (InputCheck_Screen1()) {
        main_pages->setCurrentIndex(main_pages->currentIndex() + 1);

        Screen2_InfoLayout->addRow(Screen2_SpecAssAlg_Label,
                                   Screen2_SpecAssAlg_ComboBox);

        Screen2_InfoLayout->removeItem(Screen2_BandWidth_LineEdit);
        Screen2_InfoLayout->addRow(Screen2_BandWidth_Label,
                                   Screen2_BandWidth_LineEdit);

        Screen2_InfoLayout->addRow(Screen2_mu_Label,
                                   Screen2_mu_LineEdit);

        Screen2_RoutAlg_Label->show();
        Screen2_RoutAlg_ComboBox->show();

        Screen2_InfoLayout->addRow(Screen2_RoutAlg_Label,
                                   Screen2_RoutAlg_ComboBox);

        if ((Screen1_SimulationType_ComboBox->currentIndex() != Sim_RegenNum &&
                Screen1_SimulationType_ComboBox->currentIndex() != _Sim_Null)) {

            if (Screen1_NetworkType_TL_RadioButton->isChecked()) {
                Screen2_RAAlg_Label->show();
                Screen2_RAAlg_ComboBox->show();

                Screen2_RPAlg_Label->show();
                Screen2_RPAlg_ComboBox->show();

                Screen2_TotalNumReg_Label->show();
                Screen2_TotalNumReg_LineEdit->show();
                Screen2_NumRegNode_Label->show();
                Screen2_NumRegNode_LineEdit->show();

                Screen2_InfoLayout->addRow(Screen2_RPAlg_Label,
                                           Screen2_RPAlg_ComboBox);
                Screen2_InfoLayout->addRow(Screen2_RAAlg_Label,
                                           Screen2_RAAlg_ComboBox);
                Screen2_InfoLayout->addRow(Screen2_TotalNumReg_Label,
                                           Screen2_TotalNumReg_LineEdit);
                Screen2_InfoLayout->addRow(Screen2_NumRegNode_Label,
                                           Screen2_NumRegNode_LineEdit);
            } else {
                Screen2_InfoLayout->removeWidget(Screen2_RAAlg_Label);
                Screen2_InfoLayout->removeWidget(Screen2_RAAlg_ComboBox);

                Screen2_InfoLayout->removeWidget(Screen2_RPAlg_Label);
                Screen2_InfoLayout->removeWidget(Screen2_RPAlg_ComboBox);

                Screen2_InfoLayout->removeWidget(Screen2_TotalNumReg_Label);
                Screen2_InfoLayout->removeWidget(Screen2_TotalNumReg_LineEdit);
                Screen2_InfoLayout->removeWidget(Screen2_NumRegNode_Label);
                Screen2_InfoLayout->removeWidget(Screen2_NumRegNode_LineEdit);

                Screen2_RAAlg_Label->hide();
                Screen2_RAAlg_ComboBox->hide();

                Screen2_RPAlg_Label->hide();
                Screen2_RPAlg_ComboBox->hide();

                Screen2_TotalNumReg_Label->hide();
                Screen2_TotalNumReg_LineEdit->hide();
                Screen2_NumRegNode_Label->hide();
                Screen2_NumRegNode_LineEdit->hide();
            }
        } else {
            Screen2_RAAlg_Label->show();
            Screen2_RAAlg_ComboBox->show();

            Screen2_InfoLayout->addRow(Screen2_RAAlg_Label,
                                       Screen2_RAAlg_ComboBox);

            Screen2_InfoLayout->removeWidget(Screen2_RPAlg_Label);
            Screen2_InfoLayout->removeWidget(Screen2_RPAlg_ComboBox);

            Screen2_InfoLayout->removeWidget(Screen2_TotalNumReg_Label);
            Screen2_InfoLayout->removeWidget(Screen2_TotalNumReg_LineEdit);
            Screen2_InfoLayout->removeWidget(Screen2_NumRegNode_Label);
            Screen2_InfoLayout->removeWidget(Screen2_NumRegNode_LineEdit);

            Screen2_RPAlg_Label->hide();
            Screen2_RPAlg_ComboBox->hide();

            Screen2_TotalNumReg_Label->hide();
            Screen2_TotalNumReg_LineEdit->hide();
            Screen2_NumRegNode_Label->hide();
            Screen2_NumRegNode_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_Whiskers) {
            Screen2_RepSim_Label->show();
            Screen2_RepSim_LineEdit->show();

            Screen2_InfoLayout->addRow(Screen2_RepSim_Label,
                                       Screen2_RepSim_LineEdit);
        } else {
            Screen2_InfoLayout->removeWidget(Screen2_RepSim_Label);
            Screen2_InfoLayout->removeWidget(Screen2_RepSim_LineEdit);

            Screen2_RepSim_Label->hide();
            Screen2_RepSim_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_RegenNum) {
            Screen2_MinNumRegNode_Label->show();
            Screen2_MinNumRegNode_LineEdit->show();
            Screen2_MaxNumRegNode_Label->show();
            Screen2_MaxNumRegNode_LineEdit->show();

            Screen2_InfoLayout->addRow(Screen2_MinNumRegNode_Label,
                                       Screen2_MinNumRegNode_LineEdit);
            Screen2_InfoLayout->addRow(Screen2_MaxNumRegNode_Label,
                                       Screen2_MaxNumRegNode_LineEdit);
        } else {
            Screen2_InfoLayout->removeWidget(Screen2_MinNumRegNode_Label);
            Screen2_InfoLayout->removeWidget(Screen2_MinNumRegNode_LineEdit);
            Screen2_InfoLayout->removeWidget(Screen2_MaxNumRegNode_Label);
            Screen2_InfoLayout->removeWidget(Screen2_MaxNumRegNode_LineEdit);

            Screen2_MinNumRegNode_Label->hide();
            Screen2_MinNumRegNode_LineEdit->hide();
            Screen2_MaxNumRegNode_Label->hide();
            Screen2_MaxNumRegNode_LineEdit->hide();
        }

        Screen2_InfoGroupBox->setLayout(Screen2_InfoLayout);
        Screen2_MainLayout->addWidget(Screen2_InfoGroupBox, 0,
                                      Qt::AlignCenter);
        Screen2_MainLayout->addWidget(Screen2_PageSwitchingGroupBox, 0,
                                      Qt::AlignRight);
    } else {
        QMessageBox::information(this, tr("SSON - Error"),
                                 tr("Some values were not inputted or did not matched the proper type."));
    }

}

void MainWindow::Screen2_NextPage() {
    if (InputCheck_Screen2()) {
        main_pages->setCurrentIndex(main_pages->currentIndex() + 1);

        Screen3_InfoLayout->removeItem(Screen3_InputPower_LineEdit);
        Screen3_InfoLayout->addRow(Screen3_InputPower_Label,
                                   Screen3_InputPower_LineEdit);
        Screen3_InfoLayout->removeItem(Screen3_RefPower_LineEdit);
        Screen3_InfoLayout->addRow(Screen3_RefPower_Label,
                                   Screen3_RefPower_LineEdit);

        Screen3_InfoLayout->addRow(Screen3_NodeArc_Label,
                                   Screen3_NodeArc_BS_RadioButton);
        Screen3_InfoLayout->addRow(Screen3_Spacer1,
                                   Screen3_NodeArc_SS_RadioButton);

        if (Screen1_SimulationType_ComboBox->currentIndex() == _Sim_Null ||
                Screen1_SimulationType_ComboBox->currentIndex() == Sim_AmpDist) {
            Screen3_InfoLayout->removeWidget(Screen3_InLineAmpDist_Label);
            Screen3_InfoLayout->removeItem(Screen3_InLineAmpDist_LineEdit);

            Screen3_InLineAmpDist_Label->hide();
            Screen3_InLineAmpDist_LineEdit->hide();

        } else if (Screen1_SimulationType_ComboBox->currentIndex() != Sim_AmpDist) {
            Screen3_InLineAmpDist_Label->show();
            Screen3_InLineAmpDist_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_InLineAmpDist_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_InLineAmpDist_Label,
                                       Screen3_InLineAmpDist_LineEdit);
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_PbReq) {
            Screen3_MinLaNet_Label->show();
            Screen3_MaxLaNet_Label->show();
            Screen3_PointResLaNet_Label->show();
            Screen3_MinLaNet_LineEdit->show();
            Screen3_MaxLaNet_LineEdit->show();
            Screen3_PointResLaNet_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_MinLaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MinLaNet_Label,
                                       Screen3_MinLaNet_LineEdit);
            Screen3_InfoLayout->removeItem(Screen3_MaxLaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MaxLaNet_Label,
                                       Screen3_MaxLaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_PointResLaNet_Label,
                                       Screen3_PointResLaNet_LineEdit);
        } else {
            Screen3_InfoLayout->removeWidget(Screen3_MinLaNet_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MaxLaNet_Label);
            Screen3_InfoLayout->removeWidget(Screen3_PointResLaNet_Label);
            Screen3_InfoLayout->removeItem(Screen3_MinLaNet_LineEdit);
            Screen3_InfoLayout->removeItem(Screen3_MaxLaNet_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_PointResLaNet_LineEdit);

            Screen3_MinLaNet_Label->hide();
            Screen3_MaxLaNet_Label->hide();
            Screen3_PointResLaNet_Label->hide();
            Screen3_MinLaNet_LineEdit->hide();
            Screen3_MaxLaNet_LineEdit->hide();
            Screen3_PointResLaNet_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_AmpDist ||
                Screen1_SimulationType_ComboBox->currentIndex() == Sim_OSNR) {
            Screen3_MinOSNR_Label->show();
            Screen3_MaxOSNR_Label->show();
            Screen3_PointResOSNR_Label->show();
            Screen3_MinOSNR_LineEdit->show();
            Screen3_MaxOSNR_LineEdit->show();
            Screen3_PointResOSNR_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_MinOSNR_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MinOSNR_Label,
                                       Screen3_MinOSNR_LineEdit);
            Screen3_InfoLayout->removeItem(Screen3_MaxOSNR_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MaxOSNR_Label,
                                       Screen3_MaxOSNR_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_PointResOSNR_Label,
                                       Screen3_PointResOSNR_LineEdit);
        } else {
            Screen3_InfoLayout->removeWidget(Screen3_MinOSNR_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MaxOSNR_Label);
            Screen3_InfoLayout->removeWidget(Screen3_PointResOSNR_Label);
            Screen3_InfoLayout->removeItem(Screen3_MinOSNR_LineEdit);
            Screen3_InfoLayout->removeItem(Screen3_MaxOSNR_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_PointResOSNR_LineEdit);

            Screen3_MinOSNR_Label->hide();
            Screen3_MaxOSNR_Label->hide();
            Screen3_PointResOSNR_Label->hide();
            Screen3_MinOSNR_LineEdit->hide();
            Screen3_MaxOSNR_LineEdit->hide();
            Screen3_PointResOSNR_LineEdit->hide();
        }

        if (Screen2_RAAlg_ComboBox->currentIndex() == RA_Both) {
            Screen3_RABothDecRatio_Label->show();
            Screen3_RABothDecRatio_LineEdit->show();

            Screen3_InfoLayout->addRow(Screen3_RABothDecRatio_Label,
                                       Screen3_RABothDecRatio_LineEdit);
        } else {
            Screen3_InfoLayout->removeWidget(Screen3_RABothDecRatio_Label);
            Screen3_InfoLayout->removeWidget(Screen3_RABothDecRatio_LineEdit);

            Screen3_RABothDecRatio_Label->hide();
            Screen3_RABothDecRatio_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_SlotsNum) {
            Screen3_LaNet_Label->show();
            Screen3_MinSPL_Label->show();
            Screen3_MaxSPL_Label->show();
            Screen3_LaNet_LineEdit->show();
            Screen3_MinSPL_LineEdit->show();
            Screen3_MaxSPL_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_LaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_LaNet_Label,
                                       Screen3_LaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MinSPL_Label,
                                       Screen3_MinSPL_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MaxSPL_Label,
                                       Screen3_MaxSPL_LineEdit);
        } else if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_Whiskers ||
                   Screen1_SimulationType_ComboBox->currentIndex() == Sim_RegenNum ||
                   Screen1_SimulationType_ComboBox->currentIndex() == Sim_OSNR) {

            Screen3_InfoLayout->removeWidget(Screen3_MinSPL_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MaxSPL_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MinSPL_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_MaxSPL_LineEdit);

            Screen3_MinSPL_Label->hide();
            Screen3_MaxSPL_Label->hide();
            Screen3_MinSPL_LineEdit->hide();
            Screen3_MaxSPL_LineEdit->hide();

            Screen3_LaNet_Label->show();
            Screen3_LaNet_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_LaNet_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_LaNet_Label,
                                       Screen3_LaNet_LineEdit);
        } else {
            Screen3_InfoLayout->removeWidget(Screen3_LaNet_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MinSPL_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MaxSPL_Label);
            Screen3_InfoLayout->removeItem(Screen3_LaNet_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_MinSPL_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_MaxSPL_LineEdit);

            Screen3_LaNet_Label->hide();
            Screen3_MinSPL_Label->hide();
            Screen3_MaxSPL_Label->hide();
            Screen3_LaNet_LineEdit->hide();
            Screen3_MinSPL_LineEdit->hide();
            Screen3_MaxSPL_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_AmpDist) {
            Screen3_MinInLineAmpDist_Label->show();
            Screen3_MinInLineAmpDist_LineEdit->show();
            Screen3_MaxInLineAmpDist_Label->show();
            Screen3_MaxInLineAmpDist_LineEdit->show();
            Screen3_PointResInLineAmpDist_Label->show();
            Screen3_PointResInLineAmpDist_LineEdit->show();

            Screen3_InfoLayout->removeItem(Screen3_MinInLineAmpDist_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MinInLineAmpDist_Label,
                                       Screen3_MinInLineAmpDist_LineEdit);
            Screen3_InfoLayout->removeItem(Screen3_MaxInLineAmpDist_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_MaxInLineAmpDist_Label,
                                       Screen3_MaxInLineAmpDist_LineEdit);
            Screen3_InfoLayout->addRow(Screen3_PointResInLineAmpDist_Label,
                                       Screen3_PointResInLineAmpDist_LineEdit);
        } else {
            Screen3_InfoLayout->removeWidget(Screen3_MinInLineAmpDist_Label);
            Screen3_InfoLayout->removeItem(Screen3_MinInLineAmpDist_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_MaxInLineAmpDist_Label);
            Screen3_InfoLayout->removeItem(Screen3_MaxInLineAmpDist_LineEdit);
            Screen3_InfoLayout->removeWidget(Screen3_PointResInLineAmpDist_Label);
            Screen3_InfoLayout->removeWidget(
                Screen3_PointResInLineAmpDist_LineEdit);

            Screen3_MinInLineAmpDist_Label->hide();
            Screen3_MinInLineAmpDist_LineEdit->hide();
            Screen3_MaxInLineAmpDist_Label->hide();
            Screen3_MaxInLineAmpDist_LineEdit->hide();
            Screen3_PointResInLineAmpDist_Label->hide();
            Screen3_PointResInLineAmpDist_LineEdit->hide();
        }

        if (Screen1_SimulationType_ComboBox->currentIndex() == Sim_AmpDist ||
                Screen1_SimulationType_ComboBox->currentIndex() == _Sim_Null) {
            Screen3_InfoLayout->removeWidget(Screen3_MinNumBloqReq_Label);
            Screen3_InfoLayout->removeWidget(Screen3_MinNumBloqReq_LineEdit);

            Screen3_MinNumBloqReq_Label->hide();
            Screen3_MinNumBloqReq_LineEdit->hide();
        } else {
            Screen3_MinNumBloqReq_Label->show();
            Screen3_MinNumBloqReq_LineEdit->show();

            Screen3_InfoLayout->addRow(Screen3_MinNumBloqReq_Label,
                                       Screen3_MinNumBloqReq_LineEdit);
        }

        Screen3_InfoGroupBox->setLayout(Screen3_InfoLayout);
        Screen3_MainLayout->addWidget(Screen3_InfoGroupBox, 0, Qt::AlignCenter);
        Screen3_MainLayout->addWidget(Screen3_PageSwitchingGroupBox, 0,
                                      Qt::AlignRight);
    } else {
        QMessageBox::information(this, tr("Error"),
                                 tr("Some values were not inputted or did not matched the proper type."));
    }

}

void MainWindow::Screen2_PreviousPage() {
    main_pages->setCurrentIndex(main_pages->currentIndex() - 1);

    Screen2_SpecAssAlg_ComboBox->setStyleSheet("QComboBox");
    Screen2_BandWidth_LineEdit->Text->setStyleSheet("QLineEdit");
    Screen2_mu_LineEdit->setStyleSheet("QLineEdit");
    Screen2_RoutAlg_ComboBox->setStyleSheet("QComboBox");
    Screen2_RPAlg_ComboBox->setStyleSheet("QComboBox");
    Screen2_RAAlg_ComboBox->setStyleSheet("QComboBox");
    Screen2_TotalNumReg_LineEdit->setStyleSheet("QLineEdit");
    Screen2_NumRegNode_LineEdit->setStyleSheet("QLineEdit");
}

QString MainWindow::Screen4_getGnuplotCommmand(Screen4_GraphType
        GraphType) {
    QByteArray GnuplotCommand;
    QFile Command_File;

    switch (AuxiliarMain::choose_Simulation) {
        case Sim_PbReq:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/BlockingProbability.txt");
            break;

        case Sim_OSNR:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/OSNR.txt");
            break;

        case Sim_AmpDist:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/InLineAmplifiersDistance.txt");
            break;

        case Sim_SlotsNum:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/SlotsPerLinkNumber.txt");
            break;

        case Sim_Whiskers:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/Whiskers.txt");
            break;

        case Sim_RegenNum:
            Command_File.setFileName(QString::fromStdString(AuxiliarMain::Installed_Path) +
                                     "GnuplotCommands/RegeneratorNumber.txt");
            break;

        case _Sim_Null:
            break;
    }

    Command_File.open(QIODevice::ReadOnly);
    GnuplotCommand = Command_File.readAll();
    Command_File.close();

    GnuplotCommand.replace(QByteArray("%LANETMIN"),
                           QByteArray::number((int) AuxiliarMain::LaNet_Min - 1));
    GnuplotCommand.replace(QByteArray("%LANETMAX"),
                           QByteArray::number((int) AuxiliarMain::LaNet_Max + 1));
    GnuplotCommand.replace(QByteArray("%OSNRMIN"),
                           QByteArray::number((int) AuxiliarMain::OSNR_Min - 1));
    GnuplotCommand.replace(QByteArray("%OSNRMAX"),
                           QByteArray::number((int) AuxiliarMain::OSNR_Max + 1));
    GnuplotCommand.replace(QByteArray("%DISTMIN"),
                           QByteArray::number((int) AuxiliarMain::AmpDist_Min - 2));
    GnuplotCommand.replace(QByteArray("%DISTMAX"),
                           QByteArray::number((int) AuxiliarMain::AmpDist_Max + 2));
    GnuplotCommand.replace(QByteArray("%REGNMIN"),
                           QByteArray::number((int) AuxiliarMain::RegPerNode_Min - 1));
    GnuplotCommand.replace(QByteArray("%REGNMAX"),
                           QByteArray::number((int) AuxiliarMain::RegPerNode_Max + 1));
    GnuplotCommand.replace(QByteArray("%GNUPLOTFILE"),
                           gnuplot_TempPlot->fileName().toUtf8());
    GnuplotCommand.replace(QByteArray("%RESULTSPATH"),
                           QString(QString::fromStdString(AuxiliarMain::Results_Path) +
                                   "Result.txt").toUtf8());
    GnuplotCommand.replace(QByteArray("%GREATERRESULT"),
                           QString(QString::fromStdString(AuxiliarMain::Results_Path) +
                                   "DAmpGreaterThanThreshold.txt").toUtf8());
    GnuplotCommand.replace(QByteArray("%LESSERRESULT"),
                           QString(QString::fromStdString(AuxiliarMain::Results_Path) +
                                   "DAmpLesserThanThreshold.txt").toUtf8());
    GnuplotCommand.replace(QByteArray("%BLOCKPROBXLABEL"),
                           QByteArray(tr("Network Load (Erlang)").toUtf8()));
    GnuplotCommand.replace(QByteArray("%BLOCKPROBYLABEL"),
                           QByteArray(tr("Requisition Blocking Probability").toUtf8()));
    GnuplotCommand.replace(QByteArray("%INLINEXLABEL"),
                           QByteArray(tr("In-Line Amplifiers Distance (km)").toUtf8()));
    GnuplotCommand.replace(QByteArray("%INLINEYLABEL"),
                           QByteArray(tr("Input OSNR (dB)").toUtf8()));
    GnuplotCommand.replace(QByteArray("%ABOVETHRESH"),
                           QByteArray(tr("Above Threshold").toUtf8()));
    GnuplotCommand.replace(QByteArray("%UNDERTHRESH"),
                           QByteArray(tr("Under Threshold").toUtf8()));
    GnuplotCommand.replace(QByteArray("%OSNRXLABEL"),
                           QByteArray(tr("Input OSNR (dB)").toUtf8()));
    GnuplotCommand.replace(QByteArray("%OSNRYLABEL"),
                           QByteArray(tr("Requisition Blocking Probability").toUtf8()));
    GnuplotCommand.replace(QByteArray("%REGNXLABEL"),
                           QByteArray(tr("Number of Regenerators per Node").toUtf8()));
    GnuplotCommand.replace(QByteArray("%REGNYLABEL"),
                           QByteArray(tr("Requisition Blocking Probability").toUtf8()));
    GnuplotCommand.replace(QByteArray("%SLOTSTITLE"),
                           QByteArray(tr("Reasons why Requisitions are Blocked").toUtf8()));
    GnuplotCommand.replace(QByteArray("%SLOTSXLABEL"),
                           QByteArray(tr("Number of Slots per Link").toUtf8()));
    GnuplotCommand.replace(QByteArray("%SLOTSYLABEL"),
                           QByteArray(tr("Requisition Blocking Probability").toUtf8()));
    GnuplotCommand.replace(QByteArray("%SLOTSQUALITY"),
                           QByteArray(tr("Quality").toUtf8()));
    GnuplotCommand.replace(QByteArray("%SLOTSRESOURCES"),
                           QByteArray(tr("Network Resources").toUtf8()));
    GnuplotCommand.replace(QByteArray("%WHISKERSYLABEL"),
                           QByteArray(tr("Requisition Blocking Probability").toUtf8()));

    switch (GraphType) {
        case Screen4_PNG:
            GnuplotCommand.replace(QByteArray("%GRAPHTYPE"),
                                   QByteArray("set terminal pngcairo size 600,400"));
            break;

        case Screen4_EPS:
            GnuplotCommand.replace(QByteArray("%GRAPHTYPE"),
                                   QByteArray("set terminal cairolatex colour size 4.5,3 linewidth 3"));
            break;
    }

    return (QString) GnuplotCommand;
}

int MainWindow::Screen4_getNumSteps() {
    switch (AuxiliarMain::choose_Simulation) {
        case Sim_AmpDist:
            return 1;

        case Sim_PbReq:
            return ceil((AuxiliarMain::LaNet_Max - AuxiliarMain::LaNet_Min) /
                        AuxiliarMain::LaNet_Step) + 1;

        case Sim_Whiskers:
            return Def::maxSim_Whiskers + 1;

        case Sim_OSNR:
            return ceil((AuxiliarMain::OSNR_Max - AuxiliarMain::OSNR_Min) /
                        AuxiliarMain::OSNR_Step) + 1;

        case Sim_RegenNum:
            return ceil((AuxiliarMain::RegPerNode_Max - AuxiliarMain::RegPerNode_Min) / 2) +
                   1;

        case Sim_SlotsNum:
            return ceil((AuxiliarMain::SlotsNum_Max - AuxiliarMain::SlotsNum_Min) /
                        (1.0 * AuxiliarMain::SlotsNum_Min)) + 1;

        case _Sim_Null:
            return -1;
    }

    return -1;
}

QString MainWindow::Screen4_getResultFileName() {
    switch (AuxiliarMain::choose_Simulation) {
        case Sim_PbReq:
        case Sim_OSNR:
        case Sim_Whiskers:
        case Sim_RegenNum:
            return QString("Result.txt");

        default:
            return QString("");
    }
}

void MainWindow::Screen4_getTableRowNames(QStandardItemModel
        *Model_Table) {

    switch (AuxiliarMain::choose_Simulation) {
        case Sim_PbReq:
            Model_Table->setColumnCount(5);
            Model_Table->setHorizontalHeaderItem(0,
                                                 new QStandardItem(tr("Network Load (Erlang)")));
            Model_Table->setHorizontalHeaderItem(1,
                                                 new QStandardItem(tr("Req. Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(2,
                                                 new QStandardItem(tr("Req. Acceptance Probability")));
            Model_Table->setHorizontalHeaderItem(3,
                                                 new QStandardItem(tr("Slots Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(4,
                                                 new QStandardItem(tr("Average Number of Hops in Established Routes")));
            break;

        case Sim_OSNR:
            Model_Table->setColumnCount(4);
            Model_Table->setHorizontalHeaderItem(0,
                                                 new QStandardItem(tr("Input OSNR (dB)")));
            Model_Table->setHorizontalHeaderItem(1,
                                                 new QStandardItem(tr("Req. Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(2,
                                                 new QStandardItem(tr("Slots Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(3,
                                                 new QStandardItem(tr("Average Number of Hops in Established Routes")));
            break;

        case Sim_SlotsNum:
            Model_Table->setColumnCount(4);
            Model_Table->setHorizontalHeaderItem(0,
                                                 new QStandardItem(tr("Network Load (Erlang)")));
            Model_Table->setHorizontalHeaderItem(1,
                                                 new QStandardItem(tr("Total Req. Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(2,
                                                 new QStandardItem(tr("Quality-due Req. Blocking Probability")));
            Model_Table->setHorizontalHeaderItem(3,
                                                 new QStandardItem(tr("Network Resources-due Req. Blocking Probability")));
            break;

        case Sim_RegenNum:
            Model_Table->setColumnCount(2);
            Model_Table->setHorizontalHeaderItem(0,
                                                 new QStandardItem(tr("Number of Regenerators per Node")));
            Model_Table->setHorizontalHeaderItem(1,
                                                 new QStandardItem(tr("Req. Blocking Probability")));
            break;

        case Sim_Whiskers:
            Model_Table->setColumnCount(1);
            Model_Table->setHorizontalHeaderItem(0,
                                                 new QStandardItem(tr("Req. Blocking Probability")));
            break;

        case Sim_AmpDist:
        case _Sim_Null:
            break;
    }

    Model_Table->setRowCount(0);
}

void MainWindow::Set_AddParamPage() {
    AddParamPage_MainLayout = new QVBoxLayout(addparam_page);
    AddParamPage_DialogLayout = new QHBoxLayout;
    AddParamPage_DialogGroupBox = new QGroupBox;
    AddParamPage_OkButton = new QPushButton(addparam_page);
    AddParamPage_CancelButton = new QPushButton(addparam_page);

    AddParamPage_TableView = new QTableView(addparam_page);
    AddParamPage_Model = new QStandardItemModel(9, 2, addparam_page);

    AddParamPage_numReqMax_Param = new QStandardItem;
    AddParamPage_numReqMax_Cont = new QStandardItem;
    AddParamPage_L_SSS_Param = new QStandardItem;
    AddParamPage_L_SSS_Cont = new QStandardItem;
    AddParamPage_freq_Param = new QStandardItem;
    AddParamPage_freq_Cont = new QStandardItem;
    AddParamPage_BRef_Param = new QStandardItem;
    AddParamPage_BRef_Cont = new QStandardItem;
    AddParamPage_FAmp_Param = new QStandardItem;
    AddParamPage_FAmp_Cont = new QStandardItem;
    AddParamPage_OSNRin_Param = new QStandardItem;
    AddParamPage_OSNRin_Cont = new QStandardItem;
    AddParamPage_BR_Param = new QStandardItem;
    AddParamPage_BR_Cont = new QStandardItem;

    AddParamPage_CancelButton->setText(tr("Cancel"));
    AddParamPage_OkButton->setText(tr("Ok"));

    AddParamPage_CancelButton->setFixedSize(100, 27);
    AddParamPage_OkButton->setFixedSize(100, 27);

    AddParamPage_Model->setHorizontalHeaderItem(0,
            new QStandardItem(tr("Parameter")));
    AddParamPage_Model->setHorizontalHeaderItem(1,
            new QStandardItem(tr("Content")));

    AddParamPage_numReqMax_Param->setText(tr("Maximum Number of Requisitons"));
    AddParamPage_numReqMax_Param->setEditable(false);
    AddParamPage_L_SSS_Param->setText("Losses in the SSS elements");
    AddParamPage_L_SSS_Param->setEditable(false);
    AddParamPage_freq_Param->setText("Frequency");
    AddParamPage_freq_Param->setEditable(false);
    AddParamPage_BRef_Param->setText("Reference Bandwidth");
    AddParamPage_BRef_Param->setEditable(false);
    AddParamPage_FAmp_Param->setText("Amplifier's Noise Factor");
    AddParamPage_FAmp_Param->setEditable(false);
    AddParamPage_OSNRin_Param->setText("Input OSNR");
    AddParamPage_OSNRin_Param->setEditable(false);
    AddParamPage_BR_Param->setText("Bitrate of each Regenerator");
    AddParamPage_BR_Param->setEditable(false);

    AddParamPage_Model->setItem(0, 0, AddParamPage_numReqMax_Param);
    AddParamPage_Model->setItem(0, 1, AddParamPage_numReqMax_Cont);
    AddParamPage_Model->setItem(1, 0, AddParamPage_L_SSS_Param);
    AddParamPage_Model->setItem(1, 1, AddParamPage_L_SSS_Cont);
    AddParamPage_Model->setItem(2, 0, AddParamPage_freq_Param);
    AddParamPage_Model->setItem(2, 1, AddParamPage_freq_Cont);
    AddParamPage_Model->setItem(3, 0, AddParamPage_BRef_Param);
    AddParamPage_Model->setItem(3, 1, AddParamPage_BRef_Cont);
    AddParamPage_Model->setItem(4, 0, AddParamPage_FAmp_Param);
    AddParamPage_Model->setItem(4, 1, AddParamPage_FAmp_Cont);
    AddParamPage_Model->setItem(5, 0, AddParamPage_OSNRin_Param);
    AddParamPage_Model->setItem(5, 1, AddParamPage_OSNRin_Cont);
    AddParamPage_Model->setItem(6, 0, AddParamPage_BR_Param);
    AddParamPage_Model->setItem(6, 1, AddParamPage_BR_Cont);

    AddParamPage_TableView->setModel(AddParamPage_Model);
    AddParamPage_TableView->resizeColumnsToContents();

    AddParamPage_DialogLayout->addWidget(AddParamPage_CancelButton);
    AddParamPage_DialogLayout->addWidget(AddParamPage_OkButton);
    AddParamPage_DialogLayout->setSpacing(20);
    AddParamPage_DialogGroupBox->setLayout(AddParamPage_DialogLayout);

    AddParamPage_MainLayout->setSpacing(20);
    AddParamPage_MainLayout->addWidget(AddParamPage_TableView);
    AddParamPage_MainLayout->addWidget(AddParamPage_DialogGroupBox, 0,
                                       Qt::AlignRight);
    addparam_page->setLayout(AddParamPage_MainLayout);

    addparam_page->setWindowTitle(tr("SSON - Set Additional Parameters"));
    addparam_page->setFixedSize(500, 500);

    connect(AddParamPage_OkButton, SIGNAL(clicked(bool)), this,
            SLOT(UpdateParam()));
    connect(AddParamPage_CancelButton, SIGNAL(clicked(bool)), this,
            SLOT(Close_AddParametersPage()));
}

void MainWindow::Set_OpeningPage() {
    opening_page->setFixedSize(800, 600);

    QHBoxLayout *OpeningPage_HLayout = new QHBoxLayout(opening_page);
    QHBoxLayout *OpeningPage_H2Layout = new QHBoxLayout();
    QVBoxLayout *OpeningPage_VLayout = new QVBoxLayout();

    QLabel *LabelRadiometria = new QLabel(opening_page);
    QImage *ImageRadiometria = new QImage(QString::fromStdString(
            AuxiliarMain::Installed_Path) +
                                          "Figures/Radiometria.png");
    LabelRadiometria->setPixmap(QPixmap::fromImage(ImageRadiometria->scaled(150,
                                150, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    QLabel *LabelUFCG = new QLabel(opening_page);
    QImage *ImageUFCG = new QImage(QString::fromStdString(
                                       AuxiliarMain::Installed_Path) + "Figures/UFCG.png");
    LabelUFCG->setPixmap(QPixmap::fromImage(ImageUFCG->scaled(120, 120,
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation)));


    OpeningPage_NewSimulation_Button = new QPushButton(opening_page);
    OpeningPage_Exit_Button = new QPushButton(opening_page);
    OpeningPage_OpenSimulation_Button = new QPushButton(opening_page);

    OpeningPage_NewSimulation_Button->setText(tr("New Simulation"));
    OpeningPage_Exit_Button->setText(tr("Exit"));
    OpeningPage_OpenSimulation_Button->setText(tr("Open Simulation"));

    OpeningPage_H2Layout->addWidget(LabelUFCG);
    OpeningPage_H2Layout->addSpacing(50);
    OpeningPage_H2Layout->addWidget(LabelRadiometria);

    OpeningPage_VLayout->addStretch();
    OpeningPage_VLayout->addWidget(OpeningPage_NewSimulation_Button);
    OpeningPage_VLayout->addStretch();
    OpeningPage_VLayout->addWidget(OpeningPage_OpenSimulation_Button);
    OpeningPage_VLayout->addStretch();
    OpeningPage_VLayout->addWidget(OpeningPage_Exit_Button);
    OpeningPage_VLayout->addStretch();
    OpeningPage_VLayout->addLayout(OpeningPage_H2Layout);
    OpeningPage_VLayout->addStretch();

    OpeningPage_HLayout->addStretch();
    OpeningPage_HLayout->addLayout(OpeningPage_VLayout);
    OpeningPage_HLayout->addStretch();

    connect(OpeningPage_NewSimulation_Button, SIGNAL(clicked()), this,
            SLOT(OpeningScreen_NextPage()));
    connect(OpeningPage_Exit_Button, SIGNAL(clicked()), this,
            SLOT(Close_Program()));
    connect(OpeningPage_OpenSimulation_Button, SIGNAL(clicked(bool)), this,
            SLOT(Slot_LoadSimulation_Op()));

    opening_page->setLayout(OpeningPage_HLayout);
}

void MainWindow::Set_Screen1() {
    Screen1->setFixedSize(800, 600);

    QVBoxLayout *FirstScreen_MainLayout = new QVBoxLayout(Screen1);
    QFormLayout *FirstScreen_FormLayout = new QFormLayout;
    QGroupBox *FirstScreen_GridGroupBox = new QGroupBox(Screen1);

    Screen1_NetworkType_Label = new QLabel(Screen1);
    Screen1_NetworkType_TL_RadioButton = new QRadioButton(Screen1);
    Screen1_NetworkType_TP_RadioButton = new QRadioButton(Screen1);
    Screen1_NetworkType_ButtonGroup = new QButtonGroup(Screen1);

    Screen1_Topology_Label = new QLabel(Screen1);
    Screen1_Topology_ComboBox = new QComboBox(Screen1);
    Screen1_Topology_Button = new QPushButton(Screen1);
    Screen1_Topology_GraphicsView = new QGraphicsView(Screen1);

    connect(Screen1_NetworkType_ButtonGroup,
            SIGNAL(buttonToggled(QAbstractButton *, bool)), this,
            SLOT(Povoate_SimulationType()));

    Screen1_SimulationType_Label = new QLabel(Screen1);
    Screen1_SimulationType_ComboBox = new QComboBox(Screen1);

    Screen1_NextPage_Button = new QPushButton(Screen1);

    Screen2_Spacer1 = new QLabel;
    Screen2_Spacer2 = new QLabel;

    Screen1_SimulationType_Label->setText(tr("Choose a simulation:"));
    Screen1_SimulationType_ComboBox->setEnabled(false);

    Screen1_Topology_Label->setText(tr("Choose a topology:"));
    Screen1_Topology_Button->setText(tr("Open Topology"));
    Screen1_Topology_ComboBox->addItem(tr(""));
    Screen1_Topology_ComboBox->addItem(tr("Pacific Bell"));
    Screen1_Topology_ComboBox->addItem(tr("NSFNet"));
    Screen1_Topology_ComboBox->addItem(tr("NSFNet Modified (Reduced)"));
    Screen1_Topology_ComboBox->addItem(tr("Point-To-Point with 4 Nodes"));
    Screen1_Topology_ComboBox->addItem(tr("Point-To-Point with 8 Nodes"));
    Screen1_Topology_ComboBox->addItem(tr("Top1"));
    Screen1_Topology_ComboBox->addItem(tr("Top2"));
    Screen1_Topology_ComboBox->addItem(tr("European"));
    Screen1_Topology_ComboBox->addItem(tr("German"));
    Screen1_Topology_ComboBox->addItem(tr("US Backbone"));

    Screen1_NetworkType_Label->setText(tr("Choose the network type:"));
    Screen1_NetworkType_TL_RadioButton->setText(tr("Translucent"));
    Screen1_NetworkType_TP_RadioButton->setText(tr("Transparent"));
    Screen1_NetworkType_ButtonGroup->addButton(
        Screen1_NetworkType_TL_RadioButton);
    Screen1_NetworkType_ButtonGroup->addButton(
        Screen1_NetworkType_TP_RadioButton);
    Screen1_NextPage_Button->setText(tr("Next"));

    Screen1_Topology_GraphicsView->setFixedSize(600, 300);
    Screen1_Topology_Image = new QImage;
    Screen1_Topology_GrapScene = new QGraphicsScene(this);

    Screen1_Topology_Image->load(QString::fromStdString(
                                     AuxiliarMain::Installed_Path) + "Figures/Blank.png");
    Screen1_Topology_ImagePixMap = QPixmap::fromImage(
                                       *Screen1_Topology_Image);
    Screen1_Topology_GrapScene->addPixmap(Screen1_Topology_ImagePixMap);
    Screen1_Topology_GrapScene->setSceneRect(
        Screen1_Topology_ImagePixMap.rect());
    Screen1_Topology_GraphicsView->setScene(Screen1_Topology_GrapScene);

    Screen1_SimulationType_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_SimulationType_ComboBox->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_NetworkType_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_NetworkType_TL_RadioButton->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_NetworkType_TP_RadioButton->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_Topology_Label->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen1_Topology_ComboBox->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen1_Topology_Button->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen1_NextPage_Button->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen2_Spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    Screen2_Spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    FirstScreen_FormLayout->addRow(Screen1_NetworkType_Label,
                                   Screen1_NetworkType_TL_RadioButton);
    FirstScreen_FormLayout->addRow(Screen2_Spacer1,
                                   Screen1_NetworkType_TP_RadioButton);
    FirstScreen_FormLayout->addRow(Screen1_Topology_Label,
                                   Screen1_Topology_ComboBox);
    FirstScreen_FormLayout->addRow(Screen2_Spacer2,
                                   Screen1_Topology_Button);
    FirstScreen_FormLayout->addRow(Screen1_SimulationType_Label,
                                   Screen1_SimulationType_ComboBox);
    FirstScreen_FormLayout->setSpacing(10);
    FirstScreen_GridGroupBox->setLayout(FirstScreen_FormLayout);

    FirstScreen_MainLayout->addWidget(FirstScreen_GridGroupBox, 0, Qt::AlignCenter);
    FirstScreen_MainLayout->addWidget(Screen1_Topology_GraphicsView, 0,
                                      Qt::AlignCenter);
    FirstScreen_MainLayout->addWidget(Screen1_NextPage_Button, 0,
                                      Qt::AlignRight);
    FirstScreen_MainLayout->setSpacing(20);

    connect(Screen1_NextPage_Button, SIGNAL(clicked()), this,
            SLOT(Screen1_NextPage()));
    connect(Screen1_Topology_ComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(ChangeTopologyPic()));

    Screen1->setLayout(FirstScreen_MainLayout);

}

void MainWindow::Set_Screen2() {
    Screen2->setFixedSize(800, 600);

    Screen2_MainLayout = new QVBoxLayout(Screen2);
    Screen2_PageSwitchingLayout = new QHBoxLayout;
    Screen2_PageSwitchingGroupBox = new QGroupBox;

    Screen2_InfoGroupBox = new QGroupBox;
    Screen2_InfoLayout = new QFormLayout;

    Screen2_SpecAssAlg_Label = new QLabel(Screen2);
    Screen2_SpecAssAlg_ComboBox = new QComboBox(Screen2);
    Screen2_BandWidth_Label = new QLabel(Screen2);
    Screen2_BandWidth_LineEdit = new UnitLineEdit("GHz");
    Screen2_mu_Label = new QLabel(Screen2);
    Screen2_mu_LineEdit = new QLineEdit(Screen2);
    Screen2_RoutAlg_Label = new QLabel(Screen2);
    Screen2_RoutAlg_ComboBox = new QComboBox(Screen2);
    Screen2_RAAlg_Label = new QLabel(Screen2);
    Screen2_RAAlg_ComboBox = new QComboBox(Screen2);
    Screen2_RPAlg_Label = new QLabel(Screen2);
    Screen2_RPAlg_ComboBox = new QComboBox(Screen2);
    Screen2_TotalNumReg_Label = new QLabel(Screen2);
    Screen2_TotalNumReg_LineEdit = new QLineEdit(Screen2);
    Screen2_NumRegNode_Label = new QLabel(Screen2);
    Screen2_NumRegNode_LineEdit = new QLineEdit(Screen2);
    Screen2_RepSim_Label = new QLabel(Screen2);
    Screen2_RepSim_LineEdit = new QLineEdit(Screen2);
    Screen2_MinNumRegNode_Label = new QLabel(Screen2);
    Screen2_MaxNumRegNode_Label = new QLabel(Screen2);
    Screen2_MinNumRegNode_LineEdit = new QLineEdit(Screen2);
    Screen2_MaxNumRegNode_LineEdit = new QLineEdit(Screen2);

    Screen2_NextPage_Button = new QPushButton(Screen2);
    Screen2_PreviousPage_Button = new QPushButton(Screen2);

    Screen2_BandWidth_LineEdit->Text->setValidator(new QDoubleValidator(0,
            9999, 6,
            Screen2));
    Screen2_mu_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                      Screen2));
    Screen2_TotalNumReg_LineEdit->setValidator(new QIntValidator(0, 9999,
            Screen2));
    Screen2_NumRegNode_LineEdit->setValidator(new QIntValidator(0, 9999,
            Screen2));
    Screen2_RepSim_LineEdit->setValidator(new QIntValidator(0, 65000,
                                          Screen2));
    Screen2_MinNumRegNode_LineEdit->setValidator(new QDoubleValidator(0, 9999,
            6, Screen2));
    Screen2_MaxNumRegNode_LineEdit->setValidator(new QDoubleValidator(0, 9999,
            6, Screen2));

    Screen2_NextPage_Button->setText(tr("Next"));
    Screen2_PreviousPage_Button->setText(tr("Back"));
    Screen2_NextPage_Button->setFixedSize(100, 27);
    Screen2_PreviousPage_Button->setFixedSize(100, 27);

    Screen2_RoutAlg_Label->setText(tr("Choose a routing algorithm:"));
    Screen2_RoutAlg_ComboBox->addItem(tr(""));
    Screen2_RoutAlg_ComboBox->addItem(tr("Minimum Hops"));
    Screen2_RoutAlg_ComboBox->addItem(tr("Shortest Path"));
    Screen2_RoutAlg_ComboBox->addItem(tr("LOR Allocation Possibilities"));
    Screen2_RoutAlg_ComboBox->addItem(tr("LOR Availability"));
    Screen2_RoutAlg_ComboBox->addItem(tr("OSNR-R"));

    Screen2_SpecAssAlg_Label->setText(
        tr("Choose the spectrum assigment algorithm:"));
    Screen2_SpecAssAlg_ComboBox->addItem(tr(""));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("Random"));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("FirstFit"));
    Screen2_SpecAssAlg_ComboBox->addItem(tr("Most Used"));

    Screen2_RAAlg_Label->setText(tr("Choose the RA algorithm:"));
    Screen2_RAAlg_ComboBox->addItem(tr(""));
    Screen2_RAAlg_ComboBox->addItem(tr("FNS"));
    Screen2_RAAlg_ComboBox->addItem(tr("FLR"));
    Screen2_RAAlg_ComboBox->addItem(tr("Both"));

    Screen2_RPAlg_Label->setText(tr("Choose the RP algorithm:"));
    Screen2_RPAlg_ComboBox->addItem(tr(""));
    Screen2_RPAlg_ComboBox->addItem(tr("NDF"));
    Screen2_RPAlg_ComboBox->addItem(tr("CNF"));
    Screen2_RPAlg_ComboBox->addItem(tr("TLP"));
    Screen2_RPAlg_ComboBox->addItem(tr("SQP"));
    Screen2_RPAlg_ComboBox->addItem(tr("MU"));
    Screen2_RPAlg_ComboBox->addItem(tr("MSU"));

    Screen2_RepSim_Label->setText(
        tr("How many repetitions of the simulation?"));

    Screen2_BandWidth_Label->setText(tr("Enter Slots Bandwidth:"));

    Screen2_TotalNumReg_Label->setText(
        tr("Enter the total number of regenerators:"));
    Screen2_NumRegNode_Label->setText(
        tr("Enter the number of regenerators per node:"));

    Screen2_mu_Label->setText(
        tr("Enter the connections deactivation rate:"));

    Screen2_MinNumRegNode_Label->setText(
        tr("Enter the minimum number of regenerators per node:"));
    Screen2_MaxNumRegNode_Label->setText(
        tr("Enter the maximum number of regenerators per node:"));

    Screen2_RoutAlg_Label->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Fixed);
    Screen2_RoutAlg_ComboBox->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen2_SpecAssAlg_Label->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen2_SpecAssAlg_ComboBox->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_RAAlg_Label->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    Screen2_RAAlg_ComboBox->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen2_RPAlg_Label->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    Screen2_RPAlg_ComboBox->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen2_RepSim_Label->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Fixed);
    Screen2_RepSim_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen2_BandWidth_Label->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Expanding);
    Screen2_TotalNumReg_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_TotalNumReg_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_NumRegNode_Label->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen2_NumRegNode_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_mu_Label->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Fixed);
    Screen2_mu_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    Screen2_MinNumRegNode_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_MaxNumRegNode_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_MinNumRegNode_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen2_MaxNumRegNode_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);

    Screen2_RAAlg_Label->setToolTip(tr("Regenerator Allocation Algorithm"));
    Screen2_RPAlg_Label->setToolTip(tr("Regenerator Placement Algorithm"));
    Screen2_BandWidth_Label->setToolTip(
        tr("Common values are 1.5625, 3.125, 6.25 or 12.5"));

    Screen2_NumRegNode_Label->setEnabled(false);
    Screen2_NumRegNode_LineEdit->setEnabled(false);

    Screen2_InfoLayout->setSpacing(20);

    Screen2_PageSwitchingLayout->addWidget(Screen2_PreviousPage_Button);
    Screen2_PageSwitchingLayout->addWidget(Screen2_NextPage_Button);

    Screen2_PageSwitchingGroupBox->setLayout(Screen2_PageSwitchingLayout);

    Screen2->setLayout(Screen2_MainLayout);

    connect(Screen2_NextPage_Button, SIGNAL(clicked()), this,
            SLOT(Screen2_NextPage()));
    connect(Screen2_PreviousPage_Button, SIGNAL(clicked()), this,
            SLOT(Screen2_PreviousPage()));
    connect(Screen2_RPAlg_ComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(RPAlg_Change()));

}

void MainWindow::Set_Screen3() {
    Screen3->setFixedSize(800, 600);

    Screen3_MainLayout = new QVBoxLayout(Screen3);
    Screen3_PageSwitchingLayout = new QHBoxLayout;
    Screen3_PageSwitchingGroupBox = new QGroupBox;
    Screen3_InfoLayout = new QFormLayout;
    Screen3_InfoGroupBox = new QGroupBox;

    Screen3_InputPower_Label = new QLabel(Screen3);
    Screen3_InputPower_LineEdit = new UnitLineEdit("dBm");
    Screen3_RefPower_Label = new QLabel(Screen3);
    Screen3_RefPower_LineEdit = new UnitLineEdit("dBm");

    Screen3_NodeArc_Label = new QLabel(Screen3);
    Screen3_NodeArc_BS_RadioButton = new QRadioButton(Screen3);
    Screen3_NodeArc_SS_RadioButton = new QRadioButton(Screen3);
    Screen3_NodeArc_ButtonGroup = new QButtonGroup(Screen3);
    Screen3_Spacer1 = new QLabel(Screen3);

    Screen3_InLineAmpDist_Label = new QLabel(Screen3);
    Screen3_InLineAmpDist_LineEdit = new UnitLineEdit("km");

    Screen3_MinLaNet_Label = new QLabel(Screen3);
    Screen3_MaxLaNet_Label = new QLabel(Screen3);
    Screen3_PointResLaNet_Label = new QLabel(Screen3);
    Screen3_MinLaNet_LineEdit = new UnitLineEdit("Erlang");
    Screen3_MaxLaNet_LineEdit = new UnitLineEdit("Erlang");
    Screen3_PointResLaNet_LineEdit = new QLineEdit(Screen3);

    Screen3_MinOSNR_Label = new QLabel(Screen3);
    Screen3_MaxOSNR_Label = new QLabel(Screen3);
    Screen3_PointResOSNR_Label = new QLabel(Screen3);
    Screen3_MinOSNR_LineEdit = new UnitLineEdit("dB");
    Screen3_MaxOSNR_LineEdit = new UnitLineEdit("dB");
    Screen3_PointResOSNR_LineEdit = new QLineEdit(Screen3);

    Screen3_RABothDecRatio_Label = new QLabel(Screen3);
    Screen3_RABothDecRatio_LineEdit = new QLineEdit(Screen3);

    Screen3_LaNet_Label = new QLabel(Screen3);
    Screen3_MinSPL_Label = new QLabel(Screen3);
    Screen3_MaxSPL_Label = new QLabel(Screen3);
    Screen3_LaNet_LineEdit = new UnitLineEdit("Erlang");
    Screen3_MinSPL_LineEdit = new QLineEdit(Screen3);
    Screen3_MaxSPL_LineEdit = new QLineEdit(Screen3);

    Screen3_MinInLineAmpDist_Label = new QLabel(Screen3);
    Screen3_MinInLineAmpDist_LineEdit = new UnitLineEdit("km");
    Screen3_MaxInLineAmpDist_Label = new QLabel(Screen3);
    Screen3_MaxInLineAmpDist_LineEdit = new UnitLineEdit("km");
    Screen3_PointResInLineAmpDist_Label = new QLabel(Screen3);
    Screen3_PointResInLineAmpDist_LineEdit = new QLineEdit(Screen3);

    Screen3_MinNumBloqReq_Label = new QLabel(Screen3);
    Screen3_MinNumBloqReq_LineEdit = new QLineEdit(Screen3);

    Screen3_Simulate_Button = new QPushButton(Screen3);
    Screen3_PreviousPage_Button = new QPushButton(Screen3);

    Screen3_InputPower_LineEdit->setValidator(new QDoubleValidator(-50, 9999, 6,
            Screen2));
    Screen3_RefPower_LineEdit->setValidator(new QDoubleValidator(-50, 9999, 6,
                                            Screen2));
    Screen3_InLineAmpDist_LineEdit->setValidator(new QDoubleValidator(0, 9999,
            6, Screen2));
    Screen3_MinLaNet_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                            Screen2));
    Screen3_MaxLaNet_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                            Screen2));
    Screen3_PointResLaNet_LineEdit->setValidator(new QIntValidator(0, 9999,
            Screen2));
    Screen3_MinOSNR_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                           Screen2));
    Screen3_MaxOSNR_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                           Screen2));
    Screen3_PointResOSNR_LineEdit->setValidator(new QIntValidator(0, 9999,
            Screen2));
    Screen3_LaNet_LineEdit->setValidator(new QDoubleValidator(0, 9999, 6,
                                         Screen2));
    Screen3_MinSPL_LineEdit->setValidator(new QIntValidator(0, 9999,
                                          Screen2));
    Screen3_MaxSPL_LineEdit->setValidator(new QIntValidator(0, 9999,
                                          Screen2));
    Screen3_MinInLineAmpDist_LineEdit->setValidator(new QDoubleValidator(0,
            9999, 6, Screen2));
    Screen3_MaxInLineAmpDist_LineEdit->setValidator(new QDoubleValidator(0,
            9999, 6, Screen2));
    Screen3_PointResInLineAmpDist_LineEdit->setValidator(new QIntValidator(0,
            9999, Screen2));
    Screen3_MinNumBloqReq_LineEdit->setValidator(new QDoubleValidator(0, 9999,
            6, Screen2));
    Screen3_RABothDecRatio_LineEdit->setValidator(new QDoubleValidator(0, 1, 6,
            Screen2));

    Screen3_Simulate_Button->setText(tr("Simulate"));
    Screen3_PreviousPage_Button->setText(tr("Back"));

    Screen3_InLineAmpDist_Label->setText(
        tr("Enter the distance between InLine Amplifiers:"));

    Screen3_MinLaNet_Label->setText(tr("Enter the minimum load:"));
    Screen3_MaxLaNet_Label->setText(tr("Enter the maximum load:"));
    Screen3_PointResLaNet_Label->setText(tr("Points in the result (LaNet):"));

    Screen3_MinOSNR_Label->setText(tr("Enter the minimum OSNR:"));
    Screen3_MaxOSNR_Label->setText(tr("Enter the maximum OSNR:"));
    Screen3_PointResOSNR_Label->setText(tr("Points in the result (OSNR):"));

    Screen3_LaNet_Label->setText(tr("Enter the network load:"));

    Screen3_MinSPL_Label->setText(
        tr("Enter the minimum Slots per Link number:"));
    Screen3_MaxSPL_Label->setText(
        tr("Enter the maximum Slots per Link number:"));

    Screen3_MinInLineAmpDist_Label->setText(
        tr("Enter the minimum distance between inline amplifiers:"));
    Screen3_MaxInLineAmpDist_Label->setText(
        tr("Enter the maximum distance between inline amplifiers:"));
    Screen3_PointResInLineAmpDist_Label->setText(
        tr("Points in the result (Inline Amplifiers Distance):"));

    Screen3_InputPower_Label->setText(tr("Enter the input power:"));
    Screen3_RefPower_Label->setText(
        tr("Enter the fiber's reference power:"));

    Screen3_NodeArc_Label->setText(tr("Select the node architecture:"));
    Screen3_NodeArc_BS_RadioButton->setText(tr("Broadcast and Select"));
    Screen3_NodeArc_SS_RadioButton->setText(tr("Spectrum Switching"));
    Screen3_NodeArc_ButtonGroup->addButton(Screen3_NodeArc_BS_RadioButton);
    Screen3_NodeArc_ButtonGroup->addButton(Screen3_NodeArc_SS_RadioButton);

    Screen3_MinNumBloqReq_Label->setText(
        tr("Enter the minimum number of blocked requisitions before ending:"));

    Screen3_RABothDecRatio_Label->setText(
        tr("Enter the threshold ratio between FNS and FLR:"));

    Screen3_Simulate_Button->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen3_PreviousPage_Button->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_InLineAmpDist_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_InLineAmpDist_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinLaNet_Label->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen3_MaxLaNet_Label->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen3_PointResLaNet_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinLaNet_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MaxLaNet_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_PointResLaNet_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinOSNR_Label->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Fixed);
    Screen3_MaxOSNR_Label->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Fixed);
    Screen3_PointResOSNR_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinOSNR_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen3_MaxOSNR_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen3_PointResOSNR_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_LaNet_Label->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    Screen3_MinSPL_Label->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Fixed);
    Screen3_MaxSPL_Label->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Fixed);
    Screen3_LaNet_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen3_MinSPL_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen3_MaxSPL_LineEdit->setSizePolicy(QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    Screen3_MinInLineAmpDist_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinInLineAmpDist_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MaxInLineAmpDist_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MaxInLineAmpDist_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_PointResInLineAmpDist_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_PointResInLineAmpDist_LineEdit->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Fixed);
    Screen3_InputPower_Label->setSizePolicy(QSizePolicy::Expanding,
                                            QSizePolicy::Fixed);
    Screen3_InputPower_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_RefPower_Label->setSizePolicy(QSizePolicy::Expanding,
                                          QSizePolicy::Fixed);
    Screen3_RefPower_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_NodeArc_Label->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Fixed);
    Screen3_NodeArc_BS_RadioButton->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_NodeArc_SS_RadioButton->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_Spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    Screen3_MinNumBloqReq_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_MinNumBloqReq_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_RABothDecRatio_Label->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);
    Screen3_RABothDecRatio_LineEdit->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Fixed);

    Screen3_MinLaNet_Label->setToolTip(
        tr("La is the Connections Arrival Rate"));
    Screen3_MaxLaNet_Label->setToolTip(
        tr("La is the Connections Arrival Rate"));
    Screen3_LaNet_Label->setToolTip(tr("La is the Connections Arrival Rate"));

    Screen3_InfoLayout->setSpacing(10);

    Screen3_PageSwitchingLayout->addWidget(Screen3_PreviousPage_Button);
    Screen3_PageSwitchingLayout->addWidget(Screen3_Simulate_Button);

    Screen3_PageSwitchingGroupBox->setLayout(Screen3_PageSwitchingLayout);

    Screen3->setLayout(Screen3_MainLayout);

    connect(Screen3_Simulate_Button, SIGNAL(clicked()), this,
            SLOT(Simulate()));
    connect(Screen3_PreviousPage_Button, SIGNAL(clicked()), this,
            SLOT(PreviousPage()));
}

void MainWindow::Set_Screen4() {
    Screen4_LastPlotted.start();
    Screen4_LastRefreshedProgressBar.start();

    Screen4->setMinimumSize(800, 600);

    Screen4_HorLayout = new QHBoxLayout(Screen4);
    Screen4_HorLayout_2 = new QHBoxLayout();
    Screen4_VerLayout = new QVBoxLayout();

    Screen4_ReplotSaveButton = new QPushButton(Screen4);
    Screen4_ReplotSaveButton->setText(tr("Replot"));
    Screen4_ReplotSaveButton->setMinimumSize(150, 50);
    Screen4_ReplotSaveButton->setMaximumSize(100, 50);

    Screen4_ButtonToggleToTable = new QPushButton(Screen4);
    Screen4_ButtonToggleToTable->setText(tr("Table"));
    Screen4_ButtonToggleToTable->setMinimumSize(150, 50);
    Screen4_ButtonToggleToTable->setMaximumSize(150, 50);

    Screen4_ButtonToggleToGraph = new QPushButton(Screen4);
    Screen4_ButtonToggleToGraph->setText(tr("Graph"));
    Screen4_ButtonToggleToGraph->setMinimumSize(150, 50);
    Screen4_ButtonToggleToGraph->setMaximumSize(150, 50);
    Screen4_ButtonToggleToGraph->setHidden(true);

    Screen4_ResultsTable = new QTableView(Screen4);
    Screen4_ResultsTable->setHidden(true);
    Screen4_ResultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Screen4_ResultsTable->setMinimumSize(600, 400);
    Screen4_ResultsTable->setMaximumSize(600, 400);

    Screen4_PlotScreen = new QLabel(this);
    Screen4_PlotScreen->setMinimumSize(600, 400);
    Screen4_PlotScreen->setMaximumSize(600, 400);

    Screen4_ProgressBar = new QProgressBar(this);
    Screen4_ProgressBar->setMinimum(0);
    Screen4_ProgressBar->setValue(0);

    Screen4_HorLayout->addStretch();
    Screen4_HorLayout->addLayout(Screen4_VerLayout);
    Screen4_HorLayout->addStretch();
    Screen4_HorLayout_2->addStretch();
    Screen4_HorLayout_2->addWidget(Screen4_ReplotSaveButton);
    Screen4_HorLayout_2->addStretch();

    if (AuxiliarMain::choose_Simulation != Sim_AmpDist) {
        Screen4_HorLayout_2->addWidget(Screen4_ButtonToggleToTable);
        Screen4_HorLayout_2->addWidget(Screen4_ButtonToggleToGraph);
        Screen4_HorLayout_2->addStretch();
    } else {
        Screen4_ButtonToggleToTable->setHidden(true);
    }

    Screen4_VerLayout->addStretch();
    Screen4_VerLayout->addWidget(Screen4_PlotScreen);
    Screen4_VerLayout->addWidget(Screen4_ResultsTable);
    Screen4_VerLayout->addStretch();
    Screen4_VerLayout->addWidget(Screen4_ProgressBar);
    Screen4_VerLayout->addStretch();
    Screen4_VerLayout->addLayout(Screen4_HorLayout_2);
    Screen4_VerLayout->addStretch();
    Slot_Screen4_RefreshTable();

    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_RefreshGraph()));
    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_RefreshTable()));

    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ButtonToggleToTable, SLOT(show()));
    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ButtonToggleToGraph, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ButtonToggleToTable, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ButtonToggleToGraph, SLOT(show()));

    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_PlotScreen, SLOT(show()));
    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ResultsTable, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_PlotScreen, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ResultsTable, SLOT(show()));

    Screen4_PlotScreen->setPixmap(QPixmap(gnuplot_TempPlot->fileName()));

    Screen4->setLayout(Screen4_HorLayout);
}

void MainWindow::ShowParam() {
    double AuxDoub;

    AuxDoub = Def::get_NumReqMax();
    AddParamPage_numReqMax_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Def::get_LSSS();
    AddParamPage_L_SSS_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Def::get_freq();
    AddParamPage_freq_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Def::get_BRef();
    AddParamPage_BRef_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Def::get_FAmp();
    AddParamPage_FAmp_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Def::get_OSRNin();
    AddParamPage_OSNRin_Cont->setText(QString::number(AuxDoub));
    AuxDoub = Regenerators::get_BR();
    AddParamPage_BR_Cont->setText(QString::number(AuxDoub));

    AddParamPage_TableView->resizeColumnsToContents();
}

void MainWindow::Show_Screen4() {
    choose_lang_menu->setDisabled(true);

    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_RefreshGraph()));
    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_RefreshTable()));

    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ButtonToggleToTable, SLOT(show()));
    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ButtonToggleToGraph, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ButtonToggleToTable, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ButtonToggleToGraph, SLOT(show()));

    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_PlotScreen, SLOT(show()));
    connect(Screen4_ButtonToggleToGraph, SIGNAL(released()),
            Screen4_ResultsTable, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_PlotScreen, SLOT(hide()));
    connect(Screen4_ButtonToggleToTable, SIGNAL(released()),
            Screen4_ResultsTable, SLOT(show()));

    Screen4_PlotScreen->setPixmap(QPixmap(gnuplot_TempPlot->fileName()));

    Screen4_ReplotSaveButton->setText(tr("Replot"));
    Screen4_ReplotSaveButton->setHidden(false);
    Screen4_ButtonToggleToTable->setText(tr("Table"));
    Screen4_ButtonToggleToTable->setHidden(false);
    Screen4_ButtonToggleToGraph->setText(tr("Graph"));
    Screen4_ButtonToggleToGraph->setHidden(true);
}

void MainWindow::Simulate() {
    if (InputCheck_Screen3()) {
        QMessageBox *Warning = new QMessageBox(QMessageBox::Warning,
                                               tr("SSON - Warning"),
                                               tr("The simulation is about to start.\nDo you want to save your changes?"),
                                               QMessageBox::Save | QMessageBox::No
                                               | QMessageBox::Cancel,
                                               this);
        Warning->setButtonText(QMessageBox::Save, tr("Save"));
        Warning->setButtonText(QMessageBox::No, tr("No"));
        Warning->setButtonText(QMessageBox::Cancel, tr("Cancel"));

        int Resp = Warning->exec();

        delete Warning;

        if (Resp == QMessageBox::Save) {
            Slot_SaveSimulation();

            std::thread Thread(exec_Interface);
            Thread.detach();
            Show_Screen4();
            main_pages->setCurrentIndex(main_pages->currentIndex() + 1);
        } else if (Resp == QMessageBox::No) {
            std::thread Thread(exec_Interface);
            Thread.detach();
            Show_Screen4();
            main_pages->setCurrentIndex(main_pages->currentIndex() + 1);
        }
    } else {
        QMessageBox::information(this, tr("Error"),
                                 tr("Some values were not inputted or did not matched the proper type."));
    }

}

void MainWindow::Simulation_Ended() {
    this->trigger_SimulationEndedActions();
}

void MainWindow::Slot_LoadDefaultValues() {
    QString FileName = QString::fromStdString(AuxiliarMain::Installed_Path) +
                       "Misc/Default.sson";

    if (main_pages->currentIndex() == 0) {
        SaveLoadInfo::LoadInfo(this, FileName);

        if (!SaveLoadInfo::CurrentFile.isEmpty()) {
            if (main_pages->currentIndex() == 0) {
                main_pages->setCurrentIndex(1);
            }
        }
    } else if (main_pages->currentIndex() == 4)  {
        QMessageBox::information(this, tr("SSON - Error"),
                                 tr("There is a simulation already running."));
    } else {
        SaveLoadInfo::LoadInfo(this, FileName);
        main_pages->setCurrentIndex(1);
    }
}

void MainWindow::Slot_LoadSimulation() {
    if (main_pages->currentIndex() == 0) {
        SaveLoadInfo::LoadInfo(this);

        if (!SaveLoadInfo::CurrentFile.isEmpty()) {
            if (main_pages->currentIndex() == 0) {
                main_pages->setCurrentIndex(1);
            }
        }
    } else if (main_pages->currentIndex() == 4)  {
        QMessageBox::information(this, tr("SSON - Error"),
                                 tr("There is a simulation already running."));
    } else {
        SaveLoadInfo::LoadInfo(this);
        main_pages->setCurrentIndex(1);
    }
}

void MainWindow::Slot_LoadSimulation_Op() {
    SaveLoadInfo::LoadInfo(this);

    if (!SaveLoadInfo::CurrentFile.isEmpty()) {
        main_pages->setCurrentIndex(main_pages->currentIndex() + 1);
    }
}

void MainWindow::Slot_NewSimulation() {
    if (main_pages->currentIndex() != 4) {

        QMessageBox *Warning = new QMessageBox(QMessageBox::Warning,
                                               tr("SSON - Warning"),
                                               tr("You are about to start a new simulation.\nDo you want to save your changes?"),
                                               QMessageBox::Save | QMessageBox::No
                                               | QMessageBox::Cancel,
                                               this);
        Warning->setButtonText(QMessageBox::Save, tr("Save"));
        Warning->setButtonText(QMessageBox::No, tr("No"));
        Warning->setButtonText(QMessageBox::Cancel, tr("Cancel"));

        int Resp = Warning->exec();

        delete Warning;

        if (Resp == QMessageBox::Save) {
            SaveLoadInfo::SaveInfo(this);

            ClearSimulation();
            main_pages->setCurrentIndex(1);
        } else if (Resp == QMessageBox::No) {
            ClearSimulation();
            main_pages->setCurrentIndex(1);
        }
    } else {
        QMessageBox::information(this, tr("SSON - Information"),
                                 tr("There is a simulation running, wait for it to finish."));
    }
}

void MainWindow::Slot_SaveAsSimulation() {
    if (main_pages->currentIndex() == 0) {
        QMessageBox::information(this, tr("SSON - Information"),
                                 tr("You have not started a simulation."));
    } else {
        SaveLoadInfo::SaveAsInfo(this);
    }
}
void MainWindow::Slot_SaveSimulation() {
    SaveLoadInfo::SaveInfo(this);
}

bool MainWindow::Slot_Screen4_RefreshGraph(long double WaitTime,
        Screen4_GraphType GraphType) {
    if (Screen4_LastPlotted.elapsed() > 1000) {
        Screen4_LastPlotted.restart();
        string Executable_Path;
        Executable_Path = "gnuplot";

        QString Command = Screen4_getGnuplotCommmand(GraphType);
        FILE *fp;
        fp = popen(Executable_Path.c_str(), "w");
        fprintf(fp, Command.toStdString().c_str());

        if (GraphType == Screen4_EPS) {
            fprintf(fp, "unset output\n");
        }

        fflush(fp);
        Delay(WaitTime);
        fprintf(fp, "quit\n");

        if (GraphType == Screen4_PNG) {
            Screen4_PlotScreen->setPixmap(QPixmap(gnuplot_TempPlot->fileName()));
        }

        return true;
    }

    return false;
}

void MainWindow::Slot_Screen4_RefreshProgressBar() {
    MainWindow::ProgressBarCont ++;

    if (Screen4_LastRefreshedProgressBar.elapsed() > 500 &&
            AuxiliarMain::SupremeFlag) {
        Screen4_LastRefreshedProgressBar.restart();
        Screen4_ProgressBar->setMaximum(Screen4_getNumSteps());
        Screen4_ProgressBar->setValue(MainWindow::ProgressBarCont);
        QCoreApplication::processEvents();
    }
}

void MainWindow::Slot_Screen4_RefreshTable() {
    QFile Result_Stream(QString::fromStdString(
                            AuxiliarMain::Results_Path) + Screen4_getResultFileName());
    Result_Stream.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream Input_ResultStream(&Result_Stream);

    QStandardItemModel *Model_Table = new QStandardItemModel();
    Screen4_getTableRowNames(Model_Table);

    int NumLines = 0;

    while (!Input_ResultStream.atEnd()) {
        QString Line = Input_ResultStream.readLine();
        QStringList Fields = Line.split("\t");

        for (int Col = 0; Col < Model_Table->columnCount(); Col++) {
            QStandardItem *Table_Item = new QStandardItem(Fields[Col]);
            Table_Item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
            Model_Table->setItem(NumLines, Col, Table_Item);
        }

        NumLines++;
    }

    Result_Stream.close();
    Screen4_ResultsTable->setModel(Model_Table);
    Screen4_ResultsTable->resizeColumnsToContents();
}

void MainWindow::Slot_Screen4_SaveGraph() {
    if (Screen4_ButtonToggleToGraph->isHidden()) {
        QFileDialog SaveDialog(this);
        QString Filter;
        SaveDialog.setDefaultSuffix(".png");
        QString GraphPath = SaveDialog.getSaveFileName(this, tr("Export Graph As..."),
                            QDir::homePath(), tr("Image Files (*.png);;TeX Files (*.tex)"), &Filter);

        if (GraphPath != "") {
            if (Filter == "TeX Files (*.tex)") {
                if (!GraphPath.endsWith(".tex")) {
                    GraphPath.append(".tex");
                }
            } else {
                if (!GraphPath.endsWith(".png")) {
                    GraphPath.append(".png");
                }
            }

            QString oldFileName = gnuplot_TempPlot->fileName();
            gnuplot_TempPlot->close();
            gnuplot_TempPlot->setFileName(GraphPath);
            gnuplot_TempPlot->open();

            if (Filter == "TeX Files (*.tex)") {
                Slot_Screen4_RefreshGraph(1000, Screen4_EPS);
            } else if (Filter == "Image Files (*.png)") {
                Slot_Screen4_RefreshGraph(1000, Screen4_PNG);
            }

            while (Screen4_LastPlotted.elapsed() < 2000) {
                Delay(100);
            }

            gnuplot_TempPlot->close();
            gnuplot_TempPlot->setFileName(oldFileName);
            gnuplot_TempPlot->open();

            Slot_Screen4_RefreshGraph(1000, Screen4_PNG);
        }
    }
}

void MainWindow::Slot_Screen4_SaveTable() {
    if (Screen4_ButtonToggleToTable->isHidden()) {
        QFileDialog SaveDialog(this);
        SaveDialog.setDefaultSuffix(".txt");
        QString ResultsPath = SaveDialog.getSaveFileName(this, tr("Export Results"),
                              QDir::homePath(), tr("Text Files (*.txt)"));

        if (ResultsPath != "") {
            if (!ResultsPath.endsWith(".txt")) {
                ResultsPath .append(".txt");
            }

            QFile::copy(QString::fromStdString(AuxiliarMain::Results_Path) +
                        Screen4_getResultFileName(), ResultsPath);
        }
    }
}

void MainWindow::Slot_Screen4_SimulationEnded() {
    Screen4_ReplotSaveButton->setText(tr("Export As..."));
    Screen4_ReplotSaveButton->adjustSize();

    disconnect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
               SLOT(Slot_Screen4_RefreshGraph()));
    disconnect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
               SLOT(Slot_Screen4_RefreshTable()));

    while (Screen4_LastPlotted.elapsed() < 1000) {
        Delay(100);
    }

    Slot_Screen4_RefreshGraph(1000);
    Slot_Screen4_RefreshTable();

    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_SaveGraph()));
    connect(Screen4_ReplotSaveButton, SIGNAL(released()), this,
            SLOT(Slot_Screen4_SaveTable()));

    Screen4_ProgressBar->setValue(Screen4_ProgressBar->maximum());
    Screen4_ProgressBar->repaint();

}

bool MainWindow::TextCheck_Double(QString text) {
    int ErrorCount = 0, DotCount = 0, Cont, DecPartSize = 0 ;
    bool Flag = false;

    if (text == "") {
        ErrorCount++;
    }

    for (Cont = 0; Cont <= text.length() ; Cont++) {
        if (text[Cont] == ' ') {
            ErrorCount++;
        }
    }

    for (Cont = 0; Cont <= text.length() ; Cont++) {
        if (text[Cont] == '.') {
            DotCount++;
        }
    }

    for (Cont = 0; Cont <= text.length() ; Cont++) {
        if (text[Cont] == '.') {
            Flag = true;
        } else if (Flag) {
            DecPartSize++;
        }
    }

    if (ErrorCount != 0 || DotCount > 1 || DecPartSize > 7) {
        return true;
    } else {
        return false;
    }
}

bool MainWindow::TextCheck_Int(QString text) {
    int ErrorCount = 0;

    if (text == "") {
        ErrorCount++;
    }

    for (int cont = 0; cont <= text.length(); cont++) {
        if (text[cont] == '.') {
            ErrorCount++;
        }
    }

    if (ErrorCount != 0) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::Translate_SSON() {
    for (map<The_Translations, QAction *>::iterator iter = edit_Languages.begin();
            iter != edit_Languages.end(); ++iter) {
        if (QObject::sender() == iter->second) {
            AuxiliarMain::TheGraphicThread->app->removeTranslator(
                &SSON_Thread::Translators[AuxiliarMain::Translation]->The_Translator);
            AuxiliarMain::Translation = iter->first;
            AuxiliarMain::TheGraphicThread->app->installTranslator(
                &SSON_Thread::Translators[iter->first]->The_Translator);
            retranslateUi();
            about_page->retranslateUi();
        }
    }
}

void MainWindow::UpdateParam() {
    Def::set_NumReqMax(AddParamPage_numReqMax_Cont->text().toDouble());
    Def::set_LSSS(AddParamPage_L_SSS_Cont->text().toDouble());
    Def::set_freq(AddParamPage_freq_Cont->text().toDouble());
    Def::set_BRef_Norm(AddParamPage_BRef_Cont->text().toDouble());
    Def::set_FAmp(AddParamPage_FAmp_Cont->text().toDouble());
    Def::set_OSNRin(AddParamPage_OSNRin_Cont->text().toDouble());
    Regenerators::set_BR(AddParamPage_BR_Cont->text().toDouble());

    addparam_page->close();
}
