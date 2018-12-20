#include "Render.h"

#include <sstream>
#include <iostream>
#include <vector>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;
GLuint kamen;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("mytexture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glBindTexture(GL_TEXTURE_2D, 0);
	OpenGL::LoadBMP("kamen.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
	//���������� �� ��� ��������
	glGenTextures(1, &kamen);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, kamen);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

double* normal(double x1, double y1, double x2, double y2)
{
	struct vector
	{
		double x;
		double y;
		double z;
	} AC, AB, Normal;

	AC.x = x2 - x1; AC.y = y2 - y1; AC.z = 0;
	AB.x = 0; AB.y = 0; AB.z = 2 - 0;

	Normal.x = AC.y * AB.z - AB.y * AC.z;
	Normal.y = -(AC.x * AB.z - AB.x * AC.z);
	Normal.z = AC.x * AB.y - AB.x * AC.y;

	double length = sqrt(Normal.x * Normal.x + Normal.y * Normal.y + Normal.z * Normal.z);
	Normal.x = Normal.x / length;
	Normal.y = Normal.y / length;
	Normal.z = Normal.z / length;

	double finalNormal[3] = { Normal.x, Normal.y, Normal.z };
	return finalNormal;
}

void f1()
{
	double A[] = { 1, 8, 0 };
	double B[] = { 12, 12, 0 };
	double C[] = { 9, 1, 0 };
	double D[] = { 8, 5, 0 };
	double E[] = { 4, 1, 0 };
	double F[] = { 2, 3, 0 };
	double G[] = { 4, 5, 0 };
	double A1[] = { 1, 8, 2 };
	double B1[] = { 12, 12, 2 };
	double C1[] = { 9, 1, 2 };
	double D1[] = { 8, 5, 2 };
	double E1[] = { 4, 1, 2 };
	double F1[] = { 2, 3, 2 };
	double G1[] = { 4, 5, 2 };
	double M[] = { 4, 8, 0 };
	double M1[] = { 4, 8, 2 };



	glColor3d(0, 0, 0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	
	// ������� �����

	glNormal3dv(normal(8, 5, 9, 1));

	glTexCoord2d(1, 0);
	glVertex3dv(C);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 0);
	glVertex3dv(D);

	glNormal3dv(normal(4, 1, 8, 5));
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(0, 0);
	glVertex3dv(E);

	glNormal3dv(normal(2, 3, 4, 1));
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 0);
	glVertex3dv(F);

	glNormal3dv(normal(4, 5, 2, 3));
	glTexCoord2d(1, 0);
	glVertex3dv(F);
	glTexCoord2d(1, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 0);
	glVertex3dv(G);

	glNormal3dv(normal(1, 8, 4, 5));
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(0, 0);
	glVertex3dv(A);

	glNormal3dv(normal(9, 1, 12, 12));
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 0);
	glVertex3dv(C);

	glEnd();


	glColor4d(1, 0.23, 0.3583, 0.9);
	glBindTexture(GL_TEXTURE_2D, kamen);
	glBegin(GL_TRIANGLES);
	
	// ������ �����

	glNormal3d(0, 0, -1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(F);
	glTexCoord2d(0, 0);
	glVertex3dv(E);

	glNormal3d(0, 0, -1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(E);
	glTexCoord2d(0, 0);
	glVertex3dv(D);

	glNormal3d(0, 0, -1);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(D);
	glTexCoord2d(0, 0);
	glVertex3dv(C);

	// ������� �����

	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 0);
	glVertex3dv(G1);
	glTexCoord2d(0.5, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 0);
	glVertex3dv(E1);

	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 0);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(0, 0);
	glVertex3dv(D1);

	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 0);
	glVertex3dv(C1);

	glEnd();

}

void f2() {

	double o[] = { 10.5, 6.5, 0 };
	double o3[] = { 10.5, 6.5, 2 };
	double r = 5.7;
	double x0 = 9;
	double y0 = 1;
	double xt = 0.4;
	double yt = 0;
	double rad = 0;
	for (double a = -105.3; a < 74.8; a = a + 0.1)
	{
		rad = a * (3.14 / 180);
		double x = r * cos(rad) + 10.5;
		double y = r * sin(rad) + 6.5;
		double o1[] = { x, y, 0 };
		double o2[] = { x, y, 2 };
		double o4[] = { x0, y0, 0 };
		double o5[] = { x0, y0, 2 };

		double xt1 = 0.5 * cos(rad) + 0.5;
		double yt1 = 0.5 * sin(rad) + 0.5;
		double k1[] = { xt1, yt1 };
		double k2[] = { xt, yt };

		// ������� �����

		glColor3d(0, 0, 0);
		glBindTexture(GL_TEXTURE_2D, kamen);
		glBegin(GL_QUADS);

		glNormal3dv(normal(x0, y0, x, y));
		glTexCoord2d(0, 1);
		glVertex3dv(o1);
		glTexCoord2d(1, 1);
		glVertex3dv(o2);
		glTexCoord2d(1, 0);
		glVertex3dv(o5);
		glTexCoord2d(0, 0);
		glVertex3dv(o4);

		glEnd();

		//������� � ������ �����

		glColor4d(0.3384, 0.4529, 0.72, 0.9);
		glBindTexture(GL_TEXTURE_2D, kamen);
		glBegin(GL_TRIANGLES);

		glNormal3d(0, 0, -1);
		glTexCoord2d(0.5, 0.5);
		glVertex3dv(o);
		glTexCoord2d(0, 1);
		glVertex3dv(o4);
		glTexCoord2d(0, 0);
		glVertex3dv(o1);

		glNormal3d(0, 0, 1);
		glTexCoord2d(0.5, 0.5);
		glVertex3dv(o3);
		glTexCoord2dv(k2);
		glVertex3dv(o5);
		glTexCoord2dv(k1);
		glVertex3dv(o2);

		glEnd();

		x0 = x;
		y0 = y;

	}
}

