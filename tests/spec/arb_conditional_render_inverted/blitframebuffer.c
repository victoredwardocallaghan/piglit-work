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
 * @file blitframebuffer.c
 *
 * Tests that conditional rendering also affects glBlitFramebuffer().
 *
 * It is clarified on page 679 of OpenGL 4.4 spec:
 *    "Added BlitFramebuffer to commands affected by conditional rendering in
 *     section 10.10 (Bug 9562)."
 */

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;
	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGBA;

PIGLIT_GL_TEST_CONFIG_END

static void fill_tex(int level, int w, int h, const GLfloat *color)
{
	GLfloat *data;
	int i;

	data = malloc(w * h * 4 * sizeof(GLfloat));
	for (i = 0; i < 4 * w * h; i += 4) {
		data[i + 0] = color[0];
		data[i + 1] = color[1];
		data[i + 2] = color[2];
		data[i + 3] = color[3];
	}
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, w, h, 0,
		     GL_RGBA, GL_FLOAT, data);
	free(data);
}

static void blit_window_to_tex(GLuint tex, int w, int h)
{
	GLuint fb;

	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, fb);
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex, 0);

	assert(glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);

	glBlitFramebufferEXT(0, h, w, 2 * h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, piglit_winsys_fbo);
	glDeleteFramebuffersEXT(1, &fb);
}

enum piglit_result
piglit_display(void)
{
	bool pass = true;
	float green[4] = {0.0, 1.0, 0.0, 0.0};
	GLuint q, texture;

	glClearColor(0.5, 0.5, 0.5, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);

	glGenQueries(1, &q);

	/* Generate query pass: draw bottom half of screen. */
	glColor4f(0.0, 0.0, 1.0, 0.0);
	glBeginQuery(GL_SAMPLES_PASSED, q);
	piglit_draw_rect(-1, -1, 2, 1);
	glEndQuery(GL_SAMPLES_PASSED);

	/* Draw bottom half of window to green. */
	glColor4fv(green);
	piglit_draw_rect(-1, -1, 2, 1);
	glColor4f(1, 1, 1, 1);

	/* Set up a green texture. */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	fill_tex(0, piglit_width, piglit_height / 2, green);

	/* BlitFramebuffer() should not be affected by conditional rendering. */
	glBeginConditionalRender(q, GL_QUERY_WAIT_INVERTED);
	/* Blit top half of the window to texture. */
	blit_window_to_tex(texture, piglit_width, piglit_height / 2);
	glEndConditionalRender();

	/* Draw the texture to top half of the window. */
	glEnable(GL_TEXTURE_2D);
	piglit_draw_rect_tex(-1, 0, 2, 1,
			     0, 0, 1, 1);
	glDisable(GL_TEXTURE_2D);

	pass = piglit_probe_rect_rgba(0, piglit_width / 2,
				      piglit_width, piglit_height / 2,
				      green);

	piglit_present_results();

	glDeleteQueries(1, &q);

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	piglit_require_gl_version(30);

	piglit_require_extension("GL_ARB_conditional_render_inverted");
	piglit_require_extension("GL_EXT_framebuffer_object");
	piglit_require_extension("GL_EXT_framebuffer_blit");
}
