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

/*
 * Implementation of UDP socket interface using Berkeley sockets. (See
 * http://en.wikipedia.org/wiki/Berkeley_sockets.)
 */

#include <arpa/inet.h>

#include <unistd.h>

#include "udp-bsd.h"

/*** NiceUDPSocket ***/

static gint
socket_recv (
  NiceUDPSocket *sock,
  NiceAddress *from,
  guint len,
  gchar *buf)
{
  gint recvd;
  struct sockaddr_in sin = {0,};
  guint from_len = sizeof (sin);

  recvd = recvfrom (sock->fileno, buf, len, 0, (struct sockaddr *) &sin,
      &from_len);

  from->type = NICE_ADDRESS_TYPE_IPV4;
  from->addr_ipv4 = ntohl (sin.sin_addr.s_addr);
  from->port = ntohs (sin.sin_port);

  return recvd;
}

static gboolean
socket_send (
  NiceUDPSocket *sock,
  NiceAddress *to,
  guint len,
  const gchar *buf)
{
  struct sockaddr_in sin;

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl (to->addr_ipv4);
  sin.sin_port = htons (to->port);

  sendto (sock->fileno, buf, len, 0, (struct sockaddr *) &sin, sizeof (sin));
  return TRUE;
}

static void
socket_close (NiceUDPSocket *sock)
{
  close (sock->fileno);
}

/*** NiceUDPSocketFactory ***/

static gboolean
socket_factory_init_socket (
  G_GNUC_UNUSED
  NiceUDPSocketFactory *man,
  NiceUDPSocket *sock,
  NiceAddress *addr)
{
  gint sockfd;
  struct sockaddr_in name = {0,};
  guint name_len = sizeof (name);

  sockfd = socket (PF_INET, SOCK_DGRAM, 0);

  if (sockfd < 0)
    return FALSE;

  name.sin_family = AF_INET;

  if (addr != NULL)
    {
      if (addr->addr_ipv4 != 0)
        name.sin_addr.s_addr = htonl (addr->addr_ipv4);
      else
        name.sin_addr.s_addr = INADDR_ANY;

      if (addr->port != 0)
        name.sin_port = htons (addr->port);
    }

  if (bind (sockfd, (struct sockaddr *) &name, sizeof (name)) != 0)
    {
      close (sockfd);
      return FALSE;
    }

  if (getsockname (sockfd, (struct sockaddr *) &name, &name_len) != 0)
    {
      close (sockfd);
      return FALSE;
    }

  if (name.sin_addr.s_addr == INADDR_ANY)
    sock->addr.addr_ipv4 = 0;
  else
    sock->addr.addr_ipv4 = ntohl (name.sin_addr.s_addr);

  sock->addr.port = ntohs (name.sin_port);

  sock->fileno = sockfd;
  sock->send = socket_send;
  sock->recv = socket_recv;
  sock->close = socket_close;
  return TRUE;
}

static void
socket_factory_close (
  G_GNUC_UNUSED
  NiceUDPSocketFactory *man)
{
}

void
nice_udp_bsd_socket_factory_init (
  G_GNUC_UNUSED
  NiceUDPSocketFactory *man)
{
  man->init = socket_factory_init_socket;
  man->close = socket_factory_close;
}

