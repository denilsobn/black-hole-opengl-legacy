#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define NUM_STARS 8000       
#define STAR_RADIUS 600.0f
#define PI 3.14159265359

#define BLACK_HOLE_RADIUS 15.0f
#define ISCO (BLACK_HOLE_RADIUS * 2.0f) // Innermost Stable Circular Orbit (ISCO)
#define DISK_OUTER_RADIUS 90.0f
#define NUM_PARTICLES 100000

int winW = 800;
int winH = 600;
GLuint bgTextureID;

struct Star {
    float x, y, z;
    float r, g, b;
};

struct Particula {
    float angle;
    float raio;
    float altura;
    float r, g, b;
};

Star stars[NUM_STARS];
Particula particulas[NUM_PARTICLES];

float camAnguloX = 0.0f;
float camAnguloY = 0.2f;
float camRaio = 150.0f;

float mouseX = -1, mouseY = -1;
bool mousePressed = false;

void drawDistorcion(float x, float y) {

    float x_curr = x * (float(winW) / (float)winH); 

    float r = sqrt(x_curr * x_curr + y * y);

    float u = x * 0.5f + 0.5f;
    float v = y * 0.5f + 0.5f;

    float eventHorizon = 0.1f;
    
    if (r > eventHorizon) {
        
        // massa do buraco negro em unidades arbitrárias
        float mass = 0.009f;
        float shift = mass / (r * r);
        u -= shift * (x / r);
        v -= shift * (y / r);

    }

    glTexCoord2f(u, v);
    glVertex2f(x, y);

}

void initStars() {
    srand((unsigned int)time(NULL));
    
    for (int i = 0; i < NUM_STARS; i++) {
        float u = (float)rand() / RAND_MAX;
        float v = (float)rand() / RAND_MAX;

        float theta = 2.0f * PI * u;               
        float phi = acos(2.0f * v - 1.0f);         

        float radius = STAR_RADIUS + ((float)rand() / RAND_MAX * 200.0f - 100.0f);

        stars[i].x = radius * sin(phi) * cos(theta);
        stars[i].y = radius * sin(phi) * sin(theta);
        stars[i].z = radius * cos(phi);

        float colorRand = (float)rand() / RAND_MAX;
        if (colorRand > 0.85f) {
            stars[i].r = 0.7f; stars[i].g = 0.8f; stars[i].b = 1.0f; // Estrelas Azuis
        } else if (colorRand > 0.70f) {
            stars[i].r = 1.0f; stars[i].g = 0.8f; stars[i].b = 0.6f; // Estrelas Amarelas/Laranjas
        } else {
            stars[i].r = 1.0f; stars[i].g = 1.0f; stars[i].b = 1.0f; // Estrelas Brancas
        }
        
        float intensity = 0.2f + 0.8f * ((float)rand() / RAND_MAX);
        stars[i].r *= intensity;
        stars[i].g *= intensity;
        stars[i].b *= intensity;
    }
}

void initParticulas() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        float t = (float)rand() / RAND_MAX;
        particulas[i].raio = ISCO +  t * (DISK_OUTER_RADIUS - ISCO);
        particulas[i].angle = ((float)rand() / RAND_MAX) * 2.0f * PI;

        float h = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        particulas[i].altura = h * h * h * 1.5f;

        particulas[i].r = 1.0f;
        particulas[i].g = 0.3f + ((float)rand() / RAND_MAX) * 0.5f;
        particulas[i].b = 0.1f * ((float)rand() / RAND_MAX);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glEnable(GL_DEPTH_TEST);            
    glDisable(GL_LIGHTING);
    
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenTextures(1, &bgTextureID);
    glBindTexture(GL_TEXTURE_2D, bgTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    initStars();
    initParticulas();
}


