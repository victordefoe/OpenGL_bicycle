
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include<GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>
#include <string>
#include <fstream>
#include <time.h>

#include "bicycle.h"


using namespace std;



GLuint testDL;//������ʾ�б�Ϊ����ʾ��ʾ�б��ʹ�÷���
GLuint loadBMP_custom(const char * imagepath);//BMPͼƬ�ļ���ȡ��������ͼ��
GLuint skyBoxFront, skyBoxLeft, skyBoxRight, skyBoxBack, skyBoxTop, grassTexture;//�������õı���
int subMenu1, subMenu2, subMenu3;
float speed = 5.0;
bool lighting;

GLfloat my_set_material[4] = { 0.0, 0.0, 0.0, 0.0 };


//void SetupRC();
void myReshape(int, int); //���ڵ�һ�γ���ʱ�Լ��ı䴰�ڴ�Сʱ����
//void ChangeSize(int , int );
void normalKey(unsigned char, int, int);
void SpecialKeys(int, int, int);
void mouseMotion(int, int);


void setLight();//��Դ������
void moveLight();//��Դ�ĵ�����OpenGL�й�Դ��λ����Ҫ�������ƣ���������������
void readTexture();//��ȡ����ͼƬ
void drawSkyBox(GLuint);

void get_normal(utils_funcs::Face&); //���㷨�߹��ߺ���
vector<string> split(const string &, const string &);//�ָ��ַ������ߺ���
utils_funcs::myMesh * ReaderOBj(string); //��ȡģ���ļ����ߺ���



//static void testDisplayList();//��ʾ�б�
static void initDisplayList();//ע����ʾ�б�
void myDisplay(); //��Ҫ��ʾ���Ƶĺ�����renderscence��
void GLUTRedraw(); //��ʾ���г��Ӻ��� 

void timeFunc(int); //ˢ��
void mouseFunc(int, int, int, int);


struct bycicle;

int windowsWidth, windowsHeight;//���ڵĴ�С����
utils_funcs bike;
bool if_face=true, if_line=false, if_point=false;
utils_funcs::myMesh *mesh;


//ObjLoader bicycleObj;


