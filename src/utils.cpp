#include "stdafx.h"
#include <windows.h>
#include<GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>

#include "bicycle.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;


void utils_funcs::data_deal()
{
	//��ȡ�����Լ���model��ʽ;
	;
}

void utils_funcs::get_normal(Face& face){
	//�����淨�ߣ�
	//������ķ���
	//ͨ����ʽ���㣺
	face.normal[0] = face.normal[1] = face.normal[2] = 0;
	Vertex *v1 = face.verts[face.vert_number - 1];
	for (int i = 0; i < face.vert_number; i++) {
			Vertex *v2 = face.verts[i];
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





void utils_funcs::draw_control(string model_path, utils_funcs::myMesh & mesh)
{
	

	//this->ReaderOBj(model_path, *mesh);
	float bbox[2][3] = { { 1.0E30F, 1.0E30F, 1.0E30F }, { -1.0E30F, -1.0E30F, -1.0E30F } };
	for (int i = 0; i < mesh.vert_number; i++) {
		utils_funcs::Vertex& vert = mesh.verts[i];
		if (vert.x < bbox[0][0]) bbox[0][0] = vert.x;
		else if (vert.x > bbox[1][0]) bbox[1][0] = vert.x;
		if (vert.y < bbox[0][1]) bbox[0][1] = vert.y;
		else if (vert.y > bbox[1][1]) bbox[1][1] = vert.y;
		if (vert.z < bbox[0][2]) bbox[0][2] = vert.z;
		else if (vert.z > bbox[1][2]) bbox[1][2] = vert.z;
	}
	// Setup initial viewing scale
	float dx = bbox[1][0] - bbox[0][0];
	float dy = bbox[1][1] - bbox[0][1];
	float dz = bbox[1][2] - bbox[0][2];
	this->scale = 2.0 / sqrt(dx*dx + dy*dy + dz*dz);
	// Setup initial viewing center
	this->center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
	this->center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
	this->center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);
}























