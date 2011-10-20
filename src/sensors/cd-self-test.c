/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2010-2011 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <limits.h>
#include <stdlib.h>

#include <glib.h>
#include <glib-object.h>

#include "cd-buffer.h"
#include "cd-math.h"
#include "cd-usb.h"

static void
cd_test_buffer_func (void)
{
	guchar buffer[4];

	cd_buffer_write_uint16_be (buffer, 255);
	g_assert_cmpint (buffer[0], ==, 0x00);
	g_assert_cmpint (buffer[1], ==, 0xff);
	g_assert_cmpint (cd_buffer_read_uint16_be (buffer), ==, 255);

	cd_buffer_write_uint16_le (buffer, 8192);
	g_assert_cmpint (buffer[0], ==, 0x00);
	g_assert_cmpint (buffer[1], ==, 0x20);
	g_assert_cmpint (cd_buffer_read_uint16_le (buffer), ==, 8192);
}

static void
cd_test_usb_func (void)
{
	CdUsb *usb;
	gboolean ret;
	GError *error = NULL;

	usb = cd_usb_new ();
	g_assert (usb != NULL);
	g_assert (!cd_usb_get_connected (usb));
	g_assert (cd_usb_get_device_handle (usb) == NULL);

	/* try to load */
	ret = cd_usb_load (usb, &error);
	g_assert_no_error (error);
	g_assert (ret);

	/* attach to the default mainloop */
	ret = cd_usb_attach_to_context (usb, NULL, &error);
	g_assert (ret);
	g_assert_no_error (error);

	/* connect to a non-existant device */
	ret = cd_usb_connect (usb, 0xffff, 0xffff, 0x1, 0x1, &error);
	g_assert (!ret);
	g_assert_error (error, CD_USB_ERROR, CD_USB_ERROR_INTERNAL);
	g_error_free (error);

	g_object_unref (usb);
}

static void
cd_test_math_func (void)
{
	CdMat3x3 mat;
	CdMat3x3 matsrc;

	/* matrix */
	mat.m00 = 1.00f;
	cd_mat33_clear (&mat);
	g_assert_cmpfloat (mat.m00, <, 0.001f);
	g_assert_cmpfloat (mat.m00, >, -0.001f);
	g_assert_cmpfloat (mat.m22, <, 0.001f);
	g_assert_cmpfloat (mat.m22, >, -0.001f);

	/* multiply two matrices */
	cd_mat33_clear (&matsrc);
	matsrc.m01 = matsrc.m10 = 2.0f;
	cd_mat33_matrix_multiply (&matsrc, &matsrc, &mat);
	g_assert_cmpfloat (mat.m00, <, 4.1f);
	g_assert_cmpfloat (mat.m00, >, 3.9f);
	g_assert_cmpfloat (mat.m11, <, 4.1f);
	g_assert_cmpfloat (mat.m11, >, 3.9f);
	g_assert_cmpfloat (mat.m22, <, 0.001f);
	g_assert_cmpfloat (mat.m22, >, -0.001f);
}

int
main (int argc, char **argv)
{
	g_type_init ();
	g_test_init (&argc, &argv, NULL);

	/* only critical and error are fatal */
	g_log_set_fatal_mask (NULL, G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL);

	/* tests go here */
	g_test_add_func ("/colord/usb", cd_test_usb_func);
	g_test_add_func ("/colord/buffer", cd_test_buffer_func);
	g_test_add_func ("/colord/math", cd_test_math_func);
	return g_test_run ();
}