struct Grass//���ɲݵ�
{
	GLfloat grass_vert[101][101][3];//�ݵض�������,��101*101���㣬ÿ����3��ֵ
	GLuint grass_list[100][100][6];//������������˳�򣬹�100*100�����Ӽ�20000�������Σ�ÿ��������3����
	GLfloat grass_norm[101][101][3];//���㷨������ͬ����
	GLfloat grass_coor[101][101][2];//������������101*101���㣬ÿ����������������
	const int hillNum = 50;//ɽ������
	const int meshNum = 100;//��������
	const int meshNum1 = 101;//��������
	int hill_coord[50][3];//ɽ�����ĵ�����
	float getHeight(float x, float y)//���õ�ɽ������Ӻ�ͨ���ú�����õ�ͼ������һ��ĵ��θ߶�
	{
		float hillHeight = 0;
		for (int v0 = 0; v0 < hillNum; v0++)
		{
			float temp = hill_coord[v0][2] - (x + 5000 - hill_coord[v0][0])*(x + 5000 - hill_coord[v0][0]) / 5000 - (y + 5000 - hill_coord[v0][1])*(y + 5000 - hill_coord[v0][1]) / 5000;
			hillHeight += (temp > 0 ? temp : 0);
		}
		return hillHeight - 500;
	}
	void genGrass()//���ɲݵ�
	{
		srand((unsigned)time(NULL));//���������
		for (int v = 0; v < hillNum; v++)
		{
			//�����ָ�Ϊɽ�����ĵ�����ɴ��룬ע����������ķ�Χ��0-10000����101������100�������ÿ�����Ϊ100��
			hill_coord[v][0] = rand() % 8000 + 1000;//��1000-9000�ķ�Χ������������ĵ�ĺ�����
			hill_coord[v][1] = rand() % 8000 + 1000;//��1000-9000�ķ�Χ������������ĵ��������
			hill_coord[v][2] = rand() % 200 + 100;//��100��300�ķ�Χ������������ĵ�߶�
		}
		for (int v = 0; v < meshNum1; v++)
			for (int v1 = 0; v1 < meshNum1; v1++)
			{
				float hillHeight = 0;
				for (int v2 = 0; v2 < hillNum; v2++)
				{
					//����ÿһ������ĸ߶ȣ�����ÿһ��ɽ�����Ӽ����������ڸõ��ֵ��С��0��ͳһΪ�㣬����������
					//z=c-(x^2/a+y^2/b)
					//ÿһ�������ڸõ�Ĺ���Ϊ���õ�ĸ߶� ��ȥ ��һ���㵽�õ��ˮƽ�����ƽ������һ���������������е������ó����������¶ȴ�С��
					float temp = hill_coord[v2][2] - (v * 100 - hill_coord[v2][0])*(v * 100 - hill_coord[v2][0]) / 5000 - (v1 * 100 - hill_coord[v2][1])*(v1 * 100 - hill_coord[v2][1]) / 5000;
					hillHeight += (temp > 0 ? temp : 0);
				}
				grass_vert[v][v1][0] = v * 100 - 5000;
				grass_vert[v][v1][1] = hillHeight - 500;
				grass_vert[v][v1][2] = v1 * 100 - 5000;
				//�����������ƶ���0,0,0
				grass_coor[v][v1][0] = v / 18.;
				grass_coor[v][v1][1] = v1 / 18.;
				//ÿʮ�˸����Ӧһ��ͼƬ��ͼ����䷽ʽΪ�����ظ����ο�drawScene�����е�����
				if (v1 < meshNum && v < meshNum)
				{
					//���ڵ�[v][v1]���㣬��Ҫ��[v][v1+1],[v+1][v1]���������⻹��Ҫ[v][v1+1],[v+1][v1]��[v+1][v1+1]��������Ϊ��20000��������
					grass_list[v][v1][0] = v * meshNum1 + v1;//[v][v1]
					grass_list[v][v1][1] = v * meshNum1 + v1 + 1;//[v][v1+1]
					grass_list[v][v1][2] = (v + 1) * meshNum1 + v1;//[v+1][v1]

					grass_list[v][v1][3] = (v + 1) * meshNum1 + v1;//[v+1][v1]
					grass_list[v][v1][4] = v * meshNum1 + v1 + 1;//[v][v+1]
					grass_list[v][v1][5] = (v + 1) * meshNum1 + v1 + 1;//[v+1][v1+1]
				}
			}
		for (int v = 1; v < meshNum; v++)
			for (int v1 = 1; v1 < meshNum; v1++)
			{
				//ÿһ����ķ���������������������ɵ���������������������ɵ�������˵õ�
				glm::vec3 vec1(grass_vert[v - 1][v1][0], grass_vert[v - 1][v1][1], grass_vert[v - 1][v1][2]);
				glm::vec3 vec2(grass_vert[v + 1][v1][0], grass_vert[v + 1][v1][1], grass_vert[v + 1][v1][2]);
				glm::vec3 vec3(grass_vert[v][v1 - 1][0], grass_vert[v][v1 - 1][1], grass_vert[v][v1 - 1][2]);
				glm::vec3 vec4(grass_vert[v][v1 + 1][0], grass_vert[v][v1 + 1][1], grass_vert[v][v1 + 1][2]);
				glm::vec3 temp = glm::normalize(glm::cross(vec4 - vec3, vec2 - vec1));//�Բ�˽����һ��
				grass_norm[v][v1][0] = temp.x;
				grass_norm[v][v1][1] = temp.y;
				grass_norm[v][v1][2] = temp.z;
			}
		for (int v = 0; v<meshNum1; v++)
			for (int v1 = 0; v1 < meshNum1; v1++)
			{
				if (v == 0 || v == meshNum1 || v1 == 0 || v1 == meshNum1)
				{
					//�Ա�Ե�ķ�����ȫ����ֵΪ0�����ʵ��Ч���б�Ե���ڲ�����ᷢ�ڡ�
					grass_norm[v][v1][0] = 0;
					grass_norm[v][v1][1] = 0;
					grass_norm[v][v1][2] = 0;
				}
			}
	}
};
Grass grass;





struct ViewControl {
	float yaw = 0;
	float pitch = 0;
	bool leftButton = false;
	int lastX = 0, lastY = 0;
};
ViewControl viewControl;

float translation[3] = { 0.0, 0.0, -80.0 };//�������г�ƽ�Ƶı���
static float rotation[3] = { 0.0, 140.0, 0.0 };




