#include "math.h"
#include "glos.h"
#include "gl.h"
#include "glu.h"
#include "glaux.h"
#include "glut.h"

GLuint texture;
static GLfloat pozSursa[] = { 60.0, 200.0, -50.0, 1.0 }; // 60.0 200.0 -50.0 1.0
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4],
    GLfloat mat[4][4]);
void calcCoeficientiPlan(float P[3][3], float coef[4]);

GLuint textureId2;
GLuint textureId3;

GLuint incarcaTextura(const char* s)
{
    GLuint textureId = 0;
    AUX_RGBImageRec* pImagineTextura = auxDIBImageLoad(s);

    if (pImagineTextura != NULL)
    {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, pImagineTextura->sizeX, pImagineTextura->sizeY,
            0, GL_RGB, GL_UNSIGNED_BYTE, pImagineTextura->data);
    }
    if (pImagineTextura)
    {
        if (pImagineTextura->data) {
            free(pImagineTextura->data);
        }
        free(pImagineTextura);
    }
    return textureId;
}

void calcCoeficientiPlan(float P[3][3], float coef[4])
{
    float v1[3], v2[3];
    float length;
    static const int x = 0;
    static const int y = 1;
    static const int z = 2;
    //calculeazã doi vectori din trei puncte
    v1[x] = P[0][x] - P[1][x];
    v1[y] = P[0][y] - P[1][y];
    v1[z] = P[0][z] - P[1][z];

    v2[x] = P[1][x] - P[2][x];
    v2[y] = P[1][y] - P[2][y];
    v2[z] = P[1][z] - P[2][z];

    //se cacluleazã produsul vectorial al celor doi vectori
    // care reprezintã un al treilea vector perpendicular pe plan 
    // ale cãrui componente sunt chiar coeficienþii A, B, C din ecuaþia planului
    coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
    coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
    coef[z] = v1[x] * v2[y] - v1[y] * v2[x];
    //normalizeazã vectorul
    length = (float)sqrt((coef[x] * coef[x]) + (coef[y] * coef[y]) + (coef[z] * coef[z]));
    coef[x] /= length;
    coef[y] /= length;
    coef[z] /= length;
}

//creazã matricea care dã umbra cunoscându-se coeficienþii planului ºi 
//poziþia sursei. În mat se salveazã matricea.
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4],
    GLfloat mat[4][4])
{
    GLfloat coefPlan[4];
    GLfloat temp;
    //determinã coeficienþii planului
    calcCoeficientiPlan(puncte, coefPlan);
    //determinã si pe D
    coefPlan[3] = -(
        (coefPlan[0] * puncte[2][0]) +
        (coefPlan[1] * puncte[2][1]) +
        (coefPlan[2] * puncte[2][2]));
    //temp=A*xL+B*yL+C*zL+D*w
    temp = coefPlan[0] * pozSursa[0] +
        coefPlan[1] * pozSursa[1] +
        coefPlan[2] * pozSursa[2] +
        coefPlan[3] * pozSursa[3];
    //prima coloanã
    mat[0][0] = temp - pozSursa[0] * coefPlan[0];
    mat[1][0] = 0.0f - pozSursa[0] * coefPlan[1];
    mat[2][0] = 0.0f - pozSursa[0] * coefPlan[2];
    mat[3][0] = 0.0f - pozSursa[0] * coefPlan[3];
    //a doua coloana
    mat[0][1] = 0.0f - pozSursa[1] * coefPlan[0];
    mat[1][1] = temp - pozSursa[1] * coefPlan[1];
    mat[2][1] = 0.0f - pozSursa[1] * coefPlan[2];
    mat[3][1] = 0.0f - pozSursa[1] * coefPlan[3];
    //a treia coloana
    mat[0][2] = 0.0f - pozSursa[2] * coefPlan[0];
    mat[1][2] = 0.0f - pozSursa[2] * coefPlan[1];
    mat[2][2] = temp - pozSursa[2] * coefPlan[2];
    mat[3][2] = 0.0f - pozSursa[2] * coefPlan[3];
    //a patra coloana
    mat[0][3] = 0.0f - pozSursa[3] * coefPlan[0];
    mat[1][3] = 0.0f - pozSursa[3] * coefPlan[1];
    mat[2][3] = 0.0f - pozSursa[3] * coefPlan[2];
    mat[3][3] = temp - pozSursa[3] * coefPlan[3];
}
void plan(void)
{
	glPushMatrix();

    glBegin(GL_QUADS);
    glVertex3f(500.0f, -10.0f, -500.0f);
    glVertex3f(-500.0f, -10.0f, -500.0f);
    glVertex3f(-500.0f, -10.0f, 500.0f);
    glVertex3f(500.0f, -10.0f, 500.0f);
    glEnd();
	glPopMatrix();
}

void myInit()
{
    glClearColor(0.0, 1.0, 0.0, 1.0);
    glShadeModel(GL_SMOOTH);
    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 128.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    //permite urmãrirea culorilor
    glEnable(GL_COLOR_MATERIAL);
    //seteazã proprietãþile de material pentru a urma valorile glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    //seteazã sursa		
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
    //seteazã materialul	    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glClearColor(0.7, 0.85, 0.95, 1.0);
    glEnable(GL_DEPTH_TEST);//ascunderea suprafeþelor
    textureId2 = incarcaTextura("bricks2.bmp");
	textureId3 = incarcaTextura("wood.bmp");

    glEnable(GL_TEXTURE_2D);
}

float angleX = 0.0f;
float angleZ = 0.0f;

void rotateX(float angle) {
    angleX += angle;
    if (angleX > 360.0f) {
        angleX -= 360.0f;
    }
}

