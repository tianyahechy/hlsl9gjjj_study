//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;
texture g_NormalTexture;
texture g_HeightTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4 	 g_vEyePosition;			//观察者位置
float4 	 g_vLightPosition;			//灯光位置

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshNormalSampler = sampler_state
  {
    Texture = <g_NormalTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
  
sampler MeshHeightSampler = sampler_state
  {
    Texture = <g_HeightTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   	: POSITION;   	//顶点位置
	float3 Normal		: NORMAL;		//顶点上的法向量
	float3 Tanget		: TANGENT;		//模型顶点的切线坐标
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标 	
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   	//变换后的顶点位置
    float2 TextureUV  : TEXCOORD1;  	//顶点上的纹理坐标
	float3 vLookAt	  : TEXCOORD3;		//模型上每个顶点指向观察者的方向
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
	//将世界坐标变换到切线空间的变换矩阵
	float3x3 matWorldToModel = float3x3( mul(In.Tanget, g_matWorld ).xyz, mul( cross(In.Tanget, In.Normal), g_matWorld ).xyz, mul(In.Normal, g_matWorld).xyz );
	float4 Position = mul(In.Position, g_matWorld );
	//每个顶点上指向观察者的方向
	Output.vLookAt = mul( matWorldToModel, normalize(Position - g_vEyePosition ));	
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	//计算视差贴图的偏移
	float3 viewDir = normalize(In.vLookAt);
	float2 offsetUV = In.TextureUV;
	float3 h = tex2D(MeshHeightSampler, In.TextureUV ).y;
	offsetUV += float2(-viewDir.x, viewDir.y) * ( h * 0.04 - 0.02 );
    return tex2D(MeshTextureSampler, offsetUV);
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
