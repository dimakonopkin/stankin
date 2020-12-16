#include "Render.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"
#include "MyOGL.h"

#include "CVector.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "MyShaders.h"
#include "ObjLoader.h"
#include "GUItextRectangle.h"
#include "Texture.h"

#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f


GLuint texId[3];
GuiTextRectangle rec;
bool textureMode = true;
bool lightMode = true;
bool shadow = false;

//небольшой дефайн для упрощения кода
#define POP glPopMatrix()
#define PUSH glPushMatrix()

ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //массивчик для десяти шейдеров
Shader frac;
Shader cassini;



double angle2 = 10;
bool DeerFlag = true;
bool StarFlag = false;

Vector3 orcBodyPositionCam;

class Vector4
{

public: double rotationAngle;
	    int x;
	    int y;
	    int z;

		Vector4(){}

	    Vector4(double ang, int x1, int y1, int z1) {
			rotationAngle = ang;
			x = x1;
			y = y1;
			z = z1;
		}
};

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	Vector3 cameraFront;

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
		lookPoint.setCoords(orcBodyPositionCam.X(), orcBodyPositionCam.Y(), orcBodyPositionCam.Z());
		//lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1)  + orcBodyPositionCam.X(),
			camDist * cos(fi2) * sin(fi1) + orcBodyPositionCam.Y(),
			camDist * sin(fi2) + orcBodyPositionCam.Z());

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

	void SetLookPoint(double x, double y, double z)
	{
		lookPoint.setCoords(x, y, z);
	}

	void SetLookPoint(Vector3 vec)
	{
		lookPoint = vec;
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
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
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
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
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
}
light;  //создаем источник света
//старые координаты мыши
int mouseX = 0, mouseY = 0;
float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//обработчик движения мыши
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.005 * dx;
		camera.fi2 += -0.005 * dy;
	
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}
	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

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

//обработчик вращения колеса  мыши
void mouseWheelEvent(OpenGL *ogl, int delta)
{
	/*
	//Тут просто скролл при вращении камерой
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.camDist += 0.005 * delta;
		camera.pos.setCoords(camera.camDist * cos(camera.fi2) * cos(camera.fi1),
			camera.camDist * cos(camera.fi2) * sin(camera.fi1),
			camera.camDist * sin(camera.fi2));
			}
	*/

		float _tmpZ = delta * 0.003;
		if (ogl->isKeyPressed('Z'))
			_tmpZ *= 10;
		zoom += 0.2 * zoom * _tmpZ;


		if (delta < 0 && camera.camDist <= 1)
			return;
		if (delta > 0 && camera.camDist >= 100)
			return;

		camera.camDist += 0.01 * delta;
	

}

double inputW;
double inputA;
double inputD;
double inputS;

Vector3 orcBodyPosition;
Vector3 orcRightHandPosition;
Vector3 orcLeftHandPosition;

double orcBodyRotation;
double orcRightHandRotation;
double orcLeftHandRotation;

Vector3 orcTorus1Position;
Vector3 orcTorus2Position;
Vector3 orcTorus3Position;
Vector3 orcTorus4Position;
Vector3 orcTorus5Position;

double orcTorus1Rotation;
double orcTorus2Rotation;
double orcTorus3Rotation;
double orcTorus4Rotation;
double orcTorus5Rotation;

double rotSpeed;
double moveSpeed;

bool hit;
bool startHit;
bool revHit;

double hitSpeed;
double revSpeed;

double hitPosition;
double revPosition;

bool bodyUp;

bool torusUp1;
bool torusUp2;
bool torusUp3;
bool torusUp4;
bool torusUp5;

double ras;
bool death;
Vector3 holePositions[5];
Vector3 dogPosition;
double dogSpeed;
double dogRotation;
int dogFall;
int  dogCount;
bool hitIt;

Vector3 hammerPosition;

