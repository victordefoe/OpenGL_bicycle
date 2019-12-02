
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



GLuint testDL;//测试显示列表，为了演示显示列表的使用方法
GLuint loadBMP_custom(const char * imagepath);//BMP图片文件读取函数，贴图用
GLuint skyBoxFront, skyBoxLeft, skyBoxRight, skyBoxBack, skyBoxTop, grassTexture;//绑定纹理用的变量
int subMenu1, subMenu2, subMenu3;
float speed = 5.0;
bool lighting;

GLfloat my_set_material[4] = { 0.0, 0.0, 0.0, 0.0 };


//void SetupRC();
void myReshape(int, int); //窗口第一次出现时以及改变窗口大小时调用
//void ChangeSize(int , int );
void normalKey(unsigned char, int, int);
void SpecialKeys(int, int, int);
void mouseMotion(int, int);


void setLight();//光源的设置
void moveLight();//光源的调整，OpenGL中光源的位置需要单独控制，否则会出现意外结果
void readTexture();//读取纹理图片
void drawSkyBox(GLuint);

void get_normal(utils_funcs::Face&); //计算法线工具函数
vector<string> split(const string &, const string &);//分割字符串工具函数
utils_funcs::myMesh * ReaderOBj(string); //读取模型文件工具函数



//static void testDisplayList();//显示列表
static void initDisplayList();//注册显示列表
void myDisplay(); //主要显示绘制的函数（renderscence）
void GLUTRedraw(); //显示自行车子函数 

void timeFunc(int); //刷新
void mouseFunc(int, int, int, int);


struct bycicle;

int windowsWidth, windowsHeight;//窗口的大小参数
utils_funcs bike;
bool if_face=true, if_line=false, if_point=false;
utils_funcs::myMesh *mesh;


//ObjLoader bicycleObj;


