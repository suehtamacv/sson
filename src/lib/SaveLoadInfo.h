#ifndef SAVELOADINFO_H
#define SAVELOADINFO_H

#include "MainWindow.h"
#include "AuxiliarMain.h"
#include "Def.h"
#include "Constant.h"
#include "Regenerators.h"

#include <map>
#include <fstream>
#include <string>
#include <QtGui>
#include <QFileDialog>

using namespace std;

class SaveLoadInfo : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief SaveLoadInfo it is the constructor of the class SaveLoadInfo.
     */
    SaveLoadInfo();
    /**
     * @brief SaveAsInfo saves the data about the current simulation in a file selected by the user.
     */
    static void SaveAsInfo(MainWindow *);
    /**
     * @brief SaveInfo saves the data about the current simulation.
     */
    static void SaveInfo(MainWindow *);
    /**
     * @brief LoadInfo loads the data about a simulation saved in a previous file.
     */
    static void LoadInfo(MainWindow *, QString FileName = "");
    /**
     * @brief SaveFile writes in a file, all the data about the current simulation.
     */
    static void SaveFile(MainWindow *);
    /**
     * @brief LoadFile reads from a file, all the data about a simulation, saving it in a class template map
     * and then upload this information to a simulation.
     */
    static void LoadFile(QString, MainWindow *);
    /**
     * @brief WordSeparator separates the key and value information from a line.
     */
    static void WordSeparator(QString, std::string *, std::string *);
    /**
     * @brief CorrectText corrects the string in the argument, removing commas.
     * @return returns a proper string for use in the simulation.
     */
    static QString CorrectText(QString);
    /**
     * @brief CurrentFile the name of the current file being simulate.
     */
    static QString CurrentFile;
};

#endif // SAVELOADINFO_H