//обработчик нажатия кнопок клавиатуры
void keyDownEvent(OpenGL *ogl, int key)
{
	

	if (OpenGL::isKeyPressed('L'))
	{
		lightMode = !lightMode;
	}

	if (OpenGL::isKeyPressed('T'))
	{
		textureMode = !textureMode;
	}	   

	if (OpenGL::isKeyPressed('F'))
	{
		light.pos = camera.pos;
	}

	if (OpenGL::isKeyPressed('W'))
	{
		inputW = 1;
	}
	else
	{
		inputW = 0;
	}

	if (OpenGL::isKeyPressed('A'))
	{
		inputA = 1;
	}
	else
	{
		inputA = 0;
	}

	if (OpenGL::isKeyPressed('D'))
	{
		inputD = 1;
	}
	else
	{
		inputD = 0;
	}

	if (OpenGL::isKeyPressed('S'))
	{
		inputS = 1;
	}
	else
	{
		inputS = 0;
	}

	if (OpenGL::isKeyPressed(VK_LBUTTON)) {

		if (!hit) {
			hit = true;
			startHit = true;
		}
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}

ObjFile orcBody, orcRightHand, orcLeftHand, orcTorus1, orcTorus2, orcTorus3, orcTorus4, orcTorus5, dog, sand, hole1, hole2, hole3, hole4, hole5;
Texture orcTex, orcTorusTex, dogTex, sandTex;

Vector3 WASDMoving(Vector3 pos, double rot) {


	pos.SetCoordY(pos.Y() + (inputS - inputW) * cos(rot * 3.14 / 180) * moveSpeed);
	pos.SetCoordX(pos.X() - (inputS - inputW) * sin(rot * 3.14 / 180) * moveSpeed);
	
	return pos;
}

double WASDRotation(double rotAngle) {

	rotAngle = rotAngle + (inputA - inputD) * rotSpeed;

	return rotAngle;
}

//выполняется перед первым рендером
void initRender(OpenGL* ogl)
{

	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   загрузка текстуры из файла
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	frac.FshaderFileName = "shaders\\frac.frag"; //имя файла фрагментного шейдера
	frac.LoadShaderFromFile(); //загружаем шейдеры из файла
	frac.Compile(); //компилируем

	cassini.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	cassini.FshaderFileName = "shaders\\cassini.frag"; //имя файла фрагментного шейдера
	cassini.LoadShaderFromFile(); //загружаем шейдеры из файла
	cassini.Compile(); //компилируем


	s[0].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[0].FshaderFileName = "shaders\\light.frag"; //имя файла фрагментного шейдера
	s[0].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[0].Compile(); //компилируем

	s[1].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //имя файла фрагментного шейдера
	s[1].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[1].Compile(); //компилируем



	 //так как гит игнорит модели *.obj файлы, так как они совпадают по расширению с объектными файлами, 
	 // создающимися во время компиляции, я переименовал модели в *.obj_m


	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\dog.obj", &dog);
	dogTex.loadTextureFromFile("textures//dog.bmp");
	dogTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\sand.obj", &sand);
	sandTex.loadTextureFromFile("textures//sand.bmp");
	sandTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &hole1);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &hole2);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &hole3);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &hole4);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &hole5);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcBody.obj", &orcBody);
	orcTex.loadTextureFromFile("textures//orc.bmp");
	orcTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcRightHand.obj", &orcRightHand);


	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcLeftHand.obj", &orcLeftHand);


	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus1.obj", &orcTorus1);
	orcTorusTex.loadTextureFromFile("textures//Torus.bmp");
	orcTorusTex.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus2.obj", &orcTorus2);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus3.obj", &orcTorus3);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus4.obj", &orcTorus4);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\orcTorus5.obj", &orcTorus5);


	inputW = 0;
	inputA = 0;
	inputD = 0;
	inputS = 0;

	orcBodyPosition = Vector3(0, 0, 10);
	orcRightHandPosition = Vector3(0, 0, 10);
	orcLeftHandPosition = Vector3(0, 0, 10);

	orcBodyRotation = 0;
	orcRightHandRotation = 0;
	orcLeftHandRotation = 0;

	orcTorus1Position = Vector3(0, 0, 3);
	orcTorus2Position = Vector3(0, 0, 2);
	orcTorus3Position = Vector3(0, 0, 1);
	orcTorus4Position = Vector3(0, 0, 0);
	orcTorus5Position = Vector3(0, 0, -1);

	orcTorus1Rotation = 0;
	orcTorus2Rotation = 0;
	orcTorus3Rotation = 0;
	orcTorus4Rotation = 0;
	orcTorus5Rotation = 0;



	rotSpeed = 2;
	moveSpeed = 0.5;

	hit = false;
	startHit = false;
	revHit = false;
	hitSpeed = 1;
	revSpeed = 0.1;
	hitPosition = 0;

	bodyUp = false;

	torusUp1 = false;
	torusUp2 = false;
	torusUp3 = false;
	torusUp4 = false;
	torusUp5 = false;

	ras = 0;
	death = false;

	holePositions[0] = Vector3(15, 5, 0);
	holePositions[1] = Vector3(-15, -10, 0);
	holePositions[2] = Vector3(25, -15, 0);
	holePositions[3] = Vector3(-25, 25, 0);
	holePositions[4] = Vector3(5, -5, 0);
    dogCount = 0;

	dogPosition = holePositions[0];
	dogRotation = 0;
	dogFall = 1;
	dogSpeed = 0;

	hitIt = false;
	
}


