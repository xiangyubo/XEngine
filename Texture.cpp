#include "Texture.h"
#include "Utility.h"
#include "SRVBatch.h"

Texture::Texture()
{
    texture = NULL;
}


Texture::Texture(const Texture& other)
{
    texture = other.texture;
}


Texture::~Texture()
{
    texture = NULL;
}


bool Texture::Initialize(const string &path, const string &filename)
{
    texture = SRVBatch::GetInstance()->LoadSRV(path, filename);
	if(texture == NULL)
	{
		return false;
	}
	return true;
}


void Texture::Shutdown()
{
    texture = NULL;
}


ID3D11ShaderResourceView* Texture::GetTexture()
{
	return texture;
}