#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <windows.h>


typedef struct {
    float red;
    float green;
    float blue;
    int occ; /* used to indicate the pixel is part or the zerotree */

    } pixel;


typedef struct {
    int w;
    int h;
    pixel *contents;
} proc_matrix;


void DrawPixel(SDL_Surface *screen, int x, int y, Uint32 Pixel);
Uint32  getpixel(SDL_Surface *surface, int x, int y);
SDL_Surface* sdl_initialize(SDL_Surface *screen);
void drawText();
void drawrectangle( SDL_Surface *destimg, int xorig, int yorig, int width, int height, int red,int green,int blue);

int fill_images();

void blitimages();

void reconstruct(proc_matrix *matrix, proc_matrix *matrix2, proc_matrix* matrix3);
int iter (int po, int endx, int endy , proc_matrix *matrix);
int checkzerotree(int x, int y,  Uint8 threshold ,int offset, int mode , proc_matrix *matrix);
int copylastimage( proc_matrix *matrix,  proc_matrix *matrix3 );
int ezwencode();

static void draw();
int fill_images();


void cleanocc();
void setocc(int x, int y);

void assign_pixvalues2(SDL_Surface *image1, SDL_Surface *image2 , int i, int j, int step, int step2, int mode , proc_matrix *matrix, proc_matrix *matrix2 ,proc_matrix *matrix3);
int new_matrix(proc_matrix *matr, int width, int height);

int DrawPixel_matrix(proc_matrix *mat, int x, int y, float red, float green, float blue, int occ);

pixel* getpixel_matrix(proc_matrix *mat, int x, int y);

int free_matrix(proc_matrix *mat);

int new_matrix_from_surf(proc_matrix *matr, SDL_Surface *sf) ;    


int iterreconstruct(proc_matrix *matrix, proc_matrix *matrix2 , proc_matrix *matrix3);

int ezwencode2(proc_matrix *matrix);

void applywavelet(proc_matrix *mat, proc_matrix *mat2);


int matrix_zero(proc_matrix *mat);

void drawrectangle_matrix( proc_matrix *mat, int xorig, int yorig, int width, int height,float red,float green,float blue);


int subordinate_pass (float actual_color, int thr, int position, int linenumber);

float get_max_coeff (float red, float green, float blue);
