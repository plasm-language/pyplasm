surface occlusion_shader(varying point V = point(0,0,0) )
{
	point Vt = transform("world", "current", transform("camera", "object", V));
	normal Nn = normalize(N);
	Ci = 1-occlusion(Vt, Nn, 1024,"maxdist",1e18,"hitsides", "both");
	Oi = 1;
}