struct Grass//生成草地
{
	GLfloat grass_vert[101][101][3];//草地顶点数组,共101*101个点，每个点3个值
	GLuint grass_list[100][100][6];//顶点数组连接顺序，共100*100个格子即20000个三角形，每个三角形3个点
	GLfloat grass_norm[101][101][3];//顶点法线数组同顶点
	GLfloat grass_coor[101][101][2];//顶点纹理坐标101*101个点，每个点两个纹理坐标
	const int hillNum = 50;//山丘数量
	const int meshNum = 100;//格子数量
	const int meshNum1 = 101;//顶点数量
	int hill_coord[50][3];//山丘中心点坐标
	float getHeight(float x, float y)//当得到山丘的种子后，通过该函数获得地图上任意一点的地形高度
	{
		float hillHeight = 0;
		for (int v0 = 0; v0 < hillNum; v0++)
		{
			float temp = hill_coord[v0][2] - (x + 5000 - hill_coord[v0][0])*(x + 5000 - hill_coord[v0][0]) / 5000 - (y + 5000 - hill_coord[v0][1])*(y + 5000 - hill_coord[v0][1]) / 5000;
			hillHeight += (temp > 0 ? temp : 0);
		}
		return hillHeight - 500;
	}
	void genGrass()//生成草地
	{
		srand((unsigned)time(NULL));//随机数种子
		for (int v = 0; v < hillNum; v++)
		{
			//下面的指令即为山丘中心点的生成代码，注意我们网格的范围是0-10000（即101个点有100个间隔，每个间隔为100）
			hill_coord[v][0] = rand() % 8000 + 1000;//在1000-9000的范围内随机生成中心点的横坐标
			hill_coord[v][1] = rand() % 8000 + 1000;//在1000-9000的范围内随机生成中心点的纵坐标
			hill_coord[v][2] = rand() % 200 + 100;//在100到300的范围内随机生成中心点高度
		}
		for (int v = 0; v < meshNum1; v++)
			for (int v1 = 0; v1 < meshNum1; v1++)
			{
				float hillHeight = 0;
				for (int v2 = 0; v2 < hillNum; v2++)
				{
					//计算每一个顶点的高度，即对每一个山丘种子计算抛物面在该点的值，小于0则统一为零，最后叠加起来
					//z=c-(x^2/a+y^2/b)
					//每一个种子在该点的贡献为，该点的高度 减去 任一顶点到该点的水平距离的平方除以一个常数（可以自行调整，该常数决定了坡度大小）
					float temp = hill_coord[v2][2] - (v * 100 - hill_coord[v2][0])*(v * 100 - hill_coord[v2][0]) / 5000 - (v1 * 100 - hill_coord[v2][1])*(v1 * 100 - hill_coord[v2][1]) / 5000;
					hillHeight += (temp > 0 ? temp : 0);
				}
				grass_vert[v][v1][0] = v * 100 - 5000;
				grass_vert[v][v1][1] = hillHeight - 500;
				grass_vert[v][v1][2] = v1 * 100 - 5000;
				//将网格中心移动到0,0,0
				grass_coor[v][v1][0] = v / 18.;
				grass_coor[v][v1][1] = v1 / 18.;
				//每十八个格对应一张图片，图像填充方式为镜像重复，参考drawScene函数中的设置
				if (v1 < meshNum && v < meshNum)
				{
					//对于第[v][v1]个点，需要和[v][v1+1],[v+1][v1]相连，此外还需要[v][v1+1],[v+1][v1]与[v+1][v1+1]相连，因为有20000个三角形
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
				//每一个点的法向量由它上下两个点组成的向量和它左右两个点组成的向量叉乘得到
				glm::vec3 vec1(grass_vert[v - 1][v1][0], grass_vert[v - 1][v1][1], grass_vert[v - 1][v1][2]);
				glm::vec3 vec2(grass_vert[v + 1][v1][0], grass_vert[v + 1][v1][1], grass_vert[v + 1][v1][2]);
				glm::vec3 vec3(grass_vert[v][v1 - 1][0], grass_vert[v][v1 - 1][1], grass_vert[v][v1 - 1][2]);
				glm::vec3 vec4(grass_vert[v][v1 + 1][0], grass_vert[v][v1 + 1][1], grass_vert[v][v1 + 1][2]);
				glm::vec3 temp = glm::normalize(glm::cross(vec4 - vec3, vec2 - vec1));//对叉乘结果归一化
				grass_norm[v][v1][0] = temp.x;
				grass_norm[v][v1][1] = temp.y;
				grass_norm[v][v1][2] = temp.z;
			}
		for (int v = 0; v<meshNum1; v++)
			for (int v1 = 0; v1 < meshNum1; v1++)
			{
				if (v == 0 || v == meshNum1 || v1 == 0 || v1 == meshNum1)
				{
					//对边缘的法向量全部赋值为0，因此实际效果中边缘由于不发光会发黑。
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

float translation[3] = { 0.0, 0.0, -80.0 };//控制自行车平移的变量
static float rotation[3] = { 0.0, 140.0, 0.0 };




struct Camera {//相机结构体，用来表示相机的位置
	glm::vec4 direction = glm::vec4(0, 0, -1, 0);
	glm::vec4 right = glm::vec4(1, 0, 0, 0);
	glm::vec4 up = glm::vec4(0, 1, 0, 0);
	glm::vec4 pos = glm::vec4(0, 0, 0, 0);
	//视角变化,通过yaw，pitch算出各个朝向的值。
	void changeView(float yaw, float pitch)
	{
		glm::mat4 yawRotate = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), glm::vec3(0, 1, 0));
		//构造旋转矩阵，结果传给yawRotate，glm::rotate函数的参数含义分别为：
		//一个mat4单位矩阵，旋转角度（注意是弧度值，所以要用radians函数转换，）
		//另外注意值的正负与旋转方向的关系，旋转时所绕的轴（注意这里是vec3类型）
		//这样就得到了一个以(0,1,0)为轴旋转yaw角度的旋转矩阵
		direction = yawRotate * glm::vec4(0, 0, -1, 0);//绕(0,1,0)旋转会影响direction和right两个方向
		right = yawRotate * glm::vec4(1, 0, 0, 0);
		glm::mat4 pitchRotate = glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), glm::vec3(right));
		//得到一个以right为轴（注意要以right为轴而不是(1,0,0)），pitch为旋转角度的旋转矩阵
		direction = pitchRotate * direction;//绕right旋转会影响direction和up两个方向
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
	glm::vec4 moveVector(Camera &camera)//返回本次移动的向量
	{
		if (!(wDown | aDown | sDown | dDown))//没有按键按下
			return glm::vec4(0, 0, 0, 0);
		else
		{
			if (wDown)
				return camera.direction;//w按下时，向相机的朝向移动
			if (aDown)
				return -camera.right;//a按下，向相机右向的反方向移动
			if (sDown)
				return -camera.direction;//s按下，向相机朝向的反方向移动
			if (dDown)
				return camera.right;//d按下，向右方向移动
		}
	}


};
Move moves;

void moveLight()
{
	glPushMatrix();
	//设置LIGHT0
	GLfloat light0_position[] = { 0.0, 1.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//设置LIGHT1
	GLfloat light1_position[] = { 0, 0, 0, 1 };
	GLfloat light1_direction[] = { 0, 0, 0 };
	light1_position[0] = camera.pos.x;//根据相机位置设置LIGHT1位置
	light1_position[1] = camera.pos.y;
	light1_position[2] = camera.pos.z;
	light1_direction[0] = camera.direction.x;//根据相机朝向设置LIGHT1朝向
	light1_direction[1] = camera.direction.y;
	light1_direction[2] = camera.direction.z;
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glPopMatrix();

}

void drawSkyBox(GLuint textureID)//使用顶点数组绘制天空盒
{
	GLfloat wall_vert[][3] = {//坐标数组，每三个数据为一组
		-50000, -50000, -50000,
		50000, -50000, -50000,
		50000, 50000, -50000,
		-50000, 50000, -50000,
	};
	GLubyte index_list[][4] = {//顶点列表，按该表顺序连接上面的各个坐标点
		0, 1, 2, 3,
	};
	GLfloat tex_coor[][2] = {//对应每个坐标点的纹理坐标
		0, 0,//第一个点对应图像左上角
		1, 0,//第二个点对应图像左下角
		1, 1,//第三个点对应图像右下角
		0, 1,//第四个点对应图像右上角
		//以上对应关系可能有误，需要理解不同系统下图像坐标的表示方法
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

void drawGrass(GLuint face)//三-②草地的绘制
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

void drawScene()//三-②使用顶点数组绘制天空盒
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//设置纹理属性，当最后一项为GL_REPLACE时，贴图效果不受环境光影响
	//下面的代码通过传递不同面的贴图id实现绘画不同面的天空，注意不同系统下读取图片的方向
	//不一定相同，所以必要时需要查看实际结果对图形进行旋转才能得到合理的结果
	glPushMatrix();
	glTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);//使天空盒和玩家距离适中相等，可以提高真实感
	drawSkyBox(skyBoxFront);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxLeft);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxBack);
	glRotatef(90, 0, 1, 0);
	drawSkyBox(skyBoxRight);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);//使天空盒和玩家距离适中相等，可以提高真实感
	glRotatef(90, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	drawSkyBox(skyBoxTop);
	glPopMatrix();
	//enemy.draw(player);//敌人模型
	//bicycle.draw();
	GLUTRedraw();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//设置贴图为GL_MODULATE可以让草地有阴影效果
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//设置贴图重复方式为镜像重复
	drawGrass(grassTexture);//绘制草地
	//player.draw(0, 0);//玩家模型

}


void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);//每次绘制前要清空颜色缓冲区
	glClear(GL_DEPTH_BUFFER_BIT);//每次绘制前要清空深度缓冲区

	if (lighting)
		setLight();//光源的设置
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}

	glLoadIdentity();//设置视角前要置位矩阵
	camera.pos = camera.pos + moves.moveVector(camera) * 9.0f;//计算本次相机位置,*3.0f为速度，注意vec4类型只能与float类型直接相乘
	camera.pos.y = grass.getHeight(camera.pos.x, camera.pos.z) + 30.0f;
	moves.reset();//复位按键状态
	gluLookAt(
		camera.pos.x, camera.pos.y, camera.pos.z,
		camera.pos.x + camera.direction.x, camera.pos.y + camera.direction.y, camera.pos.z + camera.direction.z,
		camera.up.x, camera.up.y, camera.up.z
		);
	moveLight();//调整光源的位置，包括LIGHT0与LIGHT1
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	translation[1] = grass.getHeight(translation[0], translation[2]) + 30;
	//printf("%f %f %f \n", rotation[0], rotation[1], rotation[2]);
	//glRectf(-25.0F, 25.0F, 25.0F, -25.0F);

	//glCallList(testDL);//调用显示列表，绘制场景
	drawScene();
	glutSwapBuffers();//因为是双缓冲，所以要用swapbuffer显示动画
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//彩色显示，双缓冲
	glutInitWindowPosition(200, 50);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Tutorial");


	// 建立两个右键子菜单
	subMenu1 = glutCreateMenu(FaceMenu_func);
	glutAddMenuEntry("填充图", 1);
	glutAddMenuEntry("线图", 2);
	glutAddMenuEntry("点图", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	subMenu2 = glutCreateMenu(SpeedMenu_func);
	glutAddMenuEntry("快", 1);
	glutAddMenuEntry("中", 2);
	glutAddMenuEntry("慢", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	subMenu3 = glutCreateMenu(ModeMenu_func);
	//glutAddMenuEntry("白天", 1);
	glutAddMenuEntry("黑夜", 2);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	glutCreateMenu(MenuFunc);
	glutAddSubMenu("图样式", subMenu1);
	glutAddSubMenu("速度", subMenu2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);




	//initDisplayList();//初始化显示列表要在创造窗口之后调用

	

	glEnable(GL_DEPTH_TEST);//开启深度测试
	readTexture();//读取纹理
	//功能函数的绑定

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// 填充图模式

	glutDisplayFunc(myDisplay);
	
	glutReshapeFunc(myReshape);
	
	glutKeyboardFunc(&normalKey);
	glutSpecialFunc(&SpecialKeys);
	
	glutMotionFunc(&mouseMotion);
	
	glutMouseFunc(&mouseFunc);
	
	//每16毫秒调用一次该函数，保证输出60帧的动画
	glutTimerFunc(16, &timeFunc, 1);
	

	glutMainLoop();//开始显示
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
	// 重新调用渲染函数
	glutPostRedisplay(); 
}




void normalKey(unsigned char key, int x, int y)
{
	moves.set(key);
	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y)//视角移动，处理右键按下的情况
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:  
		if (state == GLUT_DOWN)//当鼠标左键按下时
		{
			viewControl.leftButton = true;
			viewControl.lastX = x;//记录左键按下时鼠标的位置
			viewControl.lastY = y;
		}
		else
			viewControl.leftButton = false;//右键抬起时，置按下状态为假
		break;
	default:
		break;
	}
}

