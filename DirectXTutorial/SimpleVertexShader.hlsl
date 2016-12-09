struct VertexShaderInput {
	float3 pos : POSITION;
	float3 col : COLOR0;
};

struct PixelShaderInput {
	float4 pos : SV_POSITION;
	float3 col : COLOR0;
};

PixelShaderInput main(VertexShaderInput input) {
	PixelShaderInput output;
	output.pos = float4(input.pos, 1.0f);
	output.col = input.col;
	return output;
}