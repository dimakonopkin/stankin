#include "Render.h"

#include <future>
#include <sstream>
#include <iostream>
#include <fstream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include <array>

#include "GUItextRectangle.h"

#define PI 3.14159265

#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f

bool textureMode = true;
bool lightMode = true;
bool changeTexture = false;
bool alpha = false;
bool CTRLisDown = false;
bool EnableWriteInFile = true;
bool EnableReadInFile = true;


std::vector <std::vector <Vector3>> points(
	{
		{
			{0, 0, 5},
			{1, -1, 7},
			{1, -2, 4},
			{0, -3, 3}
		},
		{
			{2, 0, 5},
			{1, -1, 4},
			{2, -2, -4},
			{1, -3, 3}
		},
		{
			{3, 0, 4},
			{3, -1, 5},
			{2, -2, 5},
			{2, -3, 3}
		},
		{
			{4, 0, 5},
			{4, -1, 5},
			{3, -2, 4},
			{5, -3, 4}
		}
	});

double TimeDeltaTime = 0.01;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
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
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры

//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
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
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
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

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света

int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}
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

	if (key == 'Q')
	{
		changeTexture = !changeTexture;
	}

	if (key == 'B')
	{
		alpha = !alpha;
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

	if (key == 17)
	{
		CTRLisDown = true;
	}


}

void keyUpEvent(OpenGL *ogl, int key)
{

}

GLuint texId[2];

