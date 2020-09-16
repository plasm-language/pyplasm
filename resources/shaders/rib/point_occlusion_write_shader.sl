surface point_occlusion_write_shader(string ptc_file = "default.ptc";string ptc_coordsys = "world"; )
{
	normal Nf = normalize(N);

	bake3d( ptc_file, "", P, Nf,"coordsystem", ptc_coordsys,"interpolate", 1 );
	Ci = Cs;
}