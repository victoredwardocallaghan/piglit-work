/*
 * Copyright © 2013 Intel Corporation
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

/**
 * \file sso-common.c
 * Utility functions used by multiple separate shader objects tests.
 */
#include "piglit-util-gl.h"
#include "sso-common.h"

/**
 * Pick a GLSL version that will work with explicit location layout qualifiers
 *
 * Some NVIDIA drivers have issues with layout qualifiers, 'in' keywords, and
 * 'out' keywords in "lower" GLSL versions.  If the driver supports GLSL >=
 * 1.40, use 1.40.  Otherwise, pick the highest version that the driver
 * supports.
 *
 * 1.40 is selected as the maximum version because core-profile contexts
 * aren't required to support versions earlier than 1.40.  Otherwise, 1.30
 * would also work.
 */
unsigned
pick_a_glsl_version(void)
{
	unsigned glsl_version;
	bool es;
	int glsl_major;
	int glsl_minor;

	piglit_get_glsl_version(&es, &glsl_major, &glsl_minor);
	glsl_version = ((glsl_major * 100) + glsl_minor) >= 140
		? 140 : ((glsl_major * 100) + glsl_minor);

	return glsl_version;
}