void mouseMotion(int x, int y)//2-②视角移动，处理鼠标移动
{
	if (viewControl.leftButton)//通过每次移动的距离得到当前的yaw值和pitch值
	{
		viewControl.yaw += (x - viewControl.lastX)*0.3;//*0.7为调整灵敏度
		viewControl.pitch += (y - viewControl.lastY)*0.3;
		if (viewControl.yaw > 180)//横向角度范围0-360，超界后如下处理可以保证视角连贯变化
			viewControl.yaw = -180;
		else if (viewControl.yaw < -180)
			viewControl.yaw = 180;
		if (viewControl.pitch > 75)//纵向范围-75到75，超界后限制在最大值
			viewControl.pitch = 75;
		else if (viewControl.pitch < -75)
			viewControl.pitch = -75;
		viewControl.lastX = x;
		viewControl.lastY = y;
		//得到角度后，计算camera的direction、right、up的值就可以得到新的朝向
		camera.changeView(viewControl.yaw, viewControl.pitch);
	}
}



void timeFunc(int value)
{
	glutPostRedisplay();//该命令调用myDisplay函数
	glutTimerFunc(16, timeFunc, 1);//配合上一条命令实现每秒1000/16=60次绘制
}

void myReshape(int w, int h)
{
	windowsWidth = w;
	windowsHeight = h;
	//透视投影实现，一定要按照按以下顺序，否则会显示不正确。
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(95.0, (GLfloat)w / (GLfloat)h, 1, 100000.0);//四个参数含义分别为视野角度，视野宽度，视野高度，能看到的最近距离，能看到的最远距离
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		camera.pos.x, camera.pos.y, camera.pos.z,//相机位置
		camera.pos.x + camera.direction.x, camera.pos.y + camera.direction.y, camera.pos.z + camera.direction.z,//相机的朝向向量
		camera.up.x, camera.up.y, camera.up.z//相机的顶部朝向向量
		);
}



