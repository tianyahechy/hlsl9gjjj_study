//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 全部物体
//--------------------------------------------------------------------------------------

texture  g_ColorTexture;            // 颜色纹理
float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵

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
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标     
  };

// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	float4 vColor = tex2D(MeshTextureSampler, In.TextureUV );
	return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech 
  {
    pass P0
      {
		ClipPlaneEnable	= 3;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }


