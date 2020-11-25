//传入的纹理
texture g_texTextureIn;
//传入的窗口大小
float4 g_v4ScreenSize;
//此12个点组成的采样点
float2 g_v2TwelveKernelBase[12] =
{
	//内圈的距离是1
	{1.0f, 0.0f}, { 0.5f, 0.866f }, { -0.5f, 0.866f},
	{-1.0f, 0.0f }, { -0.5f, -0.866f}, { 0.5f, -0.866f},
	//外圈的距离是1.732
	{1.5f, 	0.8660f }, { 0.0f, 1.7320f }, { -1.5f, 0.866f},
	{-1.5f, -0.866f	}, { 0.0f, -1.7320f}, { 1.5f, -0.866f},
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

sampler TextureIn = sampler_state
{
	Texture = ( g_texTextureIn );
	MipFilter = LINEAR;
	MagFilter = LINEAR;
	ADDRESSU = MIRROR;
	ADDRESSV = MIRROR;
};

VS_OUTPUT MainVS_Screen(VS_INPUT In )
{
	VS_OUTPUT Out = ( VS_OUTPUT ) 0;
	In.pos.x = sign(In.pos.x );
	In.pos.y = sign(In.pos.y );
	Out.pos = float4( In.pos.x, In.pos.y, 0.9, 1.0 );
	Out.tex = In.tex;
	Out.color = In.color;
	return Out;
}

//景深
float4 MainPS_Screen(VS_OUTPUT In) : COLOR
{
	//使用12个点来模糊像素
	#define KERNELNUM	12
	float2 v4ScreenSize = g_v4ScreenSize / 5.0;
	float4 v4Original = tex2D( TextureIn, In.tex );
	float3 v3Blurred = 0;
	for( int i = 0; i < KERNELNUM; i++)
	{
		//纹理坐标的偏移
		float2 v2Offset = float2(g_v2TwelveKernelBase[i].x / v4ScreenSize.x, g_v2TwelveKernelBase[i].y / v4ScreenSize.y);
		//按纹理坐标偏移取此像素颜色值
		float4 v4Current = tex2D(TextureIn, In.tex + v2Offset);
		//模糊因子按照线性方式模糊
		v3Blurred += lerp(v4Original.rgb, v4Current.rgb, saturate(v4Original.a * v4Original.a));
		
	}
	return float4( v3Blurred / KERNELNUM, 1.0f );
}

float4 MainPS_Screen_Line(VS_OUTPUT In ) : COLOR
{
	return float4( 1.0, 1.0, 0.0, 0.0 );
}

technique DepthOfFieldTech
{
	pass P0
	{
		FillMode = Solid;
		CullMode = CCW;
		VertexShader = compile vs_2_0 MainVS_Screen();
		PixelShader = compile ps_2_0 MainPS_Screen();
	}
}