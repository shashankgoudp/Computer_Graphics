#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h>
#include <ctype.h>
#include<sstream>
#include<malloc.h>
#include<limits.h>
using namespace std;

#define COMPATIBILITY
//#define MODERN

#define vector(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];

#define crossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];

#define innerProduct(v,q) \
		((v)[0] * (q)[0] + \
		(v)[1] * (q)[1] + \
		(v)[2] * (q)[2]);
int triangle_count =0;
int mtlcount = 0;

//structure of a point
	struct point{
		float vertex_x;
		float vertex_y;
		float vertex_z;
	};
	
//structure of a triangle
	struct Triangle{
		struct point vertex_point1;
		struct point vertex_point2;
		struct point vertex_point3;
		char *mtl_name;
	};

//structure of Ka,Kd,Ks
	struct light_index_K{
		float r;
		float g;
		float b;
	};

//structure of newmtl values
	struct newmtl
	{
		char *mtl_name;
		light_index_K Ka;
		light_index_K Kd;
		light_index_K Ks;
		int illum;
		float Ns;
	};



	struct point vertices[2800];
	struct Triangle Triangles[2800];
	newmtl newmtl_obj[100];
	float Intersection_point[3];
	bool rayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2);
	float find_distance(float d1[],float d2[]);
	float Normalisation(float arr[]);
	

/*********************************************************************************************************************
* OpenGL 3.x+ example.
*********************************************************************************************************************/



/*********************************************************************************************************************
* Immediate mode example.
*********************************************************************************************************************/

#ifdef COMPATIBILITY

// include GLUT
#include <gl\GL.h>
#include <gl\GLU.h>
#include "glut/glut.h"

struct PixelInfo {
	float r;
	float g;
	float b;
};


