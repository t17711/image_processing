// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// HW.h - Header file for HW class. Base class of homework solutions.
//
// Written by: George Wolberg, 2015
// ======================================================================

#ifndef QGLDISPLAY_H
#define QGLDISPLAY_H

#include <QtWidgets>
#include <QGLWidget>
#include <GL/glu.h>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QtOpenGL>

typedef QVector2D vec2;
typedef QVector3D vec3;

enum {
	ATTRIB_VERTEX,
	ATTRIB_COLOR,
	ATTRIB_TEXTURE_POSITION
};

// ----------------------------------------------------------------------
// standard include files
//
class QGLDisplay : public QGLWidget, protected QGLFunctions {
	Q_OBJECT
	public:
		explicit QGLDisplay(QWidget *parent=0);		// constructor
		bool	initalizeShader();  // pass shader files to this
		void	initializeTexture();
		void	display(int flag); // flag 0 = input , 1= output
		void	updateImage();

	protected:
		void		initializeGL();		// init GL state
		void		resizeGL(int, int);	// resize GL widget
		void		paintGL();		// render GL scene

private:
	int		  m_winW;			// window width
	int		  m_winH;			// window height

	QGLShaderProgram  *m_program;
	//QGLTexture *m_texture;
	GLuint m_textures, m_position, m_texcoord; //texture stored here
	
	QVector<QVector3D> m_vertices;
	QVector<QVector2D> m_texCoords;

	//// shader file names
	//QString v_shader;
	//QString f_shader;

	//// vertex shader files
	//QGLShader* m_vert;
	//QGLShader* m_frag;

	};

#endif // HW0A_H


