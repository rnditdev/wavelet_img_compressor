#include "./sdl_util.h"


void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void drawText()
{
/*        TTF_Font* font;
        SDL_Rect textRec;
        textRec.x =400;
        textRec.y=300;
        SDL_Color color={255,255,255};
        SDL_Surface *text;
        text=TTF_RenderText_Solid(font,"d",color);

        if(SDL_BlitSurface(text, NULL, screen, &textRec) < 0)
            fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
    TTF_Init();
    font = TTF_OpenFont ("C:\\VeraMono.ttf",12);

*/
}

void drawrectangle( SDL_Surface *destimg, int xorig, int yorig, int width, int height,int red,int green,int blue){

int counter1,counter2;
counter1=0;
counter2=0;

        for (counter1=xorig; counter1< xorig+width; counter1=counter1+1){
        for ( counter2=yorig; counter2< yorig+height ; counter2=counter2+1) {
                DrawPixel(destimg, counter1,counter2 , SDL_MapRGBA(destimg->format,red,green,blue, 255 ));
                                                                              }
                                                                            }
}



void drawrectangle_matrix( proc_matrix *mat, int xorig, int yorig, int width, int height,float red,float green,float blue){

int counter1,counter2;

        for (counter1=xorig; counter1< xorig+width; counter1=counter1+1){
        for ( counter2=yorig; counter2< yorig+height ; counter2=counter2+1) {
                DrawPixel_matrix(mat, counter1,counter2 , red, green, blue, 0);
                                                                              }
                                                                            }
}


SDL_Surface* sdl_initialize(SDL_Surface *screen){
    char *msg;
    /* Initialize SDL */
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        sprintf (msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
        MessageBox (0, msg, "Error", MB_ICONHAND);
        free (msg);
        exit (1);
    }
    atexit (SDL_Quit);
    /* Set 640x480 16-bits video mode */
    screen = SDL_SetVideoMode (640, 650, 16, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        sprintf (msg, "Couldn't set 640x480x16 video mode: %s\n",
          SDL_GetError ());
        MessageBox (0, msg, "Error", MB_ICONHAND);
        free (msg);
        exit (2);
    }
    SDL_WM_SetCaption ("Wavelet Image compression", NULL);
    return screen;

}





/* A new structure that will a  matrix of colour coefficients*/

/*The coefficient matrix will be of floats */


int new_matrix(proc_matrix *matr,int width, int height)
    {
    pixel *pix;

    int i,j;


    pix=(pixel*)calloc(width*height,sizeof(pixel));
 
    matr->contents=pix;
    matr->w=width;
    matr->h=height;

    }



int new_matrix_from_surf(proc_matrix *matr, SDL_Surface *sf)
    {
    pixel *pix;

    int i,j;
    Uint8 red,green,blue,alpha;
    int height=sf->h;
    int width=sf->w;

    pix=(pixel*)calloc(width*height,sizeof(pixel));
    
    


    matr->contents=pix;
    matr->w=width;
    matr->h=height;
    
    for (i=0;i<width;i++) {

        for (j=0;j<height;j++) {
      		SDL_GetRGBA(getpixel(sf, i , j),sf->format,&red,&green,&blue,&alpha);
            DrawPixel_matrix(matr, i,j,(float) red,(float) green, (float) blue, 0);
        }

    }

    return 1;

    }


int DrawPixel_matrix(proc_matrix *mat, int x, int y, float red, float green, float blue, int occ)

    {
    if (x<(mat->w) && y < (mat->h) ){
        int offset= (int)mat->w*x+y;

        pixel *p= (mat->contents)+offset;

        p->red = red;
        p->green=green;
        p->blue=blue;
        p->occ=occ;

        }

    }


pixel* getpixel_matrix(proc_matrix *mat, int x, int y)

    {
    int offset= (int)mat->w*x+y;
    
    pixel *p= (mat->contents)+offset;
    
    return p;
    }


int free_matrix(proc_matrix *mat)

    {
    free(mat->contents);
    
    return 1;

    }





int fill_surface_from_matrix( SDL_Surface *sf , proc_matrix *matr)

{

    int i,j;
    Uint8 red,green,blue,alpha;
    int height=sf->h;
    int width=sf->w;

    for (i=0;i<width;i++) {

        for (j=0;j<height;j++) {

            DrawPixel(sf, i,j , SDL_MapRGBA(sf->format,(Uint8) getpixel_matrix(matr,i,j)->red,(Uint8) getpixel_matrix(matr,i,j)->green,(Uint8) getpixel_matrix(matr,i,j)->blue, 20 ));		
            
        }

    }

    return 1;
    
    }
