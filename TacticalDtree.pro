CONFIG += console c++14
CONFIG -= app_bundle

CONFIG += qt
ROOT_DIR = $$PWD
DEFINES += INCLUDE_QT
DEFINES += TREE_PATH_ROAD=$${ROOT_DIR}/tacticalplanner/tactical_tree #设置离线存储的决策树的位置
DEFINES += TREE_PATH_LONGITUDINAL=$${ROOT_DIR}/tacticalplanner/tactical_tree/longitudinal
#DEFINES += CSV #is csv formatROOT_DIR

QMAKE_LFLAGS += "/STACK:65536,4096"  #设置栈保留大小65536K 提交大小4096K


SOURCES += \
    TestSec/gentestdatas.cpp \
    dataprocessing/data_processing.cpp \
      main.cpp\
    tacticaldtree.cpp \


HEADERS += \
    TestSec/gentestdatas.h \
    common.h \
    dataprocessing/data_processing.h \
    decisionstructure.h \
    tacticaldtree.h \


DISTFILES += \
    UCIdata \
    serialtree \
    readme
