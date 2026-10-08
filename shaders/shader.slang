struct VSInput
{
    float3 color;
    float2 position;
};
    
struct VSOutput {
    float3 color;
    float4 position : SV_Position; 
};

[shader("vertex")]
VSOutput vertexMain(VSInput input) {
    VSOutput output;
    output.color = input.color;
    output.position = float4(input.position, 0.0, 1.0);
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput output) : SV_Target
{
    return float4(output.color, 1.0);
}

