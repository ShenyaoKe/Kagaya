#include "OGLViewer.h"

#include "Core/ProgressBar.h"
#include "Geometry/Mesh.h"
#include "IO/ObjLoader.h"
#include "IO/SceneLoader.h"
#include <embree3/rtcore_ray.h>
#include <QImage>

namespace Kaguya
{

const int OGLViewer::sResolutionGateVertexCount = 4;

OGLViewer::OGLViewer(QWidget* parent)
	: QOpenGLWidget(parent)
	, selectMode(OBJECT_SELECT)
	, mRenderBuffer(new RenderBuffer(default_resX, default_resY))
	, mScene(SceneLoader::load("scene/unitest_scene.json"))
{
	// Set surface format for current widget
	QSurfaceFormat format;
	format.setDepthBufferSize(32);
	format.setStencilBufferSize(8);
	//format.setSamples(16);
	format.setVersion(4, 5);
	//format.setProfile(QSurfaceFormat::CoreProfile);
	this->setFormat(format);

	// Get projective camera
	view_cam = std::static_pointer_cast<ProjectiveCamera>(mScene->getCamera());
	// Commit scene
	mScene->commitScene();
}

OGLViewer::~OGLViewer()
{
}

/************************************************************************/
/* OpenGL Rendering Modules                                             */
/************************************************************************/
void OGLViewer::initializeGL()
{
	// OpenGL extension initialization
	glewInit();

	// Print OpenGL version
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	std::cout << "OpenGL version supported " << glGetString(GL_VERSION) << endl;

	// Enable OpenGL features
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front

	//////////////////////////////////////////////////////////////////////////

	// Create model_shader files
	//#ifdef _DEBUG
	triShader = std::make_shared<GLSLProgram>("resources/shaders/TriangleMesh_vs.glsl",
											  "resources/shaders/TriangleMesh_fs.glsl",
											  "resources/shaders/TriangleMesh_gs.glsl");
	quadShader = std::make_shared<GLSLProgram>("resources/shaders/QuadMesh_vs.glsl",
											   "resources/shaders/QuadMesh_fs.glsl",
											   "resources/shaders/QuadMesh_gs.glsl");
	curveShader = std::make_shared<GLSLProgram>("resources/shaders/Curve_vs.glsl",
												"resources/shaders/Curve_fs.glsl",
												"resources/shaders/Curve_gs.glsl");
	gate_shader = std::make_unique<GLSLProgram>("resources/shaders/gate_vs.glsl",
												"resources/shaders/gate_fs.glsl",
												"resources/shaders/gate_gs.glsl");

	for (size_t i = 0; i < mScene->getPrimitiveCount(); i++)
	{
		mRBOs.emplace_back(createRenderObject(mScene->getRenderBuffer(i)));
	}

	gate_shader->add_uniformv("transform");
	bindReslotionGate();
}

RenderBufferObject OGLViewer::createRenderObject(const RenderBufferTrait &trait)
{
	RenderBufferObject ret;

	// VBO
	glCreateBuffers(1, &ret.vbo);
	glNamedBufferData(ret.vbo, trait.vertex.size, trait.vertex.data, GL_STATIC_DRAW);
	// IBO
	glCreateBuffers(1, &ret.ibo);
	glNamedBufferData(ret.ibo, trait.index.size, trait.index.data, GL_STATIC_DRAW);
	ret.indexCount = trait.index.count;

	// Bind VAO
	glCreateVertexArrays(1, &ret.vao);
	glEnableVertexArrayAttrib(ret.vao, 0);

	// Attach VBO and IBO to VAO
	glVertexArrayAttribFormat(ret.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(ret.vao, 0, ret.vbo, trait.vertex.offset, trait.vertex.stride);
	glVertexArrayAttribBinding(ret.vao, 0, 0);
	glVertexArrayElementBuffer(ret.vao, ret.ibo);

	switch (trait.renderType)
	{
	case GPURenderType::CURVE:
	{
		ret.primMode = GL_PATCHES;
		break;
	}
	case GPURenderType::TRIANGLE:
	{
		ret.primMode = GL_TRIANGLES;
		ret.shader = triShader;
		ret.patchSize = 3;
		break;
	}
	case GPURenderType::QUAD:
	{
		ret.primMode = GL_LINES_ADJACENCY;
		ret.shader = quadShader;
		ret.patchSize = 4;
		break;
	}
	default:
		break;
	}

	return ret;
}

void OGLViewer::bindReslotionGate()
{
	const GLfloat filmWidth = view_cam->getFilm().width;
	const GLfloat filmHeight = view_cam->getFilm().height;
	const std::array<Point2f, sResolutionGateVertexCount> resoluationGate{
		Point2f(0, 0), Point2f(filmWidth, 0),
		Point2f(filmWidth, filmHeight), Point2f(0, filmHeight) };
	printf("Film of (%f, %f)\n", filmWidth, filmHeight);
	// DSA
	// Create VAO
	glCreateBuffers(1, &resgate_vbo);
	glNamedBufferData(resgate_vbo,
					  sizeof(Point2f) * resoluationGate.size(),
					  resoluationGate.data(),
					  GL_STATIC_DRAW);

	// VAO
	glCreateVertexArrays(1, &resgate_vao);
	glEnableVertexArrayAttrib(resgate_vao, 0);

	// Setup the formats
	glVertexArrayAttribFormat(resgate_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayVertexBuffer(resgate_vao,
							  0,
							  resgate_vbo,
							  0,
							  sizeof(Point2f));
	glVertexArrayAttribBinding(resgate_vao, 0, 0);
}

void OGLViewer::paintGL()
{
	// Make current window
	makeCurrent();
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	// Clear background and color buffer
	glClearColor(0.6, 0.6, 0.6, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// Model
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // cull back face
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (auto &rbo : mRBOs)
	{
		glBindVertexArray(rbo.vao);
		rbo.shader->use_program();
		// Apply uniform matrix
		glUniformMatrix4fv((*rbo.shader)["view_matrix"], 1, GL_FALSE,
						   view_cam->world_to_cam());// View Matrix
		glUniformMatrix4fv((*rbo.shader)["proj_matrix"], 1, GL_FALSE,
						   view_cam->cam_to_screen());// Projection
		glDrawElements(rbo.primMode, rbo.indexCount, GL_UNSIGNED_INT, 0);
	}

	////////////////////////////////////////////
	glBindVertexArray(resgate_vao);
	gate_shader->use_program();
	glUniformMatrix4fv((*gate_shader)("transform"), 1, GL_FALSE,
					   view_cam->raster_to_screen());

	glLineWidth(2.0);
	glDrawArrays(GL_LINE_LOOP, 0, sResolutionGateVertexCount);

	glBindVertexArray(0);
	gate_shader->unuse();

	doneCurrent();
}
// Resize function
void OGLViewer::resizeGL(int w, int h)
{
	// Widget resize operations
	view_cam->resizeViewport(width() / static_cast<Float>(height()));
}
/************************************************************************/
/* Qt User Operation Functions                                          */
/************************************************************************/

void OGLViewer::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Home)
	{
		//initParas();
	}
	// Save frame buffer
	else if (e->key() == Qt::Key_P &&
			 e->modifiers() == Qt::ControlModifier)
	{
		this->saveFrameBuffer();
	}
	// Render
	else if (e->key() == Qt::Key_R &&
			 e->modifiers() == Qt::ControlModifier)
	{
		this->renderPixels();
	}
	else
	{
		QOpenGLWidget::keyPressEvent(e);
	}
	update();
}

void OGLViewer::mousePressEvent(QMouseEvent* e)
{
	lastMousePos[0] = e->x();
	lastMousePos[1] = e->y();
	if (e->buttons() == Qt::LeftButton &&
		e->modifiers() == Qt::AltModifier)
	{
		// Do something here
	}
};

void OGLViewer::mouseReleaseEvent(QMouseEvent* e)
{
	lastMousePos[0] = e->x();
	lastMousePos[1] = e->y();
}

void OGLViewer::mouseMoveEvent(QMouseEvent* e)
{
	int dx = e->x() - lastMousePos[0];
	int dy = e->y() - lastMousePos[1];

	//printf("dx: %d, dy: %d\n", dx, dy);

	if (e->buttons() == Qt::LeftButton &&
		e->modifiers() == Qt::AltModifier)
	{
		view_cam->rotate(dy * 0.25, -dx * 0.25, 0.0);
		update();
	}
	else if (e->buttons() == Qt::RightButton &&
			 e->modifiers() == Qt::AltModifier)
	{
		if (dx != e->x() && dy != e->y())
		{
			view_cam->zoom(0.0, 0.0, dx * 0.05);
			update();
		}
	}
	else if (e->buttons() == Qt::MidButton &&
			 e->modifiers() == Qt::AltModifier)
	{
		if (dx != e->x() && dy != e->y())
		{
			view_cam->zoom(-dx * 0.05, dy * 0.05, 0.0);
			update();
		}
	}
	else
	{
		QOpenGLWidget::mouseMoveEvent(e);
	}

	lastMousePos[0] = e->x();
	lastMousePos[1] = e->y();
}

void OGLViewer::wheelEvent(QWheelEvent* e)
{
	view_cam->zoom(0.0, 0.0, -e->delta() * 0.01);
	update();
}

/************************************************************************/
/* Application Functions                                                */
/************************************************************************/

void OGLViewer::saveFrameBuffer()
{
	QString filename = QFileDialog::getSaveFileName(
		this, "Save Screen shot file...", "default", tr("PNG(*.png)"));
	this->grab().save(filename);
}

void OGLViewer::renderPixels()
{
	makeCurrent();

	clock_t startT, endT;
	startT = clock();
	ConsoleProgressBar progBar;

	int index = 0;
	Ray traceRay;
	cameraSampler camsmp;
	Transform w2o;

	const Film& film = view_cam->getFilm();

	QImage retImg(film.width, film.height, QImage::Format_ARGB32);

	Point3f lightpos(3, 10, 1);
	Float cosVal;
	Intersection isec;
	mRenderBuffer->cleanBuffer();
	for (uint32_t j = 0; j < film.height; j++)
	{
		for (uint32_t i = 0; i < film.width; i++)
		{
			camsmp.imgX = i;
			camsmp.imgY = j;

			view_cam->generateRay(camsmp, &traceRay);
			Float tHit(INFINITY), rayEp(0);

			cosVal = 0;
			Point2f n(0, 0);
			if (mScene->intersect(traceRay, &isec, &tHit, &rayEp))
			{
				/*cosVal = tHit;
				isec.shape->postIntersect(traceRay, &isec);
				cosVal = (dot(normalize(lightpos - isec.P), isec.Ng) + 1) * 0.5;
				pixmap->setBuffer(uint32_t(i), uint32_t(j), isec, tHit);
				size_t index = (j * width() + i) << 2;
				pixmap->beauty[index++] = cosVal;
				pixmap->beauty[index++] = cosVal;
				pixmap->beauty[index++] = cosVal;
				pixmap->beauty[index++] = cosVal;

				pixmap->setBuffer()
				pixmap->empty();
				pixmap->setBuffer(0);
				pixmap->doSomething();
				*/
				mRenderBuffer->setBuffer(i, j, isec, tHit);
				traceRay.Ng.normalize();
				int rgb[]{ static_cast<int>((traceRay.Ng.x*0.5f + 0.5f) * 255),
					static_cast<int>((traceRay.Ng.y*0.5f + 0.5f) * 255),
					static_cast<int>((traceRay.Ng.z*0.5f + 0.5f) * 255) };
				retImg.setPixelColor(i, film.height - j - 1,
									 QColor(rgb[0], rgb[1], rgb[2]));

			}
		}
		progBar.print(j / float(film.height));
	}
	progBar.complete();

	retImg.save("result.png");

	endT = clock();
	cout << "Rendering Time:\t" << (Float)(endT - startT) / CLOCKS_PER_SEC << "s" << endl;//Timer
	doneCurrent();
}

}
