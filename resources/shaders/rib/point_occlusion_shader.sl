
surface point_occlusion_shader(string ptc_file = "default.ptc";varying point V = point(0,0,0))
{
	point Vt = transform("world", "current", transform("camera", "object", V));
	normal Nn = normalize(N);
	Ci = 1-occlusion(Vt, Nn, "filename", ptc_file, "pointbased", 1, "bias", 0, "clamp", 1, "hitsides", "both" );
	Oi = 1;
}