#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <wchar.h>
#include "ft2build.h"
#include "png.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define MAX_CHAR_IN_STRING 50
#define LINE_SPACING 2

#ifdef DYNAMIC
#include <dlfcn.h>

	int (*SO_FT_Init_FreeType)(FT_Library *);
	int (*SO_FT_New_Face)(FT_Library, const char *, FT_Long, FT_Face *);
	int (*SO_FT_Set_Pixel_Sizes)(FT_Face, FT_UInt, FT_UInt);
	int (*SO_FT_Load_Char)(FT_Face, FT_ULong, FT_Int32);
	int (*SO_FT_Get_Char_Index)(FT_Face face, FT_ULong charcode);
	int (*SO_FT_Load_Glyph)(FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags);
	int (*SO_FT_Render_Glyph)(FT_GlyphSlot slot, FT_Render_Mode  render_mode);
	int (*SO_FT_Get_Glyph)(FT_GlyphSlot slot, FT_Glyph *aglyph);
	int (*SO_FT_Glyph_To_Bitmap)(FT_Glyph* the_glyph, FT_Render_Mode render_mode, FT_Vector* origin, FT_Bool destroy);
	int (*SO_FT_Done_Glyph)(FT_Glyph glyph);

	void *(*SO_png_create_write_struct)(const char *, void *, void *, void *);
	void *(*SO_png_create_info_struct)(void *);
	void (*SO_png_init_io)(void *, void *);
	void (*SO_png_set_IHDR)(void *, void *, unsigned int, unsigned int, int, int, int, int, int);
	void (*SO_png_write_end)(void *, void *);
	jmp_buf*(*SO_png_set_longjmp_fn)(png_structrp png_ptr, png_longjmp_ptr longjmp_fn, size_t jmp_buf_size);
	void(*SO_png_set_rows)(png_const_structrp png_ptr, png_inforp info_ptr, png_bytepp row_pointers);
	void(*SO_png_write_png)(png_structrp png_ptr, png_inforp info_ptr, int transforms, png_voidp params);
	void (*SO_png_destroy_write_struct)(void *, void *);

	#define FT_Init_FreeType	SO_FT_Init_FreeType
	#define FT_New_Face			SO_FT_New_Face
	#define FT_Set_Pixel_Sizes	SO_FT_Set_Pixel_Sizes
	#define FT_Load_Char		SO_FT_Load_Char
	#define FT_Get_Char_Index	SO_FT_Get_Char_Index
	#define FT_Load_Glyph		SO_FT_Load_Glyph
	#define FT_Render_Glyph		SO_FT_Render_Glyph
	#define FT_Get_Glyph		SO_FT_Get_Glyph
	#define FT_Glyph_To_Bitmap	SO_FT_Glyph_To_Bitmap
	#define FT_Done_Glyph		SO_FT_Done_Glyph

	#define png_create_write_struct		SO_png_create_write_struct
	#define png_create_info_struct		SO_png_create_info_struct
	#define png_init_io					SO_png_init_io
	#define png_set_IHDR				SO_png_set_IHDR
	#define png_write_end				SO_png_write_end
	#define png_set_longjmp_fn			SO_png_set_longjmp_fn
	#define png_set_rows				SO_png_set_rows
	#define png_write_png				SO_png_write_png
	#define png_destroy_write_struct	SO_png_destroy_write_struct

	void *SO_freetype;
	void *SO_libpng;
		int SO_open()
		{
		SO_freetype=dlopen("./libfreetype.so", RTLD_LAZY);
		if(!SO_freetype){
			printf("Bad load libfreetype.so\n");
			return 0;
		}
		SO_libpng=dlopen("./libpng.so", RTLD_LAZY);
		if(!SO_libpng){
			printf("Bad load libpng.so\n");
			return 0;
		}

		SO_FT_Init_FreeType=	dlsym(SO_freetype, "FT_Init_FreeType");
		SO_FT_New_Face=			dlsym(SO_freetype, "FT_New_Face");
		SO_FT_Set_Pixel_Sizes=	dlsym(SO_freetype, "FT_Set_Pixel_Sizes");
		SO_FT_Load_Char=		dlsym(SO_freetype, "FT_Load_Char");
		SO_FT_Get_Char_Index=	dlsym(SO_freetype, "FT_Get_Char_Index");
		SO_FT_Load_Glyph=		dlsym(SO_freetype, "FT_Load_Glyph");
		SO_FT_Render_Glyph=		dlsym(SO_freetype, "FT_Render_Glyph");
		SO_FT_Get_Glyph=		dlsym(SO_freetype, "FT_Get_Glyph");
		SO_FT_Glyph_To_Bitmap=	dlsym(SO_freetype, "FT_Glyph_To_Bitmap");
		SO_FT_Done_Glyph=		dlsym(SO_freetype, "FT_Done_Glyph");

		SO_png_create_write_struct=		dlsym(SO_libpng, "png_create_write_struct");
		SO_png_create_info_struct=		dlsym(SO_libpng, "png_create_info_struct");
		SO_png_init_io=					dlsym(SO_libpng, "png_init_io");
		SO_png_set_IHDR=				dlsym(SO_libpng, "png_set_IHDR");
		SO_png_write_end=				dlsym(SO_libpng, "png_write_end");
		SO_png_set_longjmp_fn=			dlsym(SO_libpng, "png_set_longjmp_fn");
		SO_png_set_rows=				dlsym(SO_libpng, "png_set_rows");
		SO_png_write_png=				dlsym(SO_libpng, "png_write_png");
		SO_png_destroy_write_struct=	dlsym(SO_libpng, "png_destroy_write_struct");

		return 1;
	}

	int SO_close(){
		dlclose(SO_freetype);
		dlclose(SO_libpng);
	}
