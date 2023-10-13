#include <iostream>
#include <Windows.h>
#include <GL/glut.h>
#include "READ_BMP.h"
#include <vector>
#include <string>

using namespace std;

struct point
{
    int x;
    int y;

    point(int ix, int iy) : x(ix), y(iy) {};
};

struct pattern
{
    point point1;
    point point2;
    point center;
    int width;
    int height;
    int color;
    int type;
    int id;
    GLfloat lineWidth;

    pattern(point p1, point p2, point ct, int w, int h, int cl, int t, int i, GLfloat lw) :
        point1(p1), point2(p2), center(ct), width(w), height(h), color(cl), type(t), id(i), lineWidth(lw) {};
};

struct button
{
    int x1;
    int y1;
    int x2;
    int y2;
    int w;
    int h;
    bool pressed;
    string text;
};

int mainWin, subWin1, subWin2, subWin3;

int currentColor = 0; // 0: 紅色, 1: 綠色, 2: 藍色
GLfloat colorValues[5][4] = { {1.0f, 0.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 0.0f} };
int currentType = 0; // 0: 線, 1: 圓, 2: 矩形
int mode = 0; //0: draw, 1: select
GLfloat currentLineWidth = 1.0f;

int startX, startY, endX, endY;
vector<pattern> patterns;
bool isDrawing = false;
int mouse_center_distance[5];
int drawPatternId;

BYTE* bmpImg;
int bmpWidth, bmpHeight;
GLuint textureID;

vector<button> buttons;
string buttonText[16] = {"Draw", "Line", "Circle", "Rectangle", "Select", "Clear", "Exit", "Red", "Green", "Blue", "Black", "LineWidth1", "LineWidth3", "LineWidth5", "Eraser"};
int pressingButtonId;
bool pressingButton;
int lastPressButtonId;

void setButtons()
{
    pressingButton = false;
    lastPressButtonId = 15;

    button b;
    int x, y;
    for (y = 100; y > 0; y -= 50)
        for (x = 0; x < 896; x += 128)
        {
            b.x1 = x + 16, b.y1 = y - 10, b.x2 = x + 112, b.y2 = y - 40, b.w = 96, b.h = 30, b.pressed = false;
            buttons.emplace_back(b);
        }
    y = 100;
    b.x1 = x + 16, b.y1 = y - 10, b.x2 = x + 112, b.y2 = y - 90, b.w = 96, b.h = 80;
    buttons.emplace_back(b);
    for (int i = 0; i < buttons.size(); ++i) buttons[i].text = buttonText[i];
}

void modeChange(int buttonId)
{
    if (buttonId > 0 && buttonId < 4)
        currentType = buttonId - 1;
    else if (buttonId > 6 && buttonId < 11)
        currentColor = buttonId - 7;
    else
    {
        switch (buttonId)
        {
            case 0:
                mode = 0;
                break;
            case 4:
                mode = 1;
                break;
            case 5:
                patterns.clear();
                startX = 0, startY = 0, endX = 0, endY = 0;
                glutSetWindow(subWin1);
                glutPostRedisplay();
                glutSetWindow(subWin3);
                break;
            case 6:
                exit(0);
                break;
            case 11:
                currentLineWidth = 1.0f;
                break;
            case 12:
                currentLineWidth = 3.0f;
                break;
            case 13:
                currentLineWidth = 5.0f;
                break;
            case 14:
                mode = 2;
                break;
        }
    }

    
}

void drawPatten(point p1, point p2, int color, int type, int line_width)
{
    glColor3f(colorValues[color][0], colorValues[color][1], colorValues[color][2]);
    glLineWidth(line_width);
    if (type == 0)
    {
        glBegin(GL_LINES);
        glVertex2i(p1.x, p1.y);
        glVertex2i(p2.x, p2.y);
        glEnd();
    }
    else if (type == 1)
    {
        glBegin(GL_LINE_LOOP);
        float r = sqrt(pow((p1.x - p2.x), 2) + pow(p1.y - p2.y, 2));
        for (int i = 0; i < 100; i++) 
        {
            float theta = 2.0f * 3.1415926f * float(i) / float(100);
            float x = r * cosf(theta);
            float y = r * sinf(theta);
            glVertex2f(x + p1.x, y + p1.y);
        }
        glEnd();
    }
    else
    {
        glBegin(GL_LINE_LOOP);
        glVertex2i(p1.x, p1.y);
        glVertex2i(p2.x, p1.y);
        glVertex2i(p2.x, p2.y);
        glVertex2i(p1.x, p2.y);
        glEnd();
    }
}

