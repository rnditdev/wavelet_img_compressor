#include "./sdl_util.h"

/* The screen surface */

SDL_Surface *screen = NULL;
SDL_Surface *image = NULL ;
SDL_Surface *image2 = NULL ;
SDL_Surface *image3 = NULL ;

#define P_COEF  11
#define N_COEF  10
#define Z_COEF  22
#define T_COEF  88

struct {
    float red;
    float green;
    float blue;
    int  x;
    int y;
    int occ;
    int previous;
    }
    coef_f[256*256];



float subordinate_coeffs[2000000];



float pixels4f[4] [4];


int encoded[1000000][30];
int counter;
int i,j,k,l,newi,newj,m, dimx, dimy;
int steps=4;
int finalstep=0;



int ezwencode2(proc_matrix *matrix){
	int i, thr, coefficient_position,output_stage_line, j, sub_position;

    int redcoefpos =0;
    int greencoefpos = 0;
    int bluecoefpos = 0;
    
    int redsubposition =0;
    int greensubposition =0;
    int bluesubposition =0;


	int partial_subordinate_result;
	
	float actual_color;

	output_stage_line=0;


	for (k=6; k>5; k=k-1) {
		coefficient_position=1;
        sub_position =1;
		thr = pow (2,k);
		
		printf ("threshold: % \n", thr);
		
		/*see all elements using the Morton order */ 
		
		for (i=0; i<256*256; i++) {
            
            
            /* Take the pixel */

            pixel *actual_pixel= getpixel_matrix(matrix,coef_f[i].x,coef_f[i].y);
            
            
            
            /* Do the steps for each channel */
            
            for (j=0; j<3; j++)

            {
            /* Check if the pixel isn't on a Zerotree */
                if (actual_pixel->occ == 0) 
                   
                    { 
                        
                    switch (j) {
                        
                        case 0: 
                            actual_color= actual_pixel->red;
                            
                            redcoefpos ++;
                            
                            coefficient_position = redcoefpos;
                            
                            printf ("red : %f\n" , actual_pixel->red );
                            
                            break;
                        
                        case 1:
                            actual_color= actual_pixel->green;
                            
                            greencoefpos ++;

                            coefficient_position = greencoefpos;
                            
                            printf ("green : %f\n" , actual_pixel->green );
                            
                            break;
                        
                        case 2:
                            actual_color= actual_pixel->blue;
                            
                            bluecoefpos ++;

                            coefficient_position = bluecoefpos;
                            
                            printf ("blue : %f\n" , actual_pixel->blue );
                            
                            break;
                        
                        }
                    
                    
                    
                    
                    
                    if ((actual_color > thr)  || ( actual_color < -thr ) ) 
                    
                    {
                        
                    switch (j) {

                        case 0:
                            redsubposition ++;
                            sub_position = redsubposition;
                            break;

                        case 1:
                            greensubposition ++;
                            sub_position = greensubposition;
                            break;

                        case 2:
                            bluesubposition ++;
                            sub_position = bluesubposition;
                            break;

                        }
                        
                        printf ("SUB_pos  %d " , sub_position) ;
                        
                        
                    }                    
    
                    /*See if the pixel > threshold */
                    			
        			if (actual_color > thr ) 
                    
                    /* The encoding lines will be placed separated by the subordinated sequenced */
                    
                    {
                            
                        printf ("actual_color %f > thr %d \n" , actual_color, thr);
                        
                        encoded[coefficient_position][output_stage_line +2*j]=P_COEF;
                                            
                        zero_color( actual_pixel, j ); /* Eliminate coefficient */
                        
                        subordinate_pass (actual_color, thr, sub_position, output_stage_line +2*j + 1 );


                    } 
        			
        			/*See if the pixel < (-threshold) */
        			
        			else if ( actual_color < -thr ) 
                    
                    { 
                        
                        printf ("actual_color %f < - thr - %d \n" , actual_color, thr);
                        
                        encoded[coefficient_position][output_stage_line + 2*j ]=N_COEF;  /*Negative coefficient */
                        
                        zero_color( actual_pixel, j );
                        
                        subordinate_pass ( actual_color , thr, sub_position , output_stage_line +2*j + 1 );
                        
                    }
                    /*See if the pixel is the root of a zerotree */
        
        			else if (checkzerotree(coef_f[i].x, coef_f[i].y, thr , coefficient_position, 0 , matrix)){        

                    printf ("actual_color %f zerotree thr %d  \n" , actual_color, thr);


                            checkzerotree(coef_f[i].x, coef_f[i].y, thr , coefficient_position, 1 , matrix);
        	       			encoded[coefficient_position][output_stage_line + 2*j ]=T_COEF;
                    }
        
                    /*It is an isolated zero*/
                    
        			else {
                        
                        printf ("Isolated zero %f zerotree thr %d \n " , actual_color, thr);
                        
        				encoded[coefficient_position][output_stage_line + 2*j ]= Z_COEF;
                        }
                    

                }

            }
        
        }
        
		encoded[0][output_stage_line]=coefficient_position;
		encoded[0][output_stage_line+1]=sub_position;
		output_stage_line=output_stage_line+6;

	}




	for (i=0;i<20;i++) {
    for (j=0;j< 300;j++) {
            printf ("%d %d %d - ", i,j,  encoded[j][i]);
        }
        printf ("\n \n");
    }



}


