// ============================================================

// Correlation.cpp

#include "MainWindow.h"
#include "Correlation.h"
#include "hw2/IPconvolve.cpp"

extern MainWindow *g_mainWindowP;
enum { WSTEP_S, HSTEP_S, WSIZE_K, HSIZE_K, WSTEP_K, HSTEP_K, OFFSET, COLOR,SAMPLER, KERNEL,PASS };

Correlation::Correlation(QWidget *parent) : ImageFilter(parent)
{
	m_kernel = NULL;
	m_passthrough = false;
	m_gpu_processed = false;
	
	
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
	m_GPU_out = new QPushButton("Gpu Display");

	// create label  widget
	m_kernel_label = new QLabel();
	m_kernel_label->setMaximumSize(100, 100);
	m_kernel_label->setMinimumSize(50, 50);

	// assemble dialog
	vbox->addWidget(m_button);
	vbox->addWidget(m_kernel_label);
	vbox->addWidget(m_GPU_out);
	m_GPU_out->setDisabled(true);
	m_ctrlGrp->setLayout(vbox);

	// init signal/slot connections
	connect(m_button, SIGNAL(clicked()), this, SLOT(load()));
	connect(m_GPU_out, SIGNAL(clicked()), this, SLOT(GPU_out()));

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

	if (I1->imageType() == BW_IMAGE) m_color = false;
	else m_color = true;

	// convolve image
	if (!(gpuFlag && m_shaderFlag)) {
		m_passthrough = false;
		m_gpu_processed = false;
		m_GPU_out->setDisabled(true);
		corr(I1, m_kernel, I2);
	}
	else {
		g_mainWindowP->glw()->applyFilterGPU(m_nPasses);
	}
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
	m_gpu_processed = false;
	m_passthrough = false;
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
	
	QImage m_image;
	IP_IPtoQImage(m_kernel, m_image);

	m_kernel_label->setPixmap(QPixmap::fromImage(m_image, Qt::AutoColor));
	
	//m_tex = (g_mainWindowP->glw()->setTemplateTexture(m_image));
	//	g_mainWindowP->glw()->m_setTemplate(m_uniform[PASS1][KERNEL]);

	g_mainWindowP->preview();

		return 1;
}

void Correlation::setoutput(ImagePtr I1, ImagePtr kernel, ImagePtr I2, int xx, int yy)
{
	IP_copyImageHeader(I1, I2);
	int total = m_width_i * m_height_i;

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
			x = x%m_width_i; // make it be in range
		}

		// go rest
		for (endd = p1_2 + total; p1 < endd;) *p2++ = (*p1++) / 2;

	}
}

void Correlation::corr(ImagePtr I1, ImagePtr kernel, ImagePtr I2)
{
	// if the coordinate falls between (xx, yy), (xx+kernel_width, yy +yernel_height)
	if (m_width_k > m_width_i || m_height_k > m_height_i) {

		IP_copyImageHeader(I1, I2);
		IP_copyImage(I1, I2);

		return;
	}


	int xx;
	int yy;
	float val = 0.0f;

	// do correlation on grey
	ImagePtr I1_BW, Kernel_BW;
	IP_copyImageHeader(kernel, Kernel_BW);
	IP_castImage(kernel, BW_IMAGE, Kernel_BW);

	IP_copyImageHeader(I1, I1_BW);
	IP_castImage(I1, BW_IMAGE, I1_BW);

	if (m_color) {
		val = IP_correlation(I1_BW, Kernel_BW, 1, 1, xx, yy);
	}
	else {
		val = IP_correlation(I1, Kernel_BW, 1, 1, xx, yy);
	}
	
	// displlay output
	if (!m_color) {
		setoutput(I1, Kernel_BW, I2, xx, yy);
	}
	else {
		setoutput(I1, kernel, I2, xx, yy);
	}
}


int Correlation::GPU_out()
{

	// there is already image in  g_mainwindou destination. 
	// get max position from that in x and y coordinate and send to setoutput(ImagePtr I1, ImagePtr kernel, ImagePtr I2, int xx, int yy) function
	if (!m_gpu_processed) return 0;
	if (m_kernel.isNull()) return 0;

	//ImagePtr m_Gpu_output;
	int w = m_width_i;
	int h = m_height_i;
	int total = w*h;

	std::vector<int> val;
	g_mainWindowP->glw()->get_img(PASS1, val, w, h);

	int max_pos = 0;
	int pos = 0;

	int x = 0;
	int y = 0;
	int kw = m_kernel->width();
	int kh = m_kernel->height();

	pos = 0;
	int max = val[pos];

	// loop through pixel dont have t look h-kh & w - kw place
	while (pos < total) {
		if (max < val[pos]) {
			max = val[pos];
			max_pos = pos;
		}
		pos++;
	}

	x = max_pos%w;
	y = max_pos / w;


	if (x > (w - kw)) x = w - kw;
	if (y > (h - kh)) y = h - kh;

	// do correlation on grey
	ImagePtr  Kernel_BW;
	ImagePtr m_gpu_out;// = IP_allocImage(w, h, RGB_TYPE);

	
	// displlay output
	if (!m_color) {
		IP_castImage(m_kernel, BW_IMAGE, Kernel_BW);
		m_gpu_out = IP_allocImage(w, h, BW_TYPE);
		setoutput(g_mainWindowP->imageSrc(), Kernel_BW, m_gpu_out, x, y);
	}
	else {
		m_gpu_out = IP_allocImage(w, h, RGB_TYPE);
		setoutput(g_mainWindowP->imageSrc(), m_kernel, m_gpu_out, x, y);
	}

	QImage m_image;
	IP_IPtoQImage(m_gpu_out, m_image);

	g_mainWindowP->glw()->setInTexture(m_image);
	g_mainWindowP->glw()->applyFilterGPU(m_nPasses);
	g_mainWindowP->glw()->update();

	return 1;
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
	uniforms["u_Kernel"] = KERNEL;
	uniforms["u_Color"] = COLOR;
	uniforms["u_passthrough"] = PASS;
	QString v_name = ":/vshader_passthrough";
	QString f_name = ":/hw2/fshader_correlation";


#ifdef __APPLE__
	v_name += "_Mac";
	f_name += "_Mac";
#endif   

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


	if (m_passthrough) {
		m_passthrough = false;
		m_gpu_processed = false;
		m_GPU_out->setDisabled(true);
	}
	else {
		m_gpu_processed = true;
		m_passthrough = true;
		m_GPU_out->setDisabled(false);
		QImage m_image;
		IP_IPtoQImage(m_kernel, m_image);
		m_tex = (g_mainWindowP->glw()->setTemplateTexture(m_image));
	}

	glUniform1i(m_uniform[pass][PASS], !m_passthrough);


	// pass values for texture

	glUniform1f(m_uniform[pass][WSTEP_S], (GLfloat) 1.0f / m_width_i);
	glUniform1f(m_uniform[pass][HSTEP_S], (GLfloat) 1.0f / m_height_i);

	// pass values for correlate
	glUniform1i(m_uniform[pass][HSIZE_K], h_size);
	glUniform1i(m_uniform[pass][WSIZE_K], w_size);
	glUniform1f(m_uniform[pass][WSTEP_K], (GLfloat) 1.0f / (w_size));
	glUniform1f(m_uniform[pass][HSTEP_K], (GLfloat) 1.0f / (h_size));
	glUniform1i(m_uniform[pass][COLOR], m_color);
	glUniform1i(m_uniform[pass][SAMPLER], 0);

}