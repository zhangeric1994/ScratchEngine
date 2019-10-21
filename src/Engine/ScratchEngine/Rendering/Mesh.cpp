#include "Mesh.h"
#include "RenderingEngine.h"

ScratchEngine::Rendering::Mesh::Mesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device2* device)
{
	CreateMesh(vertices, verticesNumber, indices, indicesNumber, device);
}

ScratchEngine::Rendering::Mesh::Mesh(ID3D11Device2* device, char* filename)
{
	// File input object
	std::ifstream obj(filename);

	// Check for successful open
	if (!obj.is_open())
		return;
	
	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];                     // String for line reading

	// Still have data left?
	while (obj.good()){
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n'){
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't'){
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v'){
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f'){
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;

			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;

			// Flip normal Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12){
				// Make the last vertex
				Vertex v4;
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();


	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.

	int verticesNumber = static_cast<int>(verts.size());
	int indicesNumber = static_cast<int>(indices.size());

	CreateMesh(&verts[0], verticesNumber, &indices[0], indicesNumber, device);
}

ScratchEngine::Rendering::Mesh::Mesh(const Mesh& mesh)
{
	this->vertexBuffer = mesh.vertexBuffer;
	this->indexBuffer = mesh.indexBuffer;
	indicesNum = mesh.indicesNum;
}

ScratchEngine::Rendering::Mesh::~Mesh()
{
	//release vertex buffer and index buffer
	if (vertexBuffer)
		vertexBuffer->Release();

	if (indexBuffer)
		indexBuffer->Release();
}

void* ScratchEngine::Rendering::Mesh::operator new(size_t size)
{
	return RenderingEngine::GetSingleton()->materialAllocator.Allocate();
}

void ScratchEngine::Rendering::Mesh::operator delete(void * p)
{
	RenderingEngine::GetSingleton()->materialAllocator.Free(p);
}

ID3D11Buffer* ScratchEngine::Rendering::Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* ScratchEngine::Rendering::Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int ScratchEngine::Rendering::Mesh::GetIndexCount()
{
	return indicesNum;
}

//use assimp lib to load obj & mtl files
bool ScratchEngine::Rendering::Mesh::loadFile(ID3D11Device2* device, const std::string & filename) {
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR::ASSIMP::%s", import.GetErrorString());
		return false;
	}

	//// process all the node's meshes (if any)
	//for (unsigned int i = 0; i < node->mNumMeshes; i++)
	//{
	//	aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
	//	meshes.push_back(processMesh(mesh, scene));
	//}
	//// then do the same for each of its children
	//for (unsigned int i = 0; i < node->mNumChildren; i++)
	//{
	//	processNode(node->mChildren[i], scene);
	//}

	/*aiNode* node = scene->mRootNode;

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		
	}*/
	
	
	
	
	return false;
}

void ScratchEngine::Rendering::Mesh::CreateMesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device2* device)
{
	ComputeTangent(vertices, verticesNumber, indices, indicesNumber);

	//initialize vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * verticesNumber;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	//initialize index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indicesNumber;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);

	//initialize indicesNum
	indicesNum = indicesNumber;
}

void ScratchEngine::Rendering::Mesh::ComputeTangent(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber)
{
	for (int i = 0; i < verticesNumber; i++) {
		vertices[i].tangent = XMFLOAT3(0, 0, 0);
	}

	for (int i = 0; i < indicesNumber; i += 3) {
		Vertex v0 = vertices[indices[i]];
		Vertex v1 = vertices[indices[i + 1]];
		Vertex v2 = vertices[indices[i + 2]];

		float positionX0 = v0.Position.x;
		float positionY0 = v0.Position.y;
		float positionZ0 = v0.Position.z;

		float positionX1 = v1.Position.x;
		float positionY1 = v1.Position.y;
		float positionZ1 = v1.Position.z;

		float positionX2 = v2.Position.x;
		float positionY2 = v2.Position.y;
		float positionZ2 = v2.Position.z;

		float U0 = v0.UV.x;
		float V0 = v0.UV.y;

		float U1 = v1.UV.x;
		float V1 = v1.UV.y;

		float U2 = v2.UV.x;
		float V2 = v2.UV.y;

		float deltaPosX1 = positionX1 - positionX0;
		float deltaPosY1 = positionY1 - positionY0;
		float deltaPosZ1 = positionZ1 - positionZ0;

		float deltaPosX2 = positionX2 - positionX0;
		float deltaPosY2 = positionY2 - positionY0;
		float deltaPosZ2 = positionZ2 - positionZ0;

		float deltaU1 = U1 - U0;
		float deltaV1 = V1 - V0;

		float deltaU2 = U2 - U0;
		float deltaV2 = V2 - V0;

		float r = 1.0f / (deltaU1 * deltaV2 - deltaV1 * deltaU2);

		float x = (deltaPosX1 * deltaV2 - deltaPosX2 * deltaV1) * r;
		float y = (deltaPosY1 * deltaV2 - deltaPosY2 * deltaV1) * r;
		float z = (deltaPosZ1 * deltaV2 - deltaPosZ2 * deltaV1) * r;

		vertices[indices[i]].tangent.x += x;
		vertices[indices[i]].tangent.y += y;
		vertices[indices[i]].tangent.z += z;

		vertices[indices[i + 1]].tangent.x += x;
		vertices[indices[i + 1]].tangent.y += y;
		vertices[indices[i + 1]].tangent.z += z;

		vertices[indices[i + 2]].tangent.x += x;
		vertices[indices[i + 2]].tangent.y += y;
		vertices[indices[i + 2]].tangent.z += z;
	}
}