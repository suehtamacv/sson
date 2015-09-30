#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QString>
#include <QTranslator>
#include <map>
#include "MainWindow.h"

/**
 * @brief The SSON_Thread class
 * evokes a textual or graphical interface to the user.
 * Also intermediates the calls between the graphic interface and the simulation part of the SSON.
 */
class SSON_Thread : public QThread {
    Q_OBJECT

  public:
    /**
     * @brief The ThreadType enum
     * is used to identify whether the interface showed is graphic or textual based.
     */
    enum ThreadType {
        Graphic_Thread, Textual_Thread
    };

    /**
     * @brief SSON_Thread
     * @param T
     * is the ThreadType, indicating whether this object should implement a graphic or textual based interface
     */
    SSON_Thread(ThreadType T);

    /**
     * @brief run
     * executes the thread. Accordingly to the ThreadType, shows a graphic or textual interface
     * @param argc
     * is a standard parameter from main
     * @param argv
     * is a standard parameter from main
     */
    void run(int argc, char *argv[]);

  public slots:
    /**
     * @brief trigger_GraphicRefresh
     * is called when the graphic in the graphic interface must be refreshed, as when a new data point has been obtained
     */
    void trigger_GraphicRefresh();
    /**
     * @brief trigger_SimulationEndedActions
     * is called when the simulation has ended. In the graphic interface, makes possible to the user to save the results
     */
    void trigger_SimulationEndedActions();

  public:
    struct Translator {
        QString Language_Name;
        QTranslator The_Translator;

        Translator(QString Language_Name, QString FileName) {
            Translator::Language_Name = Language_Name;
            The_Translator.load(FileName);
        }
    };

    static std::map<The_Translations, Translator *> Translators;
    QApplication *app;
  private:
    ThreadType _T;
    MainWindow *window;

    int run_Graphic(int argc, char *argv[]);
    void run_Textual();
};

#endif // THREAD_H
