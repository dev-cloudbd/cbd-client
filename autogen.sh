#!/bin/sh
set -ex
make -C man -f Makefile.am cbd-client.8.sh.in cbdtab.5.sh.in
make -C systemd -f Makefile.am cbd@.service.sh.in
exec autoreconf -f -i
