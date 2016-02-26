#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "Vector.h"

int vertexNum=0;
int faceNum=0;

char* readFlyFile(char *fileName, int* vNum, int* fNum, float** vHH, char** fHH){
    FILE* plyFile = fopen(fileName, "r");
    if(plyFile == NULL){
        std::cout<<"invalid file name"<<std::endl;
        exit(2);
    }
    if(fseek(plyFile, 0, SEEK_END))
    {
        std::cout<<"fseek error"<<std::endl;
        exit(3);
    }
    
    long flength = ftell(plyFile);
    if (flength == -1) {
        std::cout<<"ftell error"<<std::endl;
    }
    
    rewind(plyFile);
    std::cout<<flength<<std::endl;
    char *plyBuffer = new char[flength];
    long ret = fread(plyBuffer, sizeof(*plyBuffer), flength, plyFile);
    if (ret == flength) {
        std::cout<< "read success"<< std::endl;
    }else if (feof(plyFile)){
        std::cout<< "unexpected EOF"<< std::endl;
        exit(4);
    }if (ferror(plyFile)){
        std::cout<< "fread error"<< std::endl;
        exit(5);
    }
    
    char *headInfo = strtok(plyBuffer, "\n");
    int i=0;
    while (strcmp(headInfo, "end_header")) {
        ++i;
        headInfo = strtok(NULL, "\n");
        std::cout<<headInfo<<std::endl;
        if(i == 3){
            sscanf(headInfo, "element vertex %d", vNum);
        }
        if(i == 7) {
            sscanf(headInfo, "element face %d", fNum);
        }
    };
    char *binHead = headInfo+11;
    *vHH = (float *)binHead;
    *fHH = binHead + (*vNum)*3*4;
    return plyBuffer;
}

typedef struct _vertex{
    Vector3d vert;
} Vertex;


typedef struct _face{
    int v1;
    int v2;
    int v3;
    Vector3d normal;
} Face;

struct point {
    float x;
    float y;
    float z;
};

void setup_the_viewvol()
{
    struct point eye;
    struct point view;
    struct point up;
    
    glEnable(GL_DEPTH_TEST);
    
    /* specify size and shape of view volume */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,1.0,0.1,20.0);
    
    /* specify position for view volume */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    eye.x = 2.0; eye.y = 2.0; eye.z = 2.0;
    view.x = 0.0; view.y = 0.0; view.z = 0.0;
    up.x = 0.0; up.y = 1.0; up.z = 0.0;
    
    gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void update()
{
    usleep(10000);

    glRotatef(1.0,30.0,1.0,0.0);

    glutPostRedisplay();
}


void draw_stuff()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES,0, 3*faceNum);
    glutSwapBuffers();
}

void do_lights()
{
    /* white light */
    float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    float light0_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_specular[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
    float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};
    
    /* turn off scene default ambient */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);
    
    /* make specular correct for spots */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
    
    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void do_material()
{
    float mat_ambient[] = {0.0,0.0,0.0,1.0};
    float mat_diffuse[] = {0.9,0.9,0.1,1.0};
    float mat_specular[] = {1.0,1.0,1.0,1.0};
    float mat_shininess[] = {2.0};
    
    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

int mybuf =1;
void initOGL(int argc, char **argv, GLfloat *vertsBuff)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
    glutInitWindowSize(512,512);
    glutInitWindowPosition(100,50);
    glutCreateWindow("my_cool_cube");
    glewInit();
    setup_the_viewvol();
    do_lights();
    do_material();
    glBindBuffer(GL_ARRAY_BUFFER,mybuf);
    glBufferData(GL_ARRAY_BUFFER,3*3*faceNum*sizeof(GLfloat)*2,vertsBuff,GL_STATIC_DRAW);
    // When using VBOs, the final arg is a byte offset in buffer, not the address,
    // but gl<whatever>Pointer still expects an address type, hence the NULL.
    glVertexPointer(3,GL_FLOAT,3*sizeof(GLfloat),(GLvoid *)(0+0));
    glNormalPointer(GL_FLOAT,3*sizeof(GLfloat),(GLvoid *)(0+3*3*faceNum*sizeof(GLfloat)));
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    /* gray background */
    glClearColor(0.35,0.35,0.35,0.0);
}

void getout(unsigned char key, int x, int y)
{
    switch(key) {
        case 'q':
            glDeleteBuffers(1,(GLuint *)&mybuf); //delete buffers, 1 is how many buffers, mybuf is the buffer id array
            exit(1);
        default:
            break;
    }
}




