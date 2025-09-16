sampler2D TS0 : register(s0);
float4 gradient_max_color : register(c0);
float4 gradient_min_color : register(c1);
float4 background_color : register(c2);
float4 draining_color : register(c3);
float4 mask_color : register(c4);

float4 main(float2 texCoord : TEXCOORD0) : COLOR0 {
    float4 tex_point = tex2D(TS0, texCoord);
    float alpha_factor = 0.3;
    float meter_state = gradient_min_color.a;
    float drained_fraction = draining_color.a;
    float flipped_channels = gradient_max_color.a;

    if(flipped_channels.x > 0.5) {
        float4 inv_tex_point = tex_point;
        tex_point = float4(inv_tex_point.aaa, inv_tex_point.r);
    }

    clip(tex_point.r - alpha_factor);

    float4 tint_color;
    if(tex_point.a <= meter_state) {
        if(tex_point.a <= drained_fraction) {
            float3 gradient_color = lerp(gradient_min_color.rgb, gradient_max_color.rgb, tex_point.a);
            if(drained_fraction != meter_state) {
                tint_color.rgb = lerp(draining_color.rgb, gradient_color.rgb, saturate((drained_fraction - tex_point.a) / 0.2));
            }
            else {
                tint_color.rgb = gradient_color.rgb;
            }
        }
        else {
            tint_color.rgb = draining_color.rgb;
        }
        tint_color.a = mask_color.a;
    }
    else {
        float background_color_alpha = max(background_color.r, max(background_color.g, max(background_color.b, background_color.a)));
        tint_color.rgb = background_color.rgb;
        tint_color.a = mask_color.a * background_color_alpha;
    }

    float4 res;
    res.rgb = tex_point.rgb * tint_color.rgb * mask_color.rgb;
    res.a = tint_color.a;

    return res;
};
