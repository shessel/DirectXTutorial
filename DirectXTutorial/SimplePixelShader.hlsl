struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 col : COLOR0;
	float2 texCoords : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 main(PixelShaderInput input) : SV_TARGET {
	//return float4(input.col, 1.0f);
	return shaderTexture.Sample(SampleType, input.texCoords);
}