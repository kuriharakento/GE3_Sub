#include "3d/Model.h"

#include <cassert>
#include <fstream>
#include <sstream>

#include "base/GraphicsTypes.h"
#include "manager/TextureManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void Model::Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename)
{
	modelCommon_ = modelCommon;

	//モデルの読み込み
	modelData_ = LoadObjFile(directoryPath,filename);

	//テクスチャの読み込み
	//.objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	//読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	//描画設定の初期化
	InitializeRenderingSettings();
}

void Model::Draw()
{
	//3D描画
	modelCommon_->GetDXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//マテリアルCBufferの場所を設定
	modelCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootPatameter[2]である。
	modelCommon_->GetDXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureIndex));
	//描画！
	modelCommon_->GetDXCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;

			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

//MaterialData Model::LoadMaterialTemplateFile(const std::string& filePath)
//{
//	MaterialData materialData;
//	std::string line;
//	std::ifstream file(filePath);
//	assert(file.is_open());
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//		//identifierに応じた処理
//		if (identifier == "map_Kd")
//		{
//			std::string textureFilename;
//			s >> textureFilename;
//			//連結してファイルパスにする
//			materialData.textureFilePath = filePath + "/" + textureFilename;
//		}
//	}
//	return materialData;
//}

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;				//構築するModelData
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	//メッシュの解析
	for(uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());		//法線がないMeshは非対応
		assert(mesh->HasTextureCoords(0));	//テクスチャ座標がないMeshは非対応

		//faceの解析
		for(uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);	//三角形以外は非対応

			//三角形の頂点を解析
			for(uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				//airProcess_MakeLeftHandedはz*=-1で、右て->左手に変換するので手動で処理
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
                modelData.vertices.push_back(vertex);
			}
		}
	}

	//Materialの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}
	return modelData;
}

//ModelData Model::LoadObjFile(const std::string& filePath)
//{
//	ModelData modelData;				//構築するModelData
//	std::vector<Vector4> positions;		//位置
//	std::vector<Vector3> normals;		//法線
//	std::vector<Vector2> texcoords;		//テクスチャ座標
//	std::string line;					//ファイルから読んだ１行を格納するもの
//
//	std::ifstream file(filePath);
//	assert(file.is_open());
//
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;	//先頭の識別子を読む
//
//		//identifierに応じた処理
//		if (identifier == "v")
//		{
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			positions.push_back(position);
//		} else if (identifier == "vt")
//		{
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoords.push_back(texcoord);
//		} else if (identifier == "vn")
//		{
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		} else if (identifier == "f")
//		{
//			VertexData triangle[3];
//			//面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
//			{
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element)
//				{
//					std::string index;
//					std::getline(v, index, '/');
//					elementIndices[element] = std::stoi(index);
//				}
//				//要素へのIndexから、実際の要素の値を取得して。頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//
//				position.x *= -1.0f;
//				normal.x *= -1.0f;
//
//				texcoord.y = 1.0f - texcoord.y;
//
//				/*VertexData vertex = { position,texcoord,normal };
//				modelData.vertices.push_back(vertex);*/
//				triangle[faceVertex] = { position,texcoord,normal };
//			}
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);;
//		} else if (identifier == "mtllib")
//		{
//			//materialTemplateLibraryファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			//基本的にobjファイルと同一階層にmtlは存在させるので,ディレクトリ名とファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(materialFilename);
//		}
//	}
//	return modelData;
//
//}


void Model::CreateVertexData()
{
	/*--------------[ VertexResourceを作る ]-----------------*/

	vertexResource_ = modelCommon_->GetDXCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	/*--------------[ VertexBufferViewを作る(値を設定するだけ) ]-----------------*/

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();				//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);									//１頂点当たりのサイズ

	/*--------------[ VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる ]-----------------*/

	vertexResource_->Map(0,
		nullptr,
		reinterpret_cast<void**>(&vertexData_)
	);

	/*--------------[ モデルデータの頂点情報をコピーする ]-----------------*/

	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::CreateMaterialData()
{
	/*--------------[ MaterialResourceを作る ]-----------------*/

	materialResource_ = modelCommon_->GetDXCommon()->CreateBufferResource(sizeof(Material));

	/*--------------[ MaterialResourceにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる ]-----------------*/

	materialResource_->Map(0,
		nullptr,
		reinterpret_cast<void**>(&materialData_)
	);

	//マテリアルデータの初期値を書き込む
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 30.0f;
}

void Model::InitializeRenderingSettings()
{
	//頂点データの生成
	CreateVertexData();

	//マテリアルデータの生成
	CreateMaterialData();
}
