/*
 * Copyright 2020 Xavier Claessens
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>

typedef struct
{
  int dummy;
} MyObject;

static void
test_typeof (void)
{
#if __cplusplus >= 201103L
  // Test that with C++11 we don't get those kind of errors:
  // error: invalid conversion from ‘gpointer’ {aka ‘void*’} to ‘MyObject*’ [-fpermissive]
  MyObject *obj = g_rc_box_new0 (MyObject);
  MyObject *obj2 = g_rc_box_acquire (obj);
  g_assert_true (obj2 == obj);

  MyObject *obj3 = g_atomic_pointer_get (&obj2);
  g_assert_true (obj3 == obj);

  MyObject *obj4 = nullptr;
  g_atomic_pointer_set (&obj4, obj3);
  g_assert_true (obj4 == obj);

  MyObject *obj5 = nullptr;
  g_atomic_pointer_compare_and_exchange (&obj5, nullptr, obj4);
  g_assert_true (obj5 == obj);

  MyObject *obj6 = g_steal_pointer (&obj5);
  g_assert_true (obj6 == obj);

  g_clear_pointer (&obj6, g_rc_box_release);
  g_rc_box_release (obj);
#else
  g_test_skip ("This test requires a C++11 compiler");
#endif
}

static void
test_atomic_pointer_compare_and_exchange (void)
{
#if __cplusplus >= 201103L
  const gchar *str1 = "str1";
  const gchar *str2 = "str2";
  const gchar *atomic_string = str1;

  g_test_message ("Test that g_atomic_pointer_compare_and_exchange() with a "
                  "non-void* pointer doesn’t have any compiler warnings in C++ mode");

  g_assert_true (g_atomic_pointer_compare_and_exchange (&atomic_string, str1, str2));
  g_assert_true (atomic_string == str2);
#else
  g_test_skip ("This test requires a C++11 compiler");
#endif
}

static void
test_atomic_int_compare_and_exchange (void)
{
#if __cplusplus >= 201103L
  gint atomic_int = 5;

  g_test_message ("Test that g_atomic_int_compare_and_exchange() doesn’t have "
                  "any compiler warnings in C++ mode");

  g_assert_true (g_atomic_int_compare_and_exchange (&atomic_int, 5, 50));
  g_assert_cmpint (atomic_int, ==, 50);
#else
  g_test_skip ("This test requires a C++11 compiler");
#endif
}

int
main (int argc, char *argv[])
{
#if __cplusplus >= 201103L
  g_test_init (&argc, &argv, nullptr);
#else
  g_test_init (&argc, &argv, NULL);
#endif

  g_test_add_func ("/C++/typeof", test_typeof);
  g_test_add_func ("/C++/atomic-pointer-compare-and-exchange", test_atomic_pointer_compare_and_exchange);
  g_test_add_func ("/C++/atomic-int-compare-and-exchange", test_atomic_int_compare_and_exchange);

  return g_test_run ();
}
