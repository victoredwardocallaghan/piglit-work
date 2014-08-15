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
 * @file begin-zero.c
 *
 * Tests that starting conditional rendering on a 0 query object
 * results in INVALID_VALUE.
 *
 * From the OpenGL 3.0 spec (page 63):
 *
 * "If BeginConditionalRender is called while conditional rendering is in
 *  progress, or if EndConditionalRender is called while conditional rendering
 * is not in progress, the error INVALID OPERATION is generated. The error
 * INVALID VALUE is generated if <id> is not the name of an existing query object
 * query. The error INVALID OPERATION is generated if <id> is the name of a query
 * object with a target other than SAMPLES PASSED , or <id> is the name of a
 * query currently in progress."
 */

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	return PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_gl_version(30);

	piglit_require_extension("GL_ARB_conditional_render_inverted");

	glBeginConditionalRender(0, GL_QUERY_WAIT_INVERTED);
	if (!piglit_check_gl_error(GL_INVALID_VALUE))
		piglit_report_result(PIGLIT_FAIL);

	piglit_report_result(PIGLIT_PASS);
}
