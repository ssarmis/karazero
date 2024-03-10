#pragma once

#include "global.hpp"
#include "math.hpp"

#include <assert.h>
#include <vector>
#include <string>
#include <list>
#include "vertex.hpp"

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Material;

struct Viewport {
    static r32 width;
    static r32 height;
};

struct VertexOutput {
    v4 p;

    // used in the fragment shader
    v2 fragmentUV;
    v3 fragmentNormal;
    v3 fragmentColor;
    v3 fragmentPosition;
    v3 fragmentTangent;
    v3 fragmentLightVector;
    v3 fragmentCameraVector;

    v3 flatPosition;
    v3 flatNormal;

    static VertexOutput InterpolateBarycentric(const VertexOutput& v0, const VertexOutput& v1, const VertexOutput& v2, r32 u, r32 v, r32 w){
        VertexOutput vo = {};

        vo.flatPosition = v0.flatPosition;
        vo.flatNormal = v0.flatNormal;

        r32 pespW = u / v0.p.w + v / v1.p.w + w / v2.p.w;

        r32 tu = u * (v0.fragmentUV.x / v0.p.w) + v * (v1.fragmentUV.x / v1.p.w) + w * (v2.fragmentUV.x / v2.p.w);
        r32 tv = u * (v0.fragmentUV.y / v0.p.w) + v * (v1.fragmentUV.y / v1.p.w) + w * (v2.fragmentUV.y / v2.p.w);

        vo.fragmentUV.x = Math::Clamp(tu / pespW, 0, 1);
        vo.fragmentUV.y = Math::Clamp(tv / pespW, 0, 1);

        vo.p = v0.p * u + v1.p * v + v2.p * w;

        vo.fragmentPosition = (v0.fragmentPosition * u) + (v1.fragmentPosition * v) + (v2.fragmentPosition * w);
        vo.fragmentNormal = v0.fragmentNormal * u + v1.fragmentNormal * v + v2.fragmentNormal * w;
        vo.fragmentColor = v0.fragmentColor * u + v1.fragmentColor * v + v2.fragmentColor * w;
        vo.fragmentTangent = v0.fragmentTangent * u + v1.fragmentTangent * v + v2.fragmentTangent * w;
        vo.fragmentLightVector = v0.fragmentLightVector * u + v1.fragmentLightVector * v + v2.fragmentLightVector * w;
        vo.fragmentCameraVector = v0.fragmentCameraVector * u + v1.fragmentCameraVector * v + v2.fragmentCameraVector * w;

        return vo;
    }

    static VertexOutput Lerp(const VertexOutput& v0, const VertexOutput& v1, r32 t){
        VertexOutput result;

        result.flatNormal = v0.flatNormal;
        result.flatPosition = v0.flatPosition;

        result.p = v4::Lerp(v0.p, v1.p, t);

        result.fragmentPosition = v3::Lerp(v0.fragmentPosition, v1.fragmentPosition, t);
        result.fragmentUV = v2::Lerp(v0.fragmentUV, v1.fragmentUV, t);
        result.fragmentNormal = v3::Lerp(v0.fragmentNormal, v1.fragmentNormal, t);
        result.fragmentColor = v3::Lerp(v0.fragmentColor, v1.fragmentColor, t);
        result.fragmentTangent = v3::Lerp(v0.fragmentTangent, v1.fragmentTangent, t);
        result.fragmentLightVector = v3::Lerp(v0.fragmentLightVector, v1.fragmentLightVector, t);
        result.fragmentCameraVector = v3::Lerp(v0.fragmentCameraVector, v1.fragmentCameraVector, t);

        return result;
    }
};

struct Face {
    Vertex v0;
    Vertex v1;
    Vertex v2;
};

struct FaceOutput {
    VertexOutput v0;
    VertexOutput v1;
    VertexOutput v2;
};

struct Bitmap {
    i32 width;
    i32 height;
    u8* data;
    r32* depthBuffer;
    r32 fov;
    r32 near;
    r32 far;
    r32 aspectRatio;
    m4 modelTransform;
    m4 viewTransform;

    static Bitmap LoadFromFile(const std::string& path) {
        Bitmap bitmap;
        bitmap.data = stbi_load(path.c_str(), &bitmap.width, &bitmap.height, nullptr, 4);
        return bitmap;
    }