#endif


FT_Library ft_library;
FT_Face face;
FT_Glyph glyph;
FT_UInt glyph_index;
FT_BitmapGlyph bitmap_glyph;
FT_Bitmap bitmap;


wchar_t *CharConvertToWchar(char *string_char, int *string_wchar_length)
{
	const size_t cSize = strlen(string_char)+1;
    wchar_t* wc = (wchar_t*)malloc((cSize)*sizeof(wchar_t));;
    *string_wchar_length = mbstowcs (wc, string_char, cSize);
    return wc;
}

void FindMaxHAndW(char *string_char, int *maxH, int *maxW, int *bottomMargin, int *leftMargin)
{
	*maxH=0;
	*maxW=0;
	*leftMargin=0;
	*bottomMargin=0;
	short check_symbol[256];
	for (int i = 0; i < 256; i++)
		check_symbol[i] = 0;
	for (int i = 0; i < strlen(string_char); ++i)
	{		
		if(check_symbol[string_char[i]] == 0) 
		{
			check_symbol[string_char[i]]=1;
			FT_Load_Char(face, (wchar_t)string_char[i], FT_LOAD_RENDER);
			glyph_index=FT_Get_Char_Index(face, (wchar_t)string_char[i]);
			FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			FT_Get_Glyph(face->glyph, &glyph);
			FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
			bitmap_glyph = (FT_BitmapGlyph) glyph;
			bitmap = bitmap_glyph->bitmap;

			int tmp_Margin=bitmap.rows-bitmap_glyph->top;
			if(tmp_Margin>0&&*bottomMargin<tmp_Margin) 
			{
				*bottomMargin=tmp_Margin;
			}
			tmp_Margin=bitmap_glyph->left;
			if(tmp_Margin<0)
			{
				tmp_Margin=-tmp_Margin;
			}
			if(tmp_Margin>0&&*leftMargin<tmp_Margin) {
				*leftMargin=tmp_Margin;
			}

			if(bitmap_glyph->top>*maxH){
				*maxH=bitmap_glyph->top;
			}
			if(bitmap.width>*maxW){
				*maxW=bitmap.width;
			}
			FT_Done_Glyph(glyph);
		}
	}
}

int CreatePNG(unsigned char **image, int height, int width, char *image_name)
{
	FILE *fp=fopen(image_name, "wb");
	if(!fp){
		return 1;
	}

	png_structp png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr){
		fclose(fp);
		return 1;
	}
	png_infop png_info;
	if(!(png_info=png_create_info_struct(png_ptr))){
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		return 1;
	}

	if(setjmp(png_jmpbuf(png_ptr))){
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		return 1;
	}

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_rows(png_ptr, png_info, image);
	png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
	png_write_end(png_ptr, png_info);

	png_destroy_write_struct(&png_ptr, NULL);
	fclose(fp);
}

#ifdef BLOB

void **glob_functions;

#define N_CLOCK 0
#define N_PUTS 1
#define N_STRLEN 2
#define N_STRRCHR 3
#define N_STPCPY 4
#define N_STRSTR 5
#define N_GETENV 6
#define N_MALLOC 7
#define N_STRNCMP 8
#define N_MEMCHR 9
#define N_STRCMP 10
#define N_MEMSET 11
#define N_STRNCPY 12
#define N_MMAP 13
#define N_REALLOC 14
#define N_MEMMOVE 15
#define N_FREXP 16
#define N_MODF 17
#define N_POW 18
#define N_MEMCPY 19
#define N_FPUTC 20
#define N_MEMCMP 21
#define N_FREAD 22
#define N_GMTIME 23
#define N_FOPEN 24
#define N_FERROR 25
#define N_FCLOSE 26
#define N_REMOVE 27
#define N_MUNMAP 28
#define N_FREE 29
#define N_FWRITE 30
#define N_FFLUSH 31





