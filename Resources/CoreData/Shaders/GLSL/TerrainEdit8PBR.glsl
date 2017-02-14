#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"
#include "Constants.glsl"
#include "Fog.glsl"
#include "PBR.glsl"
#include "IBL.glsl"

#ifndef GL_ES
varying vec3 vDetailTexCoord;
#else
varying mediump vec3 vDetailTexCoord;
#endif

#if defined(NORMALMAP) || defined(IBL)
    varying vec4 vTexCoord;
    varying vec4 vTangent;
#else
    varying vec2 vTexCoord;
#endif
varying vec3 vNormal;
varying vec4 vWorldPos;
#ifdef VERTEXCOLOR
    varying vec4 vColor;
#endif
#ifdef PERPIXEL
    #ifdef SHADOW
        #ifndef GL_ES
            varying vec4 vShadowPos[NUMCASCADES];
        #else
            varying highp vec4 vShadowPos[NUMCASCADES];
        #endif
    #endif
    #ifdef SPOTLIGHT
        varying vec4 vSpotPos;
    #endif
    #ifdef POINTLIGHT
        varying vec3 vCubeMaskVec;
    #endif
#else
    varying vec3 vVertexLight;
    varying vec4 vScreenPos;
    #ifdef ENVCUBEMAP
        varying vec3 vReflectionVec;
    #endif
    #if defined(LIGHTMAP) || defined(AO)
        varying vec2 vTexCoord2;
    #endif
#endif

//Standard terrain textures
// uniform sampler2D sWeightMap0;
// uniform sampler2D sDetailMap1;
// uniform sampler2D sDetailMap2;
// uniform sampler2D sDetailMap3;

// #ifndef GL_ES
// uniform vec2 cDetailTiling;
// #else
// uniform mediump vec2 cDetailTiling;
// #endif

//##################################################################
//START TERRAIN MULTITEXTURE
//##################################################################
uniform sampler2D sWeightMap0;
uniform sampler2D sWeightMap1;
uniform sampler2DArray sDetailMap2;

#ifdef NORMALMAP
	uniform sampler2DArray sNormal3;
#endif

#ifdef USEMASKTEXTURE
	uniform sampler2D sMask4;
#endif

uniform vec3 cDetailTiling;
uniform vec4 cLayerScaling1;
uniform vec4 cLayerScaling2;

//##################################################################
//END TERRAIN MULTITEXTURE
//##################################################################

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vNormal = GetWorldNormal(modelMatrix);
    vWorldPos = vec4(worldPos, GetDepth(gl_Position));

    #ifdef VERTEXCOLOR
        vColor = iColor;
    #endif

    #if defined(NORMALMAP) || defined(DIRBILLBOARD) || defined(IBL)
        vec4 tangent = GetWorldTangent(modelMatrix);
        vec3 bitangent = cross(tangent.xyz, vNormal) * tangent.w;
        vTexCoord = vec4(GetTexCoord(iTexCoord), bitangent.xy);
        vTangent = vec4(tangent.xyz, bitangent.z);
    #else
        vTexCoord = GetTexCoord(iTexCoord);
    #endif

    vDetailTexCoord = worldPos.xyz / cDetailTiling;

    #ifdef PERPIXEL
        // Per-pixel forward lighting
        vec4 projWorldPos = vec4(worldPos, 1.0);

        #ifdef SHADOW
            // Shadow projection: transform from world space to shadow space
            for (int i = 0; i < NUMCASCADES; i++)
                vShadowPos[i] = GetShadowPos(i, vNormal, projWorldPos);
        #endif

        #ifdef SPOTLIGHT
            // Spotlight projection: transform from world space to projector texture coordinates
            vSpotPos = projWorldPos * cLightMatrices[0];
        #endif

        #ifdef POINTLIGHT
            vCubeMaskVec = (worldPos - cLightPos.xyz) * mat3(cLightMatrices[0][0].xyz, cLightMatrices[0][1].xyz, cLightMatrices[0][2].xyz);
        #endif
    #else
        // Ambient & per-vertex lighting
        #if defined(LIGHTMAP) || defined(AO)
            // If using lightmap, disregard zone ambient light
            // If using AO, calculate ambient in the PS
            vVertexLight = vec3(0.0, 0.0, 0.0);
            vTexCoord2 = iTexCoord1;
        #else
            vVertexLight = GetAmbient(GetZonePos(worldPos));
        #endif

        #ifdef NUMVERTEXLIGHTS
            for (int i = 0; i < NUMVERTEXLIGHTS; ++i)
                vVertexLight += GetVertexLight(i, worldPos, vNormal) * cVertexLights[i * 3].rgb;
        #endif

        vScreenPos = GetScreenPos(gl_Position);

        #ifdef ENVCUBEMAP
            vReflectionVec = worldPos - cCameraPos;
        #endif
    #endif
}

