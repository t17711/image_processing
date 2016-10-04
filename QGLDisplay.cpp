#include "QGLDisplay.h"
#include "MainWindow.h"

extern MainWindow *g_mainWindowP;

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
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
	initializeTexture();

	qglClearColor(Qt::yellow);
	qglColor(Qt::white);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glMatrixMode(GL_MODELVIEW);


	// initialize vertex coordinates
	// coordinates for the texture
	const vec2 cord[] = {
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),

	};

	float sz = 0.9f; // scale size

	// vertices for texture
	const vec3 vert[] = {
		vec3(	1.0f*sz,	1.0f*sz,	1.0f),
		vec3(	-1.0f*sz,	1.0f*sz,	1.0f),
		vec3(	-1.0f*sz,	-1.0f*sz,	1.0f),
		vec3(	1.0f*sz,	-1.0f*sz,	1.0f)

	};

	// now load texture coordinates, i later add this to gl on paint
	for (int i = 0; i < 4; ++i) {
		m_texCoords.append(cord[i]);
		m_vertices.append(vert[i]);
	}


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
	// compute aspect ratio
	float ar = (float)w / h;
	int side = qMin(w, h);

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

	// set viewport to occupy full canvas
	glViewport((w - side) / 2, (h - side) / 2, side, side);

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
	qglClearColor(Qt::yellow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// bind coordinates, vertices and textures to gl
	glVertexPointer(3, GL_FLOAT, 0, m_vertices.constData());  // vertex pointer 
	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords.constData());
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, m_textures); // bind texture position to gl, i have addes texture to this from initialize Texture();
	glDrawArrays(GL_TRIANGLE_FAN,0, 4);
}

void	
QGLDisplay::initializeTexture(){

	QImage I;
	I = g_mainWindowP->imageDisp();

	if (I.isNull()) { // if there is no current image then do nothing
		return;
	}
	m_textures = -1;// initialize
	m_textures = bindTexture(I.mirrored(), GL_TEXTURE_2D); // add pixel value to m_texture, to paint
	
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
	glBindAttribLocation(m_program->programId(), PROGRAM_TEXCOORD_ATTRIBUTE, "a_texcoord");
	m_program->setUniformValue("texture", 0);

	// link this program
	m_program->link();
	m_program->bind();

	glUseProgram(m_program->programId());

	return true;
}


void
QGLDisplay::updateImage(){
	initializeTexture();
	updateGL();
}