int checkzerotree(int x, int y,  Uint8 threshold ,int offset, int mode, proc_matrix *matrix){
	int  actual_power,k,i,j,nx,ny;
	float first;
	Uint8 red, green, blue, alpha;

    /*The first step will include 2*2 */
    
	int step=2;
	
	/* The power to next iter */
	
	int pownumber=1;

    /* new coordinates */
	nx=2*x;
	ny=2*y;

/* Check the q won't be out of the limits */

		while (nx<256 && ny < 256) {
		  for (i=nx; i<nx+step; i++){
			for (j=ny; j<ny+step; j++){
				
            
            if (j<256 && i < 256) {
            
                if (mode==0){
                    pixel *p=getpixel_matrix(matrix, i, j);
					if (p->red > threshold || 
                    p->green > threshold  || 
                    p->blue > threshold ){
    
					   return 0;
					}
				}
				else {
					getpixel_matrix(matrix, i, j)->occ=1;
					
					DrawPixel(image2,i,j,SDL_MapRGBA(image->format, 0 , 255, 0  , 255 ));
					
                    }
                    
            }    
                
			}
		}

        nx=2*nx;
		ny=2*ny;
		pownumber++;
		step=pow(2,pownumber);
	}
	return 1;		
		
}


/* Reconstruction of the image from the original */
void reconstruct (proc_matrix *matrix, proc_matrix *matrix2, proc_matrix *matrix3){

	int step,step2;
	copylastimage(matrix2, matrix3);

for (k=steps; k >=finalstep ; k=k-1){
    
		/* Define the steps */
		if (k>=0){step = pow(2,k);} else {step = pow(2,0);}
		step2= pow (2,k+1);


		/* Rebuild the image */
		for (i=0; i<image->h/step2; i=i+1){
			for ( j=0; j<(image->w)/step2; j=j+1) {

				/*Get the values from the image */

				assign_pixvalues2(image3, image2 , i,j, step, step2,1, matrix3, matrix2, matrix3);

				/*Draw new quadrangle */

				drawrectangle( image3, i*step2+step,j*step2,step,step,(Uint8) pixels4f[1][0],(Uint8) pixels4f[1][1],(Uint8) pixels4f[1][2]);
				drawrectangle( image3, i*step2,j*step2+step,step,step,(Uint8) pixels4f[2][0],(Uint8) pixels4f[2][1],(Uint8) pixels4f[2][2]);
				drawrectangle( image3, i*step2+step,j*step2+step,step,step,(Uint8) pixels4f[3][0],(Uint8) pixels4f[3][1],(Uint8) pixels4f[3][2]);


				drawrectangle_matrix( matrix3, i*step2+step,j*step2,step,step, pixels4f[1][0], pixels4f[1][1], pixels4f[1][2]);
				drawrectangle_matrix( matrix3, i*step2,j*step2+step,step,step, pixels4f[2][0], pixels4f[2][1], pixels4f[2][2]);
				drawrectangle_matrix (matrix3, i*step2+step,j*step2+step,step,step, pixels4f[3][0], pixels4f[3][1], pixels4f[3][2]);
	
			
			}
		}
	}
}



