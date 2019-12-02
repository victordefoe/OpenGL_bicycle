

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
		//������άͼ�ε�
		//����face�ṹ����
		float x, y, z;
	} Vertex;

	typedef struct Face {
		//����Σ������Σ���Ľṹ��
		Face(void) : vert_number(0), verts(0) {};
		int vert_number;        //��¼����ĸ���
		Vertex **verts;          //����һ��������� �������飨�������꣩
		float normal[3];         //��¼��ķ��������ֱ���x��y��z��������
		//ע���ķ�����ͨ���������Ϣ����õ���
		//����objģ����������Ѿ��õ��˷��ߵ���Ϣ
		//��ô��ֱ�������þͺã�
	} Face;

	typedef struct myMesh {
		//�Զ���mesh�Ľṹ��
		myMesh(void) : vert_number(0), verts(0), face_number(0), faces(0) {};
		//�Զ��幹����
		int vert_number;        //�ܵĶ������
		Vertex *verts;             //��������
		int face_number;                 //�����Ŀ
		Face *faces;
		vector<Vertex>point;
	} myMesh;


	////parameters
	float scale; //����ĳߴ��С�������ã�
	float center[3]; //���������λ��




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

	material() //���캯������ʼ������ Ka Kd Ks
	{
		for (size_t i = 0; i<4; i++)
			Ka[i] = Kd[i] = Ks[i] = 1.0f;
		Ns = 0.0f;
		Tr = 0.0f;
	}
};





class ObjLoader
{
	class Vertex		// ������� vertex �� property
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

	class FACE			// faceList �� structure
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
	std::vector<material>		mList;		// material ID (ÿ�� mesh ����� 100 �N material)	

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