    bool PointInTriangle(v3 p, v3 p0, v3 p1, v3 p2){
        v3 p0p1 = p1 - p0;
        v3 p0p2 = p2 - p0;
        v3 c = v3::Cross(p0p1, p0p2);

        // if(c.z < 0){
        //     v3 aux = p1;
        //     p1 = p2;
        //     p2 = aux;
        // }

        v3 e0 = p1 - p0;
        v3 e1 = p2 - p1;
        v3 e2 = p0 - p2;

        v3 n0 = v3::Perp(e0).Normalized();
        v3 n1 = v3::Perp(e1).Normalized();
        v3 n2 = v3::Perp(e2).Normalized();

        bool d0 = Math::Dot(p - p0, n0) >= (0 - 0.0001);
        bool d1 = Math::Dot(p - p1, n1) >= (0 - 0.0001);
        bool d2 = Math::Dot(p - p2, n2) >= (0 - 0.0001);

        return d0 && d1 && d2;
    }

    FaceOutput CreateClippedTriangle(r32 t0, r32 t1, r32 t2, 
                                    VertexOutput v00, VertexOutput v10,
                                    VertexOutput v01, VertexOutput v11,
                                    VertexOutput v02, VertexOutput v12){
            
            FaceOutput result;

            result.v0 = VertexOutput::Lerp(v00, v10, t0);
            result.v1 = VertexOutput::Lerp(v01, v11, t1);
            result.v2 = VertexOutput::Lerp(v02, v12, t2);

            return result;
    }
#if 1
    bool Clip(FaceOutput& face, std::list<FaceOutput>& clippedFaces, v4 point, v4 normal){
        v4 p0 = face.v0.p;
        v4 p1 = face.v1.p;
        v4 p2 = face.v2.p;

        bool t0 = Math::Dot(normal, p0 - point) >= (0 - 0.00001);
        bool t1 = Math::Dot(normal, p1 - point) >= (0 - 0.00001);
        bool t2 = Math::Dot(normal, p2 - point) >= (0 - 0.00001);

        i32 pointsIn = t0 + t1 + t2;
        
        bool t[3] = {t0, t1, t2};
        v4 p[3] = {p0, p1, p2};
        VertexOutput v[3] = {face.v0, face.v1, face.v2};

        if(pointsIn == 0){
            return true;
        } else if(pointsIn == 1) {
            for(int i = 0; i < 3; ++i){
                int point0 = i;
                int point1 = (i + 1) % 3;
                int point2 = (i + 2) % 3;

                if(!t[point0] && !t[point2]){
                    r32 t00;
                    r32 t11;
                    Math::Intersect(point, normal, p[point0], p[point1], &t00);
                    Math::Intersect(point, normal, p[point1], p[point2], &t11);

                    FaceOutput fo = CreateClippedTriangle(t00, 1, t11, 
                                                        v[point0], v[point1],
                                                        v[point0], v[point1],
                                                        v[point1], v[point2]);
                    clippedFaces.push_back(fo);
                    return true;
                } else if(!t[point0] && !t[point1]){
                    r32 t00;
                    r32 t11;
                    Math::Intersect(point, normal, p[point0], p[point2], &t00);
                    Math::Intersect(point, normal, p[point1], p[point2], &t11);

                    FaceOutput fo = CreateClippedTriangle(t00, t11, 1,
                                                        v[point0], v[point2],
                                                        v[point1], v[point2],
                                                        v[point1], v[point2]);
                    clippedFaces.push_back(fo);
                    return true;
                }
            }            
        } else if(pointsIn == 2){
            for(int i = 0; i < 3; ++i){
                int point0 = i;
                int point1 = (i + 1) % 3;
                int point2 = (i + 2) % 3;

                if(!t[point0]){
                    r32 t00;
                    r32 t11;
                    Math::Intersect(point, normal, p[point0], p[point1], &t00);
                    Math::Intersect(point, normal, p[point2], p[point0], &t11);

                    FaceOutput fo0 = CreateClippedTriangle(t00, 1, 1,
                                                            v[point0], v[point1],
                                                            v[point0], v[point1],
                                                            v[point1], v[point2]);

                    FaceOutput fo1 = CreateClippedTriangle(t00, 1, t11, 
                                                            v[point0], v[point1],
                                                            v[point1], v[point2],
                                                            v[point2], v[point0]);

                    clippedFaces.push_back(fo0);
                    clippedFaces.push_back(fo1);
                    return true;
                }
            } 
        } else {
            return false;
        }
        return false;
    }
#endif
    void LineNDC(v3 p0, v3 p1, v4 c){
        p0 = Math::NDCToSC(p0, Viewport::width, Viewport::height); 
        p1 = Math::NDCToSC(p1, Viewport::width, Viewport::height); 

        Line(v2(p0.x, p0.y), v2(p1.x, p1.y), c);
    }

