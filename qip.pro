TEMPLATE    = app
TARGET      = qip
QT 		+= widgets opengl
QT 	   	+= widgets printsupport


OBJECTS_DIR = ./obj
MOC_DIR     = ./moc
RESOURCES   = qip.qrc

win32-msvc2013 {
	LIBS 		+= -lopengl32 -lglu32
	INCLUDEPATH += ./IP/win/header
	LIBS 		+= -L./IP/win/lib
	LIBS 		+= -lopengl32 -lIP_d
	QMAKE_CXXFLAGS += /MP /Zi
}


macx{
        QMAKE_MAC_SDK = macosx10.11
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
	INCLUDEPATH += ./IP/mac/header
	LIBS        += -L./IP/mac/lib
	LIBS        += -lIP_d
}

unix:!macx {
	CONFIG += C++11
	INCLUDEPATH += ./IP/linux/header
	LIBS        += -L./IP/linux/lib
	LIBS        += -lIP_d
}


# Input
HEADERS +=	MainWindow.h	\
		QGLDisplay.h	\
		ImageFilter.h	\
		qcustomplot.h	\
		Dummy.h		\
		Threshold.h	\
		Clip.h		\
		Quantize.h	\
		Gamma.h		\
		Contrast.h	\
		HistoStretch.h	\
		HistoMatch.h	\

		
SOURCES +=	main.cpp	\ 
		MainWindow.cpp 	\
		QGLDisplay.cpp	\
		ImageFilter.cpp	\
		qcustomplot.cpp	\
		Dummy.cpp	\
		Threshold.cpp	\
		Clip.cpp	\
		Quantize.cpp	\
		Gamma.cpp	\
		Contrast.cpp	\
		HistoStretch.cpp\
		HistoMatch.cpp	\