void setLight()//光源的设置
{
	GLfloat Light_Model_Ambient[] = { 0.1, 0.1, 0.1, 1.0 }; //环境光参数
	
	GLfloat mat_diffuse[] = { 1, 1, 1, 1.0 };  //漫反射参数
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };  //镜面反射参数

	GLfloat mat_shininess[] = { 0.0 };         //高光指数
	GLfloat light1_angle[] = { 15.0 };//LIGHT1扩散角度
	GLfloat light_position[] = { 0.0, 1.0, -1.0, 0.0 };//LIGHT0位置,最后一个值为0则为平行光
	GLfloat white_light[] = { 0.3, 0.3, 0.3, 1.0 };//LIGHT0参数，灯颜色(0.3,0.3,0.3), 第四位为开关
	GLfloat yellow_light[] = { 1, 0.87, 0.315, 1.0 };//LIGHT1参数
	
	glClearColor(0.0, 0.0, 0.0, 0.0);  //背景色
	glShadeModel(GL_SMOOTH);           //多变性填充模式
	//材质属性
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//平行光LIGHT0设置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //散射光属性
	glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);  //镜面反射光
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //环境光参数 
	//锥状光源LIGHT1设置
	GLfloat lightKc = 1, lightKl = 0.0, lightKq = 0.0, lightExp = 2;//有关点光源扩散强度衰减的参数。
	glLightfv(GL_LIGHT1, GL_CONSTANT_ATTENUATION, &lightKc);
	glLightfv(GL_LIGHT1, GL_LINEAR_ATTENUATION, &lightKl);
	glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, &lightKq);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellow_light);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mat_specular);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, light1_angle);
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, &lightExp);
}




