/*
 * This file is part of the Nice GLib ICE library.
 *
 * (C) 2006, 2007 Collabora Ltd.
 *  Contact: Dafydd Harries
 * (C) 2006, 2007 Nokia Corporation. All rights reserved.
 *  Contact: Kai Vehmanen
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Nice GLib ICE library.
 *
 * The Initial Developers of the Original Code are Collabora Ltd and Nokia
 * Corporation. All Rights Reserved.
 *
 * Contributors:
 *   Dafydd Harries, Collabora Ltd.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
 * case the provisions of LGPL are applicable instead of those above. If you
 * wish to allow use of your version of this file only under the terms of the
 * LGPL and not to allow others to use your version of this file under the
 * MPL, indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL. If you do
 * not delete the provisions above, a recipient may use your version of this
 * file under either the MPL or the LGPL.
 */

#include <string.h>

#include "stun.h"

static void
test_pack_mapped_address (void)
{
  StunAttribute *attr = stun_attribute_mapped_address_new (0x02030405, 2345);
  gchar *packed;
  guint length;

  length = stun_attribute_pack (attr, &packed);

  g_assert (12 == length);
  g_assert (NULL != packed);

  g_assert (0 == memcmp (packed,
    "\x00\x01"          // type
    "\x00\x08"          // length
    "\x00\x01"          // padding, address family
    "\x09\x29"          // port
    "\x02\x03\x04\x05", // IP address
    length));
  g_free (packed);
  stun_attribute_free (attr);
}

static void
test_pack_username (void)
{
  StunAttribute *attr;
  gchar *packed;
  guint length;

  attr = stun_attribute_username_new ("abcdefghi");
  length = stun_attribute_pack (attr, &packed);

  // 4 bytes header + 9 bytes padded to 32 bits = 16
  g_assert (16 == length);
  // type
  g_assert (0 == memcmp (packed + 0, "\x00\x06", 2));
  // length
  g_assert (0 == memcmp (packed + 2, "\x00\x09", 2));
  // value
  g_assert (0 == memcmp (packed + 4, "abcdefghi\0\0\0", length - 4));

  g_free (packed);
  stun_attribute_free (attr);
}

int
main (void)
{
  test_pack_mapped_address ();
  test_pack_username ();
  return 0;
}