clock_t clock(void){
	return ((clock_t (*)(void))glob_functions[N_CLOCK])();
}

int puts(const char *str){
	return ((int (*)(const char *))glob_functions[N_PUTS])(str);
}

size_t strlen(const char *str){
	return ((size_t (*)(const char *))glob_functions[N_STRLEN])(str);
}

char *strrchr(const char *str, int ch){
	return ((char* (*)(const char *, int))glob_functions[N_STRRCHR])(str, ch);
}

char *strstr(const char *str1, const char *str2){
	return ((char* (*)(const char *, const char*))glob_functions[N_STRSTR])(str1, str2);
}

char *getenv(const char *name){
	return ((char* (*)(const char *))glob_functions[N_GETENV])(name);
}

int strncmp(const char *str1, const char *str2, size_t count){
	return ((int (*)(const char *, const char*, size_t))glob_functions[N_STRNCMP])(str1, str2, count);
}

void *memchr(const void *buffer, int ch, size_t count){
	return ((void* (*)(const void*, int, size_t))glob_functions[N_MEMCHR])(buffer, ch, count);
}

int strcmp(const char *str1, const char *str2){
	return ((int (*)(const char *, const char*))glob_functions[N_STRCMP])(str1, str2);
}

void *memset(void *buf, int ch, size_t count){
	return ((void* (*)(void*, int, size_t))glob_functions[N_MEMSET])(buf, ch, count);
}

char *strncpy(char *dest, const char *source, size_t count){
	return ((char* (*)(char*, const char *, size_t))glob_functions[N_STRNCMP])(dest, source, count);
}

void * mmap(void *start, size_t length, int prot , int flags, int fd, off_t offset){
	return ((char* (*)(void *, size_t, int, int, int, off_t))glob_functions[N_MMAP])(start, length, prot, flags, fd, offset);
}

void *realloc(void *ptr, size_t newsize){
	return ((void* (*)(void *, size_t))glob_functions[N_REALLOC])(ptr, newsize);
}

void *memmove(void *dest, const void *source, size_t count){
	return ((void* (*)(void*, const void *, size_t))glob_functions[N_MEMMOVE])(dest, source, count);
}

double frexp (double num, int *exp){
	return ((double (*)(double, int*))glob_functions[N_FREXP])(num, exp);
}

double modf (double num, double *i){
	return ((double (*)(double, double*))glob_functions[N_MODF])(num, i);
}

double pow (double base, double exp){
	return ((double (*)(double, double))glob_functions[N_POW])(base, exp);
}

int fputc(int ch, FILE *stream){
	return ((int (*)(int, FILE*))glob_functions[N_FPUTC])(ch, stream);
}

int memcmp(const void *buf1, const void *buf2, size_t count){
	return ((int (*)(const void*, const void *, size_t))glob_functions[N_MEMCMP])(buf1, buf2, count);
}

size_t fread(void *buf, size_t size, size_t count, FILE *stream){
	return ((size_t (*)(void *, size_t, size_t, FILE*))glob_functions[N_FREAD])(buf, size, count, stream);
}

struct tm *gmtime(const time_t *time){
	return ((struct tm *(*)(const time_t *))glob_functions[N_GMTIME])(time);
}

FILE *fopen(const char *fname, const char *mode){
	return ((FILE * (*)(const char *, const char *))glob_functions[N_FOPEN])(fname, mode);
}

int munmap(void *start, size_t length){
	return ((int (*)(void *, size_t))glob_functions[N_MUNMAP])(start, length);
}

void free(void *ptr){
	return ((void (*)(void*))glob_functions[N_FREE])(ptr);
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *stream){
	return ((size_t (*)(const void *, size_t, size_t, FILE*))glob_functions[N_FWRITE])(buf, size, count, stream);
}

int fflush(FILE *stream){
	return ((int (*)(FILE *))glob_functions[N_FFLUSH])(stream);
}

void *memcpy(void *dest, const void *source, size_t count){
	return ((void* (*)(void*, const void *, size_t))glob_functions[N_MEMCPY])(dest, source, count);
}

void *malloc(size_t size){
	return ((void* (*)(size_t))glob_functions[N_MALLOC])(size);
}