    void TriangleNDC(VertexOutput v0, VertexOutput v1, VertexOutput v2, Material* material){
        v3 p0 = v3(v0.p.x, v0.p.y, v0.p.z);
        v3 p1 = v3(v1.p.x, v1.p.y, v1.p.z);
        v3 p2 = v3(v2.p.x, v2.p.y, v2.p.z);

        p0 = Math::NDCToSC(p0, Viewport::width, Viewport::height); 
        p1 = Math::NDCToSC(p1, Viewport::width, Viewport::height); 
        p2 = Math::NDCToSC(p2, Viewport::width, Viewport::height); 

        v3 min = v3::Min(p0, v3::Min(p1, p2));
        v3 max = v3::Max(p0, v3::Max(p1, p2));

        v3 e0 = p1 - p0;
        v3 e1 = p2 - p0;

        for(int y = min.y; y <= max.y; ++y){
            if(y < 0 || y >= height) {
                continue;
            }
            for(int x = min.x; x <= max.x; ++x){
                if(x < 0 || x >= width) {
                    continue;
                }
                v3 sp(x, y, 0);
                if(PointInTriangle(sp, p0, p1, p2)){
                    v3 e2 = sp - p0;

                    // barycentric        
                    r32 d00 = Math::Dot(e0, e0);
                    r32 d01 = Math::Dot(e0, e1);
                    r32 d11 = Math::Dot(e1, e1);
                    r32 d20 = Math::Dot(e2, e0);
                    r32 d21 = Math::Dot(e2, e1);
                    r32 denom = d00 * d11 - d01 * d01;

                    r32 v = (d11 * d20 - d01 * d21) / denom;
                    r32 w = (d00 * d21 - d01 * d20) / denom;
                    r32 u = 1.0f - v - w;

                    r32 z = u * p0.z + v * p1.z + w * p2.z;
                    r32 depthValue = z;
                    if(depthValue > depthBuffer[x + y * width]){
                        continue;
                    }
                    depthBuffer[x + y * width] = depthValue;

                    VertexOutput vo = VertexOutput::InterpolateBarycentric(v0, v1, v2, u, v, w);

                    v4 color = FragmentFunction(vo, material);
                    SetPixel(x, y, color);
                    //SetPixel(x, y, v4(depthValue, depthValue, depthValue, 1));
                }
            }
        }
    }

    void InitializePerspective(r32 pfov, r32 pnear, r32 pfar){
        fov = pfov;
        near = pnear;
        far = pfar;
        aspectRatio = (r32)width / (r32)height;
    }
    
    void SetModelTransform(m4 transform){
        modelTransform = transform;
    }

    void SetViewTransform(m4 transform){
        viewTransform = transform;
    }
    
    v3 cameraForward;
    v3 cameraRight;
    v3 cameraUp;
    
    void SetupCamera(v3 up, v3 eye, v3 target){
        cameraForward = (target - eye).Normalized();
        cameraRight = v3::Cross(up, cameraForward).Normalized();
        cameraUp = v3::Cross(cameraForward, cameraRight).Normalized();

        m4 lookAt;
        lookAt.rows[0] = v4(cameraRight.x, cameraUp.x, cameraForward.x, 0);
        lookAt.rows[1] = v4(cameraRight.y, cameraUp.y, cameraForward.y, 0);
        lookAt.rows[2] = v4(cameraRight.z, cameraUp.z, cameraForward.z, 0);
        lookAt.rows[3] = v4(-eye.x, -eye.y, -eye.z, 1);

        viewTransform = lookAt;
    }

    std::vector<Face> faceToProcess;
    std::list<FaceOutput> facesToBeClipped;
    std::vector<FaceOutput> clippedFaces;

