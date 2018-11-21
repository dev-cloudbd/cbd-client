man_MANS = cbdtab.5
CLEANFILES = manpage.links manpage.refs
DISTCLEANFILES = cbdtab.5
MAINTAINERCLEANFILES = cbdtab.5.sh.in
EXTRA_DIST = cbdtab.5.in.sgml cbdtab.5.sh.in sh.tmpl

cbdtab.5: cbdtab.5.sh
	sh cbdtab.5.sh > cbdtab.5
cbdtab.5.sh.in: cbdtab.5.in.sgml sh.tmpl
	LC_ALL=C docbook2man cbdtab.5.in.sgml
	cat sh.tmpl > cbdtab.5.sh.in
	cat CBDTAB.5 >> cbdtab.5.sh.in
	echo "EOF" >> cbdtab.5.sh.in
	rm CBDTAB.5