void PS()
{

    //####################################################################
    //BEGIN MULTITEXTURE
    //####################################################################
     // Get material diffuse albedo
    vec4 weights0 = texture(sWeightMap0, vTexCoord.xy);
	vec4 weights1 = texture(sWeightMap1, vTexCoord.xy);
	
	#ifdef USEMASKTEXTURE
		float mask=texture(sMask4, vTexCoord.xy).r;
	#endif
	
	#ifdef TRIPLANAR
	
	vec3 nrm = normalize(vNormal);
	vec3 blending=abs(nrm);
	blending = normalize(max(blending, 0.00001));
	float b=blending.x+blending.y+blending.z;
	blending=blending/b;
	
	vec4 tex1=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling1.r, 0))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling1.r, 0))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.r, 0))*blending.y;
	vec4 tex2=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling1.g, 1))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling1.g, 1))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.g, 1))*blending.y;
	vec4 tex3=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling1.b, 2))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling1.b, 2))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.b, 2))*blending.y;
	vec4 tex4=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling1.a, 3))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling1.a, 3))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.a, 3))*blending.y;
	vec4 tex5=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling2.r, 4))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling2.r, 4))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.r, 4))*blending.y;
	vec4 tex6=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling2.g, 5))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling2.g, 5))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.g, 5))*blending.y;
	vec4 tex7=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling2.b, 6))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling2.b, 6))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.b, 6))*blending.y;
	vec4 tex8=texture(sDetailMap2, vec3(vDetailTexCoord.zy*cLayerScaling2.a, 7))*blending.x +
		texture(sDetailMap2, vec3(vDetailTexCoord.xy*cLayerScaling2.a, 7))*blending.z +
		texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.a, 7))*blending.y;
	
	#else
		vec4 tex1=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.r, 0));
		vec4 tex2=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.g, 1));
		vec4 tex3=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.b, 2));
		vec4 tex4=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling1.a, 3));
		vec4 tex5=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.r, 4));
		vec4 tex6=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.g, 5));
		vec4 tex7=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.b, 6));
		vec4 tex8=texture(sDetailMap2, vec3(vDetailTexCoord.xz*cLayerScaling2.a, 7));
	#endif
	
	#ifndef SMOOTHBLEND
		float ma=max(tex1.a+weights0.r, max(tex2.a+weights0.g, max(tex3.a+weights0.b, max(tex4.a+weights0.a, max(tex5.a+weights1.r, max(tex6.a+weights1.g, max(tex7.a+weights1.b, tex8.a+weights1.a)))))))-0.2;
		float b1=max(0, tex1.a+weights0.r-ma);
		float b2=max(0, tex2.a+weights0.g-ma);
		float b3=max(0, tex3.a+weights0.b-ma);
		float b4=max(0, tex4.a+weights0.a-ma);
		float b5=max(0, tex5.a+weights1.r-ma);
		float b6=max(0, tex6.a+weights1.g-ma);
		float b7=max(0, tex7.a+weights1.b-ma);
		float b8=max(0, tex8.a+weights1.a-ma);
	#else
		float b1=weights0.r;
		float b2=weights0.g;
		float b3=weights0.b;
		float b4=weights0.a;
		float b5=weights1.r;
		float b6=weights1.g;
		float b7=weights1.b;
		float b8=weights1.a;
	#endif
		float bsum=b1+b2+b3+b4+b5+b6+b7+b8;
	vec4 diffColor=(tex1*b1+tex2*b2+tex3*b3+tex4*b4+tex5*b5+tex6*b6+tex7*b7+tex8*b8)/bsum;
	//vec4 diffColor=tex1;
	
	#ifdef USEMASKTEXTURE
	diffColor=mix(vec4(1,0.5,0.3, diffColor.a), diffColor, mask);
	#endif

        // Get material specular albedo
    //vec3 specColor = cMatSpecColor.rgb;

    //####################################################################
    //END MULTITEXTURE
    //####################################################################




    #ifdef VERTEXCOLOR
        diffColor *= vColor;
    #endif

    #ifdef METALLIC
        vec4 roughMetalSrc = texture2D(sSpecMap, vTexCoord.xy);

        float roughness = roughMetalSrc.r + cRoughness;
        float metalness = roughMetalSrc.g + cMetallic;
    #else
        float roughness = cRoughness;
        float metalness = cMetallic;
    #endif

    roughness *= roughness;

    roughness = clamp(roughness, ROUGHNESS_FLOOR, 1.0);
    metalness = clamp(metalness, METALNESS_FLOOR, 1.0);

    vec3 specColor = mix(0.08 * cMatSpecColor.rgb, diffColor.rgb, metalness);
    diffColor.rgb = diffColor.rgb - diffColor.rgb * metalness;


    //####################################################################
    //BEGIN MULTITEXTURE
    //####################################################################
    // Get normal
    #if defined(NORMALMAP) || defined(DIRBILLBOARD) || defined(IBL)
        vec3 tangent = vTangent.xyz;
        vec3 bitangent = vec3(vTexCoord.zw, vTangent.w);
        mat3 tbn = mat3(tangent, bitangent, vNormal);
    #endif
    #ifdef NORMALMAP
        // mat3 tbn = mat3(vTangent.xyz, vec3(vTexCoord.zw, vTangent.w), vNormal);
        #ifdef TRIPLANAR
		vec3 bump1=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling1.r, 0)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling1.r, 0)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.r,0)))*blending.y;
		vec3 bump2=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling1.g, 1)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling1.g, 1)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.g,1)))*blending.y;
		vec3 bump3=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling1.b, 2)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling1.b, 2)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.b,2)))*blending.y;
		vec3 bump4=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling1.a, 3)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling1.a, 3)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.a,3)))*blending.y;
		vec3 bump5=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling2.r, 4)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling2.r, 4)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.r,4)))*blending.y;
		vec3 bump6=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling2.g, 5)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling2.g, 5)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.g,5)))*blending.y;
		vec3 bump7=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling2.b, 6)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling2.b, 6)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.b,6)))*blending.y;
		vec3 bump8=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.zy*cLayerScaling2.a, 7)))*blending.x+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xy*cLayerScaling2.a, 7)))*blending.z+
			DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.a,7)))*blending.y;
		#else
			vec3 bump1=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.r,0)));
			vec3 bump2=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.g,1)));
			vec3 bump3=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.b,2)));
			vec3 bump4=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling1.a,3)));
			vec3 bump5=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.r,4)));
			vec3 bump6=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.g,5)));
			vec3 bump7=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.b,6)));
			vec3 bump8=DecodeNormal(texture(sNormal3, vec3(vDetailTexCoord.xz*cLayerScaling2.a,7)));
		#endif
		vec3 normal=tbn*normalize(((bump1*b1+bump2*b2+bump3*b3+bump4*b4+bump5*b5+bump6*b6+bump7*b7+bump8*b8)/bsum));
    #else
        vec3 normal = normalize(vNormal);
    #endif
    //####################################################################
    //END MULTITEXTURE
    //####################################################################


    // Get fog factor
    #ifdef HEIGHTFOG
        float fogFactor = GetHeightFogFactor(vWorldPos.w, vWorldPos.y);
    #else
        float fogFactor = GetFogFactor(vWorldPos.w);
    #endif

    #if defined(PERPIXEL)
        // Per-pixel forward lighting
        vec3 lightColor;
        vec3 lightDir;
        vec3 finalColor;

        float atten = GetAtten(normal, vWorldPos.xyz, lightDir);
        float shadow = 1.0;
        #ifdef SHADOW
            shadow = GetShadow(vShadowPos, vWorldPos.w);
        #endif

        #if defined(SPOTLIGHT)
            lightColor = vSpotPos.w > 0.0 ? texture2DProj(sLightSpotMap, vSpotPos).rgb * cLightColor.rgb : vec3(0.0, 0.0, 0.0);
        #elif defined(CUBEMASK)
            lightColor = textureCube(sLightCubeMap, vCubeMaskVec).rgb * cLightColor.rgb;
        #else
            lightColor = cLightColor.rgb;
        #endif
        vec3 toCamera = normalize(cCameraPosPS - vWorldPos.xyz);
        vec3 lightVec = normalize(lightDir);
        float ndl = clamp((dot(normal, lightVec)), M_EPSILON, 1.0);

        vec3 BRDF = GetBRDF(lightDir, lightVec, toCamera, normal, roughness, diffColor.rgb, specColor);

        finalColor.rgb = BRDF * lightColor * (atten * shadow) / M_PI;

       // finalColor.rgb = diffColor.rgb  * lightColor * (atten * shadow) / M_PI;
      //  finalColor.rgb = pow(BRDF * lightColor * (atten * shadow) / M_PI, vec3(1.0/ 2.2,1.0/ 2.2,1.0/ 2.2));

        #ifdef AMBIENT
            finalColor += cAmbientColor.rgb * diffColor.rgb;
            finalColor += cMatEmissiveColor;
            gl_FragColor = vec4(GetFog(finalColor, fogFactor), diffColor.a);
        #else
            gl_FragColor = vec4(GetLitFog(finalColor, fogFactor), diffColor.a);
        #endif
    #elif defined(DEFERRED)
        // Fill deferred G-buffer
        const vec3 spareData = vec3(0,0,0); // Can be used to pass more data to deferred renderer
        gl_FragData[0] = vec4(specColor, spareData.r);
        gl_FragData[1] = vec4(diffColor.rgb, spareData.g);
        gl_FragData[2] = vec4(normal * roughness, spareData.b);
        gl_FragData[3] = vec4(EncodeDepth(vWorldPos.w), 0.0);
    #else
        // Ambient & per-vertex lighting
        vec3 finalColor = vVertexLight * diffColor.rgb;
        #ifdef AO
            // If using AO, the vertex light ambient is black, calculate occluded ambient here
            finalColor += texture2D(sEmissiveMap, vTexCoord2).rgb * cAmbientColor.rgb * diffColor.rgb;
        #endif

        #ifdef MATERIAL
            // Add light pre-pass accumulation result
            // Lights are accumulated at half intensity. Bring back to full intensity now
            vec4 lightInput = 2.0 * texture2DProj(sLightBuffer, vScreenPos);
            vec3 lightSpecColor = lightInput.a * lightInput.rgb / max(GetIntensity(lightInput.rgb), 0.001);

            finalColor += lightInput.rgb * diffColor.rgb + lightSpecColor * specColor;
        #endif

        vec3 toCamera = normalize(vWorldPos.xyz - cCameraPosPS);
        vec3 reflection = normalize(reflect(toCamera, normal));

        vec3 cubeColor = vVertexLight.rgb;

        #ifdef IBL
          vec3 iblColor = ImageBasedLighting(reflection, tangent, bitangent, normal, toCamera, diffColor.rgb, specColor.rgb, roughness, cubeColor);
          float gamma = 0.0;
          finalColor.rgb += iblColor;
        #endif

        #ifdef ENVCUBEMAP
            finalColor += cMatEnvMapColor * textureCube(sEnvCubeMap, reflect(vReflectionVec, normal)).rgb;
        #endif
        #ifdef LIGHTMAP
            finalColor += texture2D(sEmissiveMap, vTexCoord2).rgb * diffColor.rgb;
        #endif
        #ifdef EMISSIVEMAP
            finalColor += cMatEmissiveColor * texture2D(sEmissiveMap, vTexCoord.xy).rgb;
        #else
            finalColor += cMatEmissiveColor;
        #endif

       gl_FragColor = vec4(GetFog(finalColor, fogFactor), diffColor.a);



        // gl_FragColor =  vec4(pow(((1-fogFactor) * diffColor.rgb + fogFactor * finalColor.rgb),vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), diffColor.a);
        //gl_FragColor =  vec4(pow(GetExpFog(finalColor.rgb, diffColor.rgb, fogFactor),vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), diffColor.a);
       // gl_FragColor = vec4(pow(GetFog(finalColor, fogFactor), vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), diffColor.a);

    #endif
}
