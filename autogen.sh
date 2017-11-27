#!/bin/sh
set -ex
make -C man -f Makefile.am cbd-client.8.sh.in cbdtab.5.sh.in
make -C system -f Makefile.am cbdsetup@.service.sh.in cbdsetup-generator.sh.in
make -C system -f Makefile.am 99-cbd.rules.sh.in cbd_id.sh.in cbd_dmsetup.sh.in
make -C system -f Makefile.am cbddisks_start.sh.in cbddisks_stop.sh.in cbdsetup.functions.sh.in
make -C system -f Makefile.am cbddisks.sh.in
exec autoreconf -f -i
