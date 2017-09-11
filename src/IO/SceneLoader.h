#pragma once
#include "Core/Scene.h"

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>

namespace Kaguya
{

class SceneLoader
{
public:
	SceneLoader(const std::string &filename);
	~SceneLoader();

	static Scene* load(const std::string &filename);

private:
	std::shared_ptr<Camera> loadCamera(const rapidjson::Value &jsonCamera) const;
	std::shared_ptr<Geometry> loadGeometry(const rapidjson::Value &jsonCamera) const;

private:
	rapidjson::Document mDocument;
	std::string mFilePath;

};

}