    void DrawTriangles(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, Material* material){
        assert(indices.size() % 3 == 0);
        
        faceToProcess.clear();
        facesToBeClipped.clear();
        clippedFaces.clear();

        for(int i = 0; i < indices.size(); i += 3){
            Face f = {};
            f.v0 = vertices[indices[i + 0]];
            f.v1 = vertices[indices[i + 1]];
            f.v2 = vertices[indices[i + 2]];
            faceToProcess.push_back(f);
        }

        for(auto& face : faceToProcess){
            VertexOutput vo0 = VertexFunction(face.v0);
            VertexOutput vo1 = VertexFunction(face.v1);
            VertexOutput vo2 = VertexFunction(face.v2);

            FaceOutput fo  = {vo0, vo1, vo2};

            facesToBeClipped.push_back(fo);
        }


        r32 xc = 0;
        r32 clipSize = 1;
        while(!facesToBeClipped.empty()){
            FaceOutput& face = facesToBeClipped.front();

            bool n = Clip(face, facesToBeClipped, v4(0, 0, 0, near), v4(0, 0, 1, 0));
            if(!n){
                clippedFaces.push_back(face);
            }

            facesToBeClipped.pop_front();
        }
        //std::cout << clippedFaces.size() << std::endl;
        for(auto& face : clippedFaces){
            // clip coordinates to NDC
            face.v0.p = v4(face.v0.p.x / face.v0.p.w, face.v0.p.y / face.v0.p.w, face.v0.p.z / face.v0.p.w, face.v0.p.w);
            face.v1.p = v4(face.v1.p.x / face.v1.p.w, face.v1.p.y / face.v1.p.w, face.v1.p.z / face.v1.p.w, face.v1.p.w);
            face.v2.p = v4(face.v2.p.x / face.v2.p.w, face.v2.p.y / face.v2.p.w, face.v2.p.z / face.v2.p.w, face.v2.p.w);

            TriangleNDC(face.v0, face.v1, face.v2, material);
            //TriangleWireframeNDC(face.v0, face.v1, face.v2, v4(1, 1, 1, 1));
        }
    }

    void TriangleWireframeNDC(VertexOutput v0, VertexOutput v1, VertexOutput v2, v4 color){
        v3 p0 = v3(v0.p.x, v0.p.y, v0.p.z);
        v3 p1 = v3(v1.p.x, v1.p.y, v1.p.z);
        v3 p2 = v3(v2.p.x, v2.p.y, v2.p.z);

        p0 = Math::NDCToSC(p0, Viewport::width, Viewport::height); 
        p1 = Math::NDCToSC(p1, Viewport::width, Viewport::height); 
        p2 = Math::NDCToSC(p2, Viewport::width, Viewport::height); 

        Line(p0, p1, color);
        Line(p1, p2, color);
        Line(p2, p0, color);
    }

    r32 time;

    VertexOutput VertexFunction(Vertex v);
    v4 sampleSubpixel(v2 uv, Bitmap* texture);
    v4 sample(v2 uv, Bitmap * texture);
    v4 FragmentFunction(VertexOutput & o, Material * material);


    v4 GetPixel(int x, int y){
        if(x < 0 || x >= width || y < 0 || y>= height){
            return v4(1, 0, 1, 1);
        }
        r32 r = data[x * 4 + y * width * 4 + 0] / 255.0;
        r32 g = data[x * 4 + y * width * 4 + 1] / 255.0;
        r32 b = data[x * 4 + y * width * 4 + 2] / 255.0;
        r32 a = data[x * 4 + y * width * 4 + 3] / 255.0;
        return v4(r, g, b, a);
    }

    v4 GetPixelABGRToARGB(int x, int y){
        if(x < 0 || x >= width || y < 0 || y>= height){
            assert(false);
        }
        r32 a = data[x * 4 + y * width * 4 + 0] / 255;
        r32 b = data[x * 4 + y * width * 4 + 1] / 255;
        r32 g = data[x * 4 + y * width * 4 + 2] / 255;
        r32 r = data[x * 4 + y * width * 4 + 3] / 255;
        return v4(r, g, b, a);
    }

#if 0
    void Rect(v2 p, v2 s, r32 r, v4 c, Bitmap* texture=nullptr){
        v2 p0(-0.5, -0.5);
        v2 p1(0.5, -0.5);
        v2 p2(0.5, 0.5);
        v2 p3(-0.5, 0.5);

        p0 = p0 * s;
        p1 = p1 * s;
        p2 = p2 * s;
        p3 = p3 * s;

        m3 rotation = m3::Rotation(r);

        p0 = rotation * (p0) + p;
        p1 = rotation * (p1) + p;
        p2 = rotation * (p2) + p;
        p3 = rotation * (p3) + p;

        Rect(p0, p1, p2, p3, c, texture);
    }

