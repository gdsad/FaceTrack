QT       += core gui
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_ICONS = icon.ico
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


#========= Configuracion para OpenCV 4.3.0 =========
INCLUDEPATH += C:\opencv\build\include
INCLUDEPATH += C:\opencv\build\x64\vc16\bin
PATH_OPENCV_LIB = C:\opencv\build\x64\vc16\lib
LIBS += -L$$PATH_OPENCV_LIB -lopencv_core430 -lopencv_imgproc430 -lopencv_highgui430 -lopencv_imgcodecs430 -lopencv_videoio430\
-lopencv_video430 -lopencv_calib3d430 -lopencv_photo430 -lopencv_features2d430 -lopencv_objdetect430 -lopencv_face430 -lopencv_flann430

#LIBS += -L$$PATH_OPENCV_LIB -lopencv_aruco430 -lopencv_bgsegm430 -lopencv_bioinspired430 -lopencv_calib3d430 -lopencv_ccalib430 -lopencv_core430 -lopencv_datasets430 \
#-lopencv_dnn_objdetect430 -lopencv_dnn_superres430 -lopencv_dnn430 -lopencv_dpm430 -lopencv_face430 -lopencv_features2d430 -lopencv_flann430 \
#-lopencv_fuzzy430 -lopencv_gapi430 -lopencv_hfs430 -lopencv_highgui430 -lopencv_img_hash430 -lopencv_imgcodecs430 -lopencv_imgproc430 \
#-lopencv_intensity_transform430 -lopencv_line_descriptor430 -lopencv_ml430 -lopencv_objdetect430 -lopencv_optflow430 -lopencv_phase_unwrapping430 \
#-lopencv_photo430 -lopencv_plot430 -lopencv_quality430 -lopencv_rapid430 -lopencv_reg430 -lopencv_rgbd430 -lopencv_saliency430 -lopencv_shape430 \
#-lopencv_stereo430 -lopencv_stitching430 -lopencv_structured_light430 -lopencv_superres430 -lopencv_surface_matching430 -lopencv_text430 -lopencv_tracking430\
#-lopencv_video430 -lopencv_videoio430 -lopencv_videostab430 -lopencv_xfeatures2d430 -lopencv_ximgproc430 -lopencv_xobjdetect430 -lopencv_xphoto430




#========= Configuracion para ALGLIB 3.16.0 (LevMar) =========
INCLUDEPATH += C:\alglib3.16.0\cpp\src
PATH_ALGLIB_LIB = C:\alglib3.16.0\cpp\build\x64\vc16\lib
LIBS += -L$$PATH_ALGLIB_LIB -lalglib_compiled

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MatToQImage.cpp \
    facialmesh.cpp \
    facetracking.cpp \
    #facetracking2.cpp \
    logfile.cpp \
    main.cpp \
    mainwindow.cpp \
    keyframeselection.cpp \
    modelfitting.cpp \
    onlymesh.cpp \
    results.cpp \
    trackingvideo.cpp \
    PointState.cpp


HEADERS += \
    MatToQImage.h \
    facialmesh.h \
    facetracking.h \
    #facetracking2.h \
    logfile.h \
    mainwindow.h \
    keyframeselection.h \
    modelfitting.h \
    onlymesh.h \
    results.h \
    trackingvideo.h \
    PointState.h


FORMS += \
    keyframeselection.ui \
    mainwindow.ui \
    modelfitting.ui \
    onlymesh.ui \
    results.ui \
    trackingvideo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
