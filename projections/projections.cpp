// projections.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <vector>

constexpr float pi = 3.14159265358979323846f;

struct rgb
{
	rgb() {};

	rgb(float rin, float gin, float bin) :
		r(rin), g(gin), b(bin) {}

	float r, g, b;
};

struct VertexColor
{
	VertexColor(float xin, float yin, float rin, float gin, float bin) 
		: x(xin), y(yin), r(rin), g(gin), b(bin){}

	VertexColor(float xin, float yin, rgb rgbin)
		: x(xin), y(yin), r(rgbin.r), g(rgbin.g), b(rgbin.b) {}

	float x, y;
	float r, g, b;
	
};


static const char* vertex_shader_text = R"(
#version 110
uniform mat4 MVP;
attribute vec3 vCol;
attribute vec2 vPos;
varying vec3 color;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vCol;
})";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


// used by grid - doesn't use color
static const char* grid_vertex_shader_text = R"(
#version 110
uniform mat4 MVP;
attribute vec2 vPos;
void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
})";

static const char* grid_fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(0.4, 0.4, 0.4, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


float ortho_l = -4.0;
float ortho_r = 4.0;
float ortho_t = -4.0;
float ortho_b = 4.0;

// num_ring_segments will affect how round the ring appears
// num_ring_segments_per_color_segment will make sure there isn't too many colors
std::vector<VertexColor> make_ring_vertices(float inner, float outer, int num_ring_segments,
	int num_ring_segments_per_color_segment,
	rgb color1,rgb color2)
{
	
	std::vector<VertexColor> ret;
	float radians_per_segment = 2.0f * pi / num_ring_segments;
	float starting_angle = 0;// -radians_per_segment / 2.0f;

	int current_segment = 0;
	
	int prev_color = -1;
	for (int current_segment = 0; current_segment < num_ring_segments; current_segment++)
	{
		int color;
		if (current_segment % (num_ring_segments_per_color_segment*2) < num_ring_segments_per_color_segment) 
		{
			color = 0;
		}
		else
		{
			color = 1;
		}	

		rgb rgb;
		if (color == 0)
			rgb = color1;
		else
			rgb = color2;


		float t =  starting_angle + radians_per_segment * current_segment;
		// push ab
		ret.push_back(VertexColor(inner*cos(t), inner*sin(t), rgb));
		ret.push_back(VertexColor(outer*cos(t), outer*sin(t), rgb));

		// push cd
		t = starting_angle + radians_per_segment * (current_segment+1);
		ret.push_back(VertexColor(outer*cos(t), outer*sin(t), rgb));
		ret.push_back(VertexColor(inner*cos(t), inner*sin(t), rgb));
	}

	return ret;
}

struct GridVertex
{
	GridVertex(float xin, float yin) :
		x(xin), y(yin) {}

	float x, y;
};


std::vector<GridVertex> make_grid_vertices()
{
	std::vector<GridVertex> ret = { 
	// horizontal lines 
	{-2.5, 2}, {2.5, 2},
	{-2.5, 1}, {2.5, 1},
	{-2.5, 0}, {2.5, 0},
	{-2.5, -1}, {2.5, -1},
	{-2.5, -2}, {2.5, -2},

	// vertical lines
	{-2, 2.5}, {-2, -2.5},
	{-1, 2.5}, {-1, -2.5},
	{0, 2.5}, {0, -2.5},
	{1, 2.5}, {1, -2.5},
	{2, 2.5}, {2, -2.5}

	};

	return ret;
}


class Grid
{
public:
	void Create();
	void Draw();

private:
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location;
	std::vector<GridVertex> vertices;
};

void Grid::Create()
{

	vertices = make_grid_vertices();

	glGenBuffers(1, &vertex_buffer);
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &grid_vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &grid_fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
}

void Grid::Draw()
{

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GridVertex), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);

	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	mat4x4_ortho(p, ortho_l, ortho_r, ortho_t, ortho_b, 10, -10);
	mat4x4 v;
	mat4x4_translate(v, -1, 0, 0);

	// make mvp 
	mat4x4 pv;
	mat4x4_mul(pv, p, v);
	mat4x4_mul(mvp, m, pv);
	glUseProgram(program);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
	glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size());
}

class Ring
{
public:
	void Create(int num_ring_segments, int num_ring_segments_per_color);
	void Draw();

private:
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	std::vector<VertexColor> vertices;
};

void Ring::Create(int num_ring_segments, int num_ring_segments_per_color)
{
	rgb color1 = { 28.0f / 256, 117.0f / 256, 138.0f / 256 };
	rgb color2 = { 88.0f / 256, 196.0f / 256, 221.0f / 256 };
	float thickness = 0.05f;
	vertices = make_ring_vertices(1 - thickness, 1, num_ring_segments, num_ring_segments_per_color, color1, color2);

	
	// shader setup
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	// array buffer of vertices
	glGenBuffers(1, &vertex_buffer);


}

void Ring::Draw()
{
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexColor), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)(sizeof(float) * 2));


	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	// set the perspective scale
	mat4x4_ortho(p, ortho_l, ortho_r, ortho_t, ortho_b, 10, -10);

	// setup the view matrix - ie shift x by -1
	mat4x4 v;
	mat4x4_translate(v, -1, 0, 0);

	// make mvp 
	mat4x4 pv;
	mat4x4_mul(pv, p, v);
	mat4x4_mul(mvp, m, pv);
	glUseProgram(program);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
	glDrawArrays(GL_QUADS, 0, (GLsizei)vertices.size());
}

// the lines from -1,0 to each of the angle offsets
class ProjectionLines
{
public:
	void Create(int num_ring_segments, int num_ring_segments_per_color);
	void Draw();

private:
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	std::vector<VertexColor> vertices;
};

void ProjectionLines::Create(int num_ring_segments, int num_ring_segments_per_color)
{
	std::vector<VertexColor> ret;
	int num_color_segments = num_ring_segments / num_ring_segments_per_color;
	float radians_per_segment = 2.0f * pi / num_color_segments;
	float starting_angle = 0;// -radians_per_segment / 2.0f;

	rgb rgb(1, 242.0f/256.0f, 0);
	float line_len =2.f;
	for (int current_segment = 0; current_segment < num_color_segments; current_segment++)
	{
		float t = starting_angle + radians_per_segment * current_segment;
		ret.push_back(VertexColor(-1.f, 0, rgb));
		ret.push_back(VertexColor(cos(t), sin(t), rgb));
	}
	vertices = ret;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	// array buffer of vertices
	glGenBuffers(1, &vertex_buffer);

}

void ProjectionLines::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexColor), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)(sizeof(float) * 2));


	mat4x4 m, p, mvp;
	mat4x4_identity(m);
	// set the perspective scale
	mat4x4_ortho(p, ortho_l, ortho_r, ortho_t, ortho_b, 10, -10);

	// setup the view matrix - ie shift x by -1
	mat4x4 v;
	mat4x4_translate(v, -1, 0, 0);

	// make mvp 
	mat4x4 pv;
	mat4x4_mul(pv, p, v);
	mat4x4_mul(mvp, m, pv);
	glUseProgram(program);
	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
	glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size());

}



int main(void)
{
	GLFWwindow* window;
	
	glfwSetErrorCallback(error_callback);

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glfwSwapInterval(1);

	Ring r;
	int num_ring_segments = 64;
	int num_segments_per_color = 4;
	r.Create(num_ring_segments, num_segments_per_color);

	ProjectionLines pl;
	pl.Create(num_ring_segments, num_segments_per_color);

	Grid g;
	g.Create();


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		g.Draw();
		r.Draw();
		pl.Draw();

		


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}