/**
* Main 2D rendering function.
*/
void render2D() {

	GLuint x, y, index;
	GLfloat r, g, b;

	float x_var,y_var;
	int p;
	int z= -1;
	float direction[3];
	float eyevector[3];
	float light[3]; 
	int minimum_triangle;
	
	//eye position values
	eyevector[0] = 0;
	eyevector[1] = 0;
	eyevector[2] = -2;

	//light source position values
	light[0] = 0;
	light[1] = 0;
	light[2] = 10;

	bool status;
	bool coloring_status = false;
	float Triangle_vertices_point1[3];
	float Triangle_vertices_point2[3];
	float Triangle_vertices_point3[3];

	
	

	int const window_width = 256;
	int const window_height = 256;

	PixelInfo intersectionPixels[window_width][window_height];
		
	
	int x_index=0;
	int y_index=0;

	float distance;
	float min_distance = INT_MAX;
	float min_intersection_point[3];

	//size of image coordinates
	float Initial_position_x = -1;
	float Initial_position_y = -1;

	float final_position_x = 1;
	float final_position_y = 1;


	// clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	
	for(y_var=Initial_position_y;y_var<=final_position_y && y_index < window_height;y_var+=2*abs(Initial_position_x)/window_height)
	{
		x_index=0;
		for(x_var=Initial_position_x;x_var<=final_position_x && x_index< window_width;x_var+=2*abs(Initial_position_x)/window_width)
		{
			direction[0] = x_var-eyevector[0];
			direction[1] = y_var-eyevector[1];
			direction[2] = z-eyevector[2];

			for(p=0;p<=triangle_count;p++)
			{
				Triangle_vertices_point1[0] = Triangles[p].vertex_point1.vertex_x;
				Triangle_vertices_point1[1] = Triangles[p].vertex_point1.vertex_y;
				Triangle_vertices_point1[2] = Triangles[p].vertex_point1.vertex_z;

				Triangle_vertices_point2[0] = Triangles[p].vertex_point2.vertex_x;
				Triangle_vertices_point2[1] = Triangles[p].vertex_point2.vertex_y;
				Triangle_vertices_point2[2] = Triangles[p].vertex_point2.vertex_z;

				Triangle_vertices_point3[0] = Triangles[p].vertex_point3.vertex_x;
				Triangle_vertices_point3[1] = Triangles[p].vertex_point3.vertex_y;
				Triangle_vertices_point3[2] = Triangles[p].vertex_point3.vertex_z;
				
				
				status = rayIntersectsTriangle(eyevector,direction,Triangle_vertices_point1,Triangle_vertices_point2,Triangle_vertices_point3);
			
				//finding the minimum distanced intersection point
				if(status){
					
					distance = find_distance(Intersection_point,eyevector);
					if(distance < min_distance)
					{
						min_distance = distance;
						min_intersection_point[0] = Intersection_point[0];
						min_intersection_point[1] = Intersection_point[1];
						min_intersection_point[2] = Intersection_point[2];
						minimum_triangle = p;
						coloring_status = true;
												
					}

							
					status = false;
					
				}
											
			}
			
			for(int list_loop = 0; list_loop < mtlcount; list_loop++){
				
				if(coloring_status == false)
				{
					intersectionPixels[y_index][x_index].r=0;
					intersectionPixels[y_index][x_index].g=0;
					intersectionPixels[y_index][x_index].b=0;
					break;
				}

				//Appying Bling Phong Immuniation formula
				else if(strcmp( Triangles[minimum_triangle].mtl_name, newmtl_obj[list_loop].mtl_name ) == 0 )
				{
					float LA[3] = {1,1,1};
					float LD[3] = {1,1,1};
					float LS[3] = {1,1,1};
					float A_r = LA[0]*newmtl_obj[list_loop].Ka.r;
					float A_g = LA[1]*newmtl_obj[list_loop].Ka.g;
					float A_b = LA[2]*newmtl_obj[list_loop].Ka.b;

				

					float L[3];
					L[0] = light[0] - min_intersection_point[0];
					L[1] = light[1] - min_intersection_point[1];
					L[2] = light[2] - min_intersection_point[2];

					L[0] = (L[0]/Normalisation(L));
					L[1] = (L[1]/Normalisation(L));
					L[2] = (L[2]/Normalisation(L));


					float V01[3];
					float V02[3];
					float Normal[3];

					//finding normal to a triangle or a face
					V01[0] = Triangles[minimum_triangle].vertex_point1.vertex_x - Triangles[minimum_triangle].vertex_point2.vertex_x;
					V01[1] = Triangles[minimum_triangle].vertex_point1.vertex_y - Triangles[minimum_triangle].vertex_point2.vertex_y;
					V01[2] = Triangles[minimum_triangle].vertex_point1.vertex_z - Triangles[minimum_triangle].vertex_point2.vertex_z;

					V02[0] = Triangles[minimum_triangle].vertex_point3.vertex_x - Triangles[minimum_triangle].vertex_point2.vertex_x;
					V02[1] = Triangles[minimum_triangle].vertex_point3.vertex_y - Triangles[minimum_triangle].vertex_point2.vertex_y;
					V02[2] = Triangles[minimum_triangle].vertex_point3.vertex_z - Triangles[minimum_triangle].vertex_point2.vertex_z;


					crossProduct(Normal,V01,V02);

					Normal[0] = (Normal[0]/Normalisation(Normal));
					Normal[1] = (Normal[1]/Normalisation(Normal));
					Normal[2] = (Normal[2]/Normalisation(Normal));


					float NL = Normal[0]*L[0] + Normal[1]*L[1] + Normal[2]*L[2];

					if(NL < 0)
					{
						NL = 0;
					}
					
					if(NL > 1)
					{
						NL = 1;
					}

					float D_r = (LD[0]*newmtl_obj[list_loop].Kd.r)*NL;
					float D_g = (LD[1]*newmtl_obj[list_loop].Kd.g)*NL;
					float D_b = (LD[2]*newmtl_obj[list_loop].Kd.b)*NL;


					float LN = L[0]*Normal[0] + L[1]*Normal[1] + L[2]*Normal[2];

					if(LN < 0)
					{
						LN = 0;
					}
					
					if(LN > 1)
					{
						LN = 1;
					}


					float R[3];

					R[0] = (2*LN*Normal[0]) - L[0];
					R[1] = (2*LN*Normal[1]) - L[1];
					R[2] = (2*LN*Normal[2]) - L[2];

					R[0] = (R[0]/Normalisation(R));
					R[1] = (R[1]/Normalisation(R));
					R[2] = (R[2]/Normalisation(R));

					float V[3];
					V[0] = eyevector[0] - min_intersection_point[0];
					V[1] = eyevector[1] - min_intersection_point[1];
					V[2] = eyevector[2] - min_intersection_point[2];

					V[0] = (V[0]/Normalisation(V));
					V[1] = (V[1]/Normalisation(V));
					V[2] = (V[2]/Normalisation(V));

					float RV = R[0]*V[0] + R[1]*V[1] + R[2]*V[2];

					if(RV < 0)
					{
						RV = 0;
					}
					
					if(RV > 1)
					{
						RV = 1;
					}

					float RVN = pow(RV,newmtl_obj[list_loop].Ns);

					float S_r = (LS[0]*newmtl_obj[list_loop].Ks.r)*RVN;
					float S_g = (LS[1]*newmtl_obj[list_loop].Ks.g)*RVN;
					float S_b = (LS[2]*newmtl_obj[list_loop].Ks.b)*RVN;

					intersectionPixels[y_index][x_index].r=A_r + D_r + S_r;
					intersectionPixels[y_index][x_index].g=A_g + D_g + S_g;
					intersectionPixels[y_index][x_index].b=A_b + D_b + S_b;
										
					coloring_status = false;
					break;
				}
				
			}
			min_distance = INT_MAX;
			x_index++;
		}
		y_index++;
	}

	// update the buffer
	glDrawPixels( window_width, window_height, GL_RGB, GL_FLOAT, &intersectionPixels[0][0] );

	
	// swap the back buffer
	glutSwapBuffers();

}

