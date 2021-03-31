#include <iostream>
#include <GL/freeglut.h>
#include <windows.h>
#include <ctime>
#include <vector>

int n, camera[3] = {0, 0, 10};
double randLines_arr[16][2];
struct point {
    double x;
    double y;
};
std::vector <point> bluePoints;
GLdouble **cutArea;
double *cutAreaX;
double *cutAreaY;
bool blue = false;

double fRand(double fMin, double fMax) {
    double f = (double) rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void axis() {
    double axismatrix[6][3] = {{-1000, 0,     0},
                               {1000,  0,     0},
                               {0,     -1000, 0},
                               {0,     1000,  0},
                               {0,     0,     -1000},
                               {0,     0,     1000}};//матрица осей
    glBegin(GL_LINES);
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < 6; ++i) {
        if (i == 0) {
            r = 1;
        } else if (i == 2) {
            r = 0;
            g = 1;
        } else if (i == 4) {
            g = 0;
            b = 1;
        }
        glColor3f(r, g, b);
        glVertex3dv(axismatrix[i]);//связь образуется между двумя точками объявленными
        последовательно
    }
    glEnd();//закрываем бегин
}

void multiplyMatrix(double result[2][1], double A[2][2], double B[2][1])//функция для
перемножения матриц
        {
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 1; ++j) {
                        result[i][j] = 0;
                        for (int k = 0; k < 2; ++k) {
                            result[i][j] += A[i][k] * B[k][j];
                        }
                    }
                    10
                }
        }