//выполняется перед первым рендером

void Render(OpenGL *ogl)
{   
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);



	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//настройка материала
	GLfloat amb[] = { 1.0, 1.0, 1.0, 1. };
	GLfloat dif[] = { 0.6, 0.6, 0.6, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//Прогать тут  




	//s[0].UseShader();

	//передача параметров в шейдер.  Шаг один - ищем адрес uniform переменной по ее имени. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//Шаг 2 - передаем ей значение
	glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	location = glGetUniformLocationARB(s[0].program, "Ia");
	glUniform3fARB(location, 0.2, 0.2, 0.2);

	location = glGetUniformLocationARB(s[0].program, "Id");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(s[0].program, "Is");
	glUniform3fARB(location, .7, .7, .7);


	location = glGetUniformLocationARB(s[0].program, "ma");
	glUniform3fARB(location, 0.2, 0.2, 0.1);

	location = glGetUniformLocationARB(s[0].program, "md");
	glUniform3fARB(location, 0.4, 0.65, 0.5);

	location = glGetUniformLocationARB(s[0].program, "ms");
	glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	location = glGetUniformLocationARB(s[0].program, "camera");
	glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());

	//Shader::DontUseShaders();


	orcBodyPosition = WASDMoving(orcBodyPosition, orcBodyRotation);
	orcBodyRotation = WASDRotation(orcBodyRotation);

	orcRightHandPosition = WASDMoving(orcRightHandPosition, orcRightHandRotation);
	orcRightHandRotation = WASDRotation(orcRightHandRotation);

	orcLeftHandPosition = WASDMoving(orcLeftHandPosition,orcLeftHandRotation);
	orcLeftHandRotation = WASDRotation(orcLeftHandRotation);

	orcTorus1Position = WASDMoving(orcTorus1Position, orcTorus1Rotation);
	orcTorus1Rotation = WASDRotation(orcTorus1Rotation);

	orcTorus2Position = WASDMoving(orcTorus2Position, orcTorus2Rotation);
	orcTorus2Rotation = WASDRotation(orcTorus2Rotation);

	orcTorus3Position = WASDMoving(orcTorus3Position, orcTorus3Rotation);
	orcTorus3Rotation = WASDRotation(orcTorus3Rotation);

	orcTorus4Position = WASDMoving(orcTorus4Position, orcTorus4Rotation);
	orcTorus4Rotation = WASDRotation(orcTorus4Rotation);

	orcTorus5Position = WASDMoving(orcTorus5Position, orcTorus5Rotation);
	orcTorus5Rotation = WASDRotation(orcTorus5Rotation);

	Vector3 hammer = Vector3(-1 * sin((orcBodyRotation - 20) * 3.14 / 180) * (-10), 1 * cos((orcBodyRotation - 20) * 3.14 / 180) * (-10), 0);
	hammerPosition = Vector3(orcBodyPosition.X() + hammer.X(), orcBodyPosition.Y() + hammer.Y(), orcLeftHandPosition.Z()-5);

	glPushMatrix();
	glTranslated(holePositions[0].X(), holePositions[0].Y(), holePositions[0].Z() - 2);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	hole1.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(holePositions[1].X(), holePositions[1].Y(), holePositions[1].Z() - 2);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	hole2.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(holePositions[2].X(), holePositions[2].Y(), holePositions[2].Z() - 2);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	hole3.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(holePositions[3].X(), holePositions[3].Y(), holePositions[3].Z() - 2);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	hole4.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(holePositions[4].X(), holePositions[4].Y(), holePositions[4].Z() - 2);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	hole5.DrawObj();
	glPopMatrix();

	glPushMatrix();

	if (!bodyUp) {
		if (orcBodyPosition.Z() < 12) {

			orcBodyPosition.SetCoordZ(orcBodyPosition.Z() + 0.02);
			orcLeftHandPosition.SetCoordZ(orcLeftHandPosition.Z() + 0.01);
			orcRightHandPosition.SetCoordZ(orcRightHandPosition.Z() + 0.01);

		}
		else
		{
			bodyUp = true;
		}
	}
	else
	{
		if (orcBodyPosition.Z() > 10) {

			orcBodyPosition.SetCoordZ(orcBodyPosition.Z() - 0.02);
			orcLeftHandPosition.SetCoordZ(orcLeftHandPosition.Z() - 0.01);
			orcRightHandPosition.SetCoordZ(orcRightHandPosition.Z() - 0.01);

		}
		else
		{
			bodyUp = false;
		}
	}

	if (!torusUp1) {
		if (orcTorus1Position.Z() < 4) {

			orcTorus1Position.SetCoordZ(orcTorus1Position.Z() + 0.02);

		}
		else
		{
			torusUp1 = true;
		}
	}
	else
	{
		if (orcTorus1Position.Z() > 3.5) {

			orcTorus1Position.SetCoordZ(orcTorus1Position.Z() - 0.02);

		}
		else
		{
			torusUp1 = false;
		}
	}

	if (!torusUp2) {
		if (orcTorus2Position.Z() < 2.6) {

			orcTorus2Position.SetCoordZ(orcTorus2Position.Z() + 0.015);

		}
		else
		{
			torusUp2 = true;
		}
	}
	else
	{
		if (orcTorus2Position.Z() > 2.2) {

			orcTorus2Position.SetCoordZ(orcTorus2Position.Z() - 0.015);

		}
		else
		{
			torusUp2 = false;
		}
	}

	if (!torusUp3) {
		if (orcTorus3Position.Z() < 1.5) {

			orcTorus3Position.SetCoordZ(orcTorus3Position.Z() + 0.01);

		}
		else
		{
			torusUp3 = true;
		}
	}
	else
	{
		if (orcTorus3Position.Z() > 1) {

			orcTorus3Position.SetCoordZ(orcTorus3Position.Z() - 0.01);

		}
		else
		{
			torusUp3 = false;
		}
	}

	if (!torusUp4) {
		if (orcTorus4Position.Z() < 0.4) {

			orcTorus4Position.SetCoordZ(orcTorus4Position.Z() + 0.02);

		}
		else
		{
			torusUp4 = true;
		}
	}
	else
	{
		if (orcTorus4Position.Z() > 0) {

			orcTorus4Position.SetCoordZ(orcTorus4Position.Z() - 0.02);

		}
		else
		{
			torusUp4 = false;
		}
	}

	if (!torusUp5) {
		if (orcTorus5Position.Z() < -0.4) {

			orcTorus5Position.SetCoordZ(orcTorus5Position.Z() + 0.035);

		}
		else
		{
			torusUp5 = true;
		}
	}
	else
	{
		if (orcTorus5Position.Z() > -1) {

			orcTorus5Position.SetCoordZ(orcTorus5Position.Z() - 0.035);

		}
		else
		{
			torusUp5 = false;
		}
	}

	glPushMatrix();
	glTranslated(0, 0, -3);
	glRotated(90, 1, 0, 0);
	sandTex.bindTexture();
	sand.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcBodyPosition.X(), orcBodyPosition.Y(), orcBodyPosition.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcBodyRotation, 0, 1, 0);
	orcTex.bindTexture();
	orcBody.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcRightHandPosition.X(), orcRightHandPosition.Y(), orcRightHandPosition.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcRightHandRotation, 0, 1, 0);
	orcTex.bindTexture();
	orcRightHand.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcLeftHandPosition.X(), orcLeftHandPosition.Y(), orcLeftHandPosition.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcLeftHandRotation, 0, 1, 0);

	if (hit) {
		if (startHit) {
			if (hitPosition >= -220) {
				hitPosition -= hitSpeed;
				glRotated(hitPosition, 1, 0, 0);
				hitSpeed += 0.5;
			}
			else {
				revHit = true;
				startHit = false;
			}
		}

		if (revHit) {
			if (hitPosition <= 0) {
				if (hitPosition >= -60 && hitPosition <= -30) {
					hitIt = true;
				}
				else
				{
					hitIt = false;
				}
				hitPosition += revSpeed;
				glRotated(hitPosition, 1, 0, 0);
				revSpeed += 1;
			}
			else {
				revHit = false;
				startHit = true;
				hit = false;
				revSpeed = 0.1;
				hitSpeed = 1;
				hitPosition = 0;
			}
		}
	}

	orcTex.bindTexture();
	orcLeftHand.DrawObj();
	glPopMatrix();



	glPushMatrix();
	glTranslated(orcTorus1Position.X(), orcTorus1Position.Y(), orcTorus1Position.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcTorus1Rotation, 0, 1, 0);
	orcTorusTex.bindTexture();
	orcTorus1.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcTorus2Position.X(), orcTorus2Position.Y(), orcTorus2Position.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcTorus2Rotation, 0, 1, 0);
	orcTorusTex.bindTexture();
	orcTorus2.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcTorus3Position.X(), orcTorus3Position.Y(), orcTorus3Position.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcTorus3Rotation, 0, 1, 0);



	orcTorusTex.bindTexture();
	orcTorus3.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcTorus4Position.X(), orcTorus4Position.Y(), orcTorus4Position.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcTorus4Rotation, 0, 1, 0);
	orcTorusTex.bindTexture();
	orcTorus4.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(orcTorus5Position.X(), orcTorus5Position.Y(), orcTorus5Position.Z());
	glRotated(90, 1, 0, 0);
	glRotated(orcTorus5Rotation, 0, 1, 0);
	orcTorusTex.bindTexture();
	orcTorus5.DrawObj();
	glPopMatrix();


	if (death) {

		switch (dogFall)
		{
		case 1:
			if (dogPosition.Z() < 20) {
				dogPosition.SetCoordZ(dogPosition.Z() + dogSpeed);
				dogSpeed += 0.5;
			}
			else
			{
				dogFall = 2;
				dogSpeed = 0;
			}
			break;
		case 2:
			if (dogRotation < 720) {
				dogRotation += 36;
			}
			else
			{
				dogFall = 3;
			}
			break;
		case 3:
			if (dogPosition.Z() > -50) {
				dogPosition.SetCoordZ(dogPosition.Z() - dogSpeed);
				dogSpeed += 0.5;
			}
			else
			{
				dogFall = 4;
			}
			break;
		case 4:
			if (dogCount < 4) {
				dogCount++;
			}
			else
			{
				dogCount = 1;
			}
			dogPosition.SetCoordX(holePositions[dogCount].X());
			dogPosition.SetCoordY(holePositions[dogCount].Y());
			dogPosition.SetCoordZ(holePositions[dogCount].Z());

			dogFall = 1;
			dogSpeed = 0;
			dogRotation = 0;
			death = false;
			break;
		}
	}

	if (dogFall != 4) {
		glPushMatrix();
		glTranslated(dogPosition.X(), dogPosition.Y(), dogPosition.Z());
		glRotated(90, 1, 0, 0);
		glRotated(dogRotation, 1, 0, 0);
		dogTex.bindTexture();
		dog.DrawObj();
		glPopMatrix();
	}

	if (hitIt) {

		ras = sqrt((hammerPosition.X() - dogPosition.X()) * (hammerPosition.X() - dogPosition.X()) + (hammerPosition.Y() - dogPosition.Y()) * (hammerPosition.Y() - dogPosition.Y()) + (hammerPosition.Z() - dogPosition.Z()) * (hammerPosition.Z() - dogPosition.Z()));
		
		if (ras < 6.2 ) {
			death = true;
		}

	}


	orcBodyPositionCam.setCoords(orcBodyPosition.X(), orcBodyPosition.Y(), 10);

}   //конец тела функции


bool gui_init = false;

//рисует интерфейс, вызывется после обычного рендера
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
									//(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);


	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);


	std::stringstream ss;

	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	//ss << "G - двигать свет по горизонтали" << std::endl;
	//ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	//ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	ss << ras << std::endl;


	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
		
	Shader::DontUseShaders(); 
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

