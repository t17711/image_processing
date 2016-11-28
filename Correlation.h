#ifndef CORRELATION_H
#define CORRELATION_H

#include "ImageFilter.h"

class Correlation :
	public ImageFilter
{
	Q_OBJECT

public:
	Correlation				(QWidget *parent = 0);		// constructor
	QGroupBox*				controlPanel();				// create control panel
	bool					applyFilter(ImagePtr, bool, ImagePtr);
	void			setoutput(ImagePtr I1, ImagePtr kernel, ImagePtr I2, int xx, int yy);
	// apply filter to input
	void					corr(ImagePtr, ImagePtr, ImagePtr);
	void					initShader();
	void					gpuProgram(int pass);	// use GPU program to apply filter

protected slots:
	int		load();


private:
	// widgets
	QPushButton*	m_button;	// Convolve pushbutton
	QLabel*		m_kernel_label;	// text field for kernel values
	QGroupBox*	m_ctrlGrp;	// groupbox for panel

							// variables
	QString		m_file;
	QString		m_currentDir;
	ImagePtr	m_kernel;
	int			m_width_i;	// input image width
	int			m_height_i;	// input image height
	int			m_width_k;	// input image width
	int			m_height_k;	// input image height
	GLuint      m_tex;

};

#endif