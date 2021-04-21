static:
	$(MAKE) -C zlib -f myMakefile static
	$(MAKE) -C libpng -f myMakefile static
	$(MAKE) -C freetype -f myMakefile static
	$(MAKE) -C app -f Makefile static

dynamic:
	$(MAKE) -C zlib -f myMakefile dynamic
	$(MAKE) -C libpng -f myMakefile dynamic
	$(MAKE) -C freetype -f myMakefile dynamic
	$(MAKE) -C app -f Makefile dynamic

blob:
	$(MAKE) -C zlib -f myMakefile blob
	$(MAKE) -C libpng -f myMakefile blob
	$(MAKE) -C freetype -f myMakefile blob
	$(MAKE) -C app -f Makefile blob

clean:
	$(MAKE) -C zlib -f myMakefile clean
	$(MAKE) -C libpng -f myMakefile clean
	$(MAKE) -C freetype -f myMakefile clean
	$(MAKE) -C app -f Makefile clean

all: clean static dynamic blob