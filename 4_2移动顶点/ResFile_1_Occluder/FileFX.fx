//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx
// �ڵ�����
//--------------------------------------------------------------------------------------

texture g_ColorTexture;             // ��ɫ����
texture g_RenderToTexture;          // ��Ⱦ��������������

float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����
//
float4x4 g_matWorldRTT;             // RTT ���������
float4x4 g_matViewRTT;              // RTT �Ĺ۲����
float4x4 g_matProjectRTT;           // RTT ��ͶӰ����

float  g_fNear = 10.0f;
float  g_fFar  = 30.0f;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshRTTSampler = sampler_state
  {
    Texture = <g_RenderToTexture>;
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
    float4 Position         : POSITION;   // �����û��۲�ռ��еĵ��λ��
    float2 TextureUV        : TEXCOORD0;  // ģ����������
    float4 PositionInLight  : TEXCOORD1;  // ���洦�ڵƹ�ռ��еĵ��λ��
    float2 Depth            : TEXCOORD2;  // 
    
  };

//-----------------------------------------------------------------------------
// ������TextureToFloat
// ���ܣ���ĳ��λ�õ����ȡ����
//-----------------------------------------------------------------------------
float TextureToFloat( float2 tex )
  {
    float4 fTexture  = tex2D( MeshRTTSampler ,tex ) ;
    float4 fFactor = float4( 16777216.0 / 16777216.0 ,65536.0 / 16777216.0 ,256.0 / 16777216.0 , 1 / 16777216.0 );
    float  fReturn = dot( fTexture , fFactor );
    return fReturn ;
  }

// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    // �����ı任����
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    // ������任����ԴΪ�۲�������ռ�
    float4x4 matLightWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matLightWorldViewProject = mul( matLightWorldView ,g_matProjectRTT );
    Output.PositionInLight = mul( In.Position ,matLightWorldViewProject );
    Output.Depth = 0;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    float fShadow = 1.0;
    // ȡ���λ��
    float fDistance = In.PositionInLight.z / g_fFar ;
    // ͶӰ�ռ�����ת��������ռ����ꡣҲ�����ҵ�ͶӰ�ռ��еĵ������ռ��еĵ�Ķ�Ӧ��ϵ��
    // �ӵ�ǰ��Ļ�����ص�ȡ�������꣬�ٴ�������ȡ���롣���ո��������������ϸ����ص��Ӧ�ľ��룬�� -1 �� +1 ת���� 0 �� 1
    float2 f2TextureUV = 0.5 * In.PositionInLight.xy / In.PositionInLight.w + float2( 0.5, 0.5 );
    f2TextureUV.y = 1.0f - f2TextureUV.y;
    
    float fDistanceMap = TextureToFloat( f2TextureUV );
    // ע��Ƚ�ʱ����Ƚ���Χ��������ȣ����������һ�������ȴ����趨��˵���ڱ�Ե�����Կ���ʹ����ɫ��ǳ����Ӱ
    if( fDistance - 0.009> fDistanceMap  ) // ��ʾ�ڵ�һ�������
      {
        fShadow = 0.4f;
      }
    float4 vColor = tex2D( MeshTextureSampler ,In.TextureUV ) * fShadow ;
    return vColor;
    
  }

//-----------------------------------------------------------------------------
// ��Ⱦ��ȵ������ VS �� PS
// ʹ�� R8G8B8 ����������������Ȼ�����
//-----------------------------------------------------------------------------
// ʹ��24λ����Ȼ�����
float4 FloatToTexture( float fIn ) 
  { 
    float fColor =  fIn ;
    // 11111111_11111111_11111111_11111111
    float fR ,fG ,fB ,fA;
    fColor = modf( fColor * 256 ,fR );
    fColor = modf( fColor * 256 ,fG );
    fColor = modf( fColor * 256 ,fB );
    fColor = modf( fColor * 256 , fA );
    return float4( fR / 256 , fG  / 256 , fB  / 256 , fA / 256 );
  }

VS_OUTPUT RenderDepthToTextureVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProjectRTT );
    Output.Position = mul( In.Position ,matWorldViewProject );
    // ����Ҫ��������
    Output.TextureUV = In.TextureUV;
    Output.Depth = Output.Position.zw;
    // 
    Output.PositionInLight = 0;
    return Output;    
  }

float4 RenderDepthToTexturePS( VS_OUTPUT In ) :COLOR0
  { 
    //float fDistance = ( In.Depth.x - g_fNear ) / ( g_fFar - g_fNear );
    float fDistance = In.Depth.x / g_fFar ;
    float4 vColor = FloatToTexture( fDistance );
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

technique RenderDepthToTextureShader
  {
    pass P0
      {
        FillMode = Solid;
        VertexShader = compile vs_2_0 RenderDepthToTextureVS( );
        PixelShader  = compile ps_2_0 RenderDepthToTexturePS( );
      }
  }

