#include <vcl.h>
#pragma hdrstop
//--------------------------------------
USEFORM("Main.cpp", Form1);
USEFORM("About.cpp", Form3);
USEFORM("Help.cpp", Form2);
//--------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
try
{
Application->Initialize();
Application->Title = "Arkanoid3D";
Application->CreateForm(__classid(TForm1), &Form1);
Application->CreateForm(__classid(TForm2), &Form2);
Application->CreateForm(__classid(TForm3), &Form3);
Application->Run();
}
catch (Exception &exception)
{
Application->ShowException(&exception);
}
catch (...)
{
try
{
throw Exception("");
}
catch (Exception &exception)
{
Application->ShowException(&exception);
}
}
return 0;
}
//--------------------------------------
В файле vars.h:
#define INITX 150
#define INITY 100
#define WINH 800
#define WINW800
#define HEIGHT 350
#define WIDTH 300
#define BRICKL30
#define BRICKH15
#define BRICKW30
#define SPEED8
#define BRICK_ROWS 8
#define BRICK_COLS 10
#define TXTR 6
#pragma once
В файле Main.h:
//--------------------------------------
#ifndef MainH
#define MainH
//--------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <math.h>
#include <math.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include "vars.h"
#include "Brick.h"
#include "Ball.h"
#include "Board.h"
//#pragma once
using namespace std;

