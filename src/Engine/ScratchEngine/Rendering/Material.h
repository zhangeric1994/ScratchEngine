#ifndef MATERIAL_H
#define MATERIAL_H
#pragma once

#include <string>

#include "prerequisites.h"

using namespace std;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Material
		{
		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			f32 tint[4];

			ID3D11ShaderResourceView* diffuseMap;
			ID3D11ShaderResourceView* normalMap;
			ID3D11ShaderResourceView* metalnessMap;
			ID3D11ShaderResourceView* roughnessMap;

			ID3D11SamplerState* sampler;


		public:
			Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11SamplerState* sampler);
			Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11SamplerState* sampler, const string& filePath);


			SimpleVertexShader* GetVertexShader() const;
			SimplePixelShader* GetPixelShader() const;

			const f32* GetTint() const;

			bool HasTexture() const;
			bool HasNormalMap() const;
			bool HasMetalnessMap() const;
			bool HasRoughnessMap() const;
			ID3D11ShaderResourceView* GetTexture() const;
			ID3D11ShaderResourceView* GetNormalMap() const;
			ID3D11ShaderResourceView* GetMetalnessMap() const;
			ID3D11ShaderResourceView* GetRoughnessMap() const;

			ID3D11SamplerState* GetSampler() const;

			void SetTint(f32 r, f32 g, f32 b, f32 a);
			void SetTint(f32 r, f32 g, f32 b);
			void SetTint(const f32* color);

			void SetTexture(ID3D11ShaderResourceView* texture);
			void SetNormalMap(ID3D11ShaderResourceView* texture);
			void SetMetalnessMap(ID3D11ShaderResourceView* texture);
			void SetRoughnessMap(ID3D11ShaderResourceView* texture);
		};
	}
}


#include "Material.inl"

#endif // !MATERIAL_H