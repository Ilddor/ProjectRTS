struct VertexOut
{
    float4 position  : POSITION;
    float4 color : COLOR;
};

struct ConstantBufferLayout
{
    row_major float4x4 vpMatrix;
    row_major float4x4 worldMatrix;
};

ConstantBuffer<ConstantBufferLayout> vpwBuffer : register(b0);

VertexOut main(float3 position : POSITION, float4 color : COLOR)
{
    VertexOut vout;

    // Transform to homogeneous clip space.
    vout.position = float4(position, 1.f);
    vout.position = mul(vout.position, mul(vpwBuffer.vpMatrix, vpwBuffer.worldMatrix));

    // Just pass vertex color into the pixel shader.
    vout.color = color;
    //vout.color = vpwBuffer.worldMatrix[0][0];
    //vout.color = float4(1.f, 0.f, 1.f, 1.f);

    return vout;
}
