APR_DIR=@APR_DIR@
APR_CFLAGS=@APR_CFLAGS@
APR_EXTRA_LIBS=@APR_EXTRA_LIBS@

CFLAGS += $(APR_CFLAGS)
LIBS += $(APR_EXTRA_LIBS)
bin_PROGRAMS = tester

STANDARD_LDADDS = $(top_srcdir)/src/libekhtml.la \
		$(APR_DIR)/libapr.la \
		$(APR_EXTRA_LIBS)

tester_SOURCES = tester.c
tester_LDADD   = $(STANDARD_LDADDS)
tester_DEPENDENCIES = $(top_srcdir)/src/libekhtml.la

$(top_srcdir)/src/libekhtml.la:
	cd $(top_srcdir)/src && $(MAKE)

TESTS = test_basic.sh test_crazypage.sh
