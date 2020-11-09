#include <string>
#include "Render.h"

#include <sstream>
#include <iostream>

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
int proz = 0;
bool alfaNMode = false;
bool textureMode2 = true;

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

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

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
		s.scale = s.scale * 0.08;
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
			c.scale = c.scale * 1.5;
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

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
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

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}


void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
		alfaNMode = !alfaNMode;
	}

	if (key == 'O') {
		if (alfaNMode) {
			if (proz == 0) {
				proz = 1;
			}
			else
			{
				proz = 0;
			}
		}
		else
		{
			textureMode2 = !textureMode2;
		}
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

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId[2];

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId[0]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[0]);

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

	OpenGL::LoadBMP("texture2.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId[1]);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId[1]);

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


double* Normalin(double a[], double b[], double c[]) {

	double* mas;

	mas = new double[3];

	double x1[3] = { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
	double x2[3] = { c[0] - b[0], c[1] - b[1], c[2] - b[2] };
	mas[0] = x1[1] * x2[2] - x2[1] * x1[2];
	mas[1] = -1 * (x1[0] * x2[2] - x2[0] * x1[2]);
	mas[2] = x1[0] * x2[1] - x2[0] * x1[1];

	double r = sqrt(mas[0] * mas[0] + mas[1] * mas[1] + mas[2] * mas[2]);
	mas[0] /= r;
	mas[1] /= r;
	mas[2] /= r;

	return mas;
}
double* Normalin2(double a[], double b[], double c[]) {

	double* mas;

	mas = new double[3];

	double x1[3] = { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
	double x2[3] = { c[0] - b[0], c[1] - b[1], c[2] - b[2] };
	mas[0] = x1[1] * x2[2] - x2[1] * x1[2];
	mas[1] = -1 * (x1[0] * x2[2] - x2[0] * x1[2]);
	mas[2] = x1[0] * x2[1] - x2[0] * x1[1];

	double r = sqrt(mas[0] * mas[0] + mas[1] * mas[1] + mas[2] * mas[2]);
	mas[0] /= -r;
	mas[1] /= -r;
	mas[2] /= -r;

	return mas;
}


void Render(OpenGL* ogl)
{

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	if (alfaNMode) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

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
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	double A2[] = { 2, 2, 0 };
	double B2[] = { 3, 11, 0 };
	double C2[] = { 6, 9, 0 };
	double D2[] = { 9, 10, 0 };
	double E[] = { 6, 1, 0 };
	double F[] = { 6, 6, 0 };

	double A1[] = { 2, 2, 3 };
	double B1[] = { 3, 11, 3 };
	double C1[] = { 6, 9, 3 };
	double D1[] = { 9, 10, 3 };
	double E1[] = { 6, 1, 3 };
	double F1[] = { 6, 6, 3 };

	if (textureMode2) {
		glBindTexture(GL_TEXTURE_2D, texId[0]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texId[1]);
	}

	glColor3d(0.2, 0.7, 0.7);
	//BC
	glBegin(GL_QUADS);
	glNormal3dv(Normalin(B2, C2, C1));
	glTexCoord2f(1.0f, 0.0f);
	glVertex3dv(B2);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3dv(B1);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3dv(C1);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3dv(C2);
	glEnd();

	//CD
	glBegin(GL_POLYGON);
	glColor3d(0.3, 0.9, 0.4);
	glNormal3dv(Normalin2(C2, C1, D1));
	glTexCoord2f(0.5f, 0.0f);
	glVertex3dv(C2);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3dv(C1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3dv(D1);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3dv(D2);
	glEnd();

	//EF
	glBegin(GL_POLYGON);
	glColor3d(0.1, 0.2, 0.9);
	glNormal3dv(Normalin2(E, E1, F1));
	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glVertex3dv(F);
	glEnd();

	//FA
	glBegin(GL_POLYGON);
	glColor3d(0.5, 0.2, 0.3);
	glNormal3dv(Normalin2(F, F1, A1));
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(A1);
	glVertex3dv(A2);
	glEnd();

	//DE
	double CE[] = { -1.5, 8.5, 0 };
	double X[] = { 6, 1, 0 }; //E
	double X1[] = { 6, 1, 3 }; //E1
	double Y[] = { 0, 0, 0 };
	double Y1[] = { 0, 0, 3 };
	double fi = 0.1 * 3.14 / 180;


	for (int i = 0; i < 530; i++) {

		X1[0] = X[0];
		X1[1] = X[1];
		Y[0] = (X[0] - CE[0]) * cos(fi) - (X[1] - CE[1]) * sin(fi);
		Y[1] = (X[0] - CE[0]) * sin(fi) + (X[1] - CE[1]) * cos(fi);
		Y[0] += CE[0];
		Y[1] += CE[1];
		Y1[0] = Y[0];
		Y1[1] = Y[1];


		glBegin(GL_TRIANGLES);
		glColor3d(0.1, 0.9, 0.2);
		glNormal3dv(Normalin(C2, X, Y));
		glVertex3dv(C2);
		glVertex3dv(X);
		glVertex3dv(Y);
		glEnd();
		/*
		glBegin(GL_TRIANGLES);
		glColor3d(0.1, 0.2, 0.6);
		glNormal3dv(Normalin2(C1, X1, Y1));
		glVertex3dv(C1);
		glVertex3dv(X1);
		glVertex3dv(Y1);
		glEnd();
		*/
		glBegin(GL_POLYGON);
		glColor3d(0.8, 0.4, 0.2);
		glNormal3dv(Normalin(X, X1, Y1));
		glTexCoord2f(0.0 + i / 530., 0.0);
		glVertex3dv(X);
		glTexCoord2f(0.0 + i / 530., 1.0);
		glVertex3dv(X1);
		glTexCoord2f(1 / 530 + i / 530., 1.0);
		glVertex3dv(Y1);
		glTexCoord2f(1 / 530. + i / 530., 0.0);
		glVertex3dv(Y);

		X[0] = Y[0];
		X[1] = Y[1];
		glEnd();

	}

	glBegin(GL_POLYGON);
	glColor3d(0.8, 0.4, 0.2);
	glNormal3dv(Normalin(Y, Y1, D1));
	glVertex3dv(Y);
	glVertex3dv(Y1);
	glVertex3dv(D1);
	glVertex3dv(D2);
	glEnd();


	glBegin(GL_TRIANGLES);
	glColor3d(0.1, 0.9, 0.2);
	glNormal3dv(Normalin2(C2, D2, Y));
	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(Y);
	glEnd();

	/*
	glBegin(GL_TRIANGLES);
	glColor3d(0.1, 0.9, 0.2);
	glNormal3dv(Normalin(C1, D1, Y1));
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(Y1);
	glEnd();
	*/

	CE[0] = -15.5;
	CE[1] = 8.5;
	X[0] = 2;
	X[1] = 2;
	fi = 0.1 * 3.14 / 180;

	for (int i = 0; i < 280; i++) {

		X1[0] = X[0];
		X1[1] = X[1];
		Y[0] = (X[0] - CE[0]) * cos(fi) - (X[1] - CE[1]) * sin(fi);
		Y[1] = (X[0] - CE[0]) * sin(fi) + (X[1] - CE[1]) * cos(fi);
		Y[0] += CE[0];
		Y[1] += CE[1];
		Y1[0] = Y[0];
		Y1[1] = Y[1];

		glBegin(GL_TRIANGLES);
		glColor3d(0.7, 0.9, 0.7);
		glNormal3dv(Normalin2(C2, X, Y));
		glVertex3dv(C2);
		glVertex3dv(X);
		glVertex3dv(Y);
		glEnd();

		/*
		glBegin(GL_TRIANGLES);
		glColor3d(0.7, 0.9, 0.2);
		glNormal3dv(Normalin(C1, X1, Y1));
		glVertex3dv(C1);
		glVertex3dv(X1);
		glVertex3dv(Y1);
		glEnd();
		*/

		glBegin(GL_POLYGON);
		glColor3d(0.8, 0.4, 0.2);
		glNormal3dv(Normalin2(X, X1, Y1));
		glTexCoord2f((280 - i) / 280., 0.0);
		glVertex3dv(X);
		glTexCoord2f((280 - i) / 280., 1.0);
		glVertex3dv(X1);
		glTexCoord2f(279 / 280. - i / 280., 1.0);
		glVertex3dv(Y1);
		glTexCoord2f(279 / 280. - i / 280., 0.0);
		glVertex3dv(Y);

		X[0] = Y[0];
		X[1] = Y[1];
		glEnd();

	}

	glBegin(GL_POLYGON);
	glColor3d(0.8, 0.4, 0.2);
	glNormal3dv(Normalin2(Y, Y1, B1));
	glVertex3dv(Y);
	glVertex3dv(Y1);
	glVertex3dv(B1);
	glVertex3dv(B2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(0.1, 0.9, 0.2);
	glNormal3dv(Normalin(C2, B2, Y));
	glVertex3dv(C2);
	glVertex3dv(B2);
	glVertex3dv(Y);
	glEnd();


	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.5, 0.5);
	glNormal3dv(Normalin2(C2, F, A2));
	glVertex3dv(C2);
	glVertex3dv(F);
	glVertex3dv(A2);
	glEnd();

	/*
	glBegin(GL_TRIANGLES);
	glColor3d(0.1, 0.1, 0.1);
	glNormal3dv(Normalin(C1, F1, A1));
	glVertex3dv(C1);
	glVertex3dv(F1);
	glVertex3dv(A1);
	glEnd();
	*/


	//DE
	CE[0] = -1.5;
	CE[1] = 8.5;
	CE[2] = 0;
	X[0] = 6;
	X[1] = 1;
	X[2] = 0;
	X1[0] = 6;
	X1[1] = 1;
	X1[2] = 3;
	Y[0] = 0;
	Y[1] = 0;
	Y[2] = 0;
	Y1[0] = 0;
	Y1[1] = 0;
	Y1[2] = 3;
	fi = 0.1 * 3.14 / 180;


	for (int i = 0; i < 530; i++) {

		X1[0] = X[0];
		X1[1] = X[1];
		Y[0] = (X[0] - CE[0]) * cos(fi) - (X[1] - CE[1]) * sin(fi);
		Y[1] = (X[0] - CE[0]) * sin(fi) + (X[1] - CE[1]) * cos(fi);
		Y[0] += CE[0];
		Y[1] += CE[1];
		Y1[0] = Y[0];
		Y1[1] = Y[1];

		glBegin(GL_TRIANGLES);
		glColor4d(0.1, 0.2, 0.6, proz);
		glNormal3dv(Normalin2(C1, X1, Y1));
		glVertex3dv(C1);
		glVertex3dv(X1);
		glVertex3dv(Y1);
		glEnd();

		X[0] = Y[0];
		X[1] = Y[1];
		glEnd();

	}
	/*
	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.5, 0.5);
	glNormal3dv(Normalin(C2, D2, Y));
	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(Y);
	glEnd();
	*/

	glBegin(GL_TRIANGLES);
	glColor4d(0.1, 0.9, 0.2, proz);
	glNormal3dv(Normalin(C1, D1, Y1));
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(Y1);
	glEnd();

	CE[0] = -15.5;
	CE[1] = 8.5;
	X[0] = 2;
	X[1] = 2;
	fi = 0.1 * 3.14 / 180;

	for (int i = 0; i < 280; i++) {

		X1[0] = X[0];
		X1[1] = X[1];
		Y[0] = (X[0] - CE[0]) * cos(fi) - (X[1] - CE[1]) * sin(fi);
		Y[1] = (X[0] - CE[0]) * sin(fi) + (X[1] - CE[1]) * cos(fi);
		Y[0] += CE[0];
		Y[1] += CE[1];
		Y1[0] = Y[0];
		Y1[1] = Y[1];

		glBegin(GL_TRIANGLES);
		glColor4d(0.7, 0.9, 0.2, proz);
		glNormal3dv(Normalin(C1, X1, Y1));
		glVertex3dv(C1);
		glVertex3dv(X1);
		glVertex3dv(Y1);
		glEnd();


		X[0] = Y[0];
		X[1] = Y[1];
		glEnd();

	}

	glBegin(GL_POLYGON);
	glColor4d(0.8, 0.4, 0.2, proz);
	glNormal3dv(Normalin2(Y, Y1, B1));
	glVertex3dv(Y);
	glVertex3dv(Y1);
	glVertex3dv(B1);
	glVertex3dv(B2);
	glEnd();
	/*
	glBegin(GL_TRIANGLES);
	glColor4d(0.1, 0.9, 0.2, proz);
	glNormal3dv(Normalin2(C2, B2, Y));
	glVertex3dv(C2);
	glVertex3dv(B2);
	glVertex3dv(Y);
	glEnd();
	*/

	glBegin(GL_TRIANGLES);
	glColor4d(0.1, 0.9, 0.2, proz);
	glNormal3dv(Normalin2(C1, B1, Y1));
	glVertex3dv(C1);
	glVertex3dv(B1);
	glVertex3dv(Y1);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.1, 0.1, 0.1, proz);
	glNormal3dv(Normalin(C1, F1, A1));
	glVertex3dv(C1);
	glVertex3dv(F1);
	glVertex3dv(A1);
	glEnd();



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
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}