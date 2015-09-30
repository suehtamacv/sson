#include "SSON_Thread.h"
#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <thread>
#include "MainWindow.h"
#include "AuxiliarMain.h"

void exec_Interface();

std::map<The_Translations, SSON_Thread::Translator *> SSON_Thread::Translators;

SSON_Thread::SSON_Thread(ThreadType T) : _T(T) {}

int SSON_Thread::run_Graphic(int argc, char *argv[]) {
    Translators[Translation_English] = new Translator("English",
            QString(QString::fromStdString(AuxiliarMain::Installed_Path) +
                    "Translations/sson_en"));
    Translators[Translation_Portuguese] = new Translator("Português",
            QString(QString::fromStdString(AuxiliarMain::Installed_Path) +
                    "Translations/sson_pt"));
    Translators[Translation_French] = new Translator("Français",
            QString(QString::fromStdString(AuxiliarMain::Installed_Path) +
                    "Translations/sson_fr"));

    app = new QApplication(argc, argv);
    app->installTranslator(&Translators.at(
                              AuxiliarMain::Translation)->The_Translator);

    window = new MainWindow();

    window->move(QApplication::desktop()->screen()->rect().center() -
                 window->rect().center());
    window->show();
    AuxiliarMain::window = window;
    return app->exec();
}

void SSON_Thread::run(int argc, char *argv[]) {
    switch (_T) {
        case Textual_Thread:
            run_Textual();
            break;

        case Graphic_Thread:
            run_Graphic(argc, argv);
            break;
    }
}

void SSON_Thread::run_Textual() {
    exec_Interface();
}

void SSON_Thread::trigger_GraphicRefresh() {
    window->Slot_Screen4_RefreshGraph(300);
    window->Slot_Screen4_RefreshTable();
}

void SSON_Thread::trigger_SimulationEndedActions() {
    window->Slot_Screen4_SimulationEnded();
}
