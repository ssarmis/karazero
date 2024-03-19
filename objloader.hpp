#pragma once

#include <string>
#include <vector>

#include "global.hpp"
#include "vertex.hpp"

#include "OBJ_Loader.h"

class OBJLoader {
public:
	static void Load(const std::string& path, std::vector<Vertex>& vertices, std::vector<u32>& indices) {
#if 0
        objl::Loader loader;

        // Load .obj File
        bool loadout = loader.LoadFile(path);

        for (int i = 0; i < loader.LoadedMeshes.size(); i++) {
            // Copy one of the loaded meshes to be our current mesh
            objl::Mesh curMesh = loader.LoadedMeshes[i];

            for (int j = 0; j < curMesh.Vertices.size(); j++) {
                v3 p(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);
                //p = p * 0.1;
                v2 uv(curMesh.Vertices[j].TextureCoordinate.X, 1.0 - curMesh.Vertices[j].TextureCoordinate.Y);
                v3 n(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z);

                vertices.push_back(Vertex(p, uv, n, v3(1, 1, 1)));
            }

            for (int j = 0; j < curMesh.Indices.size(); j += 3) {
                u32 i0 = curMesh.Indices[j + 0];
                u32 i1 = curMesh.Indices[j + 1];
                u32 i2 = curMesh.Indices[j + 2];

                indices.push_back(i0);
                indices.push_back(i1);
                indices.push_back(i2);

                v3 p0 = vertices[i0].p;
                v3 p1 = vertices[i1].p;
                v3 p2 = vertices[i2].p;

                v2 uv0 = vertices[i0].uv;
                v2 uv1 = vertices[i1].uv;
                v2 uv2 = vertices[i2].uv;

                v3 dp0 = p1 - p0;
                v3 dp1 = p2 - p0;

                v2 duv0 = uv1 - uv0;
                v2 duv1 = uv2 - uv0;

                r32 r = 1.0f / (duv0.x * duv1.y - duv0.y * duv1.x);

                v3 tangent = (dp0 * duv1.y - dp1 * duv0.y) * r;

                vertices[i0].tangent = tangent;
                vertices[i1].tangent = tangent;
                vertices[i2].tangent = tangent;
            }
        }
#endif
	}
};