void UploadTextureInTexId(const char* name, const int NumberOfTexId)
{
	RGBTRIPLE* texarray;
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP(name, &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
	glGenTextures(1, &texId[NumberOfTexId]);
	glBindTexture(GL_TEXTURE_2D, texId[NumberOfTexId]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	free(texCharArray);
	free(texarray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	
	UploadTextureInTexId("texture.bmp", 0);
	UploadTextureInTexId("texture2.bmp", 1);

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void NormalizeVector(double* vec)
{
	double modVector = -sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));

	for (int i = 0; i < 3; ++i)
	{
		vec[i] /= modVector;
	}

}

double* FindNormal(double x, double y, double z, double x1, double y1, double z1, double x2, double y2, double z2, int FlagSwap = 0) // A - "общая"
{
	double vectorA[3], vectorB[3];
	double a[3] = { x,y,z }, b[3] = { x1,y1,z1 }, c[3] = { x2,y2,z2 };

	for (int i = 0; i < 3; ++i) // Получаем вектор A и B
	{
		vectorA[i] = a[i] - c[i];
		vectorB[i] = b[i] - c[i];
	}

	double VectorNormal[3];


	VectorNormal[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
	VectorNormal[1] = -vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2];
	VectorNormal[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];

	NormalizeVector(VectorNormal);

	if (FlagSwap != 0)
	{
		for (int i = 0; i < 3; ++i) // Получаем вектор A и B
		{
			VectorNormal[i] *= -1;
		}
	}

	return VectorNormal;
}

double* FindNormal(const double* a,const double* b,const double* c, int FlagSwap = 0) // A - "общая"
{
	double vectorA[3], vectorB[3];

	for (int i = 0; i < 3; ++i) // Получаем вектор A и B
	{
		vectorA[i] = a[i] - c[i];
		vectorB[i] = b[i] - c[i];
	}

	double VectorNormal[3];

	VectorNormal[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
	VectorNormal[1] = -vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2];
	VectorNormal[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];

	NormalizeVector(VectorNormal);

	if (FlagSwap != 0)
	{
		for (int i = 0; i < 3; ++i) // Получаем вектор A и B
		{
			VectorNormal[i] *= -1;
		}
	}

	return VectorNormal;
}

double f(double p1, double p2, double p3, double t)
{
	return p1 * (1 - t) * (1 - t) * (1 - t) + 2 * p2 * t * (1 - t) + p3 * t * t; 
}

double f(double a, double b, double t)
{
	return a * (1 - t) + b * t;
}

double f(double p1, double p2, double p3, double p4, double t)
{
	return p1 * (1 - t) * (1 - t) * (1 - t) + 3 * p2 * t * (1 - t) * (1 - t) + 3 * p3 * t * t * (1 - t) + p4 * t * t * t; //посчитанная формула
}

double fE(double p1, double p4, double vec1, double vec4, double t)
{
	return p1 * (2 * t * t * t - 3 * t * t + 1) + p4 * (-2 * t * t * t + 3 * t * t) + vec1 * (t * t * t - 2 * t * t + t) + vec4 * (t * t * t - t * t); //посчитанная формула
}

void lineB(double P1[3], double P2[3], double P3[3], double P4[3])
{
	glDisable(GL_LIGHTING);

	glColor3d(0.5, 0.7, 0.2);
	glLineWidth(2);

	glBegin(GL_LINE_STRIP);
	for (double i = 0; i <= 1.0001; i += 0.01)
	{
		double P[3];
		P[0] = f(P1[0], P2[0], P3[0], P4[0], i);
		P[1] = f(P1[1], P2[1], P3[1], P4[1], i);
		P[2] = f(P1[2], P2[2], P3[2], P4[2], i);

		glVertex3dv(P); 
	}
	glEnd();

	glColor3d(0.2, 0.5, 1);
	glLineWidth(1);
	glBegin(GL_LINES);

	glVertex3dv(P1);
	glVertex3dv(P2);

	glVertex3dv(P2);
	glVertex3dv(P3);

	glVertex3dv(P3);
	glVertex3dv(P4);

	glEnd();
	glEnable(GL_LIGHTING);
}

//Вычисляет координаты точки на кривой, без построения оной
Vector3 Traectory(double P1[3], double P2[3], double P3[3], double P4[3], double t)
{
	Vector3 Vec;
	Vec.setCoords(f(P1[0], P2[0], P3[0], P4[0], t), f(P1[1], P2[1], P3[1], P4[1], t), f(P1[2], P2[2], P3[2], P4[2], t));
	return Vec;
}

//Рисует фигуру, летящую по кривой
void Model(double P1[3])
{
	
	glPushMatrix();

	glRotated(90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glRotated(180, 0, 1, 0);


	glBegin(GL_TRIANGLES);

	glColor3ub(255, 255, 255);
	glNormal3dv(FindNormal(0.0f, 0.0f, 1.2f, -0.3f, 0.0f, 0.6f, 0.3f, 0.0f, 0.6f, 1));
	glVertex3f(0.0f, 0.0f, 1.2f);
	glVertex3f(-0.3f, 0.0f, 0.6f);
	glVertex3f(0.3f, 0.0f, 0.6f);

	glColor3ub(0, 0, 0);
	glNormal3dv(FindNormal(0.6f, 0.0f, 1.2f, 0.0f, 0.6f, 1.2f, 0.0f, 0.0f, 2.4f, 1));
	glVertex3f(0.3f, 0.0f, 0.6f);
	glVertex3f(0.0f, 0.3f, 0.6f);
	glVertex3f(0.0f, 0.0f, 1.2f);

	glColor3ub(255, 0, 0);
	glNormal3dv(FindNormal(0.0f, 0.0f, 1.2f, 0.0f, 0.3f, 0.6f, -0.3f, 0.0f, 0.6f, 1));
	glVertex3f(0.0f, 0.0f, 1.2f);
	glVertex3f(0.0f, 0.3f, 0.6f);
	glVertex3f(-0.3f, 0.0f, 0.6f);

	glColor3ub(0, 255, 0);
	glNormal3dv(FindNormal(-0.3f, 0.0f, 0.6f, 0.0f, 0.3f, 0.6f, 0.0f, 0.0f, -1.12f, 1));
	glVertex3f(-0.3f, 0.0f, 0.6f);
	glVertex3f(0.0f, 0.3f, 0.6f);
	glVertex3f(0.0f, 0.0f, -1.12f);

	glColor3ub(255, 255, 0);
	glNormal3dv(FindNormal(0.0f, 0.0f, -1.12f, 0.0f, 0.3f, 0.6f, 0.3f, 0.0f, 0.6f, 1));
	glVertex3f(0.0f, 0.0f, -1.12f);
	glVertex3f(0.0f, 0.3f, 0.6f); 
	glVertex3f(0.3f, 0.0f, 0.6f);

	glColor3ub(0, 255, 255);
	glNormal3dv(FindNormal(0.4f, 0.0f, 0.6f, -0.3f, 0.0f, 0.6f, 0.0f, 0.0f, -1.12f, 1));
	glVertex3f(0.3f, 0.0f, 0.6f);
	glVertex3f(-0.3f, 0.0f, 0.6f);
	glVertex3f(0.0f, 0.0f, -1.12f);

	glEnd();
	glPopMatrix();
}

//Строит линию и двигает по ней фигуру в обе стороны
void Rocket(double P1[3], double P2[3], double P3[3], double P4[3], double timeDeltaTime)
{
	static bool isRev = false;
	static double maxT = 0;

	if (!isRev)
	{
		maxT += timeDeltaTime / 5;

		if (maxT > 1)
		{
			maxT = 1; 
			isRev = !isRev;
		}
	}
	else
	{
		maxT -= timeDeltaTime / 5;

		if (maxT < 0)
		{
			maxT = 0; 
			isRev = !isRev;
		}
	}

	lineB(P1, P2, P3, P4);

	Vector3 target1;

	if(!isRev)
	{
		target1 = Traectory(P1, P2, P3, P4, maxT - timeDeltaTime);
	}
	else
	{
		target1 = Traectory(P1, P2, P3, P4, maxT + timeDeltaTime);
	}

	Vector3 target2 = Traectory(P1, P2, P3, P4, maxT);
	Vector3 target3 = (target2 - target1).normolize();
	Vector3 rotateX(target3.X(), target3.Y(), 0);
	rotateX = rotateX.normolize();

	Vector3 VPX = Vector3(1, 0, 0).vectProisvedenie(rotateX);
	double cosX = Vector3(1, 0, 0).ScalarProizv(rotateX);
	double aOY = acos(target3.Z()) * 180 / PI - 90;
	double aOZ = acos(cosX) * 180 / PI * (VPX.Z() / abs(VPX.Z()));

	double A[] = { -0.5,-0.5,-0.5 };
	glPushMatrix();
	glTranslated(target2.X(), target2.Y(), target2.Z());
	glRotated(aOZ, 0, 0, 1);
	glRotated(aOY, 0, 1, 0);
	Model(A);
	glPopMatrix();

	glColor3d(0, 0, 0);

}

unsigned long int Factorial(int i) 
{
	if (i == 0) return 1;
	else return i * Factorial(i - 1);
}

double Bernstein(double u, double n, int index) {
	return (Factorial(n) / (Factorial(index) * Factorial(n - index))) * pow(u, index) * pow(1 - u, n - index);
}

void CalculatePoint(double u, double v, Vector3& vec) {
	Vector3 new_v;
	int n = 3, m = 3;
	for (int i = 0; i < points.size(); ++i) {
		for (int j = 0; j < points[i].size(); ++j) 
		{
			new_v += points[i][j] * Bernstein(u, n, i) * Bernstein(v, m, j);
		}
	}
	vec = new_v;
}

std::array<Vector3, 4> PoligonT(double a, double b, double c) {

	std::array<Vector3, 4> tempMas;
	CalculatePoint(a, b, tempMas[0]);
	CalculatePoint(a, b + c, tempMas[1]);
	CalculatePoint(a + c, b, tempMas[2]);
	CalculatePoint(a + c, b + c, tempMas[3]);

	return tempMas;
}

void Surface() {

	double a = 0.1;

	for (double b = 0; b < 1.01 - a; b += a) {

		glBegin(GL_TRIANGLES);
		for (double c = 0; c < 1.01 - a; c += a) {

			std::array<Vector3, 4> mas = PoligonT(b, c, a);

			glNormal3dv(FindNormal(mas[0].toArray(), mas[1].toArray(), mas[2].toArray(), 1));
			glVertex3dv(mas[0].toArray());
			glVertex3dv(mas[1].toArray());
			glVertex3dv(mas[3].toArray());

			glNormal3dv(FindNormal(mas[0].toArray(), mas[1].toArray(), mas[2].toArray(), 1));
			glVertex3dv(mas[0].toArray());
			glVertex3dv(mas[2].toArray());
			glVertex3dv(mas[3].toArray());

		}

		glEnd();
	}
}

void Render(OpenGL *ogl)
{
	glBindTexture(GL_TEXTURE_2D, texId[0]);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	if (changeTexture)
		glBindTexture(GL_TEXTURE_2D, texId[1]);


	GLfloat amb[] = { 0.3, 0.3, 0.3, 0.3 };
	GLfloat dif[] = { 0.7, 0.7, 0.7, 0.4 };
	GLfloat spec[] = { 0.9, 0.8, 0.4, 0.5 };
	GLfloat sh = 0.1f * 256;


	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, sh);
	glShadeModel(GL_SMOOTH);

	//===================================
	//Прогать тут  

	double P1[] = { 1,1,1 }; 
	double P2[] = { -8,2,3 };
	double P3[] = { 6,2,2 };
	double P4[] = { 5,4,1 };

	Rocket(P1, P2, P3, P4, TimeDeltaTime);
	Surface();

   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();				    
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); 
	glLoadIdentity(); 
	glDisable(GL_LIGHTING);


	//классик моего авторства для удобной работы с рендером текста.

	GuiTextRectangle rec;
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);

	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "Q - смена текстур" << std::endl;
	ss << "B - вкл/выкл альфа-наложение" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "НЕ СТАЛ ДОБАВЛЯТЬ В ПРОЕКТ" << std::endl;
	ss << "КРИВЫЕ ЭРМИТА И БЕЗЬЕ, ТАК КАК" << std::endl;
	ss << "ПОКАЗЫВАЛ ИХ НА ЛАБЕ" << std::endl;
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


}