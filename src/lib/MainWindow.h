#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QTemporaryFile>
#include <QTime>
#include <QLineEdit>
#include <QFormLayout>
#include <QComboBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGraphicsView>
#include <QMenuBar>
#include <QPixmap>
#include <QGraphicsScene>
#include <QImage>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QTextStream>
#include <QProgressBar>
#include <QFileDialog>
#include <QTableView>
#include <QLineEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include "AboutPage.h"
#include "UnitLineEdit.h"

/**
 * @brief The Translations enum selects the language in which the graphic interface will be presented.
 */
enum The_Translations {
    Translation_Portuguese, Translation_French, Translation_English
};

class AuxiliarMain;

/**
 * @brief The MainWindow class is the Graphic Interface itself.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow();
    ~MainWindow();

    /**
     * @brief refresh called by Simulation, triggers refresh in Graphic Interface.
     */
    void Refresh();
    /**
     * @brief simulation_Ended called by Simulation, triggers refresh and other actions in Graphic Interface.
     */
    void Simulation_Ended();

    enum Screen4_GraphType {
        Screen4_PNG, Screen4_EPS
    };

  public slots:
    /**
     * @brief Slot_LoadDefaultValues loads the default values.
     */
    void Slot_LoadDefaultValues();
    /**
     * @brief Slot_LoadSimulation loads the data about a simulation in a previous saved file.
     */
    void Slot_LoadSimulation();
    /**
     * @brief Slot_LoadSimulation_Op loads the data about a simulation in a previous saved file. This one is for the open button on the opening screen.
     */
    void Slot_LoadSimulation_Op();
    /**
     * @brief Slot_NewSimulation starts a new simulation, cleaning all changes.
     */
    void Slot_NewSimulation();
    /**
     * @brief Slot_SaveAsSimulation saves the data about the current simulation in a file selected by the user.
     */
    void Slot_SaveAsSimulation();
    /**
     * @brief Slot_SaveSimulation saves the data about the current simulation.
     */
    void Slot_SaveSimulation();
    /**
     * @brief Slot_Screen4_RefreshGraph redraws the graphic.
     * @param WaitTime is the time necessary to generate the plot, in milliseconds.
     * @param GraphType specifies wether the graph will be saved as EPS or PNG.
     */
    bool Slot_Screen4_RefreshGraph(long double WaitTime = 300,
                                   Screen4_GraphType GraphType = Screen4_PNG);
    /**
     * @brief Slot_Screen4_RefreshProgressBar refreshes the progress bar.
     */
    void Slot_Screen4_RefreshProgressBar();
    /**
     * @brief Slot_Screen4_RefreshTable rereads the data and put them in the table.
     */
    void Slot_Screen4_RefreshTable();
    /**
     * @brief Slot_Screen4_SaveGraph makes possible to save the graph.
     */
    void Slot_Screen4_SaveGraph();
    /**
     * @brief Slot_Screen4_SaveTable makes possible to save the table in a text fashion.
     */
    void Slot_Screen4_SaveTable();
    /**
     * @brief Slot_Screen4_SimulationEnded runs actions due when the simulation ends.
     */
    void Slot_Screen4_SimulationEnded();

  signals:
    /**
     * @brief trigger_Refresh is a signal emitted when the refresh is due.
     */
    void trigger_Refresh();
    /**
     * @brief trigger_SimulationEndedActions is a signal emitted when the simulation ended actions are due.
     */
    void trigger_SimulationEndedActions();

  private slots:
    void ChangeTopologyPic();
    void Close_AddParametersPage();
    void Close_Program();
    void Screen1_NextPage();
    void Open_AboutPage();
    void Open_AddParametersPage();
    void OpeningScreen_NextPage();
    void PreviousPage();
    void RPAlg_Change();
    void Screen2_NextPage();
    void Screen2_PreviousPage();
    void ShowParam();
    void Simulate();
    void UpdateParam();

  public:
    AboutPage *about_page;

    std::map <The_Translations, QAction *> edit_Languages;

    QAction *edit_load_default;
    QAction *edit_set_parameters;
    QAction *file_exit;
    QAction *file_new_sim;
    QAction *file_open_sim;
    QAction *file_save_sim;
    QAction *file_saveas_sim;
    QAction *help_about;

    QHBoxLayout *HContainerLayout;

    QMenu *choose_lang_menu;
    QMenu *edit_menu;
    QMenu *file_menu;
    QMenu *help_menu;
    QMenu *main_menu_bar;

    QMenuBar *menubar;

    QPushButton *OpeningPage_Exit_Button;
    QPushButton *OpeningPage_NewSimulation_Button;
    QPushButton *OpeningPage_OpenSimulation_Button;

    QStackedWidget *main_pages;

    QTemporaryFile *gnuplot_TempPlot;

    QWidget *Container;
    QWidget *opening_page;
    QWidget *Screen1;
    QWidget *Screen2;
    QWidget *Screen3;
    QWidget *Screen4;

    //Screen1 Components

    QButtonGroup *Screen1_NetworkType_ButtonGroup;

    QComboBox *Screen1_SimulationType_ComboBox;
    QComboBox *Screen1_Topology_ComboBox;

    QGraphicsScene *Screen1_Topology_GrapScene;

    QGraphicsView *Screen1_Topology_GraphicsView;

    QImage  *Screen1_Topology_Image;

    QLabel *Screen1_NetworkType_Label;
    QLabel *Screen1_SimulationType_Label;
    QLabel *Screen1_Topology_Label;

    QPixmap Screen1_Topology_ImagePixMap;

    QPushButton *Screen1_NextPage_Button;
    QPushButton *Screen1_Topology_Button;

    QRadioButton *Screen1_NetworkType_TL_RadioButton;
    QRadioButton *Screen1_NetworkType_TP_RadioButton;

    //Screen2 components:

    QComboBox *Screen2_RAAlg_ComboBox;
    QComboBox *Screen2_RoutAlg_ComboBox;
    QComboBox *Screen2_RPAlg_ComboBox;
    QComboBox *Screen2_SpecAssAlg_ComboBox;

    QFormLayout *Screen2_InfoLayout;

    QGroupBox *Screen2_InfoGroupBox;
    QGroupBox *Screen2_PageSwitchingGroupBox;

    QHBoxLayout *Screen2_PageSwitchingLayout;

    QLabel *Screen2_BandWidth_Label;
    QLabel *Screen2_MaxNumRegNode_Label;
    QLabel *Screen2_MinNumRegNode_Label;
    QLabel *Screen2_mu_Label;
    QLabel *Screen2_NumRegNode_Label;
    QLabel *Screen2_RAAlg_Label;
    QLabel *Screen2_RepSim_Label;
    QLabel *Screen2_RoutAlg_Label;
    QLabel *Screen2_RPAlg_Label;
    QLabel *Screen2_Spacer1;
    QLabel *Screen2_Spacer2;
    QLabel *Screen2_SpecAssAlg_Label;
    QLabel *Screen2_TotalNumReg_Label;

    QLineEdit *Screen2_MaxNumRegNode_LineEdit;
    QLineEdit *Screen2_MinNumRegNode_LineEdit;
    QLineEdit *Screen2_mu_LineEdit;
    QLineEdit *Screen2_NumRegNode_LineEdit;
    QLineEdit *Screen2_RepSim_LineEdit;
    QLineEdit *Screen2_TotalNumReg_LineEdit;

    QPushButton *Screen2_NextPage_Button;
    QPushButton *Screen2_PreviousPage_Button;

    QVBoxLayout *Screen2_MainLayout;

    UnitLineEdit *Screen2_BandWidth_LineEdit;

    //Screen3 components:

    QButtonGroup *Screen3_NodeArc_ButtonGroup;

    QFormLayout *Screen3_InfoLayout;

    QGroupBox *Screen3_InfoGroupBox;
    QGroupBox *Screen3_PageSwitchingGroupBox;

    QHBoxLayout *Screen3_PageSwitchingLayout;

    QLabel *Screen3_InLineAmpDist_Label;
    QLabel *Screen3_InputPower_Label;
    QLabel *Screen3_LaNet_Label;
    QLabel *Screen3_MaxInLineAmpDist_Label;
    QLabel *Screen3_MinInLineAmpDist_Label;
    QLabel *Screen3_MaxLaNet_Label;
    QLabel *Screen3_MinLaNet_Label;
    QLabel *Screen3_MinNumBloqReq_Label;
    QLabel *Screen3_MaxOSNR_Label;
    QLabel *Screen3_MinOSNR_Label;
    QLabel *Screen3_MaxSPL_Label;
    QLabel *Screen3_MinSPL_Label;
    QLabel *Screen3_NodeArc_Label;
    QLabel *Screen3_PointResInLineAmpDist_Label;
    QLabel *Screen3_PointResLaNet_Label;
    QLabel *Screen3_PointResOSNR_Label;
    QLabel *Screen3_RABothDecRatio_Label;
    QLabel *Screen3_RefPower_Label;
    QLabel *Screen3_Spacer1;

    QLineEdit *Screen3_MinNumBloqReq_LineEdit;
    QLineEdit *Screen3_MaxSPL_LineEdit;
    QLineEdit *Screen3_MinSPL_LineEdit;
    QLineEdit *Screen3_PointResInLineAmpDist_LineEdit;
    QLineEdit *Screen3_PointResLaNet_LineEdit;
    QLineEdit *Screen3_PointResOSNR_LineEdit;
    QLineEdit *Screen3_RABothDecRatio_LineEdit;

    QPushButton *Screen3_PreviousPage_Button;
    QPushButton *Screen3_Simulate_Button;

    QRadioButton *Screen3_NodeArc_BS_RadioButton;
    QRadioButton *Screen3_NodeArc_SS_RadioButton;

    QVBoxLayout *Screen3_MainLayout;

    UnitLineEdit *Screen3_InLineAmpDist_LineEdit;
    UnitLineEdit *Screen3_InputPower_LineEdit;
    UnitLineEdit *Screen3_LaNet_LineEdit;
    UnitLineEdit *Screen3_MaxInLineAmpDist_LineEdit;
    UnitLineEdit *Screen3_MinInLineAmpDist_LineEdit;
    UnitLineEdit *Screen3_MaxLaNet_LineEdit;
    UnitLineEdit *Screen3_MinLaNet_LineEdit;
    UnitLineEdit *Screen3_MaxOSNR_LineEdit;
    UnitLineEdit *Screen3_MinOSNR_LineEdit;
    UnitLineEdit *Screen3_RefPower_LineEdit;

    //Screen4 components:

    QHBoxLayout *Screen4_HorLayout;
    QHBoxLayout *Screen4_HorLayout_2;

    QLabel *Screen4_PlotScreen;

    QProgressBar *Screen4_ProgressBar;

    QPushButton *Screen4_ButtonToggleToGraph;
    QPushButton *Screen4_ButtonToggleToTable;
    QPushButton *Screen4_ReplotSaveButton;

    QTableView *Screen4_ResultsTable;

    QTime Screen4_LastPlotted;
    QTime Screen4_LastRefreshedProgressBar;

    QVBoxLayout *Screen4_VerLayout;

    //Additional Parameters Page components:

    QGroupBox *AddParamPage_DialogGroupBox;

    QHBoxLayout *AddParamPage_DialogLayout;

    QPushButton *AddParamPage_CancelButton;
    QPushButton *AddParamPage_OkButton;

    QStandardItem *AddParamPage_BR_Cont;
    QStandardItem *AddParamPage_BR_Param;
    QStandardItem *AddParamPage_BRef_Cont;
    QStandardItem *AddParamPage_BRef_Param;
    QStandardItem *AddParamPage_FAmp_Cont;
    QStandardItem *AddParamPage_FAmp_Param;
    QStandardItem *AddParamPage_freq_Cont;
    QStandardItem *AddParamPage_freq_Param;
    QStandardItem *AddParamPage_L_SSS_Cont;
    QStandardItem *AddParamPage_L_SSS_Param;
    QStandardItem *AddParamPage_numReqMax_Cont;
    QStandardItem *AddParamPage_numReqMax_Param;
    QStandardItem *AddParamPage_OSNRin_Cont;
    QStandardItem *AddParamPage_OSNRin_Param;

    QStandardItemModel *AddParamPage_Model;

    QTableView *AddParamPage_TableView;

    QVBoxLayout *AddParamPage_MainLayout;

    QWidget *addparam_page;

    void closeEvent(QCloseEvent *);
    /**
     * @brief create_actions create the QActions in the menu bar.
     */
    void Create_Actions();
    /**
     * @brief create_menus creates the menu bar of the program.
     */
    void Create_Menus();
    /**
     * @brief center_main_window place the main window on the center of the screen.
     * @param the widget who is gonna be centered.
     */
    void center_main_window(QWidget *);
    /**
     * @brief set_AddParamPage sets the components of the additional parameters page, their text, size and others.
     */
    void Set_AddParamPage();
    /**
     * @brief set_OpeningPage sets the components of the opening page, their text, size and others.
     */
    void Set_OpeningPage();
    /**
     * @brief set_FirstScreen sets the components of the first screen, their text, size and others.
     */
    void Set_Screen1();
    /**
     * @brief set_SecondScreen sets the components of the second screen, their text, size and others.
     */
    void Set_Screen2();
    /**
     * @brief set_ThirdScreen sets the components of the third screen, their text, size and others.
     */
    void Set_Screen3();
    /**
     * @brief set_FourthScreen sets the components of the fourth screen, their text, size and others.
     */
    void Set_Screen4();
    /**
     * @brief show_FourthScreen shows the Fourth Screen.
     */
    void Show_Screen4();
    /**
     * @brief delay delays the progress of the simulation.
     * @param Num_MilliSeconds it is the size of the delay.
     */
    void Delay(int Num_MilliSeconds);
    /**
     * @brief InputCheck_FirstScreen checks the data provided by the user on the first screen.
     * @return the result of the check, false if everything is okay, true if not.
     */
    bool InputCheck_Screen1();
    /**
     * @brief InputCheck_SecondScreen checks the data provided by the user on the second screen.
     * @return the result of the check, false if everything is okay, true if not.
     */
    bool InputCheck_Screen2();
    /**
     * @brief InputCheck_ThirdScreen checks the data provided by the user on the third screen.
     * @return the result of the check, false if everything is okay, true if not.
     */
    bool InputCheck_Screen3();
    /**
     * @brief TextCheck_Double check the string to remove commas.
     * @param the string to be checked.
     * @return the result of the operation, true if okay, false if not.
     */
    bool TextCheck_Double(QString);
    /**
     * @brief TextCheck_Int check the string to remove dots.
     * @param the string to be checked.
     * @return the result of the operation, true if okay, false if not.
     */
    bool TextCheck_Int(QString);
    /**
     * @brief ClearSimulation clears all the changes made.
     */
    void ClearSimulation();
    /**
     * @brief GraphicInterface_LoadValues loads the data entered by the user, in the proper classes and objects for the simulation start.
     */
    void GraphicInterface_LoadValues(AuxiliarMain *Aux);


  private:
    /**
     * @brief CorrectDouble replaces commas by dots.
     * @param the string to be corrected.
     */
    void CorrectDouble(QLineEdit *);
    void CorrectDouble(UnitLineEdit *);
    static int ProgressBarCont;
    QString Screen4_getGnuplotCommmand(Screen4_GraphType GraphType =
                                           Screen4_PNG);
    void Screen4_getTableRowNames(QStandardItemModel *Model_Table);
    QString Screen4_getResultFileName();
    int Screen4_getNumSteps();

  private slots:
    void Povoate_SimulationType();
    void Translate_SSON();
    void retranslateUi();
};

#endif // MAINWINDOW_H
