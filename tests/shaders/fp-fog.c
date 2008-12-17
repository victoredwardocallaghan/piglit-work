/*
 * Copyright © 2008 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file
 * Test passing fog coordinates into a fragment program.
 *
 * \author Ian Romanick <ian.d.romanick@intel.com>
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "piglit-util.h"

static GLint prog = 0;

static const char* const program_text =
	"!!ARBfp1.0\n"
	"MOV result.color, fragment.fogcoord;\n"
	"END\n"
	;

static int Automatic = 0;

static int Width = 50, Height = 50;

static PFNGLFOGCOORDFPROC pglFogCoordf = NULL;

static void Redisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	pglFogCoordf(0.3);
	glBegin(GL_QUADS);
	glVertex2f(0, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 2);
	glVertex2f(0, 2);
	glEnd();

	pglFogCoordf(0.6);
	glBegin(GL_QUADS);
	glVertex2f(1, 1);
	glVertex2f(2, 1);
	glVertex2f(2, 2);
	glVertex2f(1, 2);
	glEnd();

	pglFogCoordf(0.8);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(1, 0);
	glVertex2f(1, 1);
	glVertex2f(0, 1);
	glEnd();

	pglFogCoordf(0.4);
	glBegin(GL_QUADS);
	glVertex2f(1, 0);
	glVertex2f(2, 0);
	glVertex2f(2, 1);
	glVertex2f(1, 1);
	glEnd();

	glutSwapBuffers();

	static const struct {
		float x, y, r;
	}
	probes[4] = {
		{ 0.5, 1.5, 0.3 },
		{ 1.5, 1.5, 0.6 },
		{ 0.5, 0.5, 0.8 },
		{ 1.5, 0.5, 0.4 },
	};
	int pass = 1;
	unsigned i;

	for (i = 0; i < 4; i++) {
		float result_color[4];
		glReadPixels((int)(probes[i].x * Width / 2),
		             (int)(probes[i].y * Height / 2),
		             1, 1,
		             GL_RGBA, GL_FLOAT, result_color);
		printf("%3.1f, %3.1f => { %f %f %f %f }\n",
		       probes[i].x, probes[i].y,
		       result_color[0], result_color[1],
		       result_color[2], result_color[3]);

		if ((result_color[1] != 0.0f)
		    || (result_color[2] != 0.0f)
		    || (result_color[3] != 1.0f)) {
			pass = 0;
		}

		if (fabs(result_color[0] - probes[i].r) > 0.01) {
			pass = 0;
		}
	}

	if (Automatic) {
		printf("\nPIGLIT: { 'result': '%s' }\n",
		       pass ? "pass" : "fail");
		exit(0);
	}
}


static void Reshape(int width, int height)
{
	Width = width;
	Height = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 2.0, 0.0, 2.0, -2.0, 6.0);
	glScalef(1.0, 1.0, -1.0); // flip z-axis
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


static void Key(unsigned char key, int x, int y)
{
	(void) x;
	(void) y;

	if (key) {
		exit(0);
	}

	glutPostRedisplay();
}


static void Init(void)
{
	printf("GL_RENDERER = %s\n", (char *) glGetString(GL_RENDERER));

	glClearColor(0.3, 0.3, 0.3, 0.3);

	if (atof((const char *) glGetString(GL_VERSION)) >= 1.4) {
		pglFogCoordf = (PFNGLFOGCOORDFPROC)
			glutGetProcAddress("glFogCoordf");
	} else if (glutExtensionSupported("GL_EXT_fog_coord")) {
		pglFogCoordf = (PFNGLFOGCOORDFPROC)
			glutGetProcAddress("glFogCoordfEXT");
	} else {
		piglit_report_result(PIGLIT_SKIP);
	}

	piglit_require_fragment_program();
	prog = piglit_compile_program(GL_FRAGMENT_PROGRAM_ARB, program_text);

	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	pglBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prog);

	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

	glReadBuffer(GL_FRONT);
	Reshape(Width, Height);
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	if ((argc > 1) && (strcmp(argv[1], "-auto") == 0)) {
		Automatic = 1;
	}

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Width, Height);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	glutDisplayFunc(Redisplay);
	Init();
	glutMainLoop();
	return 0;
}
