#include    "objfile_reader.h"
#include    <stdi.h>

ObjFileReader reader;

#define QUADS_MODLE             4
#define TRIANGLE_MODLE          3

//the function assume file.obj ,f line only 3 or 4 vertices
void DrawModel()
{
	static  float RotateAngle = 1.0f;
	static  unsigned int side = 3;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTranslatef(0.0f,0.0f,-5.0f);
	glRotatef(RotateAngle,0.5f,0.3f,0.1f);
	glColor3f(0.5f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	
	for(unsigned int i = 0 ; i < reader.m_faces_f.size(); i++){
        	unsigned int n = reader.m_faces_f[i].arg0.size();
        	if( side != n){
			glEnd();
			side = n;
			switch(side){
			    case TRIANGLE_MODLE:	glBegin(GL_TRIANGLES);break;
			    case QUADS_MODLE:		glBegin(GL_QUADS);break;
			    default:			printf("face size wrong!\n");return;
			}
        	}
		
		for(unsigned int j = 0 ; j < reader.m_faces_f[i].arg0.size(); j ++){
			unsigned int index = reader.m_faces_f[i].arg0[j] - 1;
			glVertex3f(reader.m_vertices_v[index].x,reader.m_vertices_v[index].y,reader.m_vertices_v[index].z);
		}
    	}
	glEnd();
	RotateAngle   += 1.0f;
}

void render(HDC hdc)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawModel();

	SwapBuffers(hdc);
}

//not implement yet!
HDC getMyGlWindowDC()
{
    	return 0;
}

int main()
{
    HDC hdc = getMyGlWindowDC();
    bool r = reader.readObjFile("demo.obj");
	
    if(!r){printf("read demo.obj failed!\n");return;}
	
    while(hdc){render(hdc);}
}

