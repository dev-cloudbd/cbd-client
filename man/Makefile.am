man_MANS = nbd-client.8 nbdtab.5
CLEANFILES = manpage.links manpage.refs
DISTCLEANFILES = nbd-client.8 nbdtab.5
MAINTAINERCLEANFILES = nbd-client.8.sh.in nbdtab.5.sh.in
EXTRA_DIST = nbd-client.8.in.sgml nbdtab.5.in.sgml nbd-client.8.sh.in nbdtab.5.sh.in sh.tmpl

nbd-client.8: nbd-client.8.sh
	sh nbd-client.8.sh > nbd-client.8
nbdtab.5: nbdtab.5.sh
	sh nbdtab.5.sh > nbdtab.5
nbd-client.8.sh.in: nbd-client.8.in.sgml sh.tmpl
	LC_ALL=C docbook2man nbd-client.8.in.sgml
	cat sh.tmpl > nbd-client.8.sh.in
	cat NBD-CLIENT.8 >> nbd-client.8.sh.in
	echo "EOF" >> nbd-client.8.sh.in
	rm NBD-CLIENT.8
nbdtab.5.sh.in: nbdtab.5.in.sgml sh.tmpl
	LC_ALL=C docbook2man nbdtab.5.in.sgml
	cat sh.tmpl > nbdtab.5.sh.in
	cat NBDTAB.5 >> nbdtab.5.sh.in
	echo "EOF" >> nbdtab.5.sh.in
	rm NBDTAB.5
