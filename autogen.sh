#!/bin/sh
set -ex
make -C man -f Makefile.am nbd-client.8.sh.in nbdtab.5.sh.in
make -C systemd -f Makefile.am nbd@.service.sh.in
exec autoreconf -f -i
