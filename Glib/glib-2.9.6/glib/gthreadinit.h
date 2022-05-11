/* gthreadinit.h - GLib internal thread initialization functions
 *
 *  Copyright (C) 2003 Sebastian Wilhelmi
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *   Boston, MA 02111-1307, USA.
 */

#ifndef __G_THREADINIT_H__
#define __G_THREADINIT_H__

G_BEGIN_DECLS

/* Is called from gthread/gthread-impl.c */
void g_thread_init_glib (void);

/* base initializers, may only use g_mutex_new(), g_cond_new() */
void _g_mem_thread_init_noprivate_nomessage (void) G_GNUC_INTERNAL;
/* initializers that may also use g_private_new() */
void _g_slice_thread_init_nomessage	    (void) G_GNUC_INTERNAL;
void _g_messages_thread_init_nomessage      (void) G_GNUC_INTERNAL;

/* full fledged initializersa */
void _g_convert_thread_init (void) G_GNUC_INTERNAL;
void _g_rand_thread_init (void) G_GNUC_INTERNAL;
void _g_main_thread_init (void) G_GNUC_INTERNAL;
void _g_atomic_thread_init (void) G_GNUC_INTERNAL;
void _g_utils_thread_init (void) G_GNUC_INTERNAL;
#ifdef G_OS_WIN32
void _g_win32_thread_init (void) G_GNUC_INTERNAL;
#endif

G_END_DECLS
 
#endif /* __G_THREADINIT_H__ */
