#include "QGLDisplay.h"
#include "MainWindow.h"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QGLDisplay::QGLDisplay:
//
// QGLDisplay constructor.
//

QGLDisplay::QGLDisplay(QWidget *parent)
	: QGLWidget(parent)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QGLDisplay::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
QGLDisplay::initializeGL()
{
	initializeGLFunctions();
	initalizeShader();

	// clear screen
	glClearColor(0.1f,0.1f,0.1f,0.1f);
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glMatrixMode(GL_MODELVIEW);

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QGLDisplay::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
QGLDisplay::resizeGL(int w, int h)
{
	// save window dimensions
	m_winW = w;
	m_winH = h;
	int side = qMin(w, h);
	// compute aspect ratio
	float ar = (float)w / h;

	// set xmax, ymax;
	float xmax, ymax;
	if (ar > 1.0) {		// wide screen
		xmax = ar;
		ymax = 1.;
	}
	else {		// tall screen
		xmax = 1.;
		ymax = 1 / ar;
	}


	glViewport((w - side) / 2, (h - side) / 2, side, side); // keeps original image size
	
	// init viewing coordinates for orthographic projection
	glLoadIdentity();
	glOrtho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);
	
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QGLDisplay::paintGL:
//
// Update GL scene.
//
void
QGLDisplay::paintGL()
{
	glClearColor(0.1, 0.1, 0.1, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	initializeTexture();
	// bind coordinates, vertices and textures to gl
	
}

void	
QGLDisplay::initializeTexture(){

	QImage I;
	I = g_mainWindowP->imageDisp();

	if (I.isNull()) { // if there is no current image then do nothing
		return;
	}
	
	I = QGLWidget::convertToGLFormat(I);
	
	glGenTextures(1, &m_textures);
	glBindTexture(GL_TEXTURE_2D, m_textures);
	m_textures = bindTexture(I, GL_TEXTURE_2D); // add pixel value to m_texture, to paint
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, I.width(), I.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, I.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	
	float ratio_img = (float)I.width() / I.height();
	
	float x = 0.9f, y=x;
	if (ratio_img > 1.0){ // gl screen is made to be 0-2 height and width so 1 aspect ratio
		y /= ratio_img; // if image  ratio is bigger then we compress height by image ratio
	}
	else{
		x *= ratio_img; // if image ratio is smaller we stretch image
	}
	const vec2 cord[] = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 0.0f)

	};

	// vertices for texture
	const vec2 vert[] = {
		vec2(-1.0f*x, -1.0f*y),
		vec2(-1.0f*x, 1.0f*y),
		vec2(1.0f*x, 1.0f*y),
		vec2(1.0f*x, -1.0f*y)

	};

	// now load texture coordinates, i later add this to gl on paint
	m_texCoords.clear();
	m_vertices.clear();
	for (int i = 0; i < 4; ++i) {
		m_texCoords.append(cord[i]);
		m_vertices.append(vert[i]);
	}

	glVertexPointer(2, GL_FLOAT, 0, m_vertices.constData());  // vertex pointer 
	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords.constData());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	
}

bool
QGLDisplay::initalizeShader(){

	// compile vertex shader
	m_program = new QGLShaderProgram(this);
	if (!m_program->addShaderFromSourceFile(QGLShader::Vertex, ":/vshader0.glsl")) {
		QMessageBox::critical(0, "Vertex shader error ", m_program->log(), QMessageBox::Ok);
		QApplication::quit();
	}

	// compile fragment shader
	if (!m_program->addShaderFromSourceFile(QGLShader::Fragment, ":/fshader0.glsl")) {
		QMessageBox::critical(0, "Error", "Fragment shader error", QMessageBox::Ok);
		QApplication::quit();
	}

	// set a_position in code as vertex attribute, and a_coord as texture coordinate attribute, texture as uniform
	glBindAttribLocation(m_program->programId(), ATTRIB_VERTEX, "a_Position");
	glBindAttribLocation(m_program->programId(), ATTRIB_TEXTURE_POSITION, "a_texcoord");
	glBindAttribLocation(m_program->programId(), 0, "texture");
	// link this program
	m_program->link();
	m_program->bind();
	
	glUseProgram(m_program->programId());

	return true;
}


void
QGLDisplay::updateImage(){
	updateGL();
}