struct Camera {//����ṹ�壬������ʾ�����λ��
	glm::vec4 direction = glm::vec4(0, 0, -1, 0);
	glm::vec4 right = glm::vec4(1, 0, 0, 0);
	glm::vec4 up = glm::vec4(0, 1, 0, 0);
	glm::vec4 pos = glm::vec4(0, 0, 0, 0);
	//�ӽǱ仯,ͨ��yaw��pitch������������ֵ��
	void changeView(float yaw, float pitch)
	{
		glm::mat4 yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), glm::vec3(0, 1, 0));
		//������ת���󣬽������yawRotate��glm::rotate�����Ĳ�������ֱ�Ϊ��
		//һ��mat4��λ������ת�Ƕȣ�ע���ǻ���ֵ������Ҫ��radians����ת������
		//����ע��ֵ����������ת����Ĺ�ϵ����תʱ���Ƶ��ᣨע��������vec3���ͣ�
		//�����͵õ���һ����(0,1,0)Ϊ����תyaw�Ƕȵ���ת����
		direction = yawRotate * glm::vec4(0, 0, -1, 0);//��(0,1,0)��ת��Ӱ��direction��right��������
		right = yawRotate * glm::vec4(1, 0, 0, 0);
		glm::mat4 pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), glm::vec3(right));
		//�õ�һ����rightΪ�ᣨע��Ҫ��rightΪ�������(1,0,0)����pitchΪ��ת�Ƕȵ���ת����
		direction = pitchRotate * direction;//��right��ת��Ӱ��direction��up��������
		up = pitchRotate * glm::vec4(0, 1, 0, 0);
	}
};
Camera camera;

struct Move {
	bool wDown = false;
	bool aDown = false;
	bool sDown = false;
	bool dDown = false;

	bool iDown = false;
	bool jDown = false;
	bool kDown = false;
	bool lDown = false;

	void reset() {
		wDown = aDown = sDown = dDown = false;
		iDown = jDown = kDown = lDown = false;
		//jDown = true;
	}
	void set(unsigned char key) {
		switch (key)
		{
		case 'w':
			wDown = true;
			break;
		case 'a':
			aDown = true;
			break;
		case 's':
			sDown = true;
			break;
		case 'd':
			dDown = true;
			break;
		default:
			break;
		}
	}
	glm::vec4 moveVector(Camera &camera)//���ر����ƶ�������
	{
		if (!(wDown | aDown | sDown | dDown))//û�а�������
			return glm::vec4(0, 0, 0, 0);
		else
		{
			if (wDown)
				return camera.direction;//w����ʱ��������ĳ����ƶ�
			if (aDown)
				return -camera.right;//a���£����������ķ������ƶ�
			if (sDown)
				return -camera.direction;//s���£����������ķ������ƶ�
			if (dDown)
				return camera.right;//d���£����ҷ����ƶ�
		}
	}


};
Move moves;