    void Rect(v2 p0, v2 p1, v2 p2, v2 p3, v4 c, Bitmap* texture){
        v2 min = v2::Min(p0, v2::Min(p1, v2::Min(p2, p3)));
        v2 max = v2::Max(p0, v2::Max(p1, v2::Max(p2, p3)));

        // Edges must be Clockwise
        v2 xa = p2 - p3;
        v2 ya = p0 - p3;
        
        r32 xal = xa.Length();
        r32 yal = ya.Length();

        v2 xan = xa.Normalized();
        v2 yan = ya.Normalized();

        // we can use the xan and yan vectors as normals for the others        

        for(int y = min.y; y <= max.y; ++y){
            for(int x = min.x; x <= max.x; ++x){
                v2 sp(x, y);

                bool t = Math::Dot(sp - p0, -yan) >= 0;
                bool b = Math::Dot(sp - p3, yan) >= 0;
                bool l = Math::Dot(sp - p3, xan) >= 0;
                bool r = Math::Dot(sp - p1, -xan) >= 0;

                if(t && b && l && r){
                    if(texture){
                        v2 tp = sp - p3;

                        r32 u = Math::Dot(tp, xan) / xal;
                        r32 v = Math::Dot(tp, yan) / yal;

                        r32 tx = u * (texture->width - 2);
                        r32 ty = v * (texture->height - 2);
                        
                        i32 texelX = tx;
                        i32 texelY = ty;

                        r32 fu = tx - texelX;
                        r32 fv = ty - texelY;

                        v4 pc0 = texture->GetPixel(texelX, texelY);
                        v4 pc1 = texture->GetPixel(texelX + 1, texelY);
                        v4 pc2 = texture->GetPixel(texelX + 1, texelY + 1);
                        v4 pc3 = texture->GetPixel(texelX, texelY + 1);

                        v4 pc = v4::Lerp(v4::Lerp(pc0, pc1, fu), v4::Lerp(pc3, pc2, fu), fv);
                        
                        v4 fc = c * pc;
                        v4 bc = GetPixelABGRToARGB(x, y);

                        v4 blendedColor = v4::Lerp(fc, bc, 1 - fc.w);

                        SetPixel(x, y, blendedColor);
                    } else {
                        SetPixel(x, y, c);
                    }
                }
            }
        }
    }
#endif
#if 0
    void Line(v2 a, v2 b, v4 color){
        v2 ab = b - a;
        int steps = ab.Length();
        ab = ab.Normalized();

        r32 px = a.x;
        r32 py = a.y;
        
        while(steps--){
            px += ab.x;
            py += ab.y;

            int ipx = std::ceil(px);
            int ipy = std::ceil(py);

            SetPixel(ipx, ipy, color);
        }
        SetPixel(b.x, b.y, color);
    }
#else
    void Line (v2 a, v2 b, v4 color){
        v2 ab = b - a;
        int steps = ab.Length();
        v2 nab = ab.Normalized();

        r32 currentPointX = a.x;
        r32 currentPointY = a.y;

        int roundedPixelX = currentPointX;
        int roundedPixelY = currentPointY;

        SetPixel(roundedPixelX, roundedPixelY, color);

        while(steps--){
            currentPointX += nab.x;
            currentPointY += nab.y;

            roundedPixelX = std::ceil(currentPointX);
            roundedPixelY = std::ceil(currentPointY);

            SetPixel(roundedPixelX, roundedPixelY, color);
        }
    }
#endif

    void Line(v3 a, v3 b, v4 color){
        v2 aa(a.x, a.y);
        v2 bb(b.x, b.y);
        Line(aa, bb, color);
    }

    void Clear(const v3& color){
        for(int i = 0; i < width * height * 4; i += 4){
            depthBuffer[i / 4] = 1;

            data[i + 0] = 255;
            data[i + 1] = color.z * 255;
            data[i + 2] = color.y * 255;
            data[i + 3] = color.x * 255;
        }
    }

    void SetPixel(int x, int y, v4 c){
        if(x < 0 || x >= width || y < 0 || y >= height){
            return;
        }

        c.x = Math::Clamp(c.x, 0, 1);
        c.y = Math::Clamp(c.y, 0, 1);
        c.z = Math::Clamp(c.z, 0, 1);
        c.w = Math::Clamp(c.w, 0, 1);

        data[x * 4 + y * width * 4 + 0] = c.w * 255;
        data[x * 4 + y * width * 4 + 1] = c.z * 255;
        data[x * 4 + y * width * 4 + 2] = c.y * 255;
        data[x * 4 + y * width * 4 + 3] = c.x * 255;
    }
};
