#include "SaveLoadInfo.h"

using namespace std;

QString SaveLoadInfo::CurrentFile = "";

SaveLoadInfo::SaveLoadInfo() {}

void SaveLoadInfo::SaveAsInfo(MainWindow *window) {
    QString FileName;

    FileName = QFileDialog::getSaveFileName(
                   window,
                   tr("SSON - Save as"),
                   QDir::homePath(),
                   tr("SSON Files (*.sson)"));

    if (FileName.isEmpty()) {
        FileName = tr("Simulation Data");
    }

    if (!FileName.endsWith(".sson")) {
        FileName.append(".sson");
    }

    SaveLoadInfo::CurrentFile = FileName;
    SaveLoadInfo::SaveFile(window);
}

void SaveLoadInfo::SaveInfo(MainWindow *window) {
    SaveLoadInfo::SaveAsInfo(window);
}

void SaveLoadInfo::SaveFile(MainWindow *window) {
    QFile SaveFile(SaveLoadInfo::CurrentFile);
    QTextStream Out(&SaveFile);

    if (SaveFile.open(QFile::ReadWrite | QFile::Truncate)) {
        Out << tr("- Simulation File - Do not change the values in this file, bad things happen to people who do this, or the simulation just may not work.")
            << endl << endl;

        //First Screen information
        if (window->Screen1_NetworkType_TL_RadioButton->isChecked()) {
            Out << "NetworkType=" << TranslucentNetwork << endl;
        } else if (window->Screen1_NetworkType_TP_RadioButton->isChecked()) {
            Out << "NetworkType=" << TransparentNetwork << endl;
        } else {
            Out << "NetworkType=" << _Network_Null << endl;
        }

        Out << "TopologyType=" << window->Screen1_Topology_ComboBox->currentIndex()
            << endl;
        Out << "SimulationType=" <<
            window->Screen1_SimulationType_ComboBox->currentIndex() << endl;

        //Second Screen information
        Out << "RoutingAlgorithm=" <<
            window->Screen2_RoutAlg_ComboBox->currentIndex() << endl;
        Out << "SpectrumAssigmentAlgorithm=" <<
            window->Screen2_SpecAssAlg_ComboBox->currentIndex() << endl;
        Out << "RAAlgorithm=" << window->Screen2_RAAlg_ComboBox->currentIndex() <<
            endl;
        Out << "RPAlgorithm=" << window->Screen2_RPAlg_ComboBox->currentIndex() <<
            endl;
        Out << "SimulationRepetitions=" << window->Screen2_RepSim_LineEdit->text()
            << endl;
        Out << "SlotsBandwidth=" << SaveLoadInfo::CorrectText(
                window->Screen2_BandWidth_LineEdit->Text->text()) << endl;
        Out << "TotalNumRegenerators=" <<
            window->Screen2_TotalNumReg_LineEdit->text() << endl;
        Out << "NumRegeneratorsNode=" <<
            window->Screen2_NumRegNode_LineEdit->text() << endl;
        Out << "mu=" << SaveLoadInfo::CorrectText(
                window->Screen2_mu_LineEdit->text()) << endl;
        Out << "MinNumRegeneratorsNode=" << SaveLoadInfo::CorrectText(
                window->Screen2_MinNumRegNode_LineEdit->text()) << endl;
        Out << "MaxNumRegeneratorsNode=" << SaveLoadInfo::CorrectText(
                window->Screen2_MaxNumRegNode_LineEdit->text()) << endl;

        //Third Screen information
        Out << "DistInlineAmp=" << SaveLoadInfo::CorrectText(
                window->Screen3_InLineAmpDist_LineEdit->text()) << endl;
        Out << "MinLaNet=" << SaveLoadInfo::CorrectText(
                window->Screen3_MinLaNet_LineEdit->text()) << endl;
        Out << "MaxLaNet=" << SaveLoadInfo::CorrectText(
                window->Screen3_MaxLaNet_LineEdit->text()) << endl;
        Out << "PointsLaNet=" << window->Screen3_PointResLaNet_LineEdit->text() <<
            endl;
        Out << "MinOSNR=" << SaveLoadInfo::CorrectText(
                window->Screen3_MinOSNR_LineEdit->text()) << endl;
        Out << "MaxOSNR=" << SaveLoadInfo::CorrectText(
                window->Screen3_MaxOSNR_LineEdit->text()) << endl;
        Out << "PointsOSNR=" << window->Screen3_PointResOSNR_LineEdit->text() <<
            endl;
        Out << "LaNet=" << SaveLoadInfo::CorrectText(
                window->Screen3_LaNet_LineEdit->text()) << endl;
        Out << "MinSPL=" << window->Screen3_MinSPL_LineEdit->text() << endl;
        Out << "MaxSPL=" << window->Screen3_MaxSPL_LineEdit->text() << endl;
        Out << "RABothDecRatio=" << window->Screen3_RABothDecRatio_LineEdit->text()
            << endl;
        Out << "MinInlineAmpDist=" << SaveLoadInfo::CorrectText(
                window->Screen3_MinInLineAmpDist_LineEdit->text()) << endl;
        Out << "MaxInlineAmpDist=" << SaveLoadInfo::CorrectText(
                window->Screen3_MaxInLineAmpDist_LineEdit->text()) << endl;
        Out << "PointsInlineAmpDist=" <<
            window->Screen3_PointResInLineAmpDist_LineEdit->text() << endl;
        Out << "InputPower=" << SaveLoadInfo::CorrectText(
                window->Screen3_InputPower_LineEdit->text()) << endl;
        Out << "RefPower=" << SaveLoadInfo::CorrectText(
                window->Screen3_RefPower_LineEdit->text()) << endl;

        if (window->Screen3_NodeArc_BS_RadioButton->isChecked()) {
            Out << "NodeArc=" << Arch_BS << endl;
        } else if (window->Screen3_NodeArc_SS_RadioButton->isChecked()) {
            Out << "NodeArc=" << Arch_SS << endl;
        } else {
            Out << "NodeArc=" << Arch_Null << endl;
        }

        Out << "MinNumBloqRed=" << SaveLoadInfo::CorrectText(
                window->Screen3_MinNumBloqReq_LineEdit->text()) << endl;

        //Parameters information
        Out << "NumReqMax=" << window->AddParamPage_numReqMax_Cont->text() << endl;
        Out << "LSSS=" << window->AddParamPage_L_SSS_Cont->text() << endl;
        Out << "freq=" << window->AddParamPage_freq_Cont->text() << endl;
        Out << "BRef=" << window->AddParamPage_BRef_Cont->text() << endl;
        Out << "FAmp=" << window->AddParamPage_FAmp_Cont->text() << endl;
        Out << "OSNRin=" << window->AddParamPage_OSNRin_Cont->text() << endl;
        Out << "BR=" << window->AddParamPage_BR_Cont->text() << endl;

        SaveFile.close();
    } else {
        QMessageBox::warning(
            window,
            tr("Error"), tr("File could not be open. ") + SaveFile.errorString());
    }
}

