

#ifndef BICYCLE_H
#define BICYCLE_H

#include <vector>
#include "stdafx.h"
#include <windows.h>
#include<GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <map>

#include <string>
#include <GL/glut.h>

using namespace std;



class utils_funcs
{
public:
	typedef struct Vertex {
		//定义三维图形的
		//用于face结构体中
		float x, y, z;
	} Vertex;

	typedef struct Face {
		//多边形（三角形）面的结构体
		Face(void) : vert_number(0), verts(0) {};
		int vert_number;        //记录顶点的个数
		Vertex **verts;          //这是一个面的所有 顶点数组（含有坐标）
		float normal[3];         //记录点的法向量，分别是x，y，z三个方向
		//注意点的法向量通过顶点的信息计算得到！
		//对于obj模型如果我们已经得到了法线的信息
		//那么就直接拿来用就好！
	} Face;

	typedef struct myMesh {
		//自定义mesh的结构体
		myMesh(void) : vert_number(0), verts(0), face_number(0), faces(0) {};
		//自定义构造器
		int vert_number;        //总的顶点个数
		Vertex *verts;             //定点数组
		int face_number;                 //面的数目
		Face *faces;
		vector<Vertex>point;
	} myMesh;


	////parameters
	float scale; //物体的尺寸大小（缩放用）
	float center[3]; //物体的中心位置




	void data_deal();
	void  ReaderOBj(string, myMesh&);
	void get_normal(Face&);

	void draw_control(string, myMesh&);
	

};
















class material
{
public:
	float Ka[4];		//ambient coefficient
	float Kd[4];		//diffuse coefficient
	float Ks[4];		//specular coefficient
	float Ns;			// shiness

	float Tr;			//Transpant (or d)
	std::string map_Ka;	//ambient texture
	std::string map_Kd;	//diffuse texture
	std::string map_Ks;	//specular texture

	//This mtl loader is still incomplete
	//Please see http://en.wikipedia.org/wiki/Wavefront_.obj_file#Material_template_library

	material() //构造函数：初始化变量 Ka Kd Ks
	{
		for (size_t i = 0; i<4; i++)
			Ka[i] = Kd[i] = Ks[i] = 1.0f;
		Ns = 0.0f;
		Tr = 0.0f;
	}
};





class ObjLoader
{
	class Vertex		// 储存基本 vertex 的 property
	{
	public:
		size_t v;		// vertex (index of vList)
		size_t n;		// normal (index of nList)
		size_t t;		// texture (index of tList)

		Vertex() {};
		Vertex(size_t v_index, size_t n_index, size_t t_index = 0)
		{
			v = v_index;
			n = n_index;
			t = t_index;
		}
	};
	class Vec3
	{
	public:
		float ptr[3];
		Vec3(float *v)
		{
			for (size_t i = 0; i < 3; i++)
				ptr[i] = v[i];
		}
		inline float& operator[](size_t index)
		{
			return ptr[index];
		}
	};

	class FACE			// faceList 的 structure
	{
	public:
		Vertex v[4];	// 4 vertex for each face
		int    m;		// material (index of material)

		FACE(Vertex &v1, Vertex &v2, Vertex &v3, Vertex &v4, int m_index)
		{
			v[0] = v1;
			v[1] = v2;
			v[2] = v3;
			v[3] = v4;
			m = m_index;
		}
		inline Vertex& operator[](size_t index)
		{
			return v[index];
		}
	};

	void	LoadMtl(std::string tex_file);
public:
	std::string					matFile;

	size_t						mTotal;	// total material 
	std::map<std::string, size_t>matMap;		// matMap[material_name] = material_ID
	std::vector<material>		mList;		// material ID (每個 mesh 最多有 100 種 material)	

	std::vector<Vec3>			vList;		// Vertex List (Position) - world cord.
	std::vector<Vec3>			nList;		// Normal List
	std::vector<Vec3>			tList;		// Texture List
	GLfloat* vArray;
	GLfloat* nArray;
	GLfloat* tArray;
	GLuint* fArray;
	std::vector<FACE>			faceList;	// Face List

	size_t	vTotal, tTotal, nTotal, fTotal; // number of total vertice, faces, texture coord., normal vecters, and faces

	void LoadObject(std::string obj_file);
	void polygon(int n, size_t & n_vertex, size_t & n_texture, size_t & n_normal);


	ObjLoader();
	ObjLoader(const char* obj_file);
	virtual ~ObjLoader();
	void Init(const char* obj_file);
private:
	FILE * fp_mtl;
	std::string s_file;
};






#endif