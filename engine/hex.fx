float hexDist(float2 p)
{
	p = abs(p);

	float c = dot(p, normalize(float2(1, 1.73)));
	c = max(c, p.x);

	return c;
}

#define mod(x, y) ((x) - (y) * floor(x / y))

float4 hexCoords(float2 uv, inout float2 n)
{
	float2 r = float2(1, 1.73);
	float2 h = r * 0.5;

	float2 a, b;

	a.x = mod(uv.x, r.x) - h.x;
	a.y = mod(uv.y, r.y) - h.y;

	b.x = mod((uv.x - h.x), r.x) - h.x;
	b.y = mod((uv.y - h.y), r.y) - h.y;

	float2 gv;
	if (length(a) < length(b)) gv = a;
	else gv = b;


	n = gv;

	float2 alt_uv;
	alt_uv.x = atan2(gv.x, gv.y);
	alt_uv.y =  0.5 - hexDist(gv);

	float2 id = uv - gv;


	return float4(alt_uv.xy, id.xy);
}