void moveLight()
{
	glPushMatrix();
	//����LIGHT0
	GLfloat light0_position[] = { 0.0, 1.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//����LIGHT1
	GLfloat light1_position[] = { 0, 0, 0, 1 };
	GLfloat light1_direction[] = { 0, 0, 0 };
	light1_position[0] = camera.pos.x;//�������λ������LIGHT1λ��
	light1_position[1] = camera.pos.y;
	light1_position[2] = camera.pos.z;
	light1_direction[0] = camera.direction.x;//���������������LIGHT1����
	light1_direction[1] = camera.direction.y;
	light1_direction[2] = camera.direction.z;
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glPopMatrix();

}

void drawSkyBox(GLuint textureID)//ʹ�ö������������պ�
{
	GLfloat wall_vert[][3] = {//�������飬ÿ��������Ϊһ��
		-50000, -50000, -50000,
		50000, -50000, -50000,
		50000, 50000, -50000,
		-50000, 50000, -50000,
	};
	GLubyte index_list[][4] = {//�����б����ñ�˳����������ĸ��������
		0, 1, 2, 3,
	};
	GLfloat tex_coor[][2] = {//��Ӧÿ����������������
		0, 0,//��һ�����Ӧͼ�����Ͻ�
		1, 0,//�ڶ������Ӧͼ�����½�
		1, 1,//���������Ӧͼ�����½�
		0, 1,//���ĸ����Ӧͼ�����Ͻ�
		//���϶�Ӧ��ϵ����������Ҫ��ⲻͬϵͳ��ͼ������ı�ʾ����
	};
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, wall_vert);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coor);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, index_list);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void drawGrass(GLuint face)//��-�ڲݵصĻ���
{
	glBindTexture(GL_TEXTURE_2D, face);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, grass.grass_vert);
	glTexCoordPointer(2, GL_FLOAT, 0, grass.grass_coor);
	glNormalPointer(GL_FLOAT, 0, grass.grass_norm);
	glDrawElements(GL_TRIANGLES, 60000, GL_UNSIGNED_INT, grass.grass_list);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void drawScene()//��-��ʹ�ö������������պ�
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//�����������ԣ������һ��ΪGL_REPLACEʱ����ͼЧ�����ܻ�����Ӱ��
	//����Ĵ���ͨ�����ݲ�ͬ�����ͼidʵ�ֻ滭��ͬ�����գ�ע�ⲻͬϵͳ�¶�ȡͼƬ�ķ���
	//��һ����ͬ�����Ա�Ҫʱ��Ҫ�鿴ʵ�ʽ����ͼ�ν�����ת���ܵõ�����Ľ��
	glPushMatrix();
	glTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);//ʹ��պк���Ҿ���������ȣ����������ʵ��
	drawSkyBox(skyBoxFront);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxLeft);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxBack);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxRight);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);//ʹ��պк���Ҿ���������ȣ����������ʵ��
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	drawSkyBox(skyBoxTop);
	glPopMatrix();
	//enemy.draw(player);//����ģ��
	//bicycle.draw();
	GLUTRedraw();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//������ͼΪGL_MODULATE�����òݵ�����ӰЧ��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//������ͼ�ظ���ʽΪ�����ظ�
	drawGrass(grassTexture);//���Ʋݵ�
	//player.draw(0, 0);//���ģ��

}


void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);//ÿ�λ���ǰҪ�����ɫ������
	glClear(GL_DEPTH_BUFFER_BIT);//ÿ�λ���ǰҪ�����Ȼ�����

	if (lighting)
		setLight();//��Դ������
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}

	glLoadIdentity();//�����ӽ�ǰҪ��λ����
	camera.pos = camera.pos + moves.moveVector(camera) * 9.0f;//���㱾�����λ��,*3.0fΪ�ٶȣ�ע��vec4����ֻ����float����ֱ�����
	camera.pos.y = grass.getHeight(camera.pos.x, camera.pos.z) + 30.0f;
	moves.reset();//��λ����״̬
	gluLookAt(
		camera.pos.x, camera.pos.y, camera.pos.z,
		camera.pos.x + camera.direction.x, camera.pos.y + camera.direction.y, camera.pos.z + camera.direction.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	moveLight();//������Դ��λ�ã�����LIGHT0��LIGHT1
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	translation[1] = grass.getHeight(translation[0], translation[2]) + 30;
	//printf("%f %f %f \n", rotation[0], rotation[1], rotation[2]);
	//glRectf(-25.0F, 25.0F, 25.0F, -25.0F);

	//glCallList(testDL);//������ʾ�б����Ƴ���
	drawScene();
	glutSwapBuffers();//��Ϊ��˫���壬����Ҫ��swapbuffer��ʾ����
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHTING);
}


void FaceMenu_func(int value)
{
	switch (value)
	{
	case 1:
		if_face = true;
		if_line = false;
		if_point = false;
		break;

	case 2:
		if_face = false;
		if_line = true;
		if_point = false;
		break;

	case 3:
		if_face = false;
		if_line = false;
		if_point = true;
		break;

	default:
		break;
	}

	glutPostRedisplay();
}


void SpeedMenu_func(int value)
{
	switch (value)
	{
	case 1:
		speed = 10.0f;
		break;

	case 2:
		speed = 5.0f;
		break;

	case 3:
		speed = 1.0f;
		break;

	default:
		break;
	}

	glutPostRedisplay();
}

void ModeMenu_func(int value)
{
	switch (value)
	{
	case 1:
		lighting = false;
		break;

	case 2:
		lighting = true;
		break;


	default:
		break;
	}

	glutPostRedisplay();
}

void MenuFunc(int value)
{
	;
}

