#include "bitmap.hpp"

#include "material.hpp"
#include "math.hpp"
#include "bitmap_cube.hpp"

VertexOutput Bitmap::VertexFunctionPBR(Vertex v) {
    VertexOutput output;

    m4 pM = m4::Perspective(fov, aspectRatio, near, far);
    m4& mM = modelTransform;
    m4& vM = viewTransform;

    v4 transformedWorldPosition = mM * v.p;
    v4 finalPosition = pM * vM * transformedWorldPosition;

    output.p = finalPosition;
    output.fragmentPosition = v3(transformedWorldPosition.x, transformedWorldPosition.y, transformedWorldPosition.z);
    output.fragmentUV = v.uv;
    output.fragmentColor = v.color;

    v4 normal = v4(v.n.x, v.n.y, v.n.z, 0);
    v4 transformedNormal = (mM * normal);
    transformedNormal = transformedNormal.Normalized();
    output.fragmentNormal = v3(transformedNormal.x, transformedNormal.y, transformedNormal.z);

    v4 transformedTangent = (mM * v.tangent);
    output.fragmentTangent = v3(transformedTangent.x, transformedTangent.y, transformedTangent.z).Normalized();

    v3 fragmentBitangent = v3::Cross(output.fragmentNormal, output.fragmentTangent).Normalized();


    transformedTangent.w = 0;
    transformedNormal.w = 0;

    m4 tangentTransform;
    tangentTransform.rows[0] = transformedTangent;
    tangentTransform.rows[1] = v4(fragmentBitangent.x, fragmentBitangent.y, fragmentBitangent.z, 0);
    tangentTransform.rows[2] = transformedNormal;
    tangentTransform.rows[3] = v4(0, 0, 0, 1);

    v3 lightPosition(-2 + sin(time) * 3, 2, -1);
    v4 lightPositionInTangentSpace = tangentTransform * (lightPosition - output.fragmentPosition);
    output.fragmentLightVector = v3(lightPositionInTangentSpace.x, lightPositionInTangentSpace.y, lightPositionInTangentSpace.z);


    v3 cameraPosition = v3(viewTransform.rows[0].w, viewTransform.rows[1].w, viewTransform.rows[2].w);
    v4 cameraVectorInTangentSpace = tangentTransform * (cameraPosition - output.fragmentPosition);
    output.fragmentCameraVector = v3(cameraVectorInTangentSpace.x, cameraVectorInTangentSpace.y, cameraVectorInTangentSpace.z);

    return output;
}


VertexOutput Bitmap::VertexFunction(Vertex v) {
    VertexOutput output;

    m4 pM = m4::Perspective(fov, aspectRatio, near, far);
    m4& mM = modelTransform;
    m4& vM = viewTransform;

    v4 transformedWorldPosition = mM * v.p;
    v4 finalPosition = pM * vM * transformedWorldPosition;

    output.p = finalPosition;
    output.fragmentPosition = v3(transformedWorldPosition.x, transformedWorldPosition.y, transformedWorldPosition.z);
    output.fragmentUV = v.uv;
    output.fragmentColor = v.color;

    v4 normal = v4(v.n.x, v.n.y, v.n.z, 0);
    v4 transformedNormal = (mM * normal);
    transformedNormal = transformedNormal.Normalized();
    output.fragmentNormal = v3(transformedNormal.x, transformedNormal.y, transformedNormal.z);

    return output;
}

v4 Bitmap::sampleSubpixel(v2 uv, Bitmap* texture) {
    r32 tx = uv.x * (texture->width - 2);
    r32 ty = uv.y * (texture->height - 2);
    int texelX = tx;
    int texelY = ty;

    v4 pc0 = texture->GetPixel(texelX, texelY);
    v4 pc1 = texture->GetPixel(texelX + 1, texelY);
    v4 pc2 = texture->GetPixel(texelX + 1, texelY + 1);
    v4 pc3 = texture->GetPixel(texelX, texelY + 1);

    r32 fu = tx - texelX;
    r32 fv = ty - texelY;

    return v4::Lerp(v4::Lerp(pc0, pc1, fu), v4::Lerp(pc3, pc2, fu), fv);
}

v4 Bitmap::sample(v2 uv, Bitmap* texture) {
    r32 tx = uv.x * (texture->width - 1);
    r32 ty = uv.y * (texture->height - 1);
    int texelX = tx;
    int texelY = ty;
    return texture->GetPixel(texelX, texelY);
}

constexpr int POSITIVE_X_FACE = 0;
constexpr int NEGATIVE_X_FACE = 1;
constexpr int POSITIVE_Y_FACE = 2;
constexpr int NEGATIVE_Y_FACE = 3;
constexpr int POSITIVE_Z_FACE = 4;
constexpr int NEGATIVE_Z_FACE = 5;

