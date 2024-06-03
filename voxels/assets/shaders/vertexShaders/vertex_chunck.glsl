#version 330 core

layout (location = 0) in uint caracteristic;

uniform int cx, cy, cz;

out VS_OUT{
	int blockType;
	int blockOrientation;
} vs_out;

void main() {
	// Decode caracteristic

	// x = (0 ... 32)
	// y = (0 ... 320)
	// z = (0 ... 32)
	// orientation = (0 ... 6);
	// type = (0 ... 256)

	int fn = int(caracteristic);
	int prod = 32 * 320 * 32 * 6;

	// type
	vs_out.blockType = fn / prod;
	fn = fn - vs_out.blockType * prod;
	prod /= 6;

	// orientation
	vs_out.blockOrientation = fn / prod;
	fn = fn - vs_out.blockOrientation * prod;
	prod /= 32;

	// z
	int z = fn / prod;
	fn = fn - z * prod;
	prod /= 320;

	// y
	int y = fn / prod;
	fn = fn - y * prod;
	prod /= 32;

	// x
	int x = fn;

	gl_Position = vec4(vec3(float(x + 32 * cx), float(y + 320 * cy), float(z + 32 * cz)), 1.0);
}