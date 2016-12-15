cbuffer ConstantBuffer : register(b0) {
	matrix Model;
	matrix View;
	matrix Projection;
}

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
	output.pos = mul(float4(input.pos, 1.0f), Model);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.col = input.col;
	return output;
}