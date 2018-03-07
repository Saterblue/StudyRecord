#ifndef     OBJFILE_READER_2018
#define     OBJFILE_READER_2018

#include <vector>

struct ObjVec3d
{
    float x,y,z;
    ObjVec3d(float _x,float _y,float _z):x(_x),y(_y),z(_z){}
    ObjVec3d():ObjVec3d(0,0,0){}
};//vertex 3d

//blender's obj file is little special,one grounp only 2 args,example:f 122//10 123//10 126//10 124//10
//I have seen the face more than 4 grounp if the primitive is all the same,so I not assume the face grounp nubmers 4 or 3
struct ObjFace2
{
    std::vector<int> arg0;
    std::vector<int> arg1;
    ObjFace2(){arg0.clear();arg1.clear();}
};//face one grounp only 2 args

class ObjFileReader
{
public:
    ObjFileReader();
    ~ObjFileReader();

public:
    std::vector<ObjVec3d> m_vertices_v;
    std::vector<ObjVec3d> m_normals_vn;
    std::vector<ObjFace2> m_faces_f;

public:
    void showInfo();
    void clear();
public:
    bool readObjFile(const char* fileName);//read the file,store the data to the vecters;
};

//demo:
//    ObjFileReader ofr;
//    bool r =  ofr.readObjFile("dm1.obj");
//    if(r)
//    {
//        for(unsigned int i = 0; i < ofr.m_faces_f.size(); i ++)
//        {
//            printf("face:");
//            for(unsigned int j = 0 ; j < ofr.m_faces_f[i].arg0.size(); j++)
//            {
//                printf("%d//%d ",ofr.m_faces_f[i].arg0[j],ofr.m_faces_f[i].arg1[j]);
//            }
//            printf("\n");
//        }
//    }

#endif // OBJFILE_READER_2018
