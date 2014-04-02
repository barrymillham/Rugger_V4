//=============================================================================
// lighting.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "lighthelper.fx"
 
cbuffer cbPerFrame
{
	Light gLight[7];
	int gLightType; 
	float3 gEyePosW;
	int gLightNum;
	
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
	AddressU = MIRROR;
	AddressV = MIRROR;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float2  uv	   : TEXCOORD;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
    float4 spec    : SPECULAR;
	float2  uv	   : TEXCOORD;
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
	vOut.uv      = mul(float4(vIn.uv, 0.0f, 1.0f), gTexMtx);

	return vOut;
}
 
float4 PS(VS_OUT pIn) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);
   
	float4 diffuse = gDiffuseMap.Sample( gTriLinearSam, pIn.uv );
	float4 spec    = gSpecMap.Sample( gTriLinearSam, pIn.uv );
   
    SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
    
    float3 litColor = {0.0f, 0.0f, 0.0f};
    
	litColor += PointLight(v, gLight[0], gEyePosW);
	litColor += PointLight(v, gLight[1], gEyePosW);
	litColor += Spotlight(v, gLight[2], gEyePosW);
	litColor += PointLight(v, gLight[3], gEyePosW);
	litColor += PointLight(v, gLight[4], gEyePosW);
	litColor += PointLight(v, gLight[5], gEyePosW);
	litColor += PointLight(v, gLight[6], gEyePosW);
	//litColor += PointLight(v, gLight[7], gEyePosW);
	   
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



