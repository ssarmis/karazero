#include "bitmap.hpp"

#include "material.hpp"
#include "math.hpp"

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

v4 Bitmap::FragmentFunction(VertexOutput& o, Material* material) {
    v3 position = o.fragmentPosition;

#if 0
    v3 normal = o.fragmentNormal;
#endif
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

    // TODO add reflections

    v4 ambientOcculion = sample(o.fragmentUV, &material->ambientOcclusion);
    v4 color = (diffuseColor + specularColor + emissive) * ambientOcculion;


    //color = normalSample;

    return color;
}





#if 0
v3 cameraPosition(0, 0, 0);
cameraPosition = v3(-viewTransform.m[3 + 0 * 4], -viewTransform.m[3 + 1 * 4], -viewTransform.m[3 + 2 * 4]);
position = o.fragmentPosition;
v3 ptoc = (cameraPosition - position).Normalized();
r32 outlineDot = Math::Dot(ptoc, o.fragmentNormal);
#endif