void rotateZ(float angle) {
    angleZ += angle;
    if (angleZ > 360.0f) {
        angleZ -= 360.0f;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 's':
        rotateX(5.0f);
        break;
    case 'w':
        rotateX(-5.0f);
        break;
    case 'd':
        rotateZ(5.0f);
        break;
    case 'a':
        rotateZ(-5.0f);
        break;
    }
    glutPostRedisplay();
}

void fantana()
{
	glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureId2);
    // Draw a cylinder using GLUT
    GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
    gluCylinder(quadric, 0.6, 0.6, 1.0, 32, 32);
	glPopMatrix();

	glPushMatrix();
    // Disk
    GLUquadricObj* disk = gluNewQuadric();
    glColor3f(0.0, 0.0, 1.0);
    gluDisk(disk, 0.0, 0.6, 32, 1);
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureId2);
    glColor3f(0.7, 0.3, 0.25);
    glTranslatef(0.65, 0.0, 0.0);
    // Cylinder 2
    GLUquadricObj* quadric1 = gluNewQuadric();
	gluQuadricTexture(quadric1, GL_TRUE);
    gluCylinder(quadric1, 0.1, 0.1, 2.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureId2);
    glColor3f(0.7, 0.3, 0.25);
    glTranslatef(0.75, 0.0, 2.5);
    glRotatef(270.0f, 0.0, 1.0, 0.0);
    // Cylinder 3
    GLUquadricObj* quadric2 = gluNewQuadric();
	gluQuadricTexture(quadric2, GL_TRUE);
    gluCylinder(quadric2, 0.05, 0.05, 0.8, 32, 32);
	glPopMatrix();

	glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureId2);
    glColor3f(0.7, 0.3, 0.25);
    glTranslatef(0.0, 0.0, 1.3);
    glRotatef(90.0f, 0.0, 0.0, 1.0);
    // Bucket
    GLUquadricObj* quadric3 = gluNewQuadric();
	gluQuadricTexture(quadric3, GL_TRUE);
    gluCylinder(quadric3, 0.11, 0.17, 0.3, 64, 64);
	glPopMatrix();

	glPushMatrix();
    //glColor3f(0.7, 0.3, 0.25);
    glColor3f(0.7, 0.3, 0.25);
	glTranslatef(0.0, 0.0, 1.3);
    gluDisk(quadric3, 0.0, 0.11, 64, 1);
	glPopMatrix();

    // Define control points for a Bezier curve that describes the shape of the handle
    GLdouble ctrlpoints[4][3] = {
        {0.02, 0.0, 0.0},
        {0.0, 0.2, 0.0},
        {0.35, 0.3, 0.0},
        {0.38, 0.0, 0.0}
    };
	glPushMatrix();
    // Maner galeata
    glColor3f(0.7, 0.3, 0.25);
    glTranslatef(-0.2, 0.0, 1.6);
    glRotatef(90.0f, 1.0, 0.0, 0.0);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; ++i) {
        glEvalCoord1f((GLfloat)i / 30.0);
    }
    glEnd();
    glDisable(GL_MAP1_VERTEX_3);
	glPopMatrix();

    // Sfoara maner galeata
    GLdouble ctrlpoints2[4][3] = {
        {1.0, 0.0, 0.0},
        {1.2, 0.0, 0.0},
        {1.4, 0.0, 0.0},
        {1.7, 0.0, 0.0}
    };

	glPushMatrix();
    glColor3f(0.7, 0.3, 0.25);
    glTranslatef(0.0, 0.0, 3.5);
    glRotatef(90.0f, 0.0, 1.0, 0.0);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints2[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; ++i) {
        glEvalCoord1f((GLfloat)i / 10.0);
    }
    glEnd();
    glDisable(GL_MAP1_VERTEX_3);
	glPopMatrix();

    gluDeleteQuadric(quadric);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    GLfloat matUmbra[4][4];
//oricare trei puncte din plan în sens CCW
	GLfloat puncte[3][3] = {{-50.0f, -9.9f, -50.0f},
							{-50.0, -9.9f, 50.0},
							{50.0f, -9.9f, 50.0f}};
//determinã matricea pentru calcularea umbrei	
	MatriceUmbra(puncte, pozSursa, matUmbra);
//salveaza starea matricei de modelare-vizualizare si reprezintã cubul rotit
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);

    glLoadIdentity();
    glTranslatef(0.0, -1.3, -7.0);

    glColor3f(0.5, 0.5, 0.5);
	glRotatef(270.0f, 1.0, 0.0, 0.0); // Rotate around X-axis
    glRotatef(angleX, 1.0, 0.0, 0.0); // Rotate around X-axis
    glRotatef(angleZ, 0.0, 0.0, 1.0); // Rotate around Z-axis

	fantana();
	glPopMatrix();

    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    //apoi se înmulteste matricea curentã cu matricea de umbrire
    glMultMatrixf((GLfloat*)matUmbra);
    //si cu cea de rotaþie
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);
    //se deseneazã cubul-umbrã cu gri
    glColor3f(0.4, 0.4, 0.4);

	fantana();
    glPopMatrix(); //reface matricea de proiecþie la normal

    glPushMatrix();
    glTranslatef(pozSursa[0], pozSursa[1], pozSursa[2]);
    glColor3f(1.0, 1.0, 0.0);
    auxSolidSphere(5);
    glPopMatrix();

    //deseneazã planul
    glColor3f(0.0f, 1.0f, 0.3f);
    plan();

    //restaureazã starea variabilelor de iluminare
    glPopAttrib();

    glFlush();
}

void myReshape(int w, int h)
{
    if (h == 0) return;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 3.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    //glRotatef(-30, -1.0f, 1.0f, 1.0f); //transformarea de vizualizare
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("CUB");
    myInit();
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}