void SaveLoadInfo::LoadInfo(MainWindow *window, QString FileName) {

    if (FileName == "") {
        FileName = QFileDialog::getOpenFileName(
                       window,
                       tr("SSON - Open file"),
                       QDir::homePath(),
                       tr("SSON Files (*.sson)"));
    }

    QFile LoadFile(FileName);

    if (!FileName.isEmpty()) {
        if (LoadFile.open(QFile::ReadOnly)) {
            LoadFile.close();
            SaveLoadInfo::LoadFile(FileName, window);
        } else {
            QMessageBox::warning(window, tr("Error"),
                                 tr("File could not be open. ") + LoadFile.errorString());
        }
    }

}

void SaveLoadInfo::LoadFile(QString FileName, MainWindow *window) {
    SaveLoadInfo::CurrentFile = FileName;
    QFile LoadFile(SaveLoadInfo::CurrentFile);
    QTextStream Inp(&LoadFile);
    QString Aux;
    std::string Key = "", Value = "", Temp;
    std::map <std::string, std::string> Info;
    map<std::string, std::string>::iterator InfoInterator;
    int count = 1;

    LoadFile.open(QFile::ReadOnly);

    do {
        Inp >> Aux;

        if (count >= 3) {
            SaveLoadInfo::WordSeparator(Aux, &Key, &Value);
            Info[Key] = Value;
        }

        count++;
    } while (!Aux.isNull());

    //First Screen information
    InfoInterator = Info.find("NetworkType");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);

    if (Aux.toInt() == TranslucentNetwork) {
        window->Screen1_NetworkType_TL_RadioButton->setChecked(true);
    } else if (Aux.toInt() == TransparentNetwork) {
        window->Screen1_NetworkType_TP_RadioButton->setChecked(true);
    } else if (Aux.toInt() == _Network_Null) {
        if (window->Screen1_NetworkType_TL_RadioButton->isChecked() ||
                window->Screen1_NetworkType_TP_RadioButton->isChecked()) {
            window->Screen1_NetworkType_ButtonGroup->setExclusive(false);
            window->Screen1_NetworkType_ButtonGroup->checkedButton()->setChecked(false);
            window->Screen1_NetworkType_ButtonGroup->setExclusive(true);
        }
    }

    InfoInterator = Info.find("TopologyType");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen1_Topology_ComboBox->setCurrentIndex(Aux.toInt());

    InfoInterator = Info.find("SimulationType");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen1_SimulationType_ComboBox->setCurrentIndex(Aux.toInt());

    //Second Screen information
    InfoInterator = Info.find("RoutingAlgorithm");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_RoutAlg_ComboBox->setCurrentIndex(Aux.toInt());

    InfoInterator = Info.find("SpectrumAssigmentAlgorithm");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_SpecAssAlg_ComboBox->setCurrentIndex(Aux.toInt());

    InfoInterator = Info.find("RAAlgorithm");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_RAAlg_ComboBox->setCurrentIndex(Aux.toInt());

    InfoInterator = Info.find("RPAlgorithm");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_RPAlg_ComboBox->setCurrentIndex(Aux.toInt());

    InfoInterator = Info.find("SimulationRepetitions");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_RepSim_LineEdit->setText(Aux);

    InfoInterator = Info.find("SlotsBandwidth");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_BandWidth_LineEdit->Text->setText(Aux);

    InfoInterator = Info.find("TotalNumRegenerators");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_TotalNumReg_LineEdit->setText(Aux);

    InfoInterator = Info.find("NumRegeneratorsNode");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_NumRegNode_LineEdit->setText(Aux);

    InfoInterator = Info.find("mu");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_mu_LineEdit->setText(Aux);

    InfoInterator = Info.find("MinNumRegeneratorsNode");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_MinNumRegNode_LineEdit->setText(Aux);

    InfoInterator = Info.find("MaxNumRegeneratorsNode");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen2_MaxNumRegNode_LineEdit->setText(Aux);

    //Third Screen information
    InfoInterator = Info.find("DistInlineAmp");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_InLineAmpDist_LineEdit->setText(Aux);

    InfoInterator = Info.find("MinLaNet");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MinLaNet_LineEdit->setText(Aux);

    InfoInterator = Info.find("MaxLaNet");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MaxLaNet_LineEdit->setText(Aux);

    InfoInterator = Info.find("PointsLaNet");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_PointResLaNet_LineEdit->setText(Aux);

    InfoInterator = Info.find("MinOSNR");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MinOSNR_LineEdit->setText(Aux);

    InfoInterator = Info.find("MaxOSNR");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MaxOSNR_LineEdit->setText(Aux);

    InfoInterator = Info.find("PointsOSNR");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_PointResOSNR_LineEdit->setText(Aux);

    InfoInterator = Info.find("LaNet");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_LaNet_LineEdit->setText(Aux);

    InfoInterator = Info.find("MinSPL");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MinSPL_LineEdit->setText(Aux);

    InfoInterator = Info.find("MaxSPL");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MaxSPL_LineEdit->setText(Aux);

    InfoInterator = Info.find("RABothDecRatio");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_RABothDecRatio_LineEdit->setText(Aux);

    InfoInterator = Info.find("MinInlineAmpDist");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MinInLineAmpDist_LineEdit->setText(Aux);

    InfoInterator = Info.find("MaxInlineAmpDist");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MaxInLineAmpDist_LineEdit->setText(Aux);

    InfoInterator = Info.find("PointsInlineAmpDist");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_PointResInLineAmpDist_LineEdit->setText(Aux);

    InfoInterator = Info.find("InputPower");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_InputPower_LineEdit->setText(Aux);

    InfoInterator = Info.find("RefPower");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_RefPower_LineEdit->setText(Aux);

    InfoInterator = Info.find("NodeArc");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);

    if (Aux.toInt() == Arch_BS) {
        window->Screen3_NodeArc_BS_RadioButton->setChecked(true);
    } else if (Aux.toInt() == Arch_SS) {
        window->Screen3_NodeArc_SS_RadioButton->setChecked(true);
    } else if (Aux.toInt() == Arch_Null) {
        if (window->Screen3_NodeArc_BS_RadioButton->isChecked() ||
                window->Screen3_NodeArc_SS_RadioButton->isChecked()) {
            window->Screen3_NodeArc_ButtonGroup->setExclusive(false);
            window->Screen3_NodeArc_ButtonGroup->checkedButton()->setChecked(false);
            window->Screen3_NodeArc_ButtonGroup->setExclusive(true);
        }
    }

    InfoInterator = Info.find("MinNumBloqRed");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->Screen3_MinNumBloqReq_LineEdit->setText(Aux);

    //Parameters information
    InfoInterator = Info.find("NumReqMax");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_numReqMax_Cont->setText(Aux);

    InfoInterator = Info.find("LSSS");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_L_SSS_Cont->setText(Aux);

    InfoInterator = Info.find("freq");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_freq_Cont->setText(Aux);

    InfoInterator = Info.find("BRef");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_BRef_Cont->setText(Aux);

    InfoInterator = Info.find("FAmp");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_FAmp_Cont->setText(Aux);

    InfoInterator = Info.find("OSNRin");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_OSNRin_Cont->setText(Aux);

    InfoInterator = Info.find("BR");
    Temp = InfoInterator->second;
    Aux = QString::fromStdString(Temp);
    window->AddParamPage_BR_Cont->setText(Aux);

    //Updating parameters
    if (!window->AddParamPage_numReqMax_Cont->text().isEmpty()) {
        Def::set_NumReqMax(window->AddParamPage_numReqMax_Cont->text().toDouble());
    }

    if (!window->AddParamPage_L_SSS_Cont->text().isEmpty()) {
        Def::set_LSSS(window->AddParamPage_L_SSS_Cont->text().toDouble());
    }

    if (!window->AddParamPage_freq_Cont->text().isEmpty()) {
        Def::set_freq(window->AddParamPage_freq_Cont->text().toDouble());
    }

    if (!window->AddParamPage_BRef_Cont->text().isEmpty()) {
        Def::set_BRef_Norm(window->AddParamPage_BRef_Cont->text().toDouble());
    }

    if (!window->AddParamPage_FAmp_Cont->text().isEmpty()) {
        Def::set_FAmp(window->AddParamPage_FAmp_Cont->text().toDouble());
    }

    if (!window->AddParamPage_OSNRin_Cont->text().isEmpty()) {
        Def::set_OSNRin(window->AddParamPage_OSNRin_Cont->text().toDouble());
    }

    if (!window->AddParamPage_BR_Cont->text().isEmpty()) {
        Regenerators::set_BR(window->AddParamPage_BR_Cont->text().toDouble());
    }

    LoadFile.close();
}

void SaveLoadInfo::WordSeparator(QString Line, std::string *Key,
                                 std::string *Value) {
    int count = 0;
    bool Flag = true;
    QString Aux1, Aux2;

    while (Line[count] != '\0') {
        if (Line[count] != '=' && Flag) {
            Aux1.append(Line[count]);
        } else if (Line[count] == '=') {
            Flag = false;
        } else {
            Aux2.append(Line[count]);
        }

        count++;
    }

    *Key = Aux1.toStdString();
    *Value = Aux2.toStdString();
}

QString SaveLoadInfo::CorrectText(QString Text) {
    int Count = 0;

    while (Text[Count] != '\0') {
        if (Text[Count] == ',') {
            Text[Count] = '.';
        }

        Count++;
    }

    return Text;
}
