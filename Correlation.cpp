// ============================================================

// Correlation.cpp

#include "MainWindow.h"
#include "Correlation.h"
#include "hw2/IPconvolve.cpp"

extern MainWindow *g_mainWindowP;
enum { WSTEP_S, HSTEP_S, WSIZE_K, HSIZE_K, WSTEP_K, HSTEP_K, OFFSET, COLOR,SAMPLER, KERNEL,PASS,NORMALIZER };

Correlation::Correlation(QWidget *parent) : ImageFilter(parent)
{
	m_kernel = NULL;
	
	m_passthrough = false;
	m_gpu_processed = false;
	
}


void Correlation::GPU_initialize()
{
	m_passthrough = false;
	m_gpu_processed = false;
	m_GPU_out->setDisabled(true);
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
		GPU_initialize();
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
	GPU_initialize();
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
	if (m_kernel != NULL) IP_clearImage(m_kernel);

	m_kernel = IP_readImage(qPrintable(m_file));
	m_width_k = m_kernel->width();
	m_height_k = m_kernel->height();

	// update button with filename (without path)
	m_button->setText(f.fileName());
	m_button->update();
	
	QImage m_image;
	IP_IPtoQImage(m_kernel, m_image);
	m_kernel_label->setPixmap(QPixmap::fromImage(m_image, Qt::AutoColor).scaled(m_kernel_label->height(), m_kernel_label->width()));
	
	g_mainWindowP->preview();

		return 1;
}

// this function overlaps the kernel image to input inage, both at half intensity so they look full intensity at the place of overlap
void Correlation::setoutput(ImagePtr I1, ImagePtr kernel, ImagePtr I2, int xx, int yy)
{
	IP_copyImageHeader(I1, I2);
	int total = m_width_i * m_height_i;

	int w = m_width_i;
	int h = m_height_i;

	int type;
	ChannelPtr<uchar> p1, p1_2, p2, p3, endd;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {

		int type;
		IP_getChannel(kernel, ch, p3, type);
		IP_getChannel(I2, ch, p2, type);
		// now for each channel copy all row and col
		// wherever the input inage overlaps copy both image else only copy 1 image

		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				if (row >= yy && row < yy + m_height_k && col >= xx && col < xx + m_width_k) {
					if (row == yy) *p2 = 255; // get white border
					else if (col == xx) *p2 = 255;
					else if (row == yy + m_height_k-1) *p2 = 255;
					else if (col == xx + m_width_k-1) *p2 = 255;
					else
						*p2 = *p3 / 2 + *p1 / 2;;
					p2++; p1++; p3++;
				}
				else {
					*p2 = *p1 / 2;
					p2++; p1++;
				}
			}

		}
	}


}

//this just calls gpu verison of correlarion
//i just make sure that both image are of same color when I send them for overlap
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

	// if color then make both grey to get correltion
	if (m_color) {
		IP_copyImageHeader(I1, I1_BW);
		IP_castImage(I1, BW_IMAGE, I1_BW);
		val = IP_correlation(I1_BW, Kernel_BW, 1, 1, xx, yy);
	}
	else {
		val = IP_correlation(I1, Kernel_BW, 1, 1, xx, yy);
	}
	
	// display output
	if (!m_color) {
		setoutput(I1, Kernel_BW, I2, xx, yy);
	}
	else {
		setoutput(I1, kernel, I2, xx, yy);
	}

	QImage t;
	IP_IPtoQImage(I1, t);
	g_mainWindowP->glw()->setInTexture(t);

}

// this just reads the correlation values on screen from gpu processing and creates overlapped image
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

	int x;
	int y;

	// get the coordinates, i wrote this function
	g_mainWindowP->glw()->get_img(PASS1,x,y);

	// do correlation on grey
	ImagePtr  Kernel_BW;
	ImagePtr m_gpu_out;// = IP_allocImage(w, h, RGB_TYPE);

	
	// get over lapped image
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

	// display
	g_mainWindowP->glw()->setInTexture(m_image);
	g_mainWindowP->glw()->applyFilterGPU(m_nPasses);
	g_mainWindowP->glw()->update();

	GPU_initialize();
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
	uniforms["u_normalizor"] = NORMALIZER;
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
		GPU_initialize();
	}
	else {
		m_gpu_processed = true;
		m_passthrough = true;
		m_GPU_out->setDisabled(false);
		QImage m_image;
		IP_IPtoQImage(m_kernel, m_image);

		ImagePtr c;
		IP_castImage(m_kernel, BW_IMAGE, c);
	
		m_tex = (g_mainWindowP->glw()->setTemplateTexture(m_image));
		
		ChannelPtr<uchar> p;
		int type;
		IP_getChannel(c, 0, p, type);
		int total = w_size*h_size;
		float normalizer = 0.0f;
		for (int i = 0; i<total; i++) {
			normalizer += pow((float)p[i] / 255.0f,2);
		}

		IP_clearImage(c);
		normalizer = sqrt(normalizer);
		glUniform1f(m_uniform[pass][NORMALIZER], normalizer);

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