/*Function tested */

int copylastimage( proc_matrix *matrix2 , proc_matrix *matrix3 ){

	int step, step2;
	step = pow(2,steps);
	step2= pow (2,steps+1);

	/* Rebuild the image */
	for (i=0; i<image->h/step2; i=i+1){
		for ( j=0; j<(image->w)/step2; j=j+1) {

			drawrectangle( image3, i*step2,j*step2,step2,step2,(Uint8)getpixel_matrix(matrix2, i,j )->red, (Uint8) getpixel_matrix(matrix2, i,j )->green , (Uint8) getpixel_matrix(matrix2, i,j )->blue);
			drawrectangle_matrix( matrix3, i*step2,j*step2,step2,step2,getpixel_matrix(matrix2, i,j )->red,getpixel_matrix(matrix2, i,j )->green ,getpixel_matrix(matrix2, i,j )->blue);
            
		}
	}

}





void assign_pixvalues2(SDL_Surface *image1, SDL_Surface *image2 , int i, int j, int step, int step2, int mode , proc_matrix *matrix, proc_matrix *matrix2, proc_matrix *matrix3){

    int l,m;


    pixels4f[0][0] = getpixel_matrix(matrix, i*step2, j*step2)->red;
    pixels4f[0][1] = getpixel_matrix(matrix, i*step2, j*step2)->green;
    pixels4f[0][2] = getpixel_matrix(matrix, i*step2, j*step2)->blue;


	if (mode==0){


        pixels4f[1][0] = getpixel_matrix(matrix, i*step2+step, j*step2)->red;
        pixels4f[1][1] = getpixel_matrix(matrix, i*step2+step, j*step2)->green;
        pixels4f[1][2] = getpixel_matrix(matrix, i*step2+step, j*step2)->blue;

        pixels4f[2][0] = getpixel_matrix(matrix, i*step2, j*step2 +step)->red;
        pixels4f[2][1] = getpixel_matrix(matrix, i*step2, j*step2 +step)->green;
        pixels4f[2][2] = getpixel_matrix(matrix, i*step2, j*step2 +step)->blue;

        pixels4f[3][0] = getpixel_matrix(matrix, i*step2 +step, j*step2 +step)->red;
        pixels4f[3][1] = getpixel_matrix(matrix, i*step2 +step, j*step2 +step)->green;
        pixels4f[3][2] = getpixel_matrix(matrix, i*step2 +step, j*step2 +step)->blue;
    


		for (l=1;l<4;l++){
			for (m=0;m<3;m++){
				pixels4f[l][m]=(float)((float)pixels4f[l][m]-(float)pixels4f[0][m])/2;
			}
		}		
	
	}
	else{
        
		newi= (image->w/step2)+i;
		newj= (image->h/step2)+j;
		
		
        pixels4f[1][0] = getpixel_matrix(matrix2, newi, j)->red ;
        pixels4f[1][1] = getpixel_matrix(matrix2, newi, j)->green ;
        pixels4f[1][2] = getpixel_matrix(matrix2, newi, j)->blue ;

        pixels4f[2][0] = getpixel_matrix(matrix2, i, newj )->red;
        pixels4f[2][1] = getpixel_matrix(matrix2, i, newj )->green;
        pixels4f[2][2] = getpixel_matrix(matrix2, i, newj )->blue;

        pixels4f[3][0] = getpixel_matrix(matrix2, newi, newj)->red;
        pixels4f[3][1] = getpixel_matrix(matrix2, newi, newj)->green;
        pixels4f[3][2] = getpixel_matrix(matrix2, newi, newj)->blue;



		for (l=1;l<4;l++){
			for (m=0;m<3;m++){
				pixels4f[l][m]=pixels4f[l][m]*2+pixels4f[0][m];
			}
			
		}

	}

}