int main(int argc, char** argv)
{
	grass.genGrass();
	
	string model_path = "source//cycle.obj";
	
	mesh = ReaderOBj("source//cycle.obj");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//��ɫ��ʾ��˫����
	glutInitWindowPosition(200, 50);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Tutorial");


	// ���������Ҽ��Ӳ˵�
	subMenu1 = glutCreateMenu(FaceMenu_func);
	glutAddMenuEntry("���ͼ", 1);
	glutAddMenuEntry("��ͼ", 2);
	glutAddMenuEntry("��ͼ", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	subMenu2 = glutCreateMenu(SpeedMenu_func);
	glutAddMenuEntry("��", 1);
	glutAddMenuEntry("��", 2);
	glutAddMenuEntry("��", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	subMenu3 = glutCreateMenu(ModeMenu_func);
	//glutAddMenuEntry("����", 1);
	glutAddMenuEntry("��ҹ", 2);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	glutCreateMenu(MenuFunc);
	glutAddSubMenu("ͼ��ʽ", subMenu1);
	glutAddSubMenu("�ٶ�", subMenu2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);




	//initDisplayList();//��ʼ����ʾ�б�Ҫ�ڴ��촰��֮�����

	

	glEnable(GL_DEPTH_TEST);//������Ȳ���
	readTexture();//��ȡ����
	//���ܺ����İ�

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// ���ͼģʽ

	glutDisplayFunc(myDisplay);
	
	glutReshapeFunc(myReshape);
	
	glutKeyboardFunc(&normalKey);
	glutSpecialFunc(&SpecialKeys);
	
	glutMotionFunc(&mouseMotion);
	
	glutMouseFunc(&mouseFunc);
	
	//ÿ16�������һ�θú�������֤���60֡�Ķ���
	glutTimerFunc(16, &timeFunc, 1);
	

	glutMainLoop();//��ʼ��ʾ
	return 0;
}

void SpecialKeys(int key, int x, int y)
{
	
	if (key == GLUT_KEY_UP)
		translation[2] -= speed;

	if (key == GLUT_KEY_DOWN)
		translation[2] += speed;

	if (key == GLUT_KEY_LEFT)
		translation[0] -= speed;

	if (key == GLUT_KEY_RIGHT)
		translation[0] += speed;

	/*if (key > 356.0f)
		xRot = 0.0f;

	if (key < -1.0f)
		xRot = 355.0f;

	if (key > 356.0f)
		yRot = 0.0f;

	if (key < -1.0f)
		yRot = 355.0f;
*/
	// ���µ�����Ⱦ����
	glutPostRedisplay(); 
}




void normalKey(unsigned char key, int x, int y)
{
	moves.set(key);
	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y)//�ӽ��ƶ��������Ҽ����µ����
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:  
		if (state == GLUT_DOWN)//������������ʱ
		{
			viewControl.leftButton = true;
			viewControl.lastX = x;//��¼�������ʱ����λ��
			viewControl.lastY = y;
		}
		else
			viewControl.leftButton = false;//�Ҽ�̧��ʱ���ð���״̬Ϊ��
		break;
	default:
		break;
	}
}

void mouseMotion(int x, int y)//2-���ӽ��ƶ�����������ƶ�
{
	if (viewControl.leftButton)//ͨ��ÿ���ƶ��ľ���õ���ǰ��yawֵ��pitchֵ
	{
		viewControl.yaw += (x - viewControl.lastX)*0.3;//*0.7Ϊ����������
		viewControl.pitch += (y - viewControl.lastY)*0.3;
		if (viewControl.yaw > 180)//����Ƕȷ�Χ0-360����������´�����Ա�֤�ӽ�����仯
			viewControl.yaw = -180;
		else if (viewControl.yaw < -180)
			viewControl.yaw = 180;
		if (viewControl.pitch > 75)//����Χ-75��75����������������ֵ
			viewControl.pitch = 75;
		else if (viewControl.pitch < -75)
			viewControl.pitch = -75;
		viewControl.lastX = x;
		viewControl.lastY = y;
		//�õ��ǶȺ󣬼���camera��direction��right��up��ֵ�Ϳ��Եõ��µĳ���
		camera.changeView(viewControl.yaw, viewControl.pitch);
	}
}



