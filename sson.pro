QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sson
TEMPLATE = app

SOURCES += \
    src/cpp/Def.cpp \
    src/cpp/General.cpp \
    src/cpp/Heuristics.cpp \
    src/cpp/Route.cpp \
    src/cpp/Node.cpp \
    src/cpp/ASE_Noise.cpp \
    src/cpp/RWA.cpp \
    src/cpp/OSNR_Tree.cpp \
    src/cpp/Connection.cpp \
    src/cpp/Link.cpp \
    src/cpp/AuxiliarMain.cpp \
    src/cpp/Regenerators.cpp \
    src/cpp/Metrics.cpp \
    src/cpp/AboutPage.cpp \
    src/cpp/MainWindow.cpp \
    src/cpp/SaveLoadInfo.cpp \
    src/cpp/UnitLineEdit.cpp \
    src/cpp/main_SSON.cpp \
    src/cpp/SSON_Thread.cpp

OTHER_FILES += \
    README.md \
    data/topologies/TopologyNFS.txt \
    data/topologies/TopologyPBell.txt \
    data/topologies/TopologyRed8.txt \
    data/topologies/TopologyRed4.txt \
    data/topologies/TopologyNFSMod.txt \
    data/topologies/TopologyTop1.txt \
    data/topologies/TopologyTop2.txt \
    data/topologies/TopologyEuropean.txt \
    data/topologies/TopologyGerman.txt \
    data/topologies/TopologyUSBackbone.txt \
    .astylerc

HEADERS += \
    src/lib/Def.h \
    src/lib/General.h \
    src/lib/Heuristics.h \
    src/lib/Route.h \
    src/lib/Event.h \
    src/lib/Node.h \
    src/lib/RWA.h \
    src/lib/OSNR_Tree.h \
    src/lib/Connection.h \
    src/lib/Constant.h \
    src/lib/Link.h \
    src/lib/AuxiliarMain.h \
    src/lib/Regenerators.h \
    src/lib/AboutPage.h \
    src/lib/MainWindow.h \
    src/lib/SaveLoadInfo.h \
    src/lib/UnitLineEdit.h \
    src/lib/SSON_Thread.h

QMAKE_CXXFLAGS += -fopenmp -std=c++11
QMAKE_LFLAGS += -fopenmp -std=c++11

TRANSLATIONS += \
data/translations/sson_pt.ts \
data/translations/sson_fr.ts \
data/translations/sson_en.ts

INCLUDEPATH += \
    src/lib/

INSTALL_LINUX_TRANSLATIONS.path = ~/.local/share/sson/Translations
INSTALL_LINUX_TRANSLATIONS.files = data/translations/*.qm
INSTALL_LINUX_TOPOLOGIES.path = ~/.local/share/sson/Topologies
INSTALL_LINUX_TOPOLOGIES.files = data/topologies/*
INSTALL_LINUX_GNUPLOT.path = ~/.local/share/sson/GnuplotCommands
INSTALL_LINUX_GNUPLOT.files = data/gnuplot/*
INSTALL_LINUX_FIGURES.path = ~/.local/share/sson/Figures
INSTALL_LINUX_FIGURES.files = data/fig/*
INSTALL_LINUX_ICONS.path = ~/.local/share/icons
INSTALL_LINUX_ICONS.files = data/icons/*
INSTALL_LINUX_EXE.path = ~/.local/share/sson
INSTALL_LINUX_EXE.files = $$OUT_PWD/sson
INSTALL_LINUX_EXE.extra = if ! [ -d ~/.sson ]; then mkdir ~/.sson; fi;
INSTALL_LINUX_DESKTOP.path = ~/.local/share/applications
INSTALL_LINUX_DESKTOP.files = data/desktop/sson.desktop
INSTALL_LINUX_MISC.path = ~/.local/share/sson/Misc
INSTALL_LINUX_MISC.files = data/misc/*

INSTALLS += INSTALL_LINUX_TOPOLOGIES \
    INSTALL_LINUX_GNUPLOT \
    INSTALL_LINUX_FIGURES \
    INSTALL_LINUX_ICONS \
    INSTALL_LINUX_EXE \
    INSTALL_LINUX_DESKTOP \
    INSTALL_LINUX_TRANSLATIONS \
    INSTALL_LINUX_MISC