//function to find the deterministic value for Normalising a vector
float Normalisation(float arr[])
{

	return(sqrt((arr[0]*arr[0]) + (arr[1]*arr[1]) + (arr[2]*arr[2])));

}

//function to find distance between two points
float find_distance(float d1[],float d2[])
{

	float distance;
    distance = sqrt((d1[0]-d2[0]) * (d1[0]-d2[0]) + (d1[1]-d2[1]) * (d1[1]-d2[1])  + (d1[2]-d2[2]) * (d1[2]-d2[2]));
    return distance;

}

/**
* Input handler.
*/
void keyboard( unsigned char key, int x, int y ) {
	switch( key ) {
		case 27:
			exit( 0 );
			break;
		default:
			break;
	}
}


/**
* Program Entry Point.
*/

//function to find if a ray intersects a triangle
//source--http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/

bool rayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2) {

	float e1[3],e2[3],h[3],s[3],q[3];
	float a,f,u,v,t;
	
	vector(e1,v1,v0);
	vector(e2,v2,v0);

	crossProduct(h,d,e2);
	a = innerProduct(e1,h);

	if (a > -0.00001 && a < 0.00001)
		return(false);

	f = 1/a;
	vector(s,p,v0);
	
	u = innerProduct(s,h);

	u = u * f;

	if (u < 0.0 || u > 1.0)
		return(false);

	crossProduct(q,s,e1);
	v = f * innerProduct(d,q);

	if (v < 0.0 || u + v > 1.0)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = f * innerProduct(e2,q);
	Intersection_point[0] = p[0] + t*d[0];
	Intersection_point[1] = p[1] + t*d[1];
	Intersection_point[2] = p[2] + t*d[2];

	if (t > 0.00001) // ray intersection
		return(true);

	else // this means that there is a line intersection
		 // but not a ray intersection
		 return (false);

}


int main ( int argc, char* argv[] ) {


	//if ( argc != 3 ) /* argc should be 4 for correct execution */
  // {
        /* We print argv[0] assuming it is the program name */
  //      printf( "usage: %s filename", argv[0] );
  //  }

//	else{
	//logic to parse a file
	//source--http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

	//FILE *file = fopen(argv[1],"r");
	//FILE *file = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\cube.obj","r");
	
	//FILE *file_max_val = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\dolphins.obj","r");
	//FILE *file_max_val = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\cube.obj","r");

	FILE *file_max_val = fopen("input.obj","r");

	float Vertex_max_test[3];
		
	float max = INT_MIN;
	if( file_max_val == NULL ){
		printf("Impossible to open the file !\n");
		return 0;
	}


	char lineHeader_test[128];
	fscanf(file_max_val, "%s", lineHeader_test);
	while( 1 ){
		if(strcmp( lineHeader_test, "v" ) == 0 )
		{
			
		}
		else
		{
			int res_stat = fscanf(file_max_val, "%s", lineHeader_test);
			if (res_stat == EOF)
				break;
		}
 		
		if ( strcmp( lineHeader_test, "v" ) == 0 ){
			
			fscanf(file_max_val, "%f %f %f\n", &Vertex_max_test[0], &Vertex_max_test[1], &Vertex_max_test[2]);
			
			if(Vertex_max_test[0] > max)
			{
				max = Vertex_max_test[0];
			}

			if(Vertex_max_test[1] > max)
			{
				max = Vertex_max_test[1];
			}

			if(Vertex_max_test[2] > max)
			{
				max = Vertex_max_test[2];
			} 
			
		}

		int eofstat = fscanf(file_max_val, "%s", lineHeader_test);
		if (eofstat == EOF)
			break;
								
	}


//	printf("max value is %f\n ", max);


	max = ((max*71)/100);

//	printf("max value is %f\n ", max);


	//FILE *file = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\dolphins.obj","r");
	//FILE *file = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\cube.obj","r");

	FILE *file = fopen("input.obj","r");
 	

	float Vertex[3];
	int Face_Point[3];
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		return 0;
	}

	int i =1;
	int j= 1;
	int m= 1;
	int n =1;
	char *Temp_mtlname;
	char lineHeader[128];
	while( 1 ){
 
		if(strcmp( lineHeader, "g" ) == 0 )
		{
			
		}
		else
		{
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;
		}
		if ( strcmp( lineHeader, "v" ) == 0 ){
			
			fscanf(file, "%f %f %f\n", &Vertex[0], &Vertex[1], &Vertex[2]);
			if(max > 3.5)
			{
			vertices[i].vertex_x = (Vertex[0]/(max));
			vertices[i].vertex_y = (Vertex[1]/(max));
			vertices[i].vertex_z = (Vertex[2]/(max));
			i++;
			}

			else
			{
			vertices[i].vertex_x = Vertex[0];
			vertices[i].vertex_y = Vertex[1];
			vertices[i].vertex_z = Vertex[2];
			i++;			
			}

			
		}


		if(strcmp( lineHeader, "g" ) == 0 ){
			char *mtlname_store;
			mtlname_store = new	char[128];
			fscanf(file, "%s", mtlname_store);
			fscanf(file, "%s", lineHeader);
			fscanf(file, "%s", lineHeader);
			fscanf(file, "%s", lineHeader);
			while(1){
			if ( strcmp( lineHeader, "f" ) == 0 ){

			fscanf(file, "%d %d %d\n", &Face_Point[0], &Face_Point[1], &Face_Point[2]);
			Triangles[j].vertex_point1 = vertices[Face_Point[0]];
			Triangles[j].vertex_point2 = vertices[Face_Point[1]];
			Triangles[j].vertex_point3 = vertices[Face_Point[2]];
			Triangles[j].mtl_name = mtlname_store;
			j++;
			triangle_count++;
			int stat = fscanf(file, "%s", lineHeader);
			if (stat == EOF)
			break;
			
			}

			else
			{
				break;
			}
		}
		}
		
		
	}

