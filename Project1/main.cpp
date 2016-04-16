#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

typedef struct point {
	int x, y, d;
}snakePart;

typedef struct turn {
	int x, y, d, a;
} turn;

typedef struct queue {
	turn s[50];
	int f, b;
} q;

snakePart snake[10];
int length = 0;
q turns;
int WINSIZE = 300;
int POINTSIZE = 10;
int spawnX, spawnY;

int inactive(q *c) {
	for (int i = 0; i < 20; i++) {
		if (c->s[i].a == 1)
			return 0;
	}
	return 1;
}

void push(q* c, int x, int y, int d) {
	if (inactive(c))
		c->b = c->f = 0;
	c->s[c->b].x = x;
	c->s[c->b].y = y;
	c->s[c->b].d = d;
	c->s[c->b].a = 1;
	c->b++;
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

int isEmpty(q *c){
	return c->b == 0 && c->f == 0;
}

void spawnPart() {
	spawnX = rand() % (WINSIZE - 2*POINTSIZE) + POINTSIZE;
	spawnY = rand() % (WINSIZE - 2 * POINTSIZE) + POINTSIZE;
}

int isTurn(int x, int y, int i) {
	if (isEmpty(&turns))
		return 0;
	else {
		for (int j = turns.f; j < turns.b; j++) {
			if (x >= turns.s[j].x - i && x <= turns.s[j].x + i 
				&& y >= turns.s[j].y - i && y <= turns.s[j].y + i && turns.s[j].a) {
				if (i == length - 1)
					turns.s[j].a = 0;
				return turns.s[j].d;
			}
		}
	}
	return 0;
}

void moveSnake() {
	if ((snake[0].x >= spawnX && snake[0].x < spawnX + POINTSIZE && snake[0].y >= spawnY && snake[0].y < spawnY + POINTSIZE)
		|| (spawnX >= snake[0].x && spawnX < snake[0].x + POINTSIZE && spawnY >= snake[0].y && spawnY < snake[0].y + POINTSIZE)) {
		spawnPart();
		switch (snake[length - 1].d) {
		case 1:
			snake[length].x = snake[length - 1].x;
			snake[length].y = snake[length - 1].y - POINTSIZE - 1;
			break;
		case 2:
			snake[length].x = snake[length - 1].x - POINTSIZE - 1;
			snake[length].y = snake[length - 1].y;
			break;
		case 3:
			snake[length].x = snake[length - 1].x;
			snake[length].y = snake[length - 1].y + POINTSIZE + 1;
			break;
		case 4:
			snake[length].x = snake[length - 1].x + POINTSIZE + 1;
			snake[length].y = snake[length - 1].y;
			break;
		}
		snake[length].d = snake[length - 1].d;
		length++;
		PlaySound(TEXT("eat.wav"), NULL, SND_SYNC);
		PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_LOOP);
	}
	for (int i = 0; i < length; i++) {
		int nd = isTurn(snake[i].x, snake[i].y, i);
		if (nd) {
			switch (snake[i].d) {
			case 1:
				snake[i].y += i;
				break;
			case 2:
				snake[i].x += i;
				break;
			case 3:
				snake[i].y -= i;
				break;
			case 4:
				snake[i].x -= i;
				break;
			}
			snake[i].d = nd;
			switch (snake[i].d) {
			case 1:
				snake[i].y -= i;
				break;
			case 2:
				snake[i].x -= i;
				break;
			case 3:
				snake[i].y += i;
				break;
			case 4:
				snake[i].x += i;
				break;
			}
		}
		switch (snake[i].d) {
		case 1:
			snake[i].y += POINTSIZE;
			break;
		case 2:
			snake[i].x += POINTSIZE;
			break;
		case 3:
			snake[i].y -= POINTSIZE;
			break;
		case 4:
			snake[i].x -= POINTSIZE;
			break;
		}
	}
	glutPostRedisplay();
}

