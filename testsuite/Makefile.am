bin_PROGRAMS = tester

STANDARD_LDADDS = $(top_srcdir)/src/libekhtml.la $(APR_LDFLAGS)

tester_SOURCES = tester.c
tester_LDADD   = $(STANDARD_LDADDS)
tester_DEPENDENCIES = $(top_srcdir)/src/libekhtml.la

$(top_srcdir)/src/libekhtml.la:
	cd $(top_srcdir)/src && $(MAKE)

TESTS = test_basic.sh test_crazypage.sh