void timeFunc(int value)
{
	glutPostRedisplay();//���������myDisplay����
	glutTimerFunc(16, timeFunc, 1);//�����һ������ʵ��ÿ��1000/16=60�λ���
}

void myReshape(int w, int h)
{
	windowsWidth = w;
	windowsHeight = h;
	//͸��ͶӰʵ�֣�һ��Ҫ���հ�����˳�򣬷������ʾ����ȷ��
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(95.0, (GLfloat)w / (GLfloat)h, 1, 100000.0);//�ĸ���������ֱ�Ϊ��Ұ�Ƕȣ���Ұ��ȣ���Ұ�߶ȣ��ܿ�����������룬�ܿ�������Զ����
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		camera.pos.x, camera.pos.y, camera.pos.z,//���λ��
		camera.pos.x + camera.direction.x, camera.pos.y + camera.direction.y, camera.pos.z + camera.direction.z,//����ĳ�������
		camera.up.x, camera.up.y, camera.up.z//����Ķ�����������
		);
}



void setLight()//��Դ������
{
	GLfloat Light_Model_Ambient[] = { 0.1, 0.1, 0.1, 1.0 }; //���������
	
	GLfloat mat_diffuse[] = { 1, 1, 1, 1.0 };  //���������
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };  //���淴�����

	GLfloat mat_shininess[] = { 0.0 };         //�߹�ָ��
	GLfloat light1_angle[] = { 15.0 };//LIGHT1��ɢ�Ƕ�
	GLfloat light_position[] = { 0.0, 1.0, -1.0, 0.0 };//LIGHT0λ��,���һ��ֵΪ0��Ϊƽ�й�
	GLfloat white_light[] = { 0.3, 0.3, 0.3, 1.0 };//LIGHT0����������ɫ(0.3,0.3,0.3), ����λΪ����
	GLfloat yellow_light[] = { 1, 0.87, 0.315, 1.0 };//LIGHT1����
	
	glClearColor(0.0, 0.0, 0.0, 0.0);  //����ɫ
	glShadeModel(GL_SMOOTH);           //��������ģʽ
	//��������
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//ƽ�й�LIGHT0����
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //ɢ�������
	glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);  //���淴���
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //��������� 
	//׶״��ԴLIGHT1����
	GLfloat lightKc = 1, lightKl = 0.0, lightKq = 0.0, lightExp = 2;//�йص��Դ��ɢǿ��˥���Ĳ�����
	glLightfv(GL_LIGHT1, GL_CONSTANT_ATTENUATION, &lightKc);
	glLightfv(GL_LIGHT1, GL_LINEAR_ATTENUATION, &lightKl);
	glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, &lightKq);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellow_light);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mat_specular);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, light1_angle);
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, &lightExp);
}




void readTexture()//��ȡ����
{
	
	//ע���ļ�·�����Կ�ִ���ļ���Ŀ¼��ʼ����д�Ͼ���·��
	skyBoxFront = loadBMP_custom("source//front.bmp");
	skyBoxBack = loadBMP_custom("source//back.bmp");
	skyBoxLeft = loadBMP_custom("source//left.bmp");
	skyBoxRight = loadBMP_custom("source//right.bmp");
	skyBoxTop = loadBMP_custom("source//top.bmp");
	grassTexture = loadBMP_custom("source//grass.bmp");

	
}


static void initDisplayList()
{
	testDL = glGenLists(1);//ΪtestDL��ʾ�б����ռ�
	glNewList(testDL, GL_COMPILE);//ΪtestDL�󶨺���
	//testDisplayList();//���ƹ����ڱ���������ɣ�ע�ⲻ�����䴫�ݱ���
	glEndList();//������
}



GLuint loadBMP_custom(const char * imagepath)  // ��ͼ����
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;
	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("Image could not be opened\n"); return 0; }
	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return false;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
	data = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);
	//Everything is in memory now, the file can be closed
	fclose(file);
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return textureID;
}





/////////�����г��ĺ�����

void draw_faces(utils_funcs::myMesh& mesh){

	for (int i = 0; i < mesh.face_number; i++) {
		//ע�����ǵĲ�������
		utils_funcs::Face& face = mesh.faces[i];
		glBegin(GL_POLYGON);              //���ƶ���μ���
		//�ڻ�����Ĺ��������������Ѿ�����õķ�������Ϣ
		glNormal3fv(face.normal);           //�ڻ������ʱ��ͬʱ���뷨������Ϣ
		for (int j = 0; j < face.vert_number; j++) {
			utils_funcs::Vertex *vert = face.verts[j];
			glVertex3f(vert->x, vert->y, vert->z);
		}
		glEnd();
	}
}

