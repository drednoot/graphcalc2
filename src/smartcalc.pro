QT+=core gui
greaterThan(QT_MAJOR_VERSION, 4): QT+=widgets printsupport

CONFIG+=c++17 object_parallel_to_source
RESOURCES+=view/icons/icons.qrc

SOURCES+=\
	model/calculator.cc\
	model/credit.cc\
	controller/controller.cc\
	view/graph.cc\
	view/credit.cc\
	view/view.cc\
	qcustomplot/qcustomplot.cc\
	main.cc

HEADERS+=\
	model/calculator.h\
	model/credit.h\
	controller/controller.h\
	view/graph.h\
	view/credit.h\
	view/view.h\
	qcustomplot/qcustomplot.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS+=target
