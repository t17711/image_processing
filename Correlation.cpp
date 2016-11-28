// ============================================================

// Correlation.cpp

#include "MainWindow.h"
#include "Correlation.h"
#include "hw2/IPconvolve.cpp"

extern MainWindow *g_mainWindowP;
enum { WSTEP_S, HSTEP_S, WSIZE_K, HSIZE_K, WSTEP_K, HSTEP_K, OFFSET, SAMPLER, KERNEL };

Correlation::Correlation(QWidget *parent) : ImageFilter(parent)
{
	m_kernel = NULL;
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_2D, m_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//NULL means reserve texture memory, but texels are undefined
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// correlation::controlPanel:
//
// Create group box for control panel.
//
QGroupBox * Correlation::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Convolve");

	// layout for assembling filter widget
	QVBoxLayout *vbox = new QVBoxLayout;

	// create file pushbutton
	m_button = new QPushButton("File");

	// create label  widget
	m_kernel_label = new QLabel();
	//m_kernel_label->setMaximumSize(100, 100);
	//m_kernel_label->setMinimumSize(50, 50);

	// assemble dialog
	vbox->addWidget(m_button);
	vbox->addWidget(m_kernel_label);
	m_ctrlGrp->setLayout(vbox);

	// init signal/slot connections
	connect(m_button, SIGNAL(clicked()), this, SLOT(load()));

	return(m_ctrlGrp);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Correlation::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool Correlation::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
	// error checking
	if (I1.isNull())		return 0;
	if (m_kernel.isNull())	return 0;
	m_width_i = I1->width();
	m_height_i = I1->height();
	// convolve image
	if (!(gpuFlag && m_shaderFlag))
		corr(I1, m_kernel, I2);
	else    g_mainWindowP->glw()->applyFilterGPU(m_nPasses);

	return 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Correlation::load:
//
// Slot to load filter kernel from file.
//
int
Correlation::load()
{
	QFileDialog dialog(this);

	// open the last known working directory
	if (!m_currentDir.isEmpty())
		dialog.setDirectory(m_currentDir);

	// display existing files and directories
	dialog.setFileMode(QFileDialog::ExistingFile);

	// invoke native file browser to select file
	m_file = dialog.getOpenFileName(this,
		"Open File", m_currentDir,
		"Images (*.jpg);;All files (*)");

	// verify that file selection was made
	if (m_file.isNull()) return 0;

	// save current directory
	QFileInfo f(m_file);
	m_currentDir = f.absolutePath();

	// read kernel
	m_kernel = IP_readImage(qPrintable(m_file));

	m_width_k = m_kernel->width();
	m_height_k = m_kernel->height();

	// update button with filename (without path)
	m_button->setText(f.fileName());
	m_button->update();
	QImage q;

	IP_IPtoQImage(m_kernel, q);
	
	m_kernel_label->setPixmap(QPixmap::fromImage(q, Qt::AutoColor));


	//q = GLWidget::convertToGLFormat(q);
	
	//glBindTexture(GL_TEXTURE_2D, m_tex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width_k, m_height_k, 0, GL_RGBA, GL_UNSIGNED_BYTE, q.bits());

	g_mainWindowP->preview();

		return 1;
}

void Correlation::setoutput(ImagePtr I1, ImagePtr kernel, ImagePtr I2, int xx, int yy)
{
	IP_copyImageHeader(I1, I2);
	int total = m_width_i * m_height_i;

	// if the coordinate falls between (xx, yy), (xx+kernel_width, yy +yernel_height)


	int type;
	int x = 0;
	int y = 0;
	ChannelPtr<uchar> p1, p1_2, p2, p3, endd;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		p1_2 = p1;
		IP_getChannel(I2, ch, p2, type);
		IP_getChannel(kernel, ch, p3, type);


		// go upto yy th row
		for (endd = p1_2 + yy*m_width_i; (p1 < endd);) *p2++ = (*p1++) / 2;

		// go upto m_width_i * m_kernel_height row
		int x = 0;
		for (endd += (m_height_k-1)*m_width_i; p1 < endd;)
		{
			if (xx <= x && x < (xx + m_width_k)){
				*p2++ = (*p1++) / 2 + (*p3++) / 2;
			}
			else
				*p2++ = (*p1++) / 2;
			x++;
			x = x%m_width_i; // make it
		}

		// go rest
		for (endd = p1_2 + total; p1 < endd;) *p2++ = (*p1++) / 2;

	}
}

void Correlation::corr(ImagePtr I1, ImagePtr kernel, ImagePtr I2)
{
	int xx;
	int yy;
	float val = 0.0f;

	// do correlation on grey
	ImagePtr I1_BW, Kernel_BW;
	IP_copyImageHeader(kernel, Kernel_BW);

	IP_castImage(kernel, BW_IMAGE, Kernel_BW);
	if (I1->imageType() != BW_IMAGE) {
		IP_copyImageHeader(I1, I1_BW);
		IP_castImage(I1, BW_IMAGE, I1_BW);
		val = IP_correlation(I1_BW, Kernel_BW, 1, 1, xx, yy);
	}
	else {
		val = IP_correlation(I1, Kernel_BW, 1, 1, xx, yy);
	}
	
	// displlay output
	setoutput(I1, kernel, I2, xx, yy);
}


void Correlation::initShader()
{

	m_nPasses = 1;

	// initialize GL function resolution for current context
	initializeGLFunctions();

	UniformMap uniforms;

	// init uniform hash table based on uniform variable names and location IDs
	uniforms["u_WStep_s"] = WSTEP_S;
	uniforms["u_HStep_s"] = HSTEP_S;

	uniforms["u_Wsize_k"] = WSIZE_K;
	uniforms["u_Hsize_k"] = HSIZE_K;
	uniforms["u_WStep_k"] = WSTEP_K;
	uniforms["u_HStep_k"] = HSTEP_K;
	uniforms["u_Sampler"] = SAMPLER;
//	uniforms["u_Kernel"] = KERNEL;

	QString v_name = ":/vshader_passthrough";
	QString f_name = ":/hw2/fshader_correlation";

	// compile shader, bind attribute vars, link shader, and initialize uniform var table
	g_mainWindowP->glw()->initShader(m_program[PASS1],
		v_name + ".glsl",
		f_name + ".glsl",
		uniforms,
		m_uniform[PASS1]);

	uniforms.clear();

	m_shaderFlag = true;
}

void Correlation::gpuProgram(int pass)
{
	int w_size = m_width_k;
	int h_size = m_height_k;
	if (w_size % 2 == 0) ++w_size;
	if (h_size % 2 == 0) ++h_size;

	glUseProgram(m_program[pass].programId());

	// pass values for texture
	glUniform1f(m_uniform[pass][WSTEP_S], (GLfloat) 1.0f / m_width_i);
	glUniform1f(m_uniform[pass][HSTEP_S], (GLfloat) 1.0f / m_height_i);
	
	// pass values for correlate
	glUniform1i(m_uniform[pass][HSIZE_K], h_size);
	glUniform1i(m_uniform[pass][WSIZE_K], w_size);
	glUniform1f(m_uniform[pass][WSTEP_K], (GLfloat) 1.0f / w_size);
	glUniform1f(m_uniform[pass][HSTEP_K], (GLfloat) 1.0f / h_size);
	glUniform1i(m_uniform[pass][SAMPLER], 0);

	//glBindTexture(GL_TEXTURE_2D, m_tex);
	//glUniform1i(m_uniform[PASS1][KERNEL], 0);


}
