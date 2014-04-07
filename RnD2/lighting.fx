//=============================================================================
// lighting.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "lighthelper.fx"
 
cbuffer cbPerFrame
{
	Light gLight[11];
	int gLightType; 
	float3 gEyePosW;
	int gLightNum;
	int gGlow;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP;
	float4x4 gTexMtx;
};

Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float2  texC   : TEXCOORD;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
    float4 spec    : SPECULAR;
	float2  texC   : TEXCOORD;
};

VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
		
	// Transform to homogeneous clip space.
	vOut.posH    = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.diffuse = vIn.diffuse;
	vOut.spec    = vIn.spec;
	vOut.texC    = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);

	return vOut;
}
 
float4 PS(VS_OUT pIn) : SV_Target
{

	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);
   
	pIn.diffuse += gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
	pIn.spec    += gSpecMap.Sample( gTriLinearSam, pIn.texC );
   
	//pIn.spec.a *= 256.0f;

	float3 nNormalW = normalize(pIn.normalW);
    SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
    
    float3 litColor = {0.05f, 0.05f, 0.05f};
	if (gGlow == 2) return float4(float3(1.0f,1.0f,1.0f), pIn.diffuse.a);    
	
	//directed light for scene (sun)
	litColor += ParallelLight(v, gLight[0], gEyePosW);
	//
	//litColor += PointLight(v, gLight[1], gEyePosW);
	//rotating light
	
	litColor += Spotlight(v, gLight[2], gEyePosW);
	for(int i = 3; i < gLightNum; i++)
	{
		litColor += PointLight(v, gLight[i], gEyePosW);
	}

	
    return float4(litColor, pIn.diffuse.a);
}

technique10 LightTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}



