//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

textureCUBE g_CubeTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverse;

float4 g_ViewPosition;	//观察位置

sampler MeshCubeSampler = sampler_state
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
    float3 Position   	: POSITION;   	//顶点位置
	float3 Normal		: NORMAL;		//模型顶点的位置
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标 	
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   	//变换后的顶点位置
	float2 TextureUV  : TEXCOORD0;		//模型纹理坐标
	float3 ReflectInVertex : TEXCOORD1;	//每个顶点上入射向量
	float3 Normal	  : TEXCOORD2;		//模型顶点上的法向量
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( float4(In.Position, 1.0 ) ,matWorldViewProject );
	Output.TextureUV = In.TextureUV;
	//计算法向量
	Output.Normal = normalize(mul(In.Normal, (float3x3)g_matWorld ));
	//计算模型的顶点在世界空间上的位置
	float4 PositionInWorld = mul(float4( In.Position, 1.0), g_matWorld );
	//每个顶点上入射向量，是从观察位置指向顶点，不是由顶点指向观察位置
	Output.ReflectInVertex = PositionInWorld.xyz - g_ViewPosition.xyz;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	//计算每个顶点上指向观察位置的反射向量，此向量指向立方体贴图，实际上作为纹理坐标
	float3 vReflect = refract(In.ReflectInVertex, In.Normal, 0.1);
	float4 vColor = texCUBE(MeshCubeSampler, vReflect.xyz );
    return float4(vColor.xyz, 1.0 );
  }

float4 RenderScenePS_Line( VS_OUTPUT In ) :COLOR0
  { 
    // 
    float4 vColor = float4( 1.0 , 0.0 , 0.0 , 1.0 );
    return vColor;
  }
//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        CullMode           = CCW;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
    /*
    pass P1
      {
        CullMode           = CCW;
        FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS_Line( );
      }
      */
  }
