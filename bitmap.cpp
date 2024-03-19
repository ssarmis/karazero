#include "bitmap.hpp"

#include "material.hpp"
#include "math.hpp"

VertexOutput Bitmap::VertexFunction(Vertex v) {
    VertexOutput output;

    m4 pM = m4::Perspective(fov, aspectRatio, near, far);
    m4& mM = modelTransform;
    m4& vM = viewTransform;

    m4 boneTransform(0);

    boneTransform = boneTransform + (boneTransforms[v.boneIds.m[0]] * v.boneWeights.m[0]);
    boneTransform = boneTransform + (boneTransforms[v.boneIds.m[1]] * v.boneWeights.m[1]);
    boneTransform = boneTransform + (boneTransforms[v.boneIds.m[2]] * v.boneWeights.m[2]);
    boneTransform = boneTransform + (boneTransforms[v.boneIds.m[3]] * v.boneWeights.m[3]);
    
    if (v.boneWeights.m[0] == 0) {
        boneTransform = m4(1.0);
    }

    v4 transformedWorldPosition = mM * (boneTransform * v.p);
    v4 finalPosition = pM * vM * transformedWorldPosition;

    output.p = finalPosition;
    output.fragmentPosition = v3(transformedWorldPosition.x, transformedWorldPosition.y, transformedWorldPosition.z);
    output.fragmentUV = v.uv;
    output.fragmentColor = v.color;

    v4 normal = v4(v.n.x, v.n.y, v.n.z, 0);
    v4 transformedNormal = (mM * (boneTransform * normal));
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

    v3 lightPosition(10, 10, -1);
    v4 lightPositionInTangentSpace = tangentTransform * (lightPosition - output.fragmentPosition);
    output.fragmentLightVector = v3(lightPositionInTangentSpace.x, lightPositionInTangentSpace.y, lightPositionInTangentSpace.z);


    v3 cameraPosition = v3(viewTransform.rows[0].w, viewTransform.rows[1].w, viewTransform.rows[2].w);
    v4 cameraVectorInTangentSpace = tangentTransform * (cameraPosition - output.fragmentPosition);
    output.fragmentCameraVector = v3(cameraVectorInTangentSpace.x, cameraVectorInTangentSpace.y, cameraVectorInTangentSpace.z);

    return output;
}

v4 Bitmap::sampleSubpixel(v3 uv, Bitmap* texture) {
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

v4 Bitmap::sample(v3 uv, Bitmap* texture) {
    if (texture->width == 0) {
        return v4(1, 1, 1, 1);
    }
    r32 tx = uv.x * (texture->width - 1);
    r32 ty = uv.y * (texture->height - 1);
    int texelX = tx;
    int texelY = ty;
    return texture->GetPixel(texelX, texelY);
}

v4 Bitmap::FragmentFunction(VertexOutput& o, Material* materials) {
    v3 position = o.fragmentPosition;
    Material* material = &materials[(int)(o.fragmentUV.z)];
    v3 normal;
    v4 diffuseColor;
    v3 pToL;
    if (material->normal.width == 0) {
        normal = o.fragmentNormal;
        v3 lightPosition(10, 10, -1);

        pToL = lightPosition - position;
        pToL = pToL.Normalized();
        r32 dot = Math::Dot(normal, pToL);
        dot = std::max(dot, 0.2f);

        diffuseColor = sample(o.fragmentUV, &material->diffuse) * dot;
    }
    else {
        v4 normalSample = sample(o.fragmentUV, &material->normal);
        normal = v3(normalSample.x, normalSample.y, normalSample.z);
        normal = normal * 2 - 1;

        pToL = o.fragmentLightVector;
        pToL = pToL.Normalized();
        r32 dot = Math::Dot(normal, pToL);
        dot = std::max(dot, 0.3f);

        diffuseColor = sample(o.fragmentUV, &material->diffuse) * dot;
    }

    // specular
    v3 invPToL = -pToL;
    v3 reflected = Math::Reflect(invPToL, normal).Normalized();
    v3 toCamera;
    if (material->normal.width == 0) {
        v3 cameraPosition = v3(viewTransform.rows[0].w, viewTransform.rows[1].w, viewTransform.rows[2].w);
        toCamera = (cameraPosition - position).Normalized();
    }
    else {
        toCamera = o.fragmentCameraVector.Normalized();
    }
    r32 similarity = Math::Dot(reflected, toCamera);
    similarity = std::pow(std::max(similarity, 0.0f), 128);

    v4 specularColor = v4(0, 0, 0, 0);
    if (material->roughness.width != 0) {
        specularColor = sample(o.fragmentUV, &material->roughness) * similarity;
    }
    //

    // emission
    v4 emissive(0, 0, 0, 0);
    if (material->emissive.width != 0) {
        emissive = sample(o.fragmentUV, &material->emissive);
    }
    
    //

    v4 ambientOcculion = sample(o.fragmentUV, &material->ambientOcclusion);
    v4 color = (diffuseColor + specularColor + emissive) * ambientOcculion;
    //normal = normal * 0.5 + 0.5;

    //v4 color = normal;

    return color;
}
