/*
 *  Udp.cpp: Library to send/receive UDP packets with the Arduino ethernet shield.
 *  This version only offers minimal wrapping of socket.c/socket.h
 *  Drop Udp.h/.cpp into the Ethernet library directory at hardware/libraries/Ethernet/ 
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * bjoern@cs.stanford.edu 12/30/2008
 */

#include "w5100.h"
#include "socket.h"
#include "Ethernet.h"
#include "Udp.h"
#include "Dns.h"

/* Constructor */
UDP::UDP() : _sock(MAX_SOCK_NUM) {}

/* Start UDP socket, listening at local port PORT */
uint8_t UDP::begin(uint16_t port) {
  if (_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }

  if (_sock == MAX_SOCK_NUM)
    return 0;

  _port = port;
  socket(_sock, SnMR::UDP, _port, 0);

  return 1;
}

/* Is data available in rx buffer? Returns 0 if no, number of available bytes if yes. 
 * returned value includes 8 byte UDP header!*/
int UDP::available() {
  return W5100.getRXReceivedSize(_sock);
}

/* Release any resources being used by this UDP instance */
void UDP::stop()
{
  if (_sock == MAX_SOCK_NUM)
    return;

  close(_sock);

  EthernetClass::_server_port[_sock] = 0;
  _sock = MAX_SOCK_NUM;
}

int UDP::beginPacket(const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(Ethernet.dnsServerIP());
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return beginPacket(remote_addr, port);
  } else {
    return ret;
  }
}

int UDP::beginPacket(IPAddress ip, uint16_t port)
{
  _offset = 0;
  return startUDP(_sock, ip.raw_address(), port);
}

int UDP::endPacket()
{
  return sendUDP(_sock);
}

void UDP::write(uint8_t byte)
{
  write(&byte, 1);
}

void UDP::write(const char *str)
{
  size_t len = strlen(str);
  write((const uint8_t *)str, len);
}

void UDP::write(const uint8_t *buffer, size_t size)
{
  uint16_t bytes_written = bufferData(_sock, _offset, buffer, size);
  _offset += bytes_written;
}

int UDP::parsePacket()
{
  if (available() > 0)
  {
    //HACK - hand-parse the UDP packet using TCP recv method
    uint8_t tmpBuf[8];
    int ret =0;	
    //read 8 header bytes and get IP and port from it
    ret = recv(_sock,tmpBuf,8);
    if (ret > 0)
    {
      _remoteIP = tmpBuf;
      _remotePort = tmpBuf[4];
      _remotePort = (_remotePort << 8) + tmpBuf[5];
      // When we get here, any remaining bytes are the data
      ret = available();
    }
    return ret;
  }
  // There aren't any packets available
  return 0;
}

int UDP::read()
{
  uint8_t byte;
  if (recv(_sock, &byte, 1) > 0)
  {
    // We read things without any problems
    return byte;
  }
  // If we get here, there's no data available
  return -1;
}

int UDP::read(unsigned char* buffer, size_t len)
{
  /* In the readPacket that copes with truncating packets, the buffer was
     filled with this code.  Not sure why it loops round reading out a byte
     at a time.
  int i;
  for(i=0;i<(int)bufLen;i++) {
    recv(_sock,tmpBuf,1);
    buf[i]=tmpBuf[0];
  }
  */
  return recv(_sock, buffer, len);
}

int UDP::peek()
{
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  ::peek(_sock, &b);
  return b;
}

void UDP::flush()
{
  while (available())
  {
    read();
  }
}