void inversion(double E[2][2], double A[2][2]) //инверсия матриц
{
    double temp;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            E[i][j] = 0.0;
            if (i == j)
                E[i][j] = 1.0;
        }
    for (int k = 0; k < 2; k++) {
        temp = A[k][k];
        for (int j = 0; j < 2; j++) {
            A[k][j] /= temp;
            E[k][j] /= temp;
        }
        for (int i = k + 1; i < 2; i++) {
            temp = A[i][k];
            for (int j = 0; j < 2; j++) {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
    for (int k = 2 - 1; k > 0; k--) {
        for (int i = k - 1; i >= 0; i--) {
            temp = A[i][k];
            for (int j = 0; j < 2; j++) {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }
}

void cut() {
    glBegin(GL_LINES);
    glColor3f(0, 0, 0);
    //i < n-1
    for (int i = 0; i < n - 1; ++i) {
        glVertex2d(cutArea[i][0], cutArea[i][1]);
        glVertex2d(cutArea[i + 1][0], cutArea[i + 1][1]);
    }
    //n-1 //n-1
    glVertex2d(cutArea[n - 1][0], cutArea[n - 1][1]);
    glVertex2d(cutArea[0][0], cutArea[0][1]);
    glEnd();
}

void drawLines(double temp_arr[16][2]) {
    11
    glBegin(GL_LINES);
    for (int i = 0; i < 16; ++i) {
        glVertex2d(temp_arr[i][0], temp_arr[i][1]);
    }
    glEnd();
}

void drawIntersection(double x, double y) {
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glColor3f(0, 0, 1);
    glVertex2d(x, y);
    glEnd();
}//отрисовка точек пересечения
bool pnpoly(int npol, double *xp, double *yp, double x, double y) {
    bool c = false;
    for (int i = 0, j = npol - 1; i < npol; j = i++) {
        if ((((yp[i] <= y) && (y < yp[j])) || ((yp[j] <= y) && (y < yp[i]))) &&
            (((yp[j] - yp[i]) != 0) && (x > ((xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i])
                                             + xp[i]))))
            c = !c;
    }
    return c;
}//проверка на нахождение точки в n-угольнике
void specialKeyDown(int key, int x, int y);//для отдаления/приближения камеры
void lineEquation(double x1_a, double y1_a, double x2_a, double y2_a, double x1_b, double
y1_b, double x2_b,
                  double y2_b, double temp[2]) {
    double k_a, b_a;
    double k_b, b_b;
    double x, y;
    double linearSystem[2][2];
    double linearSystemB[2][1];
    k_a = (y1_a - y2_a) / (x1_a - x2_a);//коэффициент k линии пересекающей
    b_a = y2_a - k_a * x2_a;//коэффициент b линии пересекающей
    k_b = (y1_b - y2_b) / (x1_b - x2_b);//коэффициент k линии отрезной
    b_b = y2_b - k_b * x2_b;//коэффициент b линии отрезной
    if ((x1_b == x2_b) && (x1_a != x2_a) && (y1_a != y2_a)) {//условие вертикальности
        отрезной прямой
        x = x1_b;
        y = k_a * x + b_a;
        //drawIntersection(x,y);
    } else if ((y1_b == y2_b) && (x1_a != x2_a) && (y1_a != y2_a)) {//условие
        горизонтальности
                y = y1_b;
        x = (y - b_a) / k_a;
    } else {//здесь описана система линейных уравнений для определения точек пересечения
        linearSystem[0][0] = 1;
        linearSystem[0][1] = -k_a;
        linearSystemB[0][0] = b_a;
        linearSystem[1][0] = 1;
        linearSystem[1][1] = -k_b;
        linearSystemB[1][0] = b_b;
        double opposite[2][2];
        double XY[2][1];
        inversion(opposite, linearSystem);
        multiplyMatrix(XY, opposite, linearSystemB);
        x = XY[1][0];
        y = XY[0][0];
        12
        //drawIntersection(XY[1][0], XY[0][0]);
    }
    temp[0] = x;
    temp[1] = y;
}

bool
baseCheck(double x1_a, double y1_a, double x2_a, double y2_a, double x1_b, double y1_b,
          double x2_b,
          double y2_b) {
    if ((pnpoly(n, cutAreaX, cutAreaY, x1_a, y1_a) || pnpoly(n, cutAreaX, cutAreaY, x2_a,
                                                             y2_a)) &&
        (pnpoly(n, cutAreaX, cutAreaY, x1_a, y1_a) &&
         pnpoly(n, cutAreaX, cutAreaY, x2_a, y2_a))) { return true; }//проверка на то лежит
    ли
    прямая внутри
    else {
        double temp[2];
        lineEquation(x1_a, y1_a, x2_a, y2_a, x1_b, y1_b, x2_b, y2_b, temp);
        if (((temp[0] < x1_a) && (temp[0] < x2_a) || (temp[0] > x1_a) && (temp[0] > x2_a))
            ||
            ((temp[0] < x1_b) && (temp[0] < x2_b) || (temp[0] > x1_b) && (temp[0] >
                                                                          x2_b))) {
            return
                    false;
        }//проверка на нахождение точки пересечения на существующих линиях
        else if (((temp[1] < y1_a) && (temp[1] < y2_a) || (temp[1] > y1_a) && (temp[1] >
                                                                               y2_a)) ||
                 ((temp[1] < y1_b) && (temp[1] < y2_b) || (temp[1] > y1_b) && (temp[1] >
                                                                               y2_b))) {
            return false;
        }//проверка на нахождение точки пересечения на существующих линиях
        else {
            point t;
            t.x = temp[0];
            t.y = temp[1];
            bluePoints.push_back(t);
            //drawIntersection(temp[0], temp[1]);//отрисовка точек пересечения
            return true;
        }
    }
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 0, 1, 0);
    axis();//отрисовка осей
    glColor3f(0, 0, 0);
    drawLines(randLines_arr);//отрисовка случайных линий
    cut();//отрисовка обрезающей фигуры
    GLdouble drawBlue[16][2];
    for (int i = 0; i < 16; i += 2) {
        for (int j = 0; j < n - 1; ++j) {
            if (baseCheck(randLines_arr[i][0], randLines_arr[i][1], randLines_arr[i + 1][0],
                          randLines_arr[i + 1][1],
                          cutArea[j][0], cutArea[j][1], cutArea[j + 1][0], cutArea[j +
                                                                                   1][1])) {
                drawBlue[i][0] = randLines_arr[i][0];
                drawBlue[i][1] = randLines_arr[i][1];
                drawBlue[i + 1][0] = randLines_arr[i + 1][0];
                drawBlue[i + 1][1] = randLines_arr[i + 1][1];
            }
        }
        if (baseCheck(randLines_arr[i][0], randLines_arr[i][1], randLines_arr[i + 1][0],
                      randLines_arr[i + 1][1],
                      cutArea[n - 1][0], cutArea[n - 1][1], cutArea[0][0], cutArea[0][1])) {
            drawBlue[i][0] = randLines_arr[i][0];
            drawBlue[i][1] = randLines_arr[i][1];
            drawBlue[i + 1][0] = randLines_arr[i + 1][0];
            drawBlue[i + 1][1] = randLines_arr[i + 1][1];
        }
    }
    if (blue) {
        glColor3f(0, 0, 1);
        drawLines(drawBlue);
        for (int i = 0; i < bluePoints.size(); i++)
            drawIntersection(bluePoints[i].x, bluePoints[i].y);
    }
    glutSpecialFunc(specialKeyDown);
    glutSwapBuffers();
}

void View()//функция для создания окна, вызова отрисовки,
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);//размер окна
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Lab.4-1");//открыть окно с заголовком
    glutDisplayFunc(draw);
    glutIdleFunc(draw);
    glutSpecialFunc(specialKeyDown);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(25.0, float(640) / float(480), 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glutMainLoop();
}

void specialKeyDown(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_F1:
            blue = true;
            break;
        case GLUT_KEY_F2:
            camera[2] -= 1;
            break;
        case GLUT_KEY_F3:
            camera[2] += 1;
            break;
        default:
            break;
    }
}

int main(int argc, char **argv) {
    srand(time(nullptr));
    SetConsoleOutputCP(CP_UTF8);
    glutInit(&argc, argv);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 2; j++) {
            randLines_arr[i][j] = fRand(0, 2);
        }
    }
    std::cout << "введите количество точек: ";
    std::cin >> n;
    cutArea = new GLdouble *[n];
    for (int i = 0; i < n; ++i)
        cutArea[i] = new GLdouble[2];
    14
    cutAreaX = new double[n];
    cutAreaY = new double[n];
    for (int i = 0; i < n; ++i) {
        std::cout << "x = ";
        std::cin >> cutArea[i][0];
        cutAreaX[i] = cutArea[i][0];
        std::cout << "y = ";
        std::cin >> cutArea[i][1];
        cutAreaY[i] = cutArea[i][1];
    }
    std::cout << "F1 выделить цветом и показать точки отрезания, F2 чтобы отдалить камеру ,
    F3
            чтобы
    приблизить
            камеру
    ";
    View();
    return 0;
}