void f3() {
	double c[] = { 2.5, 21, 0 };
	double r1 = 13.086;
	double x1 = 1;
	double y1 = 8;
	double rad = 0;
	double G[] = { 4, 5, 0 };
	double G1[] = { 4, 5, 2 };
	double D[] = { 8, 5, 0 };
	double D1[] = { 8, 5, 2 };
	double M[] = { 4, 8, 0 };
	double M1[] = { 4, 8, 2 };

	glColor4d(1, 0.23, 0.3583, 0.9);
	glBindTexture(GL_TEXTURE_2D, kamen);
	glBegin(GL_TRIANGLES);

	glNormal3d(0, 0, -1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(D);
	glTexCoord2d(0, 0);
	glVertex3dv(M);

	glNormal3d(0, 0, 1);
	glTexCoord2d(1, 0);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 0);
	glVertex3dv(M1);

	glEnd();

	for (double a1 = 263.4; a1 < 316.8; a1 = a1 + 0.1)
	{
		rad = a1 * (3.14 / 180);
		double x2 = r1 * cos(rad) + 2.5;
		double y2 = r1 * sin(rad) + 21;
		double c1[] = { x2, y2, 0 };
		double c2[] = { x2, y2, 2 };
		double c4[] = { x1, y1, 0 };
		double c5[] = { x1, y1, 2 };

		// ������� �����

		glColor3d(0, 0, 0);
		glBindTexture(GL_TEXTURE_2D, kamen);
		glBegin(GL_QUADS);

		glNormal3dv(normal(x2, y2, x1, y1));
		glTexCoord2d(1, 0);
		glVertex3dv(c1);
		glTexCoord2d(1, 1);
		glVertex3dv(c2);
		glTexCoord2d(0, 1);
		glVertex3dv(c5);
		glTexCoord2d(0, 0);
		glVertex3dv(c4);


		glEnd();

		// ������� � ������ �����

		glColor4d(1, 0.23, 0.3583, 0.9);
		glBindTexture(GL_TEXTURE_2D, kamen);
		glBegin(GL_TRIANGLES);

		glNormal3d(0, 0, -1);
		glTexCoord2d(1, 0);
		glVertex3dv(c1);
		glTexCoord2d(0, 1);
		glVertex3dv(c4);
		glTexCoord2d(0, 0);
		glVertex3dv(G);

		glNormal3d(0, 0, 1);
		glTexCoord2d(1, 0);
		glVertex3dv(c2);
		glTexCoord2d(0, 1);
		glVertex3dv(c5);
		glTexCoord2d(0, 0);
		glVertex3dv(G1);

		glNormal3d(0, 0, -1);
		glTexCoord2d(1, 0);
		glVertex3dv(c1);
		glTexCoord2d(0, 1);
		glVertex3dv(c4);
		glTexCoord2d(0, 0);
		glVertex3dv(D);

		glNormal3d(0, 0, 1);
		glTexCoord2d(1, 0);
		glVertex3dv(c2);
		glTexCoord2d(0, 1);
		glVertex3dv(c5);
		glTexCoord2d(0, 0);
		glVertex3dv(D1);

		glEnd();


		x1 = x2;
		y1 = y2;

	}


}

void portret() 
{

	//������ ��������� �����
	double O[] = { 0, 0 };
	double O2[] = { 0.5, 0.5 };
	double x_old = 5;
	double y_old = 0;
	double x0 = 1;
	double y0 = 0.5;
	double rad = 0;
	for (double alfa = 0; alfa < 360; alfa++)
	{
		rad = alfa * (3.14 / 180);
		double x_new = 5 * cos(rad);
		double y_new = 5 * sin(rad);
		double t1[] = { x_new, y_new };
		double t2[] = { x_old, y_old };

		double x1 = 0.5 * cos(rad) + 0.5;
		double y1 = 0.5 * sin(rad) + 0.5;
		double k1[] = { x1, y1 };
		double k2[] = { x0, y0 };


		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLES);

		glNormal3d(0, 0, 1);
		glTexCoord2dv(O2);
		glVertex2dv(O);
		glTexCoord2dv(k2);
		glVertex2dv(t2);
		glTexCoord2dv(k1);
		glVertex2dv(t1);

		glEnd();


		x_old = x_new;
		y_old = y_new;
		x0 = x1;
		y0 = y1;
	}

}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	

	/*glBindTexture(GL_TEXTURE_2D, texId);*/
	//��������������
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	f1();
	f2();
	f3();
	//portret();



	
	
	
	


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}