int noWallCollisions() {
	return snake[0].x >= 2 * POINTSIZE && snake[0].x <= WINSIZE - 2 * POINTSIZE 
		&& snake[0].y >= 2 * POINTSIZE && snake[0].y <= WINSIZE - 2 * POINTSIZE;
}

int noSelfCollision() {
	for (int i = 0; i < length; i++) {
		for (int j = i + 1; j < length; j++) {
			if (snake[j].x >= snake[i].x && snake[j].x < snake[i].x + POINTSIZE/2 &&
				snake[j].y >= snake[i].y && snake[j].y < snake[i].y + POINTSIZE/2)
				return 0;
		}
	}
	return 1;
}

void displayWinMsg() {
	glColor3f(1.0, 0.7, 0.0);
	glRasterPos2f(WINSIZE / 4, WINSIZE / 2);
	
	char string[] = { 'C', 'O', 'M', 'P', 'L', 'E', 'T', 'E' , '\0' };
	int len = strlen(string);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
	glutSwapBuffers();
}

void displayOverMsg() {
	glColor3f(1.0, 0.7, 0.0);
	glRasterPos2f(WINSIZE / 4, WINSIZE / 2);
	
	char string[] = { 'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R', '\0'};
	int len = strlen(string);
	
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
	glutSwapBuffers();
}

void drawSnake() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
		glVertex2i(spawnX, spawnY);
	glEnd();
	for (int i = 0; i < length; i++) {
		glBegin(GL_POINTS);
		glVertex2i(snake[i].x, snake[i].y);
		glEnd();
	}
	glutSwapBuffers();
	Sleep(300);
	if (length >= (WINSIZE - 30) / POINTSIZE || length == 10) {
		PlaySound(TEXT("win.wav"), NULL, SND_ASYNC);
		displayWinMsg();
	}
	else if (noWallCollisions() && noSelfCollision())
		moveSnake();
	else {
		PlaySound(TEXT("die.wav"), NULL, SND_ASYNC);
		displayOverMsg();
	}
	
	
}

void startGame() {
	length = 2;
	turns.f = turns.b = 0;
	for (int i = 0; i < length; i++) {
		snake[i].x = WINSIZE/2 + POINTSIZE * i + i;
		snake[i].y = WINSIZE/2;
		snake[i].d = 4; //initially left direction
	}
	PlaySound(TEXT("move.wav"), NULL, SND_ASYNC | SND_LOOP);
	spawnPart();
	moveSnake();
}

void init(void) {
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(POINTSIZE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINSIZE, 0.0, WINSIZE);
	glClear(GL_COLOR_BUFFER_BIT);
	startGame();
	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case 'W':
	case 'w':
		if (snake[0].d != 3) {
			snake[0].d = 1;
			push(&turns, snake[0].x, snake[0].y, 1);
		}
		break;
	case 'A':
	case 'a':
		if (snake[0].d != 2) {
			snake[0].d = 4;
			push(&turns, snake[0].x, snake[0].y, 4);
		}
		break;
	case 'S':
	case 's':
		if (snake[0].d != 1) {
			snake[0].d = 3;
			push(&turns, snake[0].x, snake[0].y, 3);
		}
		break;
	case 'D':
	case 'd':
		if (snake[0].d != 4) {
			snake[0].d = 2;
			push(&turns, snake[0].x, snake[0].y, 2);
		}
		break;
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char *argv[]) {
		glutInit(&argc, argv);
		cout << "\t\t\t\t***SNAKES***" << endl;
		cout << "Controls:\nW: Up\nS: Down\nA: Left\nD: Right" << endl;
		cout << "Enter window side length: ";
		cin >> WINSIZE;
		cout << "Enter point size: ";
		cin >> POINTSIZE;
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(WINSIZE, WINSIZE);
		glutInitWindowPosition(100, 50);
		glutCreateWindow("SNAKES");
		glutDisplayFunc(drawSnake);
		glutKeyboardFunc(onKeyboard);
		init();
		glutMainLoop();
	return 0;
}