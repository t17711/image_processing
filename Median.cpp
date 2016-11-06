// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Median.cpp - Median widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Median.h"
#include "hw2/HW_median.cpp"

extern MainWindow *g_mainWindowP;
enum { WSIZE, WSTEP, HSIZE, HSTEP, SAMPLER };
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::Median:
//
// Constructor.
//
Median::Median(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Median::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Median");

	// layout for assembling filter widget
	QGridLayout *layout = new QGridLayout;

	// alloc array of labels
	QLabel *label[2];

	// create sliders and spinboxes
	for(int i=0; i<2; i++) {
		// create label[i]
		label[i] = new QLabel(m_ctrlGrp);
		if(!i) label[i]->setText("Filter size");
		else   label[i]->setText("Iterations");

		if(!i) {
			// create slider
			m_slider [i] = new QSlider(Qt::Horizontal, m_ctrlGrp);
			m_slider [i]->setRange(3, 9);
			m_slider [i]->setValue(3);
			m_slider [i]->setSingleStep(2);
			m_slider [i]->setTickInterval(1);
			m_slider [i]->setTickPosition(QSlider::TicksBelow);

			// create spinbox
			m_spinBox[i] = new QSpinBox(m_ctrlGrp);
			m_spinBox[i]->setRange(3, 9);
			m_spinBox[i]->setValue(3);
			m_spinBox[i]->setSingleStep(2);
		} else {
			// create slider
			m_slider [i] = new QSlider(Qt::Horizontal, m_ctrlGrp);
			m_slider [i]->setRange(1, 20);
			m_slider [i]->setValue(1);
			m_slider [i]->setSingleStep(1);
			m_slider [i]->setTickInterval(2);
			m_slider [i]->setTickPosition(QSlider::TicksBelow);

			// create spinbox
			m_spinBox[i] = new QSpinBox(m_ctrlGrp);
			m_spinBox[i]->setRange(1, 20);
			m_spinBox[i]->setValue(1);
			m_spinBox[i]->setSingleStep(1);
		}

		// assemble dialog
		layout->addWidget(label    [i], i, 0);
		layout->addWidget(m_slider [i], i, 1);
		layout->addWidget(m_spinBox[i], i, 2);
	}

	// init signal/slot connections
	connect(m_slider [0], SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect(m_spinBox[0], SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	connect(m_slider [1], SIGNAL(valueChanged(int)), this, SLOT(changeItrs(int)));
	connect(m_spinBox[1], SIGNAL(valueChanged(int)), this, SLOT(changeItrs(int)));

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Median::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// collect parameters
	int size = m_slider[0]->value();	// filter size
	int itrs = m_slider[1]->value();	// iterations
	m_width  = I1->width();
	m_height = I1->height();
	// apply median filter
	if(!(gpuFlag && m_shaderFlag))
		if(itrs == 1) {
			median(I1, size, I2);
		} else {
			IP_copyImage(I1, I2);
			for(int i=0; i<itrs; i++)
				median(I2, size, I2);
		}
	else    g_mainWindowP->glw()->applyFilterGPU(m_nPasses);


	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::median:
//
// Apply median filter on image I1. Median filter has size sz x sz.
// Output is in I2.
//
void
Median::median(ImagePtr I1, int sz, ImagePtr I2)
{
	HW_median(I1, sz, sz, 0, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::changeSize:
//
// Slot to process change in median filter size caused by moving the slider.
//
void
Median::changeSize(int value)
{
	m_slider [0]->blockSignals(true);
	m_slider [0]->setValue    (value);
	m_slider [0]->blockSignals(false);
	m_spinBox[0]->blockSignals(true);
	m_spinBox[0]->setValue    (value);
	m_spinBox[0]->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::changeItrs:
//
// Slot to process change in iterations caused by moving the slider.
//
void
Median::changeItrs(int value)
{
	m_slider [1]->blockSignals(true);
	m_slider [1]->setValue    (value);
	m_slider [1]->blockSignals(false);
	m_spinBox[1]->blockSignals(true);
	m_spinBox[1]->setValue    (value);
	m_spinBox[1]->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::reset:
//
// Reset parameters.
//
void
Median::reset()
{
	m_slider[0]->setValue(3);
	m_slider[1]->setValue(1);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::initShader:
//
// init shader program and parameters.
//
void
Median::initShader() 
{
	m_nPasses = 1;
	// initialize GL function resolution for current context
	initializeGLFunctions();

	UniformMap uniforms;

	// init uniform hash table based on uniform variable names and location IDs
	uniforms["u_Wsize"] = WSIZE;
	uniforms["u_WStep"] = WSTEP;
	uniforms["u_Hsize"] = HSIZE;
	uniforms["u_HStep"] = HSTEP;
	uniforms["u_Sampler"] = SAMPLER;

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
	g_mainWindowP->glw()->initShader(m_program[PASS1],
		QString(":/hw2/vshader_median.glsl"),
		QString(":/hw2/fshader_median.glsl"),
		uniforms,
		m_uniform[PASS1]);
	uniforms.clear();
	m_shaderFlag = true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::gpuProgram:
//
// Active gpu program
//
void
Median::gpuProgram(int pass) 
{
	int w_size = m_slider[0]->value();
	int h_size = w_size;
	if (w_size % 2 == 0) ++w_size;
	if (h_size % 2 == 0) ++h_size;
	glUseProgram(m_program[pass].programId());
	glUniform1i(m_uniform[pass][WSIZE], w_size);
	glUniform1f(m_uniform[pass][WSTEP], (GLfloat) 1.0f / m_width);
	glUniform1f(m_uniform[pass][HSTEP], (GLfloat) 1.0f / m_height);
	glUniform1i(m_uniform[pass][SAMPLER], 0);
	glUniform1i(m_uniform[pass][HSIZE], h_size);
}