void readTexture()//读取纹理
{
	
	//注意文件路径，以可执行文件根目录开始或者写上绝对路径
	skyBoxFront = loadBMP_custom("source//front.bmp");
	skyBoxBack = loadBMP_custom("source//back.bmp");
	skyBoxLeft = loadBMP_custom("source//left.bmp");
	skyBoxRight = loadBMP_custom("source//right.bmp");
	skyBoxTop = loadBMP_custom("source//top.bmp");
	grassTexture = loadBMP_custom("source//grass.bmp");

	
}


static void initDisplayList()
{
	testDL = glGenLists(1);//为testDL显示列表分配空间
	glNewList(testDL, GL_COMPILE);//为testDL绑定函数
	//testDisplayList();//绘制工作在本函数中完成，注意不能向其传递变量
	glEndList();//结束绑定
}



GLuint loadBMP_custom(const char * imagepath)  // 贴图函数
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





/////////画自行车的函数们

void draw_faces(utils_funcs::myMesh& mesh){

	for (int i = 0; i < mesh.face_number; i++) {
		//注意我们的操作都是
		utils_funcs::Face& face = mesh.faces[i];
		glBegin(GL_POLYGON);              //绘制多边形即可
		//在绘制面的过程中载入我们已经计算好的法线量信息
		glNormal3fv(face.normal);           //在绘制面的时候同时载入法向量信息
		for (int j = 0; j < face.vert_number; j++) {
			utils_funcs::Vertex *vert = face.verts[j];
			glVertex3f(vert->x, vert->y, vert->z);
		}
		glEnd();
	}
}