int main(int argc, char *argv[], void **functions){
	glob_functions=functions;
#else
int main(int argc, char *argv[])
{
#endif
	float beginClock=clock();

	char *font_name=NULL;
	char *image_name=NULL;
	char *tmp_text=NULL;
	wchar_t *text=NULL;

	if(argc!=4)
	{
		printf("Error args. (Example: ./app.out test.ttf \"Tom\" Tom.png)");
		return 1;
	}

	font_name=argv[1];
	tmp_text=argv[2];
	image_name=argv[3];


	FILE *test_font=fopen(font_name, "rb");
	if(!test_font)
	{
		printf("Error *.ttf\n");
		return 1;
	}
	fclose(test_font);

	#ifdef DYNAMIC
	if(!SO_open())
	{
		return 1;
	}
	clock_t centrTime=clock();
	printf("time %ld clocks, %ld seconds\n", centrTime-startTime, (centrTime-startTime)/CLOCKS_PER_SEC);
	#endif

	FT_Init_FreeType(&ft_library);
	FT_New_Face(ft_library, font_name, 0, &face);

	int fontSize=20;
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	int height;
	int width;
	int length;
	int leftMargin;
	int bottomMargin;
	text = CharConvertToWchar(tmp_text,&length);
	FindMaxHAndW(tmp_text, &height, &width, &bottomMargin, &leftMargin);

	if(length>=500)
	{
		free(text);
		printf("Error: Large text\n");
		return 1;
	}

	int spaceCharSize=fontSize/2;
	int posX=leftMargin;
	int posY=height+LINE_SPACING;

	int heightOneStrint=height+bottomMargin+LINE_SPACING;
	height=LINE_SPACING+(length/MAX_CHAR_IN_STRING)*heightOneStrint;
	if(length%MAX_CHAR_IN_STRING!=0)
	{
		height+=heightOneStrint;
	}
	width=(width+leftMargin)*MAX_CHAR_IN_STRING+leftMargin;

	unsigned char **alfaImage=(unsigned char**)malloc(height*sizeof(unsigned char*));
	for (int i = 0; i < height; ++i)
	{
		alfaImage[i]=(unsigned char*)malloc(width*sizeof(unsigned char));
		memset(alfaImage[i], 0, width);
	}
	width=0;

	for(int i = 0; i < length; ++i)
	{
		if(i>0&&i%MAX_CHAR_IN_STRING==0) 
		{
			posY+=heightOneStrint;
			posX=leftMargin;
		}
		wchar_t c=text[i];
		if(c==L' ') {
			posX+=spaceCharSize;
			continue;
		}
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		glyph_index=FT_Get_Char_Index(face, c);
		FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		FT_Get_Glyph(face->glyph, &glyph);
		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		bitmap_glyph=(FT_BitmapGlyph)glyph;
		bitmap=bitmap_glyph->bitmap;

		for(int y = 0; y < bitmap.rows; ++y){
			for(int x = 0; x < bitmap.width; ++x){
				if(bitmap.buffer[y*bitmap.width+x]!=0){
					alfaImage[posY-(bitmap_glyph->top)+y][posX+bitmap_glyph->left+x]=bitmap.buffer[y*bitmap.width+x];
				}
			}
		}
		posX+=bitmap.width+bitmap_glyph->left;
		if(posX>width){
			width=posX;
		}
		FT_Done_Glyph(glyph);
	}
	width+=leftMargin;
	for (int i = 0; i < height; ++i)
	{
		alfaImage[i]=(unsigned char*)realloc(alfaImage[i], width*sizeof(unsigned char));
	}

	unsigned char **image=(unsigned char**)malloc(height*sizeof(unsigned char*));
	for (int i = 0; i < height; ++i)
	{
		image[i]=(unsigned char*)malloc(width*3*sizeof(unsigned char));
		memset(image[i], 255, width*3);
	}

	for (int y = 0; y < height; ++y){
		for (int x = 0; x < width; ++x){
			for (int i = 0; i < 3; ++i){
				image[y][x*3+i]=255-alfaImage[y][x];
			}
		}
	}

	//вызываются функции libpng
	if(CreatePNG(image, height, width, image_name)!=0){
		printf("error while creating png file\n");
	}

	free(text);
	for(int i=0; i < height; ++i){
		free(alfaImage[i]);
		free(image[i]);
	}
	free(alfaImage);
	free(image);

	#ifdef DYNAMIC
	SO_close();
	#endif

	float endClock=clock();
	printf("Time: %f", ((endClock-beginClock)/CLOCKS_PER_SEC));
	return 0;
}