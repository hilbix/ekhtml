EXTRA_DIST = html 

html/index.html: Doxyfile $(top_srcdir)/configure.in $(top_srcdir)/include/ekhtml.h
	rm -rf html
	doxygen

html: html/index.html