void draw_points(utils_funcs::myMesh& mesh){
	//下面绘制点的信息
	//依次将面的信息点进行遍历
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
	//进行空间的重绘
	//glLoadIdentity();
	/*glScalef(scale, scale, scale);
	glTranslatef(translation[0], translation[1], 0.0); */
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, windowsHeight / windowsHeight, 0.1, 100.0);*/
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(translation[0], translation[1], translation[2]); //沿着 X, Y 和 Z 轴移动。
	glScalef(scale, scale, scale);
	////刷新放缩的大小
	glRotatef(rotation[0], 1.0, 0.0, 0.0); //绕x轴旋转
	glRotatef(rotation[1], 0.0, 1.0, 0.0);          //控制不同角度 绕y轴旋转
	glRotatef(rotation[2], 0.0, 0.0, 1.0);  //绕z轴旋转
	//glTranslatef(-center[0], -center[1], -center[2]);
	////改变旋转中心
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////设置光照
	////载入不同光源的位置
	//glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	////定义材料信息
	////这里可以调整环境颜色和散射颜色数组
	GLfloat mat_ambient[] = { 0.329412, 0.223529, 0.027451, 1.000000 };
	GLfloat mat_diffuse[] = { 0.780392, 0.568627, 0.113725, 1.000000 };
	GLfloat mat_specular[] = { 0.992157, 0.941176, 0.807843, 1.000000 };
	GLfloat mat_shininess[] = { 27.897400 }; //材质RGBA镜面指数，数值在0～128范围内
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, my_set_material);
	// 下面开始绘制表面

	

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
		cout << "文件打开出错" << endl;
	}
	vector<vector<GLfloat>> vset;
	vector<vector<GLint>> fset;
	int v_counter = 1;
	int f_counter = 1;
	while (!f.eof()) {
		getline(f, line);//拿到obj文件中一行，作为一个字符串
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
					parameters.push_back(ans); //取出字符串中的元素，以空格切分
					ans = "";
				}
			}
		}
		cout << endl;
		if (parameters[0] == "v") {   //如果是顶点的话
			vector<GLfloat>Point;
			v_counter++;
			//            cout <<atof( parameters[1].c_str()) << "--" << atof( parameters[2].c_str()) <<" --  " << atof( parameters[3].c_str());
			Point.push_back(atof(parameters[1].c_str()));
			Point.push_back(atof(parameters[2].c_str()));
			Point.push_back(atof(parameters[3].c_str()));
			vset.push_back(Point);
		}
		else if (parameters[0] == "f") {   //如果是面的话，存放顶点的索引
			vector<GLint>vIndexSets;          //临时存放点的集合
			for (int i = 1; i < 4; i++) {
				string x = parameters[i];
				string ans = "";
				for (int j = 0; j < x.length(); j++) {   //跳过‘/’
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
				index--;//因为顶点索引在obj文件中是从1开始的，而我们存放的顶点vector是从0开始的，因此要减1
				vIndexSets.push_back(index);
			}
			fset.push_back(vIndexSets);
		}

	}
	f.close();

	utils_funcs::myMesh *meshs = new utils_funcs::myMesh();        //建立我们的mesh类
	//    cout << fset.size() << endl;
	//    cout << vset.size() << endl;
	//向结构体进行转换
	int vert_number = vset.size();
	int face_number = fset.size();
	meshs->verts = new utils_funcs::Vertex[vert_number + 1];
	assert(meshs->verts);
	//处理mesh的信息
	//----------------处理点的信息 --------------
	for (int i = 0; i < vset.size(); i++)
	{
		utils_funcs::Vertex& vert = meshs->verts[meshs->vert_number++];
		vert.x = vset[i][0];
		vert.y = vset[i][1];
		vert.z = vset[i][2];
	}
	//----------------处理面信息 --------------
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
		//首先分配第一维数组
	}
	return meshs;
}

vector<string> split(const string &str, const string &pattern)
{
	//进行字符串的切割
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
	//计算面法线！
	//计算面的法线
	//通过公式计算：
	face.normal[0] = face.normal[1] = face.normal[2] = 0;
	utils_funcs::Vertex *v1 = face.verts[face.vert_number - 1];
	for (int i = 0; i < face.vert_number; i++) {
		utils_funcs::Vertex *v2 = face.verts[i];
		//新建所有的点
		face.normal[0] += (v1->y - v2->y) * (v1->z + v2->z);
		face.normal[1] += (v1->z - v2->z) * (v1->x + v2->x);
		face.normal[2] += (v1->x - v2->x) * (v1->y + v2->y);
		//首先完成叉乘的工作
		v1 = v2;
	}
	//计算归一化法线
	float squared_normal_length = 0.0;
	squared_normal_length += face.normal[0] * face.normal[0];
	squared_normal_length += face.normal[1] * face.normal[1];
	squared_normal_length += face.normal[2] * face.normal[2];
	float normal_length = sqrt(squared_normal_length);
	//得到归一化长度
	if (normal_length > 1.0E-6) {
		face.normal[0] /= normal_length;
		face.normal[1] /= normal_length;
		face.normal[2] /= normal_length;
	}
	//然后完成归一化任务
}