int searchPattern(int x, int y)
{
    float minDist = INT_MAX;
    int minId = -1;
    for (auto& p : patterns)
    {
        if (p.type == 0)
        {
            float dist = sqrt(pow(p.point1.x - p.point2.x, 2) + pow(p.point1.y - p.point2.y, 2));
            float dist1 = sqrt(pow(p.point1.x - x, 2) + pow(p.point1.y - y, 2));
            float dist2 = sqrt(pow(p.point2.x - x, 2) + pow(p.point2.y - y, 2));
            if (dist1 + dist2 < dist + 10 && dist1 + dist2 - dist < minDist)
            {
                minDist = dist1 + dist2 - dist;
                minId = p.id;
            }
        }
        else if (p.type == 1)
        {
            float dist = sqrt(pow(p.point1.x - p.point2.x, 2) + pow(p.point1.y - p.point2.y, 2));
            float distO = sqrt(pow(p.point1.x - x, 2) + pow(p.point1.y - y, 2));
            if (distO < dist + 10 && distO < minDist)
            {
                minDist = distO;
                minId = p.id;
            }
        }
        else
        {
            float distO = sqrt(pow(p.point1.x - x, 2) + pow(p.point1.y - y, 2));
            if (abs(p.point1.x - x) < p.width + 10 && abs(p.point1.y - y) < p.height + 10 && distO < minDist)
            {
                minDist = distO;
                minId = p.id;
            }
        }
    }
    return minId;
}

bool InitWin2()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glEnable(GL_TEXTURE_2D);

    const char* filename = ".\\img\\parrot.bmp";
    if (!BMP_Read(filename, &bmpImg, bmpWidth, bmpHeight)) return false;

    glGenTextures(1, &textureID);
    return true;
}


void display() 
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void display1()
{
    glClear(GL_COLOR_BUFFER_BIT);
    point ps(startX, startY), pe(endX, endY);
    if (mode == 0) drawPatten(ps, pe, currentColor, currentType, currentLineWidth);
    for (auto& p : patterns) drawPatten(p.point1, p.point2, p.color, p.type, p.lineWidth);
    glFlush();
}

void display2()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, bmpWidth, bmpHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bmpImg);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

void display3()
{
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < buttons.size(); ++i)
    {
        button b = buttons[i];
        glColor3f(0.2f, 0.2f, 0.2f); // 外框顏色
        glRectf(b.x1 - 1, b.y1 + 1, b.x2 + 3, b.y2 - 3); // 繪製外框 -1, +1, +3, -3

        switch (i) //Button Color
        {
            case 7:
                b.pressed == true ? glColor3f(0.2f, 0.2f, 0.2f) : glColor3f(1.0f, 0.0f, 0.0f);
                break;
            case 8:
                b.pressed == true ? glColor3f(0.2f, 0.2f, 0.2f) : glColor3f(0.0f, 1.0f, 0.0f);
                break;
            case 9:
                b.pressed == true ? glColor3f(0.2f, 0.2f, 0.2f) : glColor3f(0.0f, 0.0f, 1.0f);
                break;
            case 10:
                b.pressed == true ? glColor3f(0.2f, 0.2f, 0.2f) : glColor3f(0.0f, 0.0f, 0.0f);
                break;
            default:
                b.pressed == true ? glColor3f(0.2f, 0.2f, 0.2f) : glColor3f(0.5f, 0.5f, 0.5f);
        }
        glRectf(b.x1, b.y1, b.x2, b.y2); // 繪製Button

        (i == 9 || i == 10) ? glColor3f(1.0f, 1.0f, 1.0f) : glColor3f(0.0f, 0.0f, 0.0f); //Text Color
        int textWidth = 0;
        for (auto& t : b.text) textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, t);
        glRasterPos2i(b.x1 + 7 + (42 - textWidth / 2), b.y2 + (b.h / 2 - 5));
        for (int i = 0; i < b.text.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, b.text[i]);
    }

    if (lastPressButtonId < 15)
    {
        glLineWidth(1);
        glLineStipple(0.25, 0x00FF);
        glEnable(GL_LINE_STIPPLE);
        glBegin(GL_LINE_LOOP); // 或者使用 GL_LINE_STRIP
        (lastPressButtonId == 9 || lastPressButtonId == 10) ? glColor3f(1.0f, 1.0f, 1.0f) : glColor3f(0.0f, 0.0f, 0.0f); //虛線顏色
        int border = 3;
        button b = buttons[lastPressButtonId];
        glVertex2f(b.x1 + border, b.y1 - border);
        glVertex2f(b.x2 - border, b.y1 - border);
        glVertex2f(b.x2 - border, b.y2 + border);
        glVertex2f(b.x1 + border, b.y2 + border);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
}