//	}
	
	//FILE *file2 = fopen(argv[2],"r");
	//FILE *file2 = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\cube.mtl","r");
	//FILE *file2 = fopen("F:\\ThirdSem\\Computer Graphics\\Assignment1\\dolphins.mtl","r");

	FILE *file2 = fopen("input.mtl","r");

	
	float Kvalues[3];
	int k = 0;
	int illum_val;
	float Ns_Val;
		
	char *temp;
	if( file2 == NULL ){
		printf("Impossible to open the file !\n");
		return 0;
	}

	while( 1 ){
 
		char lineHeader2[128];
		int res = fscanf(file2, "%s", lineHeader2);
		if (res == EOF)
			break;
		
		if ( strcmp( lineHeader2, "newmtl" ) == 0 ){
			mtlcount++;
			char* mtlname ;
			mtlname = new char(128);
			fscanf(file2, "%s", mtlname);
			newmtl_obj[k].mtl_name = mtlname;
			fscanf(file2, "%s", lineHeader2);
			
			if(strcmp( lineHeader2, "Ka" ) == 0 ){

				fscanf(file2, "%f %f %f\n", &Kvalues[0], &Kvalues[1], &Kvalues[2]);
				newmtl_obj[k].Ka.r = Kvalues[0];
				newmtl_obj[k].Ka.g = Kvalues[1];
				newmtl_obj[k].Ka.b = Kvalues[2];
								
			}
	
			fscanf(file2, "%s", lineHeader2);
			if(strcmp( lineHeader2, "Kd" ) == 0 ){

				fscanf(file2, "%f %f %f\n", &Kvalues[0], &Kvalues[1], &Kvalues[2]);
				newmtl_obj[k].Kd.r = Kvalues[0];
				newmtl_obj[k].Kd.g = Kvalues[1];
				newmtl_obj[k].Kd.b = Kvalues[2];
				
			}

			fscanf(file2, "%s", lineHeader2);
			if(strcmp( lineHeader2, "Ks" ) == 0 ){

				fscanf(file2, "%f %f %f\n", &Kvalues[0], &Kvalues[1], &Kvalues[2]);
				newmtl_obj[k].Ks.r = Kvalues[0];
				newmtl_obj[k].Ks.g = Kvalues[1];
				newmtl_obj[k].Ks.b = Kvalues[2];
			}

			fscanf(file2, "%s", lineHeader2);
			if(strcmp( lineHeader2, "illum" ) == 0 ){

				fscanf(file2, "%d ",&illum_val);
				newmtl_obj[k].illum = illum_val;
			}
			fscanf(file2, "%s", lineHeader2);
			if((strcmp( lineHeader2, "Ns" ) == 0) || (strcmp( lineHeader2, "N" ) == 0) ){

				fscanf(file2, "%f ",&Ns_Val);
				newmtl_obj[k].Ns = Ns_Val;
			}
			k++;
		}
	}
	
		
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize( 280, 280 );
	glutInitWindowPosition( 50, 50 );
	glutCreateWindow( "Ray Tracing" );

	// set the GL clear color
	glClearColor( 0.f, 0.f, 0.f, 0.f );

	glutDisplayFunc( render2D );
	glutKeyboardFunc( keyboard );
	glutMainLoop();

	return 0;
}

#endif