void draw_points(utils_funcs::myMesh& mesh){
	//������Ƶ����Ϣ
	//���ν������Ϣ����б���
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(2);
	glBegin(GL_POINTS);
	for (int j = 0; j < mesh.vert_number; j++) {
		glVertex3f(mesh.verts[j].x, mesh.verts[j].y, mesh.verts[j].z);
	}
	glEnd();

}

void draw_lines(utils_funcs::myMesh& mesh){
	double temp_x, temp_y, temp_z;
	for (int i = 0; i < mesh.face_number; i++) {
		utils_funcs::Face& face = mesh.faces[i];
		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		for (int j = 0; j < face.vert_number; j++) {
			utils_funcs::Vertex *vert = face.verts[j];
			if (j == 0){
				temp_x = vert->x;
				temp_y = vert->y;
				temp_z = vert->z;
				continue;
			}
			glVertex3f(temp_x, temp_y, temp_z);
			glVertex3f(vert->x, vert->y, vert->z);
			temp_x = vert->x;
			temp_y = vert->y;
			temp_z = vert->z;
		}
		glEnd();
	}
}



void GLUTRedraw(void)
{
	
	float scale;
	//bike.draw_control("aaa");
	//scale = bike.scale;
	scale = 1.0f;
	//���пռ���ػ�
	//glLoadIdentity();
	/*glScalef(scale, scale, scale);
	glTranslatef(translation[0], translation[1], 0.0); */
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, windowsHeight / windowsHeight, 0.1, 100.0);*/
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(translation[0], translation[1], translation[2]); //���� X, Y �� Z ���ƶ���
	glScalef(scale, scale, scale);
	////ˢ�·����Ĵ�С
	glRotatef(rotation[0], 1.0, 0.0, 0.0); //��x����ת
	glRotatef(rotation[1], 0.0, 1.0, 0.0);          //���Ʋ�ͬ�Ƕ� ��y����ת
	glRotatef(rotation[2], 0.0, 0.0, 1.0);  //��z����ת
	//glTranslatef(-center[0], -center[1], -center[2]);
	////�ı���ת����
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////���ù���
	////���벻ͬ��Դ��λ��
	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	////���������Ϣ
	////������Ե���������ɫ��ɢ����ɫ����
	GLfloat mat_ambient[] = { 0.329412, 0.223529, 0.027451, 1.000000 };
	GLfloat mat_diffuse[] = { 0.780392, 0.568627, 0.113725, 1.000000 };
	GLfloat mat_specular[] = { 0.992157, 0.941176, 0.807843, 1.000000 };
	GLfloat mat_shininess[] = { 27.897400 }; //����RGBA����ָ������ֵ��0��128��Χ��
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, my_set_material);
	// ���濪ʼ���Ʊ���

	

	if (if_face == true)
		draw_faces(*mesh);
	if (if_line == true)
		draw_lines(*mesh);
	if (if_point == true)
		draw_points(*mesh);
	glPopMatrix();
	//glutSwapBuffers();
}