int fill_images( ){
	/* Load Images */

	image = SDL_LoadBMP("C:\\a.bmp");
	if (image == NULL) {
		printf( "Couldn't load 2 %s: %s\n", "./a.bmp", SDL_GetError());
		return 1;
	}

	image2 = SDL_LoadBMP("C:\\a.bmp");
	if (image2 == NULL) {
		printf( "Couldn't load 2 %s: %s\n", "./a.bmp", SDL_GetError());
		return 1;
	}

	image3 = SDL_LoadBMP("C:\\a.bmp");
	if (image3 == NULL) {
		printf( "Couldn't load 2 %s: %s\n", "./a.bmp", SDL_GetError());
		return 1;
	}
	
	
}



int main (int argc, char *argv[])
{
	int done=0;

    proc_matrix *matrix;

	proc_matrix *matrix2;
	proc_matrix *matrix3;

	screen=sdl_initialize(screen);
	
	matrix = (proc_matrix*) malloc (sizeof(proc_matrix));
	matrix2 = (proc_matrix*) malloc (sizeof(proc_matrix));
	matrix3 = (proc_matrix*) malloc (sizeof(proc_matrix));


	fill_images();
	
	new_matrix_from_surf( matrix, image);
	new_matrix_from_surf( matrix2, image);
    new_matrix (matrix3, 256,256);
	drawrectangle(image3,0,0,255,255,255,255,255);
    matrix_zero ( matrix3 );
	applywavelet(matrix, matrix2);
	counter=0;
	iter(6,256,256,matrix2);

	ezwencode2(matrix2);
 /*	drawrectangle(image2,0,0,255,255,127,127,127); */
	matrix_zero ( matrix2 );

    counter=0;
	/*iterreconstruct(matrix, matrix2, matrix3); */
       	   
	while (!done)
	{
		SDL_Event event;
		/* Check for events  */

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym){
				case SDLK_a:
					steps++;

					break;
				case SDLK_s:
					if (steps>0){
						steps=steps-1;

					}
					break;
				}
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
			}
		}



		draw ();
	}
	return 0;
}


void applywavelet(proc_matrix *mat, proc_matrix *mat2){

    Uint8 red,green,blue,alpha;

	int step, step2,l,m;

	for (k=finalstep; k <= steps; k=k+1){

		step = pow(2,k);
		step2= pow (2,k+1);

		dimx=image->w/step2;
		dimy=image->h/step2;

		/*Recursively applying of the Haar wavelet operator */
		for (i=0; i< dimx; i=i+1){
			for ( j=0; j< dimy ; j=j+1) {
				newi= (image->w/step2)+i;
				newj= (image->h/step2)+j;

				/* Get the pixels and its neighbors */
				assign_pixvalues2(image, image2 , i,j, step, step2,0 , mat, mat2,mat2);

				/*Draw pixels */

				DrawPixel_matrix(mat2, i,j , pixels4f[0][0],pixels4f[0][1],pixels4f[0][2], 0 );
				DrawPixel_matrix(mat2, newi,j , pixels4f[1][0],pixels4f[1][1],pixels4f[1][2],0 );
				DrawPixel_matrix(mat2, i,newj , pixels4f[2][0],pixels4f[2][1],pixels4f[2][2], 0 );
				DrawPixel_matrix(mat2, newi,newj , pixels4f[3][0] ,pixels4f[3][1], pixels4f[3][2], 0 );
            }
		}

	}




}

static void draw ()
{
	blitimages();
	SDL_Flip (screen);
}


