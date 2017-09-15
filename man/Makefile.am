man_MANS = cbd-client.8 cbdtab.5
CLEANFILES = manpage.links manpage.refs
DISTCLEANFILES = cbd-client.8 cbdtab.5
MAINTAINERCLEANFILES = cbd-client.8.sh.in cbdtab.5.sh.in
EXTRA_DIST = cbd-client.8.in.sgml cbdtab.5.in.sgml cbd-client.8.sh.in cbdtab.5.sh.in sh.tmpl

cbd-client.8: cbd-client.8.sh
	sh cbd-client.8.sh > cbd-client.8
cbdtab.5: cbdtab.5.sh
	sh cbdtab.5.sh > cbdtab.5
cbd-client.8.sh.in: cbd-client.8.in.sgml sh.tmpl
	LC_ALL=C docbook2man cbd-client.8.in.sgml
	cat sh.tmpl > cbd-client.8.sh.in
	cat CBD-CLIENT.8 >> cbd-client.8.sh.in
	echo "EOF" >> cbd-client.8.sh.in
	rm CBD-CLIENT.8
cbdtab.5.sh.in: cbdtab.5.in.sgml sh.tmpl
	LC_ALL=C docbook2man cbdtab.5.in.sgml
	cat sh.tmpl > cbdtab.5.sh.in
	cat CBDTAB.5 >> cbdtab.5.sh.in
	echo "EOF" >> cbdtab.5.sh.in
	rm CBDTAB.5