utils_funcs::myMesh * ReaderOBj(string fname2) {
	string line;
	fstream f;
	f.open(fname2, ios::in);
	if (!f.is_open()) {
		cout << "�ļ��򿪳���" << endl;
	}
	vector<vector<GLfloat>> vset;
	vector<vector<GLint>> fset;
	int v_counter = 1;
	int f_counter = 1;
	while (!f.eof()) {
		getline(f, line);//�õ�obj�ļ���һ�У���Ϊһ���ַ���
		vector<string>parameters;
		string tailMark = " ";
		string ans = "";
		line = line.append(tailMark);
		if (line[0] != 'v'&&line[0] != 'f'){
			continue;
		}
		for (int i = 0; i < line.length(); i++) {
			char ch = line[i];
			if (ch != ' ') {
				ans += ch;
			}
			else {
				if (ans != ""){
					parameters.push_back(ans); //ȡ���ַ����е�Ԫ�أ��Կո��з�
					ans = "";
				}
			}
		}
		cout << endl;
		if (parameters[0] == "v") {   //����Ƕ���Ļ�
			vector<GLfloat>Point;
			v_counter++;
			//            cout <<atof( parameters[1].c_str()) << "--" << atof( parameters[2].c_str()) <<" --  " << atof( parameters[3].c_str());
			Point.push_back(atof(parameters[1].c_str()));
			Point.push_back(atof(parameters[2].c_str()));
			Point.push_back(atof(parameters[3].c_str()));
			vset.push_back(Point);
		}
		else if (parameters[0] == "f") {   //�������Ļ�����Ŷ��������
			vector<GLint>vIndexSets;          //��ʱ��ŵ�ļ���
			for (int i = 1; i < 4; i++) {
				string x = parameters[i];
				string ans = "";
				for (int j = 0; j < x.length(); j++) {   //������/��
					char ch = x[j];
					if (ch != '/') {
						ans += ch;
					}
					else {
						break;
					}
				}
				vector<string >res = split(ans, "/");
				int index = atof(res[0].c_str());
				index--;//��Ϊ����������obj�ļ����Ǵ�1��ʼ�ģ������Ǵ�ŵĶ���vector�Ǵ�0��ʼ�ģ����Ҫ��1
				vIndexSets.push_back(index);
			}
			fset.push_back(vIndexSets);
		}

	}
	f.close();

	utils_funcs::myMesh *meshs = new utils_funcs::myMesh();        //�������ǵ�mesh��
	//    cout << fset.size() << endl;
	//    cout << vset.size() << endl;
	//��ṹ�����ת��
	int vert_number = vset.size();
	int face_number = fset.size();
	meshs->verts = new utils_funcs::Vertex[vert_number + 1];
	assert(meshs->verts);
	//����mesh����Ϣ
	//----------------��������Ϣ --------------
	for (int i = 0; i < vset.size(); i++)
	{
		utils_funcs::Vertex& vert = meshs->verts[meshs->vert_number++];
		vert.x = vset[i][0];
		vert.y = vset[i][1];
		vert.z = vset[i][2];
	}
	//----------------��������Ϣ --------------
	meshs->faces = new utils_funcs::Face[face_number];
	assert(meshs->faces);
	for (int i = 0; i < fset.size(); i++)
	{
		utils_funcs::Face a;
		a.vert_number = fset[i].size();
		a.verts = new utils_funcs::Vertex*[fset[i].size()];
		for (int j = 0; j < fset[i].size(); j++) {
			a.verts[j] = &meshs->verts[fset[i][j]];
		}
		get_normal(a);
		meshs->faces[meshs->face_number++] = a;
		//���ȷ����һά����
	}
	return meshs;
}

vector<string> split(const string &str, const string &pattern)
{
	//�����ַ������и�
	//const char* convert to char*
	char * strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	vector<string> resultVec;
	char* tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL)
	{
		resultVec.push_back(string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}

	delete[] strc;

	return resultVec;
}


void get_normal(utils_funcs::Face& face){
	//�����淨�ߣ�
	//������ķ���
	//ͨ����ʽ���㣺
	face.normal[0] = face.normal[1] = face.normal[2] = 0;
	utils_funcs::Vertex *v1 = face.verts[face.vert_number - 1];
	for (int i = 0; i < face.vert_number; i++) {
		utils_funcs::Vertex *v2 = face.verts[i];
		//�½����еĵ�
		face.normal[0] += (v1->y - v2->y) * (v1->z + v2->z);
		face.normal[1] += (v1->z - v2->z) * (v1->x + v2->x);
		face.normal[2] += (v1->x - v2->x) * (v1->y + v2->y);
		//������ɲ�˵Ĺ���
		v1 = v2;
	}
	//�����һ������
	float squared_normal_length = 0.0;
	squared_normal_length += face.normal[0] * face.normal[0];
	squared_normal_length += face.normal[1] * face.normal[1];
	squared_normal_length += face.normal[2] * face.normal[2];
	float normal_length = sqrt(squared_normal_length);
	//�õ���һ������
	if (normal_length > 1.0E-6) {
		face.normal[0] /= normal_length;
		face.normal[1] /= normal_length;
		face.normal[2] /= normal_length;
	}
	//Ȼ����ɹ�һ������
}
