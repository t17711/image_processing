// ===============================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2016 by George Wolberg
//
// MainWindow.cpp - MainWindow class
//
// Written by: George Wolberg, 2016
// ===============================================================

#include "MainWindow.h"
#include "Dummy.h"
#include "Threshold.h"
#include "Clip.h"
#include "Quantize.h"
#include "Gamma.h"
#include "Contrast.h"
#include "HistoStretch.h"
#include "HistoMatch.h"
#include "time.h"

using namespace IP;

enum {DUMMY, THRESHOLD, CLIP, QUANTIZE, GAMMA, CONTRAST, HISTOSTRETCH, HISTOMATCH};
enum {RGB, R, G, B, GRAY};

QString GroupBoxStyle = "QGroupBox {				\
			border: 2px solid gray;			\
			border-radius: 9px;			\
			margin-top: 0.5em;}";

MainWindow *g_mainWindowP = NULL;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::MainWindow:
//
// MainWindow constructor.
//
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  m_code(-1),
	  m_histoColor(GRAY)
{
	setWindowTitle("Image Processing");

	// set global variable for main window pointer
	g_mainWindowP = this;

	// assemble user interface
	createActions();	// insert your actions here
	createMenus  ();	// insert your menus here

	// now do open glsl thing in createwidget
	// this has group display andd create panel
	// create group display has stack widget with 2 qlabel for input and output image


	// ope gl code looks similar
	// need create gl widgets
	// creat tab widgets like thing
	/*

	have GLWidget[max enum];
	m_gl widgets[enum value] = new threshold ();
	 threshold inherits from qgl widget
	
	
	
	*/
	createWidgets();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createActions:
//
// Create actions to associate with menu and toolbar selection.
//
void
MainWindow::createActions()
{
	//////////////////////////////
	// File Actions
	//////////////////////////////

	m_actionOpen = new QAction("&Open", this);
	m_actionOpen->setShortcut(tr("Ctrl+O"));
	connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));

	m_actionSave = new QAction("&Save", this);
	m_actionSave->setShortcut(tr("Ctrl+S"));
	connect(m_actionSave, SIGNAL(triggered()), this, SLOT(save()));

	m_actionQuit = new QAction("&Quit", this);
	m_actionQuit->setShortcut(tr("Ctrl+Q"));
	connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	//////////////////////////////
	// Point Ops Actions
	//////////////////////////////

	m_actionThreshold = new QAction("&Threshold", this);
	m_actionThreshold->setShortcut(tr("Ctrl+T"));
	m_actionThreshold->setData(THRESHOLD);

	m_actionClip = new QAction("&Clip", this);
	m_actionClip->setShortcut(tr("Ctrl+C"));
	m_actionClip->setData(CLIP);

	m_actionQuantize = new QAction("Quant&ization", this);
	m_actionQuantize->setShortcut(tr("Ctrl+I"));
	m_actionQuantize->setData(QUANTIZE);

	m_actionGamma = new QAction("&Gamma Correction", this);
	m_actionGamma->setShortcut(tr("Ctrl+G"));
	m_actionGamma->setData(GAMMA);

	m_actionContrast = new QAction("Co&ntrast Enhancement", this);
	m_actionContrast->setShortcut(tr("Ctrl+N"));
	m_actionContrast->setData(CONTRAST);

	m_actionHistoStretch = new QAction("Histogram &Stretch", this);
	m_actionHistoStretch->setShortcut(tr("Ctrl+S"));
	m_actionHistoStretch->setData(HISTOSTRETCH);

	m_actionHistoMatch = new QAction("Histogram &Match", this);
	m_actionHistoMatch->setShortcut(tr("Ctrl+M"));
	m_actionHistoMatch->setData(HISTOMATCH);

	// one signal-slot connection for all actions;
	// execute() will resolve which action was triggered
	connect(menuBar(), SIGNAL(triggered(QAction*)), this, SLOT(execute(QAction*)));
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createMenus:
//
// Create menus and install in menubar.
//
void
MainWindow::createMenus()
{
	// File menu
	m_menuFile = menuBar()->addMenu("&File");
	m_menuFile->addAction(m_actionOpen);
	m_menuFile->addAction(m_actionSave);
	m_menuFile->addAction(m_actionQuit);

	// Point Ops menu
	m_menuPtOps = menuBar()->addMenu("&Point Ops");
	m_menuPtOps->addAction(m_actionThreshold   );
	m_menuPtOps->addAction(m_actionClip	   );
	m_menuPtOps->addAction(m_actionQuantize    );
	m_menuPtOps->addAction(m_actionGamma	   );
	m_menuPtOps->addAction(m_actionContrast    );
	m_menuPtOps->addAction(m_actionHistoStretch);
	m_menuPtOps->addAction(m_actionHistoMatch  );

	// disable the following menus until input image is read
	m_menuPtOps->setEnabled(false);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createWidgets:
//
// Create widgets for image display and filter control panels.
//
void
MainWindow::createWidgets()
{
	// assemble image display widget and control panel in horizontal layout
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(createGroupDisplay());
	hbox->addWidget(createGroupPanel  ());
	hbox->setStretch(0, 1);

	// create container widget and set its layout
	QWidget *w = new QWidget;
	w->setLayout(hbox);

	// set central widget so that it can be displayed
	setCentralWidget(w);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createGroupPanel:
//
// Create group box for control panel.
//
QGroupBox*
MainWindow::createGroupPanel()
{
	// init group box
	m_groupBoxPanels = new QGroupBox;
	m_groupBoxPanels->setMinimumWidth(400);

	// filter's enum indexes into container of image filters
	m_imageFilter[DUMMY	] = new Dummy;
	m_imageFilter[THRESHOLD	] = new Threshold;
	m_imageFilter[CLIP	] = new Clip;
	m_imageFilter[QUANTIZE	] = new Quantize;
	m_imageFilter[GAMMA	] = new Gamma;
	m_imageFilter[CONTRAST	] = new Contrast;
	m_imageFilter[HISTOSTRETCH]=new HistoStretch;
	m_imageFilter[HISTOMATCH] = new HistoMatch;

	// create a stacked widget to hold multiple control panels
	m_stackWidgetPanels = new QStackedWidget;

	// add filter control panels to stacked widget
	m_stackWidgetPanels->addWidget(m_imageFilter[DUMMY       ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[THRESHOLD   ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[CLIP        ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[QUANTIZE    ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[GAMMA       ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[CONTRAST    ]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[HISTOSTRETCH]->controlPanel());
	m_stackWidgetPanels->addWidget(m_imageFilter[HISTOMATCH  ]->controlPanel());

	// display blank dummmy panel initially
	m_stackWidgetPanels->setCurrentIndex(DUMMY);

	// assemble display and mode groups into horizontal layout
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(createDisplayButtons());
	hbox->addWidget(createModeButtons   ());
	hbox->addWidget(createOptionButtons());

	// init timing label
	m_labelTime = new QLabel;

        // create histogram plot
        m_histogram = new QCustomPlot;

	// set histogram title
        m_histogram->plotLayout()->insertRow(0);
        m_histogram->plotLayout()->addElement(0, 0, new QCPPlotTitle(m_histogram,"Histogram"));

        // assign label axes
        m_histogram->xAxis->setLabel("Intensity");
        m_histogram->yAxis->setLabel("Frequency");
        m_histogram->xAxis->setAutoTickStep(0);
        m_histogram->xAxis->setTickStep(32.);

        // set axes ranges, so we see all the data
        m_histogram->xAxis->setRange(0, MXGRAY);
        m_histogram->yAxis->setRange(0, MXGRAY);
	m_histogram->setMinimumHeight(400);
	m_histogram->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	m_histogram->axisRect()->setupFullAxesBox();

	// move legend below histogram
	QCPLayoutGrid *subLayout = new QCPLayoutGrid;
	subLayout->addElement(0, 0, new QCPLayoutElement(m_histogram));	// spacer
	subLayout->addElement(0, 1, m_histogram->legend);		// legend
	subLayout->addElement(0, 2, new QCPLayoutElement(m_histogram));	// spacer
	subLayout->setMaximumSize(400, 30);				// legend size
	subLayout->setMargins(QMargins(5, 5, 5, 5));			// legend margin
	m_histogram->plotLayout()->addElement(2, 0, subLayout);		// plot legend

	// create extension and insert histogram
	m_extension = new QWidget;
	QVBoxLayout *vboxExt = new QVBoxLayout(m_extension);
	vboxExt->addWidget(m_histogram);

	// start dialog with hidden histogram
	m_extension->hide();

	// set signal-slot connection
	connect(m_checkboxHisto, SIGNAL(stateChanged(int)), this, SLOT(setHisto(int)));
	connect(m_checkboxTime,  SIGNAL(stateChanged(int)), this, SLOT(setTime (int)));

	// assemble stacked widget in vertical layout
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(hbox);
	vbox->addWidget(m_stackWidgetPanels);
	vbox->addWidget(m_labelTime);
	vbox->addWidget(m_extension);
	vbox->addStretch(1);
	vbox->addLayout(createExitButtons());
	m_groupBoxPanels->setLayout(vbox);

	// disable the control panel until input image is read
	m_groupBoxPanels->setEnabled(false);

	return m_groupBoxPanels;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createGroupDisplay:
//
// Create group box for displaying images.
//
QGroupBox*
MainWindow::createGroupDisplay()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox;
	groupBox->setMinimumWidth (700);
	groupBox->setMinimumHeight(700);
	groupBox->setStyleSheet(GroupBoxStyle);

	// create stacked widget for input/output images
	m_stackWidgetImages = new QStackedWidget;

	// add QLabels to image stacked widget to display input/output images
	for(int i = 0; i<2; ++i)
		m_stackWidgetImages->addWidget(new QLabel);

	// add centering alignment on both labels
	QLabel *label;
	label = (QLabel *) m_stackWidgetImages->widget(0); label->setAlignment(Qt::AlignCenter);
	label = (QLabel *) m_stackWidgetImages->widget(1); label->setAlignment(Qt::AlignCenter);

	// set stacked widget to default setting: input image
	m_stackWidgetImages->setCurrentIndex(0);

	// assemble stacked widget in vertical layout
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_stackWidgetImages);
	groupBox->setLayout(vbox);

	return groupBox;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createDisplayButtons:
//
// Create group box for the display buttons.
//
QGroupBox*
MainWindow::createDisplayButtons()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Display");

	// create radio buttons
	m_radioDisplay[0] = new QRadioButton("Input");
	m_radioDisplay[1] = new QRadioButton("Output");

	// create button group and add radio buttons to it
	QButtonGroup *bGroup = new QButtonGroup;
	for(int i = 0; i<2; ++i)
		bGroup->addButton(m_radioDisplay[i]);

	// set input radio button to be default
	m_radioDisplay[0]->setChecked(true);

	// assemble radio buttons into vertical widget
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_radioDisplay[0]);
	vbox->addWidget(m_radioDisplay[1]);
	vbox->addStretch(1);
	groupBox->setLayout(vbox);

	// init signal/slot connections
	connect(m_radioDisplay[0], SIGNAL(clicked()), this, SLOT(displayIn()));
	connect(m_radioDisplay[1], SIGNAL(clicked()), this, SLOT(displayOut()));

	return groupBox;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createModeButtons:
//
// Create group box for the mode buttons.
//
QGroupBox*
MainWindow::createModeButtons()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Mode");

	// create radio buttons
	m_radioMode[0] = new QRadioButton("RGB");
	m_radioMode[1] = new QRadioButton("Gray");

	// create button group and add radio buttons to it
	QButtonGroup *bGroup = new QButtonGroup;
	for(int i = 0; i<2; ++i)
		bGroup->addButton(m_radioMode[i]);

	// set Gray radio button to be default
	m_radioMode[1]->setChecked(true);

	// assemble radio buttons into vertical widget
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_radioMode[0]);
	vbox->addWidget(m_radioMode[1]);
	vbox->addStretch(1);
	groupBox->setLayout(vbox);

	// init signal/slot connections
	connect(m_radioMode[0], SIGNAL(clicked()), this, SLOT(modeRGB ()));
	connect(m_radioMode[1], SIGNAL(clicked()), this, SLOT(modeGray()));

	return groupBox;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createOptionButtons:
//
// Create group box for the options buttons.
//
QGroupBox*
MainWindow::createOptionButtons()
{
	// init group box
	QGroupBox *groupBox = new QGroupBox("Options");

	// create histogram checkbox
	m_checkboxHisto = new QCheckBox("Histogram");
	m_checkboxHisto ->setCheckState (Qt::Checked);

	// create Timing checkbox
	m_checkboxTime = new QCheckBox("Timing");
	m_checkboxTime ->setCheckState (Qt::Unchecked);

	// create GPU checkbox
	m_checkboxGPU = new QCheckBox("GPU");
	m_checkboxGPU ->setCheckState (Qt::Unchecked);

	// assemble radio buttons into vertical widget
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_checkboxHisto);
	vbox->addWidget(m_checkboxTime);
	vbox->addWidget(m_checkboxGPU);
	groupBox->setLayout(vbox);

	return groupBox;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createExitButtons:
//
// Create save/quit buttons.
//
QHBoxLayout*
MainWindow::createExitButtons()
{
	// create pushbuttons
	QPushButton *buttonReset = new QPushButton("Reset");
	QPushButton *buttonQuit  = new QPushButton("Quit");

	// init signal/slot connections
	connect(buttonReset, SIGNAL(clicked()), this, SLOT(reset()));
	connect(buttonQuit , SIGNAL(clicked()), this, SLOT(quit ()));

	// assemble pushbuttons in horizontal layout
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(buttonReset);
	buttonLayout->addWidget(buttonQuit );

	return buttonLayout;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::reset:
//
// Reset application.
//
void
MainWindow::reset()
{

	m_imageFilter[m_code]->reset();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::quit:
//
// Quit application.
//
void
MainWindow::quit()
{
	// close the dialog window
	close();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::time:
//
// Compute average runtime of of filter.
//
void
MainWindow::time()
{
	// verify if time checkbox is set
	if(!m_checkboxTime->isChecked()) {
		m_labelTime->setText(QString("Execution time (ms): "));
		return;
	}

	// error checking
	if(m_code <= 0) return;

	// init current clock time
	clock_t t = clock();

	// run filter one hundred times
	for(int i = 0; i < 100; ++i)
		m_imageFilter[m_code]->applyFilter(m_imageSrc, m_imageDst);

	// compute average execution time: divide elapsed time by number of iterations
	float dt  = (clock() - t) / 100.;

	// convert to milliseconds: multiply microseconds by 1000
	dt *= (1000. / CLOCKS_PER_SEC);

	// print result
	m_labelTime->setText(QString("Execution time (ms): %2").arg(dt));
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::open:
//
// Open input image.
//
void
MainWindow::open() {
	QFileDialog dialog(this);

	// open the last known working directory
	if(!m_currentDir.isEmpty())
		dialog.setDirectory(m_currentDir);

	// display existing files and directories
	dialog.setFileMode(QFileDialog::ExistingFile);

	// invoke native file browser to select file
	m_file = dialog.getOpenFileName(this,
					"Open File", m_currentDir,
					"Images (*.jpg *.png *.ppm *.pgm *.bmp);;All files (*)");

	// verify that file selection was made
	if(m_file.isNull()) return;

	// save current directory
	QFileInfo f(m_file);
	m_currentDir = f.absolutePath();

	// read input image
	m_imageIn = IP_readImage(qPrintable(m_file));

	// set input radio button to be default
	m_radioDisplay[0]->setChecked(true);

	// display histogram by default
        m_extension->setVisible(true);

	if(m_radioMode[1]->isChecked())
		IP_castImage(m_imageIn,  BW_IMAGE, m_imageSrc);
	else	IP_castImage(m_imageIn, RGB_IMAGE, m_imageSrc);

	// init vars
	m_width  = m_imageSrc->width ();
	m_height = m_imageSrc->height();
	preview();

	// enable the following now that input image is read
	m_menuPtOps	->setEnabled(true);
	m_groupBoxPanels->setEnabled(true);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::save:
//
// Save image.
//
void
MainWindow::save() {
	QFileDialog dialog(this);

	// open the last known working directory
	if(!m_currentDir.isEmpty())
		dialog.setDirectory(m_currentDir);

	// display existing files and directories
	dialog.setFileMode(QFileDialog::ExistingFile);

	// invoke native file browser to select file
	QString saveName = dialog.getSaveFileName(this,
					"Save File", m_currentDir,
					"Images (*.jpg *.png *.ppm *.pgm *.bmp);;All files (*)");

	// verify that file selection was made
	if(saveName.isEmpty()) return;

	// save current directory
	QFileInfo f(m_file);
	m_currentDir = f.absolutePath();

	// save file
	QString tag = QFileInfo(saveName).suffix().toUpper();
	IP_saveImage(g_mainWindowP->imageSrc(), qPrintable(saveName), qPrintable(tag));
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::display:
//
// Slot functions to display input and output images.
//
void MainWindow::displayIn () { display(0); }
void MainWindow::displayOut() { display(1); }

void MainWindow::display(int flag)
{
	// error checking
	if(m_imageSrc.isNull())		return;		// no input  image
	if(m_imageDst.isNull() && flag) return;		// no output image

	// raise the appropriate widget from the stack
	m_stackWidgetImages->setCurrentIndex(flag);

	// set radio button
	m_radioDisplay[flag]->setChecked(1);

	// determine image to be displayed
	ImagePtr I;
	if(flag == 0)
		I = m_imageSrc;
	else	I = m_imageDst;

	// init image dimensions
	int  w = I->width();
	int  h = I->height();

	// init view window dimensions
	int ww = m_stackWidgetImages->width();
	int hh = m_stackWidgetImages->height();

	// convert from ImagePtr to QImage to Pixmap
	QImage q;
	IP_IPtoQImage(I, q);

	// convert from QImage to Pixmap; rescale if image is larger than view window
	QPixmap p;
	if(MIN(w, h) > MIN(ww, hh))
		p = QPixmap::fromImage(q.scaled(QSize(ww, hh), Qt::KeepAspectRatio));
	else	p = QPixmap::fromImage(q);

	// assign pixmap to label widget for display
	QLabel *widget = (QLabel *) m_stackWidgetImages->currentWidget();
	widget->setPixmap(p);

	// compute average runtime if time checkbox is set
	time();

	// compute histogram if histogram checkbox is set
	displayHistogram(I);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::displayHistogram:
//
// Display image histogram in control panel
//
void MainWindow::displayHistogram(ImagePtr I)
{
	int color;
	int histo[MXGRAY];
	int yminChannel=0, ymaxChannel=0;
	int yminHisto=0,   ymaxHisto=0;
	int xmin, xmax;
	QVector<double> x, y;
	char buf[MXSTRLEN];

	// verify if histogram checkbox is set
	if(!m_checkboxHisto->isChecked()) return;

	// clear any previous histogram plots
	m_histogram->clearGraphs();

	// visit all selected channels in I: RGB, R, G, B, or gray
	for(int ch=0; ch<I->maxChannel(); ch++) {
		// compute histogram
		IP_histogram(I, ch, histo, MXGRAY, m_histoXmin[ch], m_histoXmax[ch]);

		// init min and max for current channel
		yminChannel = ymaxChannel = histo[0];

		// init min and max histogram value among all channels
		if(!ch) yminHisto = ymaxHisto = histo[0];

		// clear vector of x- and y-coordinates
		x.clear();
		y.clear();

		// visit all histogram entries and save into x and y vectors
		for(int i=0; i<MXGRAY; i++) {
			x.push_back(i);
			y.push_back(histo[i]);

			// save channel min and max
			yminChannel = MIN(yminChannel, histo[i]);
			ymaxChannel = MAX(ymaxChannel, histo[i]);
		}

		// add new graph for histogram channel
        	m_histogram->addGraph();

		// if single channel was selected, it is in channel 0 and should
		// be drawn in corresponding color (based on m_histoColor).
		// Else, color is based on ch value: 0,1,2 corresponds to R,G,B
		// Add 1 to ch so that ch=1 corresponds to green, and ch=2 to blue
		if(!ch) color = m_histoColor;
		else	color = ch + 1;

		// convert xmin, xmax of channel into int
		xmin = m_histoXmin[ch];
		xmax = m_histoXmax[ch];

		// set histogram name and color
		switch(color) {
		case RGB:
		case R: sprintf(buf, "R: X[%d,%d] Y[%d,%d]",
			xmin, xmax, yminChannel, ymaxChannel);
			m_histogram->graph(ch)->setName (buf);
			m_histogram->graph(ch)->setPen  (QPen(Qt::red  ));
			m_histogram->graph(ch)->setBrush(QBrush(QColor(255,0,0,30)));
			break;
		case G: sprintf(buf, "G: X[%d,%d] Y[%d,%d]",
			xmin, xmax, yminChannel, ymaxChannel);
			m_histogram->graph(ch)->setName (buf);
			m_histogram->graph(ch)->setPen  (QPen(Qt::green));
			m_histogram->graph(ch)->setBrush(QBrush(QColor(0,255,0,30)));
			break;
		case B: sprintf(buf, "B: X[%d,%d] Y[%d,%d]",
			xmin, xmax, yminChannel, ymaxChannel);
			m_histogram->graph(ch)->setName (buf);
			m_histogram->graph(ch)->setPen  (QPen(Qt::blue ));
			m_histogram->graph(ch)->setBrush(QBrush(QColor(0,0,255,30)));
			break;
		case GRAY:
			sprintf(buf, "Gray: X[%d,%d] Y[%d,%d]",
			xmin, xmax, yminChannel, ymaxChannel);
			m_histogram->graph(ch)->setName (buf);
			m_histogram->graph(ch)->setPen  (QPen(Qt::black ));
			m_histogram->graph(ch)->setBrush(QBrush(QColor(80,80,80,80)));
			break;
		}

		// set data
		m_histogram->graph(ch)->setData(x,y);

		// update min and max histogram values among all channels
		yminHisto = MIN(yminChannel, yminHisto);
		ymaxHisto = MAX(ymaxChannel, ymaxHisto);
	}

	// turn on legend to print histogram params
	m_histogram->legend->setVisible(true);

	// set y-axis range to [0, ymaxHisto + 5%]
	m_histogram->yAxis->setRange(0, ymaxHisto*1.05);

	// set x-axis range to add side margins
	m_histogram->xAxis->setRange(-5, 260);

	// replot
	m_histogram->replot();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::setHisto:
//
// Slot to show/hide histogram and set/reset histogram checkbox.
//
void
MainWindow::setHisto(int flag)
{
        m_extension->setVisible(flag);
        if(flag)
                m_checkboxHisto->setCheckState(Qt::Checked);
        else    m_checkboxHisto->setCheckState(Qt::Unchecked);
        preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::setTime:
//
// Slot to show/hide runtime and set/reset time checkbox.
//
void
MainWindow::setTime(int flag)
{
        if(flag)
                m_checkboxTime->setCheckState(Qt::Checked);
        else    m_checkboxTime->setCheckState(Qt::Unchecked);
        preview();
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::mode:
//
// Slot functions to display RGB and grayscale images.
//
void MainWindow::modeRGB () { mode(0); }
void MainWindow::modeGray() { mode(1); }

void MainWindow::mode(int flag)
{
	// error checking
	if(m_imageSrc.isNull()) return;		// no input image

	if(flag)
		IP_castImage(m_imageIn,  BW_IMAGE, m_imageSrc);
	else	IP_castImage(m_imageIn, RGB_IMAGE, m_imageSrc);

	if(m_imageSrc->imageType() == BW_IMAGE)
		m_histoColor = GRAY;	// gray
	else	m_histoColor = 0;	// RGB

	// display image
	if(m_radioDisplay[0]->isChecked())	// displaying input image
		display(0);
	else	preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::preview:
//
// Display preview image.
//
void
MainWindow::preview()
{
	// apply filter to source image; save result in destination image
	if(m_code > 0) {
		m_imageFilter[m_code]->applyFilter(m_imageSrc, m_imageDst);
		display(1);
	} else {
		// display requested image
		if(m_radioDisplay[0]->isChecked())
			display(0);	// input
		else	display(1);	// output
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::execute:
//
// Determine which action was triggered and execute respective action.
// Skip this if action is not image-processing related: open(), save(), quit()
//
void
MainWindow::execute(QAction* action)
{
	// skip over menu ops that don't require image processing
	QString name = action->text();
	if(name == QString("&Open") || name == QString("&Save") || name == QString("&Quit"))
		return;

	// get code from action
	m_code = action->data().toInt();

	// set output radio button to true
	m_radioDisplay[1]->setChecked(true);

	// use code to index into stack widget and array of filters
	m_stackWidgetPanels->setCurrentIndex(m_code);
	preview();
}