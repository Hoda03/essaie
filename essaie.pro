QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    acceuil.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow1.cpp \
    mainwindow3.cpp \
    mainwindow4.cpp \
    sqp.cpp \
    sqp1.cpp \
    sqp3.cpp \
    sqp4.cpp \
    transmission_ratio.cpp \
    transmission_ratio1.cpp \
    transmission_ratio3.cpp \
    transmission_ratio4.cpp \
    version.cpp

HEADERS += \
    acceuil.h \
    mainwindow.h \
    mainwindow1.h \
    mainwindow3.h \
    mainwindow4.h \
    sqp.h \
    sqp1.h \
    sqp3.h \
    sqp4.h \
    transmission_ratio.h \
    transmission_ratio1.h \
    transmission_ratio3.h \
    transmission_ratio4.h \
    version.h

FORMS += \
    acceuil.ui \
    mainwindow.ui \
    mainwindow1.ui \
    mainwindow3.ui \
    mainwindow4.ui \
    sqp.ui \
    sqp1.ui \
    sqp3.ui \
    sqp4.ui \
    transmission_ratio.ui \
    transmission_ratio1.ui \
    transmission_ratio3.ui \
    transmission_ratio4.ui \
    version.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../../../Executable File/release/moc_mainwindow4.o
