#include    <stdio.h>
#include    <cstring>

#include    "objfile_reader.h"

ObjFileReader::ObjFileReader()
{
    clear();
}

ObjFileReader::~ObjFileReader()
{
    clear();
}

void ObjFileReader::clear()
{
    m_vertices_v.clear();
    m_normals_vn.clear();
    m_faces_f.clear();
}

void ObjFileReader::showInfo()
{
     printf("vertices:%d,normals:%d,faces:%d\n",m_vertices_v.size(),m_normals_vn.size(),m_faces_f.size());
}


//obj file format:  flag data0 data1 ...
//example:
//v -1.686617 0.105168 0.595514
//v -1.686617 0.177162 0.359221
//vn -0.0000 0.0000 1.0000
//vn 1.0000 -0.0000 0.0000
//f 135//1 8//1 1//1 122//1
//f 141//2 11//2 9//2 134//2
//the function just record v/vn/f, ignore others

#define    VERTEX_FLAG  "v"
#define    NORMAL_FLAG  "vn"
#define    FACE_FLAG    "f"

bool ObjFileReader::readObjFile(const char* fileName)//read the file,store the data to the vecters;
{
        FILE* file = fopen(fileName,"r");
        if(file == 0)
        {
            printf("file open failed:%s",fileName);
            return false;
        }

        bool run(true);

        while(run)
        {
            char buffer[128] = {0};
            int r = fscanf(file,"%s",buffer);
            if(r == EOF)
            {
                run = false;
                continue;
            }

            if( strcmp(buffer,VERTEX_FLAG) == 0)//check vertex
            {
                ObjVec3d v3;
                int n = fscanf(file,"%f %f %f",&v3.x,&v3.y,&v3.z);
                if(n != 3)
                {
                    printf("read vertex format failed!,the %d vertives\n",m_vertices_v.size());
                    fclose(file);
                    return false;
                }
                m_vertices_v.push_back(v3);
            }
            else if( strcmp(buffer,NORMAL_FLAG) == 0 )//check normal
            {
                ObjVec3d v3;
                int n = fscanf(file,"%f %f %f",&v3.x,&v3.y,&v3.z);
                if(n != 3)
                {
                    printf("read normals format failed!,the %d normals\n",m_normals_vn.size());
                    fclose(file);
                    return false;
                }
                m_normals_vn.push_back(v3);
            }
            else if( strcmp(buffer,FACE_FLAG) == 0 )//check face
            {
                ObjFace2 f;
                int a0,a1;
                int n = 1;
                while(n > 0)
                {
                    n = fscanf(file,"%d//%d",&a0,&a1);
                    if(n == 2)
                    {
                        f.arg0.push_back(a0);
                        f.arg1.push_back(a1);
                    }
                    else if(n > 0)
                    {
                        printf("read faces format failed!,the %d faces ,read args:%d\n",m_faces_f.size(),n);
                        fclose(file);
                        return false;
                    }
                }
                m_faces_f.push_back(f);
            }
        }

        fclose(file);
        return true;
}