int iter (int po, int endx, int endy , proc_matrix *matrix )
{


int newx, newi , l , m ;
	Uint8 red,green,blue,alpha;

	if (po>=-1){
		iter(po-1,endx-pow(2,po+1), endy-pow(2,po+1) , matrix);
		iter(po-1,endx, endy-pow(2,po+1) , matrix);
		iter(po-1,endx-pow(2,po+1), endy, matrix);
		iter(po-1,endx, endy, matrix);
	}
	else if (po < 1){

		
		coef_f[counter].x=endx-1;
		coef_f[counter].y=endy-1;
    
      
		coef_f[counter].red=getpixel_matrix(matrix, coef_f[counter].x, coef_f[counter].y)->red;
		coef_f[counter].green=getpixel_matrix(matrix, coef_f[counter].x, coef_f[counter].y)->green;
		coef_f[counter].blue=getpixel_matrix(matrix, coef_f[counter].x, coef_f[counter].y)->blue;

        

		counter ++;
	}
	

	return 0;
}




int iterreconstruct (proc_matrix *matrix, proc_matrix *matrix2 , proc_matrix *matrix3)
{
    int i;
    Uint8 red,green,blue,alpha;

    for (i=0; i< 256*256; i++){

    		DrawPixel(image2, coef_f[i].x, coef_f[i].y , SDL_MapRGBA(image->format, (Uint8) fabs(coef_f[i].red) ,  (Uint8) fabs( coef_f[i].green ), (Uint8) fabs (coef_f[i].blue)  , 255 ));

            DrawPixel_matrix(matrix2, coef_f[i].x, coef_f[i].y , coef_f[i].red, coef_f[i].green,coef_f[i].blue,0 ); 
  
        if (i%800==0){
            reconstruct(matrix,matrix2, matrix3);
            draw();}
    }
        
	return 0;
}



void blitimages(){

	SDL_Rect dest = {300,0};
	SDL_Rect results = {150,300};

	if (image->format->palette && screen->format->palette) {
		SDL_SetColors(screen, image->format->palette->colors, 0, image->format->palette->ncolors);
	}
	/* Blit onto the screen surface */
	if(SDL_BlitSurface(image, NULL, screen, NULL) < 0)
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
	if(SDL_BlitSurface(image2, NULL, screen, &dest) < 0)
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
	if(SDL_BlitSurface(image3, NULL, screen, &results) < 0)
		fprintf(stderr, "BlitSurface error: %s\n", SDL_GetError());
}


/*Zeroes the matrix */

int matrix_zero(proc_matrix *mat) {
    int i,j;
    for (i=0; i<mat->w; i++) { for (j=0; j< mat->w ; j++) { DrawPixel_matrix (mat, i,j,0,0,0,0); }}
    }


/* See  */



void setocc(int x, int y)
{
    
}

void cleanocc( proc_matrix* matrix_to_clean )
{

	for (i=0;i<matrix_to_clean->w;i++) {
    for (j=0;j<matrix_to_clean->h;j++) {
            getpixel_matrix( matrix_to_clean , i, j )-> occ =0;
        }
        printf ("\n");
    }
	
}


float get_max_coeff (float red, float green, float blue)

{
    
    if (fabs (red) > fabs (green) || fabs (red) > fabs (blue)){
        return red;
        }
    else if (fabs (green) > fabs (blue) ) {
        return green;
    }
    else {
        return blue;
        }
        
}
    
    

int zero_color (pixel* p, int channel)

{
            /* channel will be 0,1,2 depending on the color */
            
            switch (channel) {

               case 0:
               p->red =0 ;
               break;

               case 1:
               p->green =0 ;
               break;

               case 2:
               p->blue =0 ;
               break;

                }

}



int subordinate_pass (float actual_color, int thr, int position, int linenumber)

                {
                int partial_subordinated_result;
                
                printf ("Subordinated pass \n " );

                    /*Subordinate pass */

                    if (actual_color > (thr + thr/2))

                        {
                               partial_subordinated_result = 1;
                        }

                    else {
                               partial_subordinated_result = 0;
                         }



                    subordinate_coeffs[position] = abs (actual_color) - thr ;

                    printf ("%d %d  \n" , position, linenumber);

                    encoded[position][linenumber]= partial_subordinated_result ;

                    return 0;

}