v4 Bitmap::sampleCube(v3 vector, BitmapCube* cube){
    vector = vector.Normalized();

    float absx = std::abs(vector.x);
    float absy = std::abs(vector.y);
    float absz = std::abs(vector.z);

    float max = std::max({absx, absy, absz});
    v2 uv;
    int face;

    if (max == absx) {
        if (vector.x > 0) {
            face = POSITIVE_X_FACE;
            uv.x = 0.5 * (vector.z / vector.x + 0.5);
            uv.y = 0.5 * (-vector.y / vector.x + 0.5);
        } else {
            face = NEGATIVE_X_FACE;
            uv.x = 0.5 * (-vector.z / vector.x + 0.5);
            uv.y = 0.5 * (-vector.y / vector.x + 0.5);
        }
    } else if (max == absy) {
        if (vector.y > 0) {
            face = POSITIVE_Y_FACE;
            uv.x = 0.5 * (vector.x / vector.y + 0.5);
            uv.y = 0.5 * (vector.z / vector.y + 0.5);
        } else {
            face = NEGATIVE_Y_FACE;
            uv.x = 0.5 * (vector.x / vector.y + 0.5);
            uv.y = 0.5 * (-vector.z / vector.y + 0.5);
        }
    } else {
        if (vector.z > 0) {
            face = POSITIVE_Z_FACE;
            uv.x = 0.5 * (vector.x / vector.z + 0.5);
            uv.y = 0.5 * (-vector.y / vector.z + 0.5);
        } else {
            face = NEGATIVE_Z_FACE;
            uv.x = 0.5 * (-vector.x / vector.z + 0.5);
            uv.y = 0.5 * (-vector.y / vector.z + 0.5);
        }
    }

    uv.x = std::max(0.0f, std::min(uv.x, 1.0f));
    uv.y = std::max(0.0f, std::min(uv.y, 1.0f));

    r32 u = uv.x * (cube->directions[face].width - 1);
    r32 v = uv.y * (cube->directions[face].height - 1);
    return cube->directions[face].GetPixel(u, v);
}
#if 1
v4 Bitmap::FragmentFunctionPBR(VertexOutput& o, Material* material) {
    v3 position = o.fragmentPosition;

    v4 normalSample = sample(o.fragmentUV, &material->normal);
    v3 normal = v3(normalSample.x, normalSample.y, normalSample.z);
    normal = normal * 2 - 1; //[-1, 1]
    
    // difuse
    v3 pToL = o.fragmentLightVector;
    pToL = pToL.Normalized();
    r32 dot = Math::Dot(normal, pToL);
    dot = std::max(dot, 0.2f);

    v4 diffuseColor = sample(o.fragmentUV, &material->diffuse) * dot;
    //

    // specular
    v3 invPToL = -pToL;
    v3 reflected = Math::Reflect(invPToL, normal).Normalized();
    v3 toCamera = o.fragmentCameraVector.Normalized();
    r32 similarity = Math::Dot(reflected, toCamera);
    similarity = std::pow(std::max(similarity, 0.0f), 32);


    v4 specularColor = sample(o.fragmentUV, &material->roughness) * similarity;
    //

    // emission
    v4 emissive = sample(o.fragmentUV, &material->emissive);
    //

    // reflection
    v3 fromCamera = -toCamera;
    v3 reflecedFromCamera = Math::Reflect(fromCamera, normal).Normalized();

    v4 roughness = sample(o.fragmentUV, &material->roughness);
    v4 invroughness = sample(o.fragmentUV, &material->roughness);
    invroughness.x = 1 - roughness.x;
    invroughness.y = 1 - roughness.y;
    invroughness.z = 1 - roughness.z;
    invroughness.w = 1 - roughness.w;

    v4 reflectedColor = sampleCube(reflecedFromCamera, environment) * invroughness;
    //

    v4 ambientOcculion = sample(o.fragmentUV, &material->ambientOcclusion);
    v4 color = (diffuseColor + reflectedColor + specularColor + emissive) * ambientOcculion;

    return color;
}
#endif

#if 0
v4 Bitmap::FragmentFunctionPBR(VertexOutput& o, Material* material) {
    v4 Fdielectric = v4(0.04, 0.04, 0.04, 0.04);

    v3 position = o.fragmentPosition;

    v4 normalSample = sample(o.fragmentUV, &material->normal);
    v3 normal = v3(normalSample.x, normalSample.y, normalSample.z);
    normal = normal * 2 - 1; //[-1, 1]

    v4 albedo = sample(o.fragmentUV, &material->diffuse);
	float metalness = sample(o.fragmentUV, &material->metalic).x;
	float roughness = sample(o.fragmentUV, &material->roughness).x;

    v3 toCamera = o.fragmentCameraVector;
    float cosLo = std::max(0.0f, Math::Dot(normal, toCamera));

	v3 Lr = normal * 2.0 * cosLo - toCamera;
	v4 F0 = v4::Lerp(Fdielectric, albedo, metalness);

}
#endif
v4 Bitmap::FragmentFunction(VertexOutput& o, Material* material) {
    v3 position = o.fragmentPosition;

    v3 normal = o.fragmentNormal;
    //o.fragmentUV.y = 1 - o.fragmentUV.y; 
    o.fragmentUV.x = 1 - o.fragmentUV.x; 
    v4 diffuseColor = sample(o.fragmentUV, &material->diffuse);

    v4 color = diffuseColor;

    return color;
}





#if 0
v3 cameraPosition(0, 0, 0);
cameraPosition = v3(-viewTransform.m[3 + 0 * 4], -viewTransform.m[3 + 1 * 4], -viewTransform.m[3 + 2 * 4]);
position = o.fragmentPosition;
v3 ptoc = (cameraPosition - position).Normalized();
r32 outlineDot = Math::Dot(ptoc, o.fragmentNormal);
#endif