int main(int argc, char **argv)
{
    
    if(argc < 2){
        std::cout<<"need file name"<< std::endl;
        exit(1);
    }
    

    float *vHead;
    char *fHead;
    char *buffer = readFlyFile(argv[1], &vertexNum, &faceNum, &vHead, &fHead);
    
    printf("%f\n", *(vHead));
    printf("%d\n", *fHead);
    
    std::cout<<vertexNum<<" "<<faceNum<<std::endl;
    
    Vertex *vertices = new Vertex[vertexNum];
    Face *faces = new Face[faceNum]; 
    float MAX = 0, aveX=0, aveY=0, aveZ=0;
    
    for (int i=0; i<vertexNum; ++i) {
        float x = *(vHead+3*i+0);
        float y = *(vHead+3*i+1);
        float z = *(vHead+3*i+2);
        MAX = max(max(max(x, y), z),MAX);
        aveX += x;
        aveY += y;
        aveZ += z;
    }
    aveX /= vertexNum;
    aveY /= vertexNum;
    aveZ /= vertexNum;
    
    for (int i=0; i<vertexNum; ++i) {
        vertices[i].vert.x = (*(vHead+3*i+0) - aveX) /(MAX) * 3;
        vertices[i].vert.y = (*(vHead+3*i+1) - aveY) /(MAX) * 3;
        vertices[i].vert.z = (*(vHead+3*i+2) - aveZ) /(MAX) * 3;
    }
    
    for (int i=0; i<faceNum; ++i) {
        int v1 = *((int *)(fHead+13*i+1));
        int v2 = *((int *)(fHead+13*i+5));
        int v3 = *((int *)(fHead+13*i+9));
        
        if (v1>vertexNum || v1<0) {
            std::cout<<"index error: "<<v1<<"i: "<<i<<std::endl;
            exit(9);
        }
        faces[i].v1 = v1;
        faces[i].v2 = v2;
        faces[i].v3 = v3;
        Vector3d v1v2 = vertices[v2].vert - vertices[v1].vert;
        Vector3d v1v3 = vertices[v3].vert - vertices[v1].vert;
        Vector3d normtmp = (v1v2 % v1v3).normalize();
        Vector3d center = (v1+v2+v3)/3.0;
        if (normtmp * center > 0) {
            faces[i].normal = normtmp;
        }
        else{
            faces[i].normal = -1.0 * normtmp;
        }
    }
    
	/*    
    for (int i=1; i<10; ++i) {
        std::cout<<vertices[i].vert<<"\n";
        std::cout<<faces[i].v1<<" "<<faces[i].v2<<" "<<faces[i].v3<<"\n";
        std::cout<<faces[i].normal<<std::endl;
    }
    */
    long bufSize = 3*3*faceNum*2; // 1face: 3vertices*3floats*4bytes
    long normBase = 3*3*faceNum;
    GLfloat* vertsBuff = new GLfloat[bufSize];
    std::cout<<bufSize<<"\n";
    for (int i=0; i<faceNum; ++i) {
        vertsBuff[i*9+0] = vertices[faces[i].v1].vert.x;
        vertsBuff[i*9+1] = vertices[faces[i].v1].vert.y;
        vertsBuff[i*9+2] = vertices[faces[i].v1].vert.z;
        vertsBuff[i*9+3] = vertices[faces[i].v2].vert.x;
        vertsBuff[i*9+4] = vertices[faces[i].v2].vert.y;
        vertsBuff[i*9+5] = vertices[faces[i].v2].vert.z;
        vertsBuff[i*9+6] = vertices[faces[i].v3].vert.x;
        vertsBuff[i*9+7] = vertices[faces[i].v3].vert.y;
        vertsBuff[i*9+8] = vertices[faces[i].v3].vert.z;
        vertsBuff[normBase + i*9+0] = faces[i].normal.x;
        vertsBuff[normBase + i*9+1] = faces[i].normal.y;
        vertsBuff[normBase + i*9+2] = faces[i].normal.z;
        vertsBuff[normBase + i*9+3] = faces[i].normal.x;
        vertsBuff[normBase + i*9+4] = faces[i].normal.y;
        vertsBuff[normBase + i*9+5] = faces[i].normal.z;
        vertsBuff[normBase + i*9+6] = faces[i].normal.x;
        vertsBuff[normBase + i*9+7] = faces[i].normal.y;
        vertsBuff[normBase + i*9+8] = faces[i].normal.z;
    }
    
    
    initOGL(argc,argv, vertsBuff);
    glutDisplayFunc(draw_stuff);
    glutIdleFunc(update);
    glutKeyboardFunc(getout);
    glutMainLoop();
    
    delete [] buffer;
    delete [] vertsBuff;
    delete [] faces;
    delete [] vertices;
    return 0;
    
}