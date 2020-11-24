//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture_1;              // Color texture for mesh
texture g_ColorTexture_2;              // Color texture for mesh
texture g_ColorTexture_3;              // Color texture for mesh

float 	 g_fPI = 3.1415926;
float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

sampler MeshTextureSampler_1 = sampler_state
  {
    Texture = <g_ColorTexture_1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshTextureSampler_2 = sampler_state
  {
    Texture = <g_ColorTexture_2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
  
sampler MeshTextureSampler_3 = sampler_state
  {
    Texture = <g_ColorTexture_3>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float4 vColor_1 = tex2D( MeshTextureSampler_1 ,In.TextureUV ) * abs(sin(g_fTime - g_fTime * 2 / 3)) / 2;
    float4 vColor_2 = tex2D( MeshTextureSampler_2 ,In.TextureUV ) * abs(sin(g_fTime - g_fTime* 2 / 3)) / 2;
    float4 vColor_3 = tex2D( MeshTextureSampler_3 ,In.TextureUV ) * abs(sin(g_fTime)) / 2;
	float4 vColor = vColor_1 + vColor_2 + vColor_3;
    return vColor;
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
