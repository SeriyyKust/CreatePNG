STATIC_LIB=../freetype/l_static/libfreetype.a ../libpng/l_static/libpng.a ../zlib/l_static/libz.a
DYNAMIC_LIB=../freetype/l_dynamic/libfreetype.so ../libpng/l_dynamic/libpng.so ../zlib/l_dynamic/libz.so
L_STATIC_LIB=-L../freetype/l_static -L../libpng/l_static -L../zlib/l_static
L_DYNAMIC_LIB=-L../freetype/l_dynamic -L../libpng/l_dynamic -L../zlib/l_dynamic
INCLUDES=-I../freetype/include -I../libpng
ELFLOADERC_C=../elfloader/elf_loader.c ../elfloader/main.c ../elfloader/wheelc/list.c

CFLAGS_BLOB=-fno-stack-protector -fno-exceptions -nostdlib -pie -fPIE -fPIC -shared -DBLOB

static: app.c $(STATIC_LIB)
	cp -f test.ttf static/test.ttf
	gcc $(INCLUDES) $(L_STATIC_LIB) app.c -static -lfreetype -lpng -lz -lm -o  static/app_static.out

dynamic: app.c $(DYNAMIC_LIB)
	cp -f test.ttf dynamic/test.ttf
	cp -f ../freetype/l_dynamic/libfreetype.so dynamic/libfreetype.so
	cp -f ../libpng/l_dynamic/libpng.so dynamic/libpng.so
	gcc $(INCLUDES) -DDYNAMIC $(L_DYNAMIC_LIB) app.c -lm -ldl -o dynamic/app_dynamic.out	

blob: app.c  $(STATIC_LIB) $(ELFLOADERC_C)
	cp -f test.ttf blob/test.ttf
	gcc $(ELFLOADERC_C) -lm -ldl -o blob/elfloader.out
	gcc $(INCLUDES) $(L_STATIC_LIB) $(CFLAGS_BLOB) app.c -lfreetype -lpng -lz -lm -o blob/blob.bin

clean:
	rm -f blob/*.*
	rm -f static/*.*
	rm -f dynamic/*.*

all: clean static dynamic blob
