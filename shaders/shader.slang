struct VSInput
{
    float2 position;
    float3 color;
};
    
struct VSOutput
{
    float4 position : SV_Position; 
    float3 color;
};

struct UniformBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 object;
};

ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
    VSOutput output;
    output.color = input.color;
    output.position = mul(ubo.object, mul(ubo.view, mul(ubo.model, float4(input.position, 0.0, 1.0))));
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput output) : SV_Target
{
    return float4(output.color, 1.0);
}