void mouse1(int btn, int state, int x, int y) 
{
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        isDrawing = true;
        startX = x, startY = y;
        if (mode > 0)
        {
            drawPatternId = searchPattern(x, y);
            if (drawPatternId < 0) return;
            if (mode == 1)
            {
                mouse_center_distance[0] = patterns[drawPatternId].point1.x - startX;
                mouse_center_distance[1] = patterns[drawPatternId].point1.y - startY;
                mouse_center_distance[2] = patterns[drawPatternId].point2.x - startX;
                mouse_center_distance[3] = patterns[drawPatternId].point2.y - startY;
            }
            else
            {
                auto it = patterns.begin();
                for (int i = 0; i < drawPatternId; ++i) ++it;
                patterns.erase(it);
                for (int i = drawPatternId; i < patterns.size(); ++i) patterns[i].id = i;
                glutPostRedisplay();
            }   
        }
    }
    else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        endX = x > 512 ? 512 : x, endY = y < 0 ? 0 : y;
        if ((startX != endX || startY != endY) && mode == 0)
        {
            point p1(startX, startY), p2(endX, endY), center(0, 0);
            currentType == 0 ? center.x = (p1.x + p2.x) / 2, center.y = (p1.y + p2.y) / 2 : center.x = p1.x, center.y = p1.y;
            pattern p(p1, p2, center, abs(p1.x - p2.x), abs(p1.y - p2.y), currentColor, currentType, patterns.size(), currentLineWidth);
            patterns.emplace_back(p);
        }
        isDrawing = false;
        glutPostRedisplay();
    }
}

void mouse3(int btn, int state, int x, int y)
{
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        for (int i = 0; i < buttons.size(); ++i)
        {
            button b = buttons[i];
            if (x > b.x1 && x < b.x2 && y < b.y1 && y > b.y2)
            {
                pressingButton = true;
                buttons[i].pressed = true;
                pressingButtonId = i;
            }
        }
    }
    else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        //防呆
        if (!pressingButton) return;
        for (int i = 0; i < buttons.size(); ++i)
        {
            //檢查滑鼠是否移開按鈕
            button b = buttons[i];
            bool inRange = false;
            bool isPressing = false;
            if (x > b.x1 && x < b.x2 && y < b.y1 && y > b.y2) inRange = true;
            if (buttons[i].pressed) isPressing = true;
            if (inRange || isPressing)
            {
                if (inRange && isPressing) //若沒有移開
                {
                    modeChange(i);
                    lastPressButtonId = i;
                }
                pressingButton = false;
                break;
            }
        }
        buttons[pressingButtonId].pressed = false;
    }
    glutPostRedisplay();
}

void motion1(int x, int y)
{
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    if (isDrawing)
    {
        endX = x > 512 ? 512 : x, endY = y < 0 ? 0 : y;
        if (mode == 1 && drawPatternId >= 0)
        {
            patterns[drawPatternId].point1.x = endX + mouse_center_distance[0];
            patterns[drawPatternId].point1.y = endY + mouse_center_distance[1];
            patterns[drawPatternId].point2.x = endX + mouse_center_distance[2];
            patterns[drawPatternId].point2.y = endY + mouse_center_distance[3];
        }
        else if (mode == 2)
        {
            drawPatternId = searchPattern(x, y);
            if (drawPatternId < 0) return;
            auto it = patterns.begin();
            for (int i = 0; i < drawPatternId; ++i) ++it;
            patterns.erase(it);
            for (int i = drawPatternId; i < patterns.size(); ++i) patterns[i].id = i;
        }
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(1024, 612); // 設定視窗大小
    glutInitWindowPosition(100, 100); // 設定視窗位置
    mainWin = glutCreateWindow("OpenGL Drawing");
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    subWin1 = glutCreateSubWindow(mainWin, 0, 0, 512, 512);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutDisplayFunc(display1);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse1);
    glutMotionFunc(motion1);

    subWin2 = glutCreateSubWindow(mainWin, 512, 0, 512, 512);
    if (!InitWin2()) return 0;
    glutDisplayFunc(display2);
    glutReshapeFunc(reshape);


    setButtons();
    subWin3 = glutCreateSubWindow(mainWin, 0, 512, 1024, 100);
    glutDisplayFunc(display3);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse3);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    glutMainLoop();
    return 0;
}