void drawAccretionDisk(bool drawBehind, float camX, float camY, float camZ) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glDepthMask(GL_FALSE); 
    glPointSize(2.0f);
    glBegin(GL_POINTS);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        float px = particulas[i].raio * cos(particulas[i].angle);
        float py = particulas[i].altura;
        float pz = particulas[i].raio * sin(particulas[i].angle);

        float dotPlane = (px * camX) + (py * camY) + (pz * camZ);
        bool isBehind = (dotPlane < 0.0f);

        if (isBehind != drawBehind) continue; 

        float vx = -sin(particulas[i].angle);
        float vz = cos(particulas[i].angle);
        
        float dirX = camX - px;
        float dirZ = camZ - pz;
        float dist = sqrt(dirX*dirX + dirZ*dirZ);
        dirX /= dist; dirZ /= dist; 

        float dotProduct = (vx * dirX) + (vz * dirZ);
        float doppler = 1.0f + (dotProduct * 0.85f); 
        
        float r = particulas[i].r * doppler;
        float g = particulas[i].g * doppler;
        float b = particulas[i].b * doppler;
        
        if (dotProduct > 0.4f) {
            b += (dotProduct - 0.4f) * 1.5f;
            g += (dotProduct - 0.4f) * 0.5f;
        }

        float alpha = 1.0f;
        if (particulas[i].raio > DISK_OUTER_RADIUS - 15.0f) {
            alpha = (DISK_OUTER_RADIUS - particulas[i].raio) / 15.0f;
        } else if (particulas[i].raio < ISCO + 3.0f) {
            alpha = (particulas[i].raio - ISCO) / 3.0f;
        }

        glColor4f(r, g, b, alpha * 0.9f); 
        glVertex3f(px, py, pz);
    }
    glEnd();
    glDepthMask(GL_TRUE); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)winW / (float)winH, 1.0f, 2000.0f); 
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camX = camRaio * cos(camAnguloY) * sin(camAnguloX);
    float camY = camRaio * sin(camAnguloY);
    float camZ = camRaio * cos(camAnguloY) * cos(camAnguloX);
    float upX = -sin(camAnguloY) * sin(camAnguloX);
    float upY = cos(camAnguloY);
    float upZ = -sin(camAnguloY) * cos(camAnguloX);

    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, upX, upY, upZ);

    glPointSize(1.5f); 
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        glColor3f(stars[i].r, stars[i].g, stars[i].b);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();

    drawAccretionDisk(true, camX, camY, camZ); 

    glBindTexture(GL_TEXTURE_2D, bgTextureID);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, winW, winH, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); 

    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glDisable(GL_DEPTH_TEST); 
    glEnable(GL_TEXTURE_2D);  
    glColor3f(1.0f, 1.0f, 1.0f); 

    const int GRID_RES = 60; 
    float square = 2.0f / GRID_RES;
    glBegin(GL_QUADS);
    for (int i = 0; i < GRID_RES; i++) {
        for (int j = 0; j < GRID_RES; j++) {
            float x = -1.0f + i * square;
            float y = -1.0f + j * square;
            drawDistorcion(x, y);
            drawDistorcion(x + square, y);
            drawDistorcion(x + square, y + square);
            drawDistorcion(x, y + square);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D); glEnable(GL_DEPTH_TEST); 
    glMatrixMode(GL_PROJECTION); glPopMatrix(); 
    glMatrixMode(GL_MODELVIEW); glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f); 
    glutSolidSphere(BLACK_HOLE_RADIUS, 50, 50);

    drawAccretionDisk(false, camX, camY, camZ); 

    glutSwapBuffers();
}

void redimensionamento(int w, int h) {
    if (h == 0) h = 1;

    winW = w;
    winH = h;

    glViewport(0, 0, w, h);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            mouseX = x;
            mouseY = y;
        } else {
            mousePressed = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        float deltaX = x - mouseX;
        float deltaY = y - mouseY;

        camAnguloX += deltaX * 0.005f;
        camAnguloY += deltaY * 0.005f;

        mouseX = x;
        mouseY = y;
    }
}

void timer(int value) {

    for (int i = 0; i < NUM_PARTICLES; i++) {

        float currentSpeed = 6.0f / sqrt(pow(particulas[i].raio, 3.0f));

        particulas[i].angle += currentSpeed;

        particulas[i].raio -= 0.08f;

        if (particulas[i].raio < ISCO) {

            particulas[i].raio = DISK_OUTER_RADIUS;
            particulas[i].angle = ((float)rand() / RAND_MAX) * 2.0f * PI;

            float h = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            particulas[i].altura = h * h * h * 1.5f;

        }

    }
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Buraco Negro");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(redimensionamento);

    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}