//--------------------------------------
class TForm1: public TForm
{
__published:// IDE-managed Components
TTimer *Timer1;
TMainMenu *MainMenu1;
TMenuItem *N1;
TMenuItem *N2;
TMenuItem *N3;
TMenuItem *N4;
TMenuItem *N5;
TMenuItem *N6;
TMenuItem *N7;
TMenuItem *N8;
TMenuItem *N9;
TMenuItem *N10;
TEdit *edtLevel;
TEdit *edtScore;
TEdit *edtTime;
TEdit *edtLifes;
void __fastcall FormCreate(TObject *Sender);
void __fastcall FormDestroy(TObject *Sender);
void __fastcall FormResize(TObject *Sender);
void __fastcall FormPaint(TObject *Sender);
void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
void __fastcall Timer1Timer(TObject *Sender);
void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
int Y);
void __fastcall N2Click(TObject *Sender);
void __fastcall N8Click(TObject *Sender);
void __fastcall N9Click(TObject *Sender);
void __fastcall N4Click(TObject *Sender);
void __fastcall N10Click(TObject *Sender);
void __fastcall FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight,
bool &Resize);
void __fastcall N6Click(TObject *Sender);
void __fastcall N7Click(TObject *Sender);
void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
TShiftState Shift, int X, int Y);
private:// User declarations
HGLRC ghRC; // указатель на контекст воспроизведения (Rendering Context)
HDC hDC; // дескриптор (контекст) устройства
public:// User declarations
__fastcall TForm1(TComponent* Owner);
Brick brick[BRICK_ROWS][BRICK_COLS];
Ball ball;
Board board;
BOOL bSetupPixelFormat(HDC);
void Draw();
void NewGame();
void LoadLevel();
bool IsFinish();
void SetDifficulty(int);
TDateTime curTime, dTime;
};
GLvoid LoadGLTextures();
//--------------------------------------
extern PACKAGE TForm1 *Form1;
//--------------------------------------
#endif
В файле Main.cpp:
//--------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "Help.h"
#include "About.h"
//--------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
int tetta=91, phi=90;
unsigned short level=1, score=0;
GLuinttexture[TXTR];
float V1[8][3]={
{INITX, INITY, BRICKH},
{INITX+WIDTH, INITY, BRICKH},
{INITX+WIDTH, INITY+HEIGHT, BRICKH},
{INITX, INITY+HEIGHT, BRICKH},
{INITX, INITY+HEIGHT, 1},
{INITX, INITY, 1},
{INITX+WIDTH, INITY, 1},
{INITX+WIDTH, INITY+HEIGHT, 1}
};
//--------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
: TForm(Owner)
{
}
//--------------------------------------
BOOL TForm1::bSetupPixelFormat(HDC hDC)
{
PIXELFORMATDESCRIPTOR pfd; //Создаем структуру
int pixelformat;
pfd.nSize = sizeof (PIXELFORMATDESCRIPTOR); //Размер структуры
pfd.nVersion = 1; //Версия структуры
pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
pfd.iLayerType = PFD_MAIN_PLANE; //Тип поверхности
pfd.iPixelType = PFD_TYPE_RGBA; //Формат указания цвета
pfd.cColorBits = 16; //Глубина цвета
pfd.cDepthBits = 16; //Размер буфера глубины
pfd.cAccumBits = 0; //Общее число битовых плоскостей в буфере аккумулятора
pfd.cStencilBits = 0; //Размер буфера трафарета
if (!(pixelformat=ChoosePixelFormat(hDC, &pfd))){
MessageBox(NULL, "Невозможно выбрать формат пикселей", "Error", MB_OK);
return false;
}
if (!SetPixelFormat (hDC, pixelformat, &pfd)){
MessageBox(NULL, "Невозможно установить формат пикселей", "Error", MB_OK);
return false;
}
return true;
}
//--------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
ball.x=board.center();
ball.y=board.y;
LoadLevel();
dTime=0;
N9->Checked = true;
edtTime->Text="Время: 00:00";
edtScore->Text="Очки: 0";
edtLifes->Text="Жизни: " + IntToStr(board.lifes);
hDC = GetDC(Handle); //Handle – дескриптор окна (hwnd в WinAPI)
if (!bSetupPixelFormat(hDC)) //Устанавливаем формат пикселей
return;
ghRC = wglCreateContext(hDC); //Создаем контекст воспроизведения
wglMakeCurrent(hDC, ghRC); //Делаем его текущим
glEnable(GL_TEXTURE_2D);
glClearColor(1.0, 1.0, 1.0, 1.0); //Цвет экрана при очищении
glEnable(GL_COLOR_MATERIAL); //Разрешаем задавать цвет объектам
glEnable(GL_DEPTH_TEST); //Тест глубины для объемности изображения
glEnable(GL_LIGHTING); //Разрешаем освещение
glEnable(GL_LIGHT0); //Включили освещение 0
glEnable(GL_LINE_SMOOTH);
glShadeModel(GL_SMOOTH); // Разрешение сглаженного закрашивания
glDepthFunc(GL_LEQUAL); // Тип теста глубины
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Улучшенные вычисления перспективы
LoadGLTextures();// Загрузка текстур
float p[4]={0, 100, -300, 0};
glLightfv(GL_LIGHT0, GL_POSITION, p); //Установка позиции освещения
GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; // Значения фонового света ( НОВОЕ )
glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient); // Установка Фонового Света
}
//--------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
if(ghRC){
wglMakeCurrent(hDC, 0);
wglDeleteContext(ghRC);
}
if(hDC) ReleaseDC(Handle, hDC);
}
//--------------------------------------
void __fastcall TForm1::FormResize(TObject *Sender)
{
glViewport(-INITX, -INITY, WINW, WINH);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45, WINW/WINH, 1, 900);
glTranslatef(-260, -300, 0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}
//--------------------------------------
void TForm1::Draw()
{
glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
glPushMatrix();
glBindTexture(GL_TEXTURE_2D, texture[0]);
glColor3f(255, 255, 255);
glBegin(GL_QUADS);
glTexCoord2f(0, 0); glVertex3f(0, 0, -850);
glTexCoord2f(1, 0); glVertex3f(WINW+200, 0, -850);
glTexCoord2f(1, 1); glVertex3f(WINW+200, WINH, -850);
glTexCoord2f(0, 1); glVertex3f(0, WINH, -850);
glEnd();
glTranslatef(INITX+WIDTH/2, INITY+HEIGHT/2, 0);
glRotatef(180, 0, 0, 1);
float angle1 = tetta*M_PI/180,
angle2 = -phi*M_PI/180;
int rd=700;
int x = rd*sin(angle1)*cos(angle2),
z = rd*sin(angle1)*sin(angle2),
y = rd*cos(angle1);
gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
glTranslatef(-INITX-WIDTH/2, -INITY-HEIGHT/2, 0);
ball.Draw(texture[5]);
glBindTexture(GL_TEXTURE_2D, texture[1]);
//игровое поле
glColor3ub(255, 255, 255);
glBegin(GL_QUADS);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[0][0], V1[0][1], V1[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[1][0], V1[1][1], V1[1][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[2][0], V1[2][1], V1[2][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[3][0], V1[3][1], V1[3][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[0][0], V1[0][1], V1[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[1][0], V1[1][1], V1[1][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[6][0], V1[6][1], V1[6][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[5][0], V1[5][1], V1[5][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[0][0], V1[0][1], V1[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[3][0], V1[3][1], V1[3][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[4][0], V1[4][1], V1[4][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[5][0], V1[5][1], V1[5][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[4][0], V1[4][1], V1[4][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[5][0], V1[5][1], V1[5][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[6][0], V1[6][1], V1[6][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[7][0], V1[7][1], V1[7][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[1][0], V1[1][1], V1[1][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[2][0], V1[2][1], V1[2][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[7][0], V1[7][1], V1[7][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[6][0], V1[6][1], V1[6][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V1[2][0], V1[2][1], V1[2][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V1[3][0], V1[3][1], V1[3][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V1[4][0], V1[4][1], V1[4][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V1[7][0], V1[7][1], V1[7][2]);
glEnd();
board.Draw(texture[2]);
glBindTexture(GL_TEXTURE_2D, texture[3]);
//блоки
for(int i=0; i<BRICK_ROWS; i++){
for(int j=0; j<BRICK_COLS; j++){
Brick &b=brick[i][j];
if(b.active){
glColor3f(0.15*j, 0.11*i, 0.12*(BRICK_COLS-j+1));
if(b.hit==1)
b.Draw(i, j, texture[3]);
else
b.Draw(i, j, texture[4]);
}
if(b.bonus.active)
b.bonus.Draw();
}
}
glPopMatrix();
SwapBuffers(hDC);
}
//--------------------------------------
void __fastcall TForm1::FormPaint(TObject *Sender)
{
Form1->Draw();
}
//--------------------------------------
void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
TShiftState Shift)
{
switch(Key){
case VK_LEFT:{
if(board.x>=13)
board.x-=13;
else if(board.x>0)
board.x=0;
if(!ball.active){
ball.x=board.center();
ball.y=board.y-ball.r;
}
break;
}
case VK_RIGHT:{
if(board.x + board.w+13 <= WIDTH)
board.x+=13;
else if(board.x<WIDTH)
board.x=WIDTH-board.w;
if(!ball.active){
ball.x=board.center();
ball.y=board.y-ball.r;
}
break;
}
case VK_SPACE:{
if(!ball.active && board.lifes!=0){
ball.active=true;
ball.dx=0;
ball.series=0;
ball.dy=-ball.v;
curTime=Time();
}
break;
}
case 'A':{
phi-=2;
break;
}
case 'D':{
phi+=2;
break;
}
case 'S':{
if(tetta<178)
tetta+=2;
break;
}
case 'W':{
if(tetta>2)
tetta-=2;
break;
}
case 'P':{
if(Timer1->Enabled)
dTime+=Time()-curTime;
else
curTime=Time();
Timer1->Enabled=!Timer1->Enabled;
break;
}
}
}
//--------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
TDateTime time=(Time()+dTime-curTime);
if(ball.active){
edtTime->Text="Время: "+time.FormatString("nn:ss");
ball.move();
for(int i=0; i<BRICK_ROWS; i++){
for(int j=0; j<BRICK_COLS; j++){
Brick &b=brick[i][j];
if(ball.isCollision(b, i, j)){
ball.Bounce(b, i, j);
if(-b.hit==0){
b.active=false;
if(b.bonus.type != EMPTY){
b.bonus.active=true;
b.bonus.x=(j+0.5)*b.w;
b.bonus.y=(i+0.5)*b.l;
b.bonus.dy=b.bonus.v;
}
if(IsFinish()){
ball.active=false;
ball.dx=0;
ball.dy=0;
AnsiString s="Вы прошли уровень "+IntToStr(level)+", набрав " +
IntToStr(score)+" очков за " +(Time()+
dTime-curTime).FormatString("nn:ss")+"! Начать следующий?";
int iResult=MessageBox(Form1->Handle, s.c_str(), "Уровень пройден!", MB_YESNO|MB_ICONINFORMATION);
if(iResult == IDNO)
Form1->Close();
level++;
LoadLevel();
dTime=0;
}
}
score+=50+10*(++ball.series-1);
edtScore->Text="Очки: "+IntToStr(score);
goto o;
}
}
}
}
o:
for(int i=0; i<BRICK_ROWS; i++)
for(int j=0; j<BRICK_COLS; j++)
if(brick[i][j].bonus.active)
brick[i][j].bonus.move();
Draw();
}
//--------------------------------------
void __fastcall TForm1::FormMouseMove(TObject *Sender, TShiftState Shift, int X,
int Y)
{
GLint viewport[4];
GLdouble mvm[16];
GLdouble projm[16];
GLdouble wx, wy, wz;
GLfloat zval;
glGetIntegerv(GL_VIEWPORT, viewport);
glGetDoublev(GL_MODELVIEW_MATRIX, mvm);
glGetDoublev(GL_PROJECTION_MATRIX, projm);
glReadPixels(X, Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zval);
gluUnProject(X, 575, zval, mvm, projm, viewport, &wx, &wy, &wz);
if(wx>=INITX+board.w*0.5 && wx<=INITX+WIDTH-board.w*0.5)
board.x=wx-INITX-board.w*0.5;
if(!ball.active){
ball.x=board.center();
ball.y=board.y-ball.r;
}
}
//--------------------------------------

void TForm1::NewGame()
{
Timer1->Enabled=false;
int iResult=MessageBox(Handle, "Вы действительно хотите начать новую игру?", "Новая игра", MB_YESNO|MB_ICONQUESTION);
if(iResult == IDNO){
Timer1->Enabled=true;
return;
}
if(iResult == IDYES)
Timer1->Enabled=true;
level=1;
LoadLevel();
ball.active=false;
ball.dx=0;
ball.dy=0;
board.lifes=3;
dTime=0;
score=0;
edtTime->Text="Время: 00:00";
edtScore->Text="Очки: 0";
edtLifes->Text="Жизни: 3";
}
void __fastcall TForm1::N2Click(TObject *Sender)
{
NewGame();
}
//--------------------------------------
void __fastcall TForm1::N8Click(TObject *Sender)
{
if(!N8->Checked)
SetDifficulty(2);
}
//--------------------------------------
void __fastcall TForm1::N9Click(TObject *Sender)
{
if(!N9->Checked)
SetDifficulty(1);
}
//--------------------------------------
void __fastcall TForm1::N4Click(TObject *Sender)
{
Form1->Close();
}
//--------------------------------------
void TForm1::LoadLevel()
{
AnsiString s = "level" + IntToStr(level) + ".txt";
ifstream myfile (s.c_str());
if (myfile.is_open()){
srand(time(NULL));
int i=0;
string line;
bool exp=false,
plr=false,
ballincr=false;
while (getline(myfile, line)){
for(unsigned int j=0; j<BRICK_COLS && i<BRICK_ROWS; j++){
if(line[j]>='0' && line[j]<='3'){
brick[i][j].hit=line[j]-'0';
bonus t=EMPTY;
if(line[j]=='0')
brick[i][j].active=false;
else{
brick[i][j].active=true;
int num=random(10);
if(num==0 && !exp){
t=EXPAND;
exp=true;
}
else if(num==1 && !plr){
t=PLAYER;
plr=true;
}
else if(num==2 && ! ballincr){
t=BALLINCR;
ballincr=true;
}
}
brick[i][j].bonus = Bonus((j+0.5)*brick[i][j].w, (i+0.5)*brick[i][j].l, t);
}
}
i++;
}
myfile.close();
board.setDefSize();
ball.setDefRad();
edtLevel->Text="Уровень: "+IntToStr(level);
}
else{
int n=MessageBox(Handle, "Не удалось открыть файл уровней!", "Ошибка!", MB_RETRYCANCEL|MB_ICONERROR);
if(n==IDCANCEL)
Close();
else if(n==IDRETRY)
LoadLevel();
}
}
bool TForm1::IsFinish()
{
for(int i=0; i<BRICK_ROWS; i++)
for(int j=0; j<BRICK_COLS; j++)
if(brick[i][j].active == true)
return false;
return true;
}
void TForm1::SetDifficulty(int iDiff)
{
NewGame();
switch(iDiff){
case 2:{
Timer1->Interval=30;
N10->Checked = false;
N9->Checked = false;
N8->Checked = true;
break;
}
case 1:{
Timer1->Interval=50;
N10->Checked = false;
N9->Checked = true;
N8->Checked = false;
break;
}
case 0:{
Timer1->Interval=70;
N10->Checked = true;
N9->Checked = false;
N8->Checked = false;
break;
}
}
}
void __fastcall TForm1::N10Click(TObject *Sender)
{
if(!N10->Checked)
SetDifficulty(0);
}
// Загрузка картинки и конвертирование в текстуру
GLvoid LoadGLTextures()
{
// Загрузка картинки
AUX_RGBImageRec *texture1;
for(int i=0; i<TXTR; i++){
AnsiString s="tex"+IntToStr(i)+".bmp";
texture1 = auxDIBImageLoad(s.c_str());
// Создание текстуры
glGenTextures(1, &texture[i]);
glBindTexture(GL_TEXTURE_2D, texture[i]);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0,
GL_RGB, GL_UNSIGNED_BYTE, texture1->data);
}
}
//--------------------------------------
void __fastcall TForm1::FormCanResize(TObject *Sender, int &NewWidth,
int &NewHeight, bool &Resize)
{
Resize=false;
}
//--------------------------------------
void __fastcall TForm1::N6Click(TObject *Sender)
{
Form2->ShowModal();
}
//--------------------------------------
void __fastcall TForm1::N7Click(TObject *Sender)
{
Form3->ShowModal();
}
//--------------------------------------
void __fastcall TForm1::FormMouseDown(TObject *Sender, TMouseButton Button,
TShiftState Shift, int X, int Y)
{
if(!ball.active && board.lifes!=0 && Button == mbLeft){
ball.active=true;
ball.dx=0;
ball.series=0;
ball.dy=-ball.v;
curTime=Time();
}
}
//--------------------------------------
В файле Ball.h:
//--------------------------------------
#ifndef BallH
#define BallH
#include "vars.h"
#pragma once
class Brick;
class Ball
{
public:
float x, y, // координаты центра
dx, dy, // скорость по координатам
r, // радиус
v; // скорость
bool active; // движется ли мяч
short series; // количество кирпичей в серии
//конструктор
Ball(): x(), y(), v(SPEED), r(), dx(), dy(), active(false), series(0){}
void move(); //движение шарика
void setDefRad(); //вернуть радиус
void incRad(); //увеличить радиус
void Bounce(Brick &, int, int); //отскок от кирпичей
bool isCollision(Brick &, int, int); //столкновение с кирпичом
void Draw(GLuint); //прорисовка
};
//--------------------------------------
#endif
В файле Ball.cpp:
//--------------------------------------
#pragma hdrstop
#include "Main.h"
#include "Ball.h"
//--------------------------------------
#pragma package(smart_init)
void Ball::move()
{
x+=dx;
y+=dy;
if(x-r<=0)
{
x=r;
dx*=-1;
}
else if(y-r<=0)
{
y=r;
dy*=-1;
}
else if(x+r>=WIDTH)
{
x=WIDTH-r;
dx*=-1;
}
else if(y+r>=Form1->board.y && y+r<=Form1->board.y+Form1->board.h && x>Form1->board.x && x<Form1->board.x+Form1->board.w)
{
y=Form1->board.y-r;
series=0;
float alpha = M_PI/2+M_PI/3*(Form1->board.x+Form1->board.w*0.5-x)/(Form1->board.w*0.5);
float t=dy;
dy=-sin(alpha)*sqrt(t*t+dx*dx);
dx=cos(alpha)*sqrt(t*t+dx*dx);
}
else if(y>HEIGHT)
{
active=false;
Form1->dTime+=Time()-Form1->curTime;
x=Form1->board.center();
y=Form1->board.y-r;
dx=0;
dy=0;
Form1->board.setDefSize();
setDefRad();
Form1->edtLifes->Text="Жизни: " + IntToStr(-Form1->board.lifes);
if(Form1->board.lifes==0){
int iResult=MessageBox(Form1->Handle, "Вы проиграли. Начать новую игру?", "Вы проиграли", MB_YESNO);
if(iResult == IDYES)
Form1->NewGame();
if(iResult == IDNO)
Form1->Close();
}
}
}
void Ball::setDefRad()
{
r=7;
}
void Ball::incRad()
{
r+=2;
}
void Ball::Bounce(Brick &b, int i, int j)
{
float tx = x-j*b.w-b.w*0.5f,
ty = y-i*b.l-b.l*0.5f;
if(-abs(tx)+abs(ty)>0)
dy*=-1;
else if(abs(tx)-abs(ty)>0)
dx*=-1;
else{
if(dx>0){
if(x < j*b.w)
dx*=-1;
}
else if(x > (j+1)*b.w)
dx*=-1;
if(dy>0){
if(y < i*b.l)
dy*=-1;
}
else if(y > (i+1)*b.l)
dy*=-1;
}
}
bool Ball::isCollision(Brick &br, int row, int col)
{
float w=br.w, l=br.l;
if(br.active){
if(abs(x-col*w-w*0.5-2)<=r+w*0.5 && abs(y-row*l-l*0.5-2)<=r+l*0.5){
if(sqrt((col*w+w*0.5-x)*(col*w+w*0.5-x) + (row*l+l*0.5-y)*(row*l+l*0.5-y))
- (w*0.5-1)*sqrt(2.0) - r > r*(sqrt(2.0)-1))
return 0;
else
return 1;
}
}
return 0;
}
void Ball::Draw(GLuint txtr)
{
glBindTexture(GL_TEXTURE_2D, txtr);
//шар
GLUquadricObj *s;
s = gluNewQuadric();
glColor3ub(255, 255, 255);
glTranslatef(INITX+x, INITY+y, -BRICKH/2);
gluQuadricTexture(s, GL_TRUE);
gluQuadricDrawStyle(s, GLU_FILL);
gluSphere(s, r, 100, 100);
glTranslatef(-INITX-x, -INITY-y, BRICKH/2);
gluDeleteQuadric(s);
}
В файле Board.h:
//--------------------------------------
#ifndef BoardH
#define BoardH
#pragma once
class Board
{
public:
float x, y, //координаты платформы
w, h; //ширина и высота
int lifes; //жизни
Board(): x(100), y(340), lifes(3), w(40), h(8){} //конструктор
void expand(); //расширить платформу
void setDefSize(); //вернуть начальную ширину
float center(); //возвращает координату х центра платформы
void Draw(GLuint); //прорисовка платформы
};
//--------------------------------------
#endif
В файле Board.cpp:
//--------------------------------------
#pragma hdrstop
#include "Main.h"
#include "Board.h"
//--------------------------------------
#pragma package(smart_init)
void Board::expand()
{
w*=1.5;
if(x+w>WIDTH)
x-=w*0.5;
}
void Board::setDefSize()
{
w=40;
}
float Board::center()
{
return x+w*0.5;
}
void Board::Draw(GLuint txtr)
{
float V[8][3]={
{INITX+x, INITY+y, 0},
{INITX+x+w, INITY+y, 0},
{INITX+x+w, INITY+y+h, 0},
{INITX+x, INITY+y+h, 0},
{INITX+x, INITY+y+h, -BRICKH},
{INITX+x, INITY+y, -BRICKH},
{INITX+x+w, INITY+y, -BRICKH},
{INITX+x+w, INITY+y+h, -BRICKH}
};
glBindTexture(GL_TEXTURE_2D, txtr);
//платформа
glColor3ub(255, 255, 255);
glBegin(GL_QUADS);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[0][0], V[0][1], V[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[1][0], V[1][1], V[1][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[2][0], V[2][1], V[2][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[3][0], V[3][1], V[3][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[0][0], V[0][1], V[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[1][0], V[1][1], V[1][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[6][0], V[6][1], V[6][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[5][0], V[5][1], V[5][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[0][0], V[0][1], V[0][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[3][0], V[3][1], V[3][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[4][0], V[4][1], V[4][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[5][0], V[5][1], V[5][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[4][0], V[4][1], V[4][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[5][0], V[5][1], V[5][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[6][0], V[6][1], V[6][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[7][0], V[7][1], V[7][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[1][0], V[1][1], V[1][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[2][0], V[2][1], V[2][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[7][0], V[7][1], V[7][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[6][0], V[6][1], V[6][2]);
glTexCoord2f(0.0f, 0.0f); glVertex3f(V[2][0], V[2][1], V[2][2]);
glTexCoord2f(1.0f, 0.0f); glVertex3f(V[3][0], V[3][1], V[3][2]);
glTexCoord2f(1.0f, 1.0f); glVertex3f(V[4][0], V[4][1], V[4][2]);
glTexCoord2f(0.0f, 1.0f); glVertex3f(V[7][0], V[7][1], V[7][2]);
glEnd();
}

В файле Brick.h:
//--------------------------------------
#ifndef BrickH
#define BrickH
#include "Bonus.h"
#include "vars.h"
#pragma once
class Brick
{
public:
bool active; // активность кирпича
short hit; // прочность
Bonus bonus; // бонус
float w, l, h;// ширина, длина, высота
// конструктор
Brick(): w(BRICKW), l(BRICKL), h(-BRICKW), hit(1), bonus(), active(false){};
void Draw(int, int, GLuint); //прорисовка
};
//--------------------------------------
#endif
В файле Brick.cpp:
//--------------------------------------
#pragma hdrstop
#include "Main.h"
#include "Brick.h"
//--------------------------------------
#pragma package(smart_init)
void Brick::Draw(int i, int j, GLuint txtr)
{
glBindTexture(GL_TEXTURE_2D, txtr);
glBegin(GL_QUADS);
float k=1.0f;
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+j*w+k, INITY+i*l+k, 0);//0
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+i*l+k, 0);//1
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, 0);//2
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+j*w+k, INITY+(i+1)*l-k, 0);//3
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+j*w+k, INITY+i*l+k, 0);//0
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+i*l+k, 0);//1
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+i*(l+k), h);//6
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+j*w+k, INITY+i*(l+k), h);//5
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, 0);//2
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+(j)*w+k, INITY+(i+1)*l-k, 0);//3
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+(j)*w+k, INITY+(i+1)*l-k, h);//4
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, h);//7
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+j*w+k, INITY+i*l+k, 0);//0
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+j*w+k, INITY+(i+1)*l-k, 0);//3
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+j*w+k, INITY+(i+1)*l-k, h);//4
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+j*w+k, INITY+i*l+k, h);//5
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+(j)*w+k, INITY+(i+1)*l-k, h);//4
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+j*w+k, INITY+i*l+k, h);//5
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i)*l+k, h);//6
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, h);//7
glTexCoord2f(0.0f, 0.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i)*l+k, 0);//1
glTexCoord2f(1.0f, 0.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, 0);//2
glTexCoord2f(1.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i+1)*l-k, h);//7
glTexCoord2f(0.0f, 1.0f);
glVertex3f(INITX+(j+1)*w-k, INITY+(i)*l+k, h);//6
glEnd();
}
В файле Bonus.h:
//--------------------------------------
#ifndef BonusH
#define BonusH
#include "vars.h"
#pragma once
enum bonus{EMPTY, EXPAND, PLAYER, BALLINCR};
class Bonus
{
public:
float x, y, //координаты
r, //радиус
dy, v; //скорости
bonus type; //тип
bool active; //активность
//конструкторы
Bonus():x(), y(), v(SPEED-2), dy(), r(6), type(EMPTY), active(false){}
Bonus(float x_, float y_, bonus type_): x(x_), y(y_), v(SPEED-2),
r(6), dy(), type(type_), active(false){}
//движение
void move();
//прорисовка
void Draw();
};
//--------------------------------------
#endif
В файле Bonus.cpp:
//--------------------------------------
#pragma hdrstop
#include "Main.h"
#include "Bonus.h"
//--------------------------------------
#pragma package(smart_init)
void Bonus::move()
{
y+=dy;
if(y+r>=Form1->board.y && y+r<=Form1->board.y+Form1->board.h
&& x>Form1->board.x && x<Form1->board.x+Form1->board.w){
switch(type){
case EMPTY: break;
case EXPAND:{
Form1->board.expand();
active=false;
break;
}
case PLAYER:{
Form1->edtLifes->Text="Жизни: " + IntToStr(++Form1->board.lifes);
active=false;
break;
}
case BALLINCR:{
Form1->ball.incRad();
active=false;
break;
}
defaut: break;
}
}
else if(y>HEIGHT)
active=false;
}
void Bonus::Draw()
{
switch(type){
case EXPAND:{
glColor3ub(0, 255, 0);
break;
}
case PLAYER:{
glColor3ub(255, 216, 0);
break;
}
case BALLINCR:{
glColor3ub(0, 0, 255);
break;
}
default: break;
}
glTranslatef(INITX+x, INITY+y, -BRICKH/2);
auxSolidSphere(6);
glTranslatef(-INITX-x, -INITY-y, BRICKH/2);
}
В файле About.cpp:
//--------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "About.h"
//--------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//--------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
: TForm(Owner)
{
}
//--------------------------------------
void __fastcall TForm3::Button1Click(TObject *Sender)
{
Form3->Close();
}
//--------------------------------------
В файле About.h:
//--------------------------------------
#ifndef AboutH
#define AboutH
//--------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//--------------------------------------
class TForm3: public TForm
{
__published:// IDE-managed Components
TButton *Button1;
TLabel *Label1;
TLabel *Label2;
TLabel *Label3;
TLabel *Label4;
TLabel *Label5;
TLabel *Label6;
TLabel *Label7;
TImage *Image1;
TLabel *Label8;
void __fastcall Button1Click(TObject *Sender);
private:// User declarations
public:// User declarations
__fastcall TForm3(TComponent* Owner);
};
//--------------------------------------
extern PACKAGE TForm3 *Form3;
//--------------------------------------
#endif
В файле Help.cpp:
//--------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Help.h"
//--------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//--------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
: TForm(Owner)
{
}
//--------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
Close();
}
//--------------------------------------
В файле Help.h:
//--------------------------------------
#ifndef HelpH
#define HelpH
//--------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//--------------------------------------
class TForm2: public TForm
{
__published:// IDE-managed Components
TLabel *Label1;
TLabel *Label2;
TLabel *Label3;
TLabel *Label4;
TLabel *Label5;
TLabel *Label6;
TLabel *Label7;
TLabel *Label8;
TLabel *Label9;
TLabel *Label10;
TLabel *Label11;
TLabel *Label12;
TLabel *Label13;
TLabel *Label14;
TLabel *Label15;
TLabel *Label16;
TLabel *Label17;
TLabel *Label18;
TLabel *Label19;
TLabel *Label20;
TLabel *Label21;
TLabel *Label22;
TButton *Button1;
TLabel *Label23;
TLabel *Label24;
TLabel *Label25;
TLabel *Label26;
TLabel *Label27;
TLabel *Label28;
TLabel *Label29;
void __fastcall Button1Click(TObject *Sender);
private:// User declarations
public:// User declarations
__fastcall TForm2(TComponent* Owner);
};
//--------------------------------------
extern PACKAGE TForm2 *Form2;
//--------------------------------------
#endif
