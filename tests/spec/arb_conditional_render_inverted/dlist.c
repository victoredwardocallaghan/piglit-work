/*
 * Copyright © 2014 Intel Corporation
 * Copyright © 2014 Tobias Klausmann <tobias.johannes.klausmann@mni.thm.de>
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "piglit-util-gl.h"

/**
 * @file dlist.c
 *
 * Tests that conditional rendering appropriately affects commands
 * inside of display lists.
 */

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;
	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGBA;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	float green[4] = {0.0, 1.0, 0.0, 0.0};
	GLuint qfail, qpass;
	GLuint list;

	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	glGenQueries(1, &qfail);
	/* Generate query fail. */
	glBeginQuery(GL_SAMPLES_PASSED, qfail);
	glEndQuery(GL_SAMPLES_PASSED);

	glGenQueries(1, &qpass);
	/* Generate query pass: draw top half of screen. */
	glColor4f(0.0, 0.0, 1.0, 0.0);
	glBeginQuery(GL_SAMPLES_PASSED, qpass);
	piglit_draw_rect(-1, 0, 2, 1);
	glEndQuery(GL_SAMPLES_PASSED);

	/* Conditional render that should draw full screen. */
	glBeginConditionalRender(qfail, GL_QUERY_WAIT_INVERTED);
	glColor4f(0.0, 1.0, 0.0, 0.0);
	piglit_draw_rect(-1, -1, 2, 2);
	glEndConditionalRender();

	/* Conditional render that should not draw full screen. */
	glBeginConditionalRender(qpass, GL_QUERY_WAIT_INVERTED);
	glColor4f(1.0, 0.0, 0.0, 0.0);
	piglit_draw_rect(-1, -1, 2, 2);
	glEndConditionalRender();
	glEndList();

	glCallList(list);

	pass = piglit_probe_rect_rgba(0, 0, piglit_width, piglit_height,
				      green);

	piglit_present_results();

	glDeleteQueries(1, &qfail);
	glDeleteQueries(1, &qpass);

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_gl_version(30);

	piglit_require_extension("GL_ARB_conditional_render_inverted");
}
