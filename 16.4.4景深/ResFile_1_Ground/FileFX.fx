//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 全部物体
//--------------------------------------------------------------------------------------

texture  g_ColorTexture;            // 颜色纹理
float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵
//
//定义聚焦平面,计算模型上的各顶点到聚焦平面的位置，即模糊程度
//像平面
float4 g_vFocalPlane = float4(0.0, 0.0, -1.0, 23.0);
float g_fFactor = 0.07;
const float g_MaxBlurFactor = 5.0f;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  		//模型顶点位置
    float2 TextureUV  : TEXCOORD0;  	//纹理坐标
  };

struct VS_OUTPUT
  {
    float4 Position   	: POSITION;   	//变换后的位置
	float4 Diffuse 		: COLOR0;		//输出的颜色纹理，模糊因子包含在ALPHA中
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标 
    
  };


// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
	Output.Diffuse.xyz = float3(1.0, 1.0, 1.0);
    //将顶点变换到光源为观察点的坐标空间,再在观察坐标系中计算模糊因子
    float4 v4PositionInView = mul( In.Position ,matWorldView );
	float fBlurFactor = abs(dot(v4PositionInView, g_vFocalPlane ) ) * g_fFactor;
	fBlurFactor = fBlurFactor * fBlurFactor;
	Output.Diffuse.a = min(fBlurFactor, g_MaxBlurFactor);
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	float4 vColor = tex2D(MeshTextureSampler, In.TextureUV ) * In.Diffuse;
	return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique RenderToTextureTech 
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }

technique DefaultTech
  {
 
  }

