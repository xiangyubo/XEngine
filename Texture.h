#ifndef __TEXTURE__
#define __TEXTURE__

#include "PreHeader.h"

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(const string &path, const string &filename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView    *texture;
};

#endif