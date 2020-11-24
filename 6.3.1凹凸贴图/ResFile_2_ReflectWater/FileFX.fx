//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------
texture 	g_ColorTexture;
texture3D	g_BumpTexture;
textureCUBE g_CubeTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverse;
float4 	 g_ViewPosition;

sampler MeshColorSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshBumpSampler = sampler_state
  {
    Texture = <g_BumpTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
  
samplerCUBE MeshCubeSampler = sampler_state
  {
    Texture = <g_CubeTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float3 Position   : POSITION;   
	float3 Normal	  : NORMAL;
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   			: POSITION;   
    float2 TextureUV  			: TEXCOORD0;  
	float3 ReflectInVertex	  	: TEXCOORD1;
	float3 Normal				: TEXCOORD2;
    
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( float4(In.Position, 1.0) ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
	Output.Normal = normalize( mul(In.Normal, (float3x3) g_matWorldNormalInverse ));
	float4 PositionInWorld = mul(float4(In.Position, 1.0 ), g_matWorld);
	//每个顶点上的入射向量，是从观察位置指向顶点，不是由顶点指向观察位置
	Output.ReflectInVertex = PositionInWorld.xyz - g_ViewPosition.xyz;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
  //定义纹理融合的几个因子
  float fColor = 0.3f;
  float fBumpVector = 0.12f;
  float3 fPos;
  fPos.x = g_fTime * 0.2 + In.TextureUV.x * 1.3;
  fPos.z = g_fTime * 0.3 + In.TextureUV.y * 1.4;
  fPos.y = 0;
  //取各个纹理
  float4 v4Color = tex2D( MeshColorSampler, In.TextureUV + float2(g_fTime/2.0, g_fTime / 3.0 ));
  float3 v3BumpVector = tex3D(MeshBumpSampler, fPos);
  float3 v3Bump = 2 * v3BumpVector - 1;
  v3Bump.xy *= 0.335;
  v3Bump.z = 0.3 * abs(v3Bump.z ) + 0.2;
  float3 vReflect = reflect(In.ReflectInVertex, In.Normal + v3Bump * fBumpVector );
  float4 v4RelfectColor = texCUBE(MeshCubeSampler, vReflect.xyz );
  float3 v3ColorOut =v4Color.xyz * fColor + v4RelfectColor.xyz * ( 1- fColor );
  return float4(v3ColorOut, 1.0 );
  
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode			= CCW;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
