/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#include "GrassLoader.h"
#include "PagedGeometry.h"

#include "OgreRoot.h"
#include "OgreTimer.h"
#include "OgreCamera.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreEntity.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreHardwareBufferManager.h"
#include "OgreHardwareBuffer.h"
#include "OgreMeshManager.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreLogManager.h"
#include "OgreTextureManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreRenderSystem.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"
using namespace Ogre;


unsigned long GrassLoader::GUID = 0;

GrassLoader::GrassLoader(PagedGeometry *geom)
{
	GrassLoader::geom = geom;
	
	windDir = Vector3::UNIT_X;
	densityFactor = 1.0f;
	renderQueue = RENDER_QUEUE_6;

	windTimer.reset();
}

GrassLoader::~GrassLoader()
{
	std::list<GrassLayer*>::iterator it;
	for (it = layerList.begin(); it != layerList.end(); ++it){
		delete *it;
	}
	layerList.clear();
}

GrassLayer *GrassLoader::addLayer(const String &material)
{
	GrassLayer *layer = new GrassLayer(geom);
	layer->setMaterialName(material);
	layerList.push_back(layer);

	return layer;
}

void GrassLoader::updateAnimation()
{
	static unsigned long lastTime = 0;
	unsigned long currentTime = windTimer.getMilliseconds();
	unsigned long ellapsedTime = currentTime - lastTime;
	lastTime = currentTime;

	float ellapsed = ellapsedTime / 1000.0f;
	
	//Update the vertex shader parameters
	std::list<GrassLayer*>::iterator it;
	for (it = layerList.begin(); it != layerList.end(); ++it){
		GrassLayer *layer = *it;

		layer->_updateShaders();
		if (layer->animate){
			//Incriment animation frame
			layer->waveCount += ellapsed * (layer->animSpeed * Math::PI);
			if (layer->waveCount > Math::PI*2) layer->waveCount -= Math::PI*2;

			//Set vertex shader parameters
			GpuProgramParametersSharedPtr params = layer->material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
			params->setNamedConstant("time", layer->waveCount);
			params->setNamedConstant("frequency", layer->animFreq);

			Vector3 direction = windDir * layer->animMag;
			params->setNamedConstant("direction", Vector4(direction.x, direction.y, direction.z, 0));
		}
	}
}

void GrassLoader::loadPage(PageInfo &page)
{
	//Seed random number generator based on page indexes
	uint16 xSeed = static_cast<uint16>(page.xIndex % 0xFFFF);
	uint16 zSeed = static_cast<uint16>(page.zIndex % 0xFFFF);
	uint32 seed = (xSeed << 16) | zSeed;
	srand(seed);

	std::list<GrassLayer*>::iterator it;
	for (it = layerList.begin(); it != layerList.end(); ++it){
		GrassLayer *layer = *it;

		//Calculate how much grass needs to be added
		float volume = page.bounds.width() * page.bounds.height();
		unsigned int grassCount = layer->density * densityFactor * volume;

		//The vertex buffer can't be allocated until the exact number of polygons is known,
		//so the locations of all grasses in this page must be precalculated.

		//Precompute grass locations into an array of floats. A plain array is used for speed;
		//there's no need to use a dynamic sized array since a maximum size is known.
		float *position = new float[grassCount*2];
		grassCount = layer->_populateGrassList(page, position, grassCount);

		//Don't build a mesh unless it contains something
		if (grassCount != 0){
			//Calculate the number of quads to be added
			unsigned int quadCount;
			if (layer->renderTechnique == GRASSTECH_CROSSQUADS)
				quadCount = grassCount * 2;
			else
				quadCount = grassCount;
			
			//Create manual mesh to store grass quads
			MeshPtr mesh = MeshManager::getSingleton().createManual(getUniqueID(), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			SubMesh *subMesh = mesh->createSubMesh();
			subMesh->useSharedVertices = false;

			//Setup vertex format information
			subMesh->vertexData = new VertexData;
			subMesh->vertexData->vertexStart = 0;
			subMesh->vertexData->vertexCount = 4 * quadCount;
		
			VertexDeclaration* dcl = subMesh->vertexData->vertexDeclaration;
			size_t offset = 0;
			dcl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
			dcl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
			offset += VertexElement::getTypeSize(VET_COLOUR);
			dcl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
			offset += VertexElement::getTypeSize(VET_FLOAT2);

			//Populate a new vertex buffer with grass
			HardwareVertexBufferSharedPtr vbuf = HardwareBufferManager::getSingleton()
												.createVertexBuffer(offset, subMesh->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
			float* pReal = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

			//Calculate size variance
			float rndWidth = layer->maxWidth - layer->minWidth;
			float rndHeight = layer->maxHeight - layer->minHeight;

			float minY = Math::POS_INFINITY, maxY = Math::NEG_INFINITY;
			float *posPtr = position;	//Position array "iterator"
			for (uint16 i = 0; i < grassCount; ++i)
			{
				//Get the x and z positions from the position array
				float x = *posPtr++;
				float z = *posPtr++;

				//Get the color at the grass position
				uint32 color = layer->_getColorAt(x, z);

				//Calculate size
				float rnd = Math::UnitRandom();	//The same rnd value is used for width and height to maintain aspect ratio
				float halfScaleX = (layer->minWidth + rndWidth * rnd) * 0.5f;
				float scaleY = (layer->minHeight + rndHeight * rnd);

				//Calculate rotation
				float angle = Math::RangeRandom(0, 6.28318f);
				float xTrans = Math::Cos(angle) * halfScaleX;
				float zTrans = Math::Sin(angle) * halfScaleX;

				//Calculate heights and edge positions
				float x1 = x - xTrans, z1 = z - zTrans;
				float x2 = x + xTrans, z2 = z + zTrans;

				float y1, y2;
				if (heightFunction){
					y1 = heightFunction(x1, z1);
					y2 = heightFunction(x2, z2);
				} else {
					y1 = 0;
					y2 = 0;
				}
				
				//Add vertices
				*pReal++ = (x1 - page.centerPoint.x); *pReal++ = (y1 + scaleY); *pReal++ = (z1 - page.centerPoint.z);	//pos
				*((uint32*)pReal++) = color;							//color
				*pReal++ = 0; *pReal++ = 0;								//uv
				
				*pReal++ = (x2 - page.centerPoint.x); *pReal++ = (y2 + scaleY); *pReal++ = (z2 - page.centerPoint.z);	//pos
				*((uint32*)pReal++) = color;							//color
				*pReal++ = 1; *pReal++ = 0;								//uv

				*pReal++ = (x1 - page.centerPoint.x); *pReal++ = (y1); *pReal++ = (z1 - page.centerPoint.z);			//pos
				*((uint32*)pReal++) = color;							//color
				*pReal++ = 0; *pReal++ = 1;								//uv

				*pReal++ = (x2 - page.centerPoint.x); *pReal++ = (y2); *pReal++ = (z2 - page.centerPoint.z);			//pos
				*((uint32*)pReal++) = color;							//color
				*pReal++ = 1; *pReal++ = 1;								//uv
				
				//Update bounds
				if (y1 < minY) minY = y1;
				if (y2 < minY) minY = y2;
				if (y1 + scaleY > maxY) maxY = y1 + scaleY;
				if (y2 + scaleY > maxY) maxY = y2 + scaleY;

				if (layer->renderTechnique == GRASSTECH_CROSSQUADS){
					//Calculate heights and edge positions
					float x3 = x + zTrans, z3 = z - xTrans;
					float x4 = x - zTrans, z4 = z + xTrans;

					float y3, y4;
					if (heightFunction){
						y3 = heightFunction(x3, z3);
						y4 = heightFunction(x4, z4);
					} else {
						y3 = 0;
						y4 = 0;
					}

					//Add vertices
					*pReal++ = (x3 - page.centerPoint.x); *pReal++ = (y3 + scaleY); *pReal++ = (z3 - page.centerPoint.z);	//pos
					*((uint32*)pReal++) = color;							//color
					*pReal++ = 0; *pReal++ = 0;								//uv
					
					*pReal++ = (x4 - page.centerPoint.x); *pReal++ = (y4 + scaleY); *pReal++ = (z4 - page.centerPoint.z);	//pos
					*((uint32*)pReal++) = color;							//color
					*pReal++ = 1; *pReal++ = 0;								//uv

					*pReal++ = (x3 - page.centerPoint.x); *pReal++ = (y3); *pReal++ = (z3 - page.centerPoint.z);			//pos
					*((uint32*)pReal++) = color;							//color
					*pReal++ = 0; *pReal++ = 1;								//uv

					*pReal++ = (x4 - page.centerPoint.x); *pReal++ = (y4); *pReal++ = (z4 - page.centerPoint.z);			//pos
					*((uint32*)pReal++) = color;							//color
					*pReal++ = 1; *pReal++ = 1;								//uv

					//Update bounds
					if (y3 < minY) minY = y1;
					if (y4 < minY) minY = y2;
					if (y3 + scaleY > maxY) maxY = y3 + scaleY;
					if (y4 + scaleY > maxY) maxY = y4 + scaleY;
				}
			}
			
			vbuf->unlock();
			subMesh->vertexData->vertexBufferBinding->setBinding(0, vbuf);
			
			//Populate index buffer
			subMesh->indexData->indexStart = 0;
			subMesh->indexData->indexCount = 6 * quadCount;
			subMesh->indexData->indexBuffer = HardwareBufferManager::getSingleton()
								.createIndexBuffer(HardwareIndexBuffer::IT_16BIT, subMesh->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			uint16* pI = static_cast<uint16*>(subMesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
			for (uint16 i = 0; i < quadCount; ++i)
			{
				uint16 offset = i * 4;

				*pI++ = 0 + offset;
				*pI++ = 2 + offset;
				*pI++ = 1 + offset;

				*pI++ = 1 + offset;
				*pI++ = 2 + offset;
				*pI++ = 3 + offset;
			}
			
			subMesh->indexData->indexBuffer->unlock();

			//Finish up mesh
			AxisAlignedBox bounds(page.bounds.left - page.centerPoint.x, minY, page.bounds.top - page.centerPoint.z,
				page.bounds.right - page.centerPoint.x, maxY, page.bounds.bottom - page.centerPoint.z);
			mesh->_setBounds(bounds);
			Vector3 temp = bounds.getMaximum() - bounds.getMinimum();
			mesh->_setBoundingSphereRadius(temp.length() * 0.5f);

			LogManager::getSingleton().setLogDetail(static_cast<LoggingLevel>(0));
			mesh->load();
			LogManager::getSingleton().setLogDetail(LL_NORMAL);

			//Apply grass material to mesh
			subMesh->setMaterialName(layer->material->getName());
			
			//Add the mesh to PagedGeometry
			Entity *entity = geom->getCamera()->getSceneManager()->createEntity(getUniqueID(), mesh->getName());
			entity->setRenderQueueGroup(renderQueue);
			entity->setCastShadows(false);
			addEntity(entity, page.centerPoint, Quaternion::IDENTITY, Vector3::UNIT_SCALE);
			geom->getSceneManager()->destroyEntity(entity);

			//Store mesh for later deallocation
			page.userData = mesh.getPointer();
		} else {
			page.userData = NULL;
		}

		//Delete the position list
		delete[] position;
	}
}

void GrassLoader::unloadPage(const PageInfo &page)
{
	//Unload mesh
	Mesh *mesh = (Mesh*)page.userData;
	if (mesh)
		MeshManager::getSingleton().remove(mesh->getName());
}



GrassLayer::GrassLayer(PagedGeometry *geom)
{
	density = 1.0f;
	minWidth = 1.0f; maxWidth = 1.0f;
	minHeight = 1.0f; maxHeight = 1.0f;
	renderTechnique = GRASSTECH_QUAD;
	fadeTechnique = FADETECH_ALPHA;
	animMag = 1.0f;
	animSpeed = 1.0f;
	animFreq = 1.0f;
	waveCount = 0.0f;
	animate = false;
	blend = false;
	shaderNeedsUpdate = true;
	GrassLayer::geom = geom;

	densityMap = NULL;
	colorMap = NULL;
}

GrassLayer::~GrassLayer()
{
	_clearDensityMap();
	_clearColorMap();
}

void GrassLayer::setMaterialName(const Ogre::String &matName)
{
	if (material.isNull() || matName != material->getName()){
		material = Ogre::MaterialManager::getSingleton().getByName(matName);
		shaderNeedsUpdate = true;
		_updateShaders();
	}
}

void GrassLayer::setMinimumSize(float width, float height)
{
	minWidth = width;
	minHeight = height;
}

void GrassLayer::setMaximumSize(float width, float height)
{
	maxWidth = width;
	if (maxHeight != height){
		maxHeight = height;
		shaderNeedsUpdate = true;
		_updateShaders();
	}
}

void GrassLayer::setRenderTechnique(GrassTechnique style, bool blendBase)
{
	renderTechnique = style;
	if (blend != blendBase){
		blend = blendBase;
		shaderNeedsUpdate = true;
	}
}

void GrassLayer::setFadeTechnique(FadeTechnique style)
{
	if (fadeTechnique != style){
		fadeTechnique = style;
		shaderNeedsUpdate = true;
	}
}

void GrassLayer::setAnimationEnabled(bool enabled)
{
	if (animate != enabled){
		animate = enabled;
		shaderNeedsUpdate = true;
	}
}


void GrassLayer::_clearDensityMap()
{
	if (densityMap){
		delete[] densityMap->data;
		delete densityMap;
		densityMap = NULL;
	}
}

void GrassLayer::_clearColorMap()
{
	if (colorMap){
		delete[] colorMap->data;
		delete colorMap;
		colorMap = NULL;
	}
}

void GrassLayer::setDensityMap(const String &mapFile, MapChannel channel)
{
	if (mapFile != ""){
		//Load image
		TexturePtr map = TextureManager::getSingleton().load(mapFile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		
		//Copy image to pixelbox
		setDensityMap(map.getPointer(), channel);
	} else {
		setDensityMap(NULL);
	}
}
void GrassLayer::setDensityMap(Texture *map, MapChannel channel)
{
	_clearDensityMap();
	
	if (map){
		//Get the texture buffer
		HardwarePixelBufferSharedPtr buff = map->getBuffer();

		//Prepare a PixelBox (8-bit greyscale) to receive the density values
		densityMap = new PixelBox(Box(0, 0, buff->getWidth(), buff->getHeight()), PF_BYTE_L);
		densityMap->data = new uint8[densityMap->getConsecutiveSize()];

		if (channel == CHANNEL_COLOR){
			//Copy to the greyscale density map directly if no channel extraction is necessary
			buff->blitToMemory(*densityMap);
		} else {
			//If channel extraction is necessary, first convert to a PF_R8G8B8A8 format PixelBox
			//This is necessary for the code below to properly extract the desired channel
			PixelBox pixels(Box(0, 0, buff->getWidth(), buff->getHeight()), PF_R8G8B8A8);
			pixels.data = new uint8[pixels.getConsecutiveSize()];
			buff->blitToMemory(pixels);

			//Pick out a channel from the pixel buffer
			size_t channelOffset;
			switch (channel){
				case CHANNEL_RED: channelOffset = 3; break;
				case CHANNEL_GREEN: channelOffset = 2; break;
				case CHANNEL_BLUE: channelOffset = 1; break;
				case CHANNEL_ALPHA: channelOffset = 0; break;
				default: OGRE_EXCEPT(0, "Invalid channel", "GrassLayer::setDensityMap()"); break;
			}
			
			//And copy that channel into the density map
			uint8 *inputPtr = (uint8*)pixels.data + channelOffset;
			uint8 *outputPtr = (uint8*)densityMap->data;
			uint8 *outputEndPtr = outputPtr + densityMap->getConsecutiveSize();
			while (outputPtr != outputEndPtr){
				*outputPtr++ = *inputPtr;
				inputPtr += 4;
			}
			
			//Finally, delete the temporary PF_R8G8B8A8 pixel buffer
			delete[] pixels.data;
		}
	}
}

unsigned int GrassLayer::_populateGrassList(PageInfo page, float *posBuff, unsigned int grassCount)
{
	float *posPtr = posBuff;

	if (densityMap != NULL){
		//Use density map
		for (unsigned int i = 0; i < grassCount; ++i){
			//Pick a random position
			float x = Math::RangeRandom(page.bounds.left, page.bounds.right);
			float z = Math::RangeRandom(page.bounds.top, page.bounds.bottom);

			//Determine whether this grass will be added based on the local density.
			//For example, if localDensity is .32, grasses will be added 32% of the time.
			if (Math::UnitRandom() <= _getDensityAt(x, z)){
				//Add to list
				*posPtr++ = x;
				*posPtr++ = z;
			}
		}
	} else {
		//No density map
		for (unsigned int i = 0; i < grassCount; ++i){
			//Pick a random position
			float x = Math::RangeRandom(page.bounds.left, page.bounds.right);
			float z = Math::RangeRandom(page.bounds.top, page.bounds.bottom);

			//Add to list
			*posPtr++ = x;
			*posPtr++ = z;
		}
	}

	grassCount = (posPtr - posBuff) / 2;
	return grassCount;
}

void GrassLayer::setColorMap(const String &mapFile, MapChannel channel)
{
	if (mapFile != ""){
		//Load image
		TexturePtr map = TextureManager::getSingleton().load(mapFile, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		
		//Copy image to pixelbox
		setColorMap(map.getPointer(), channel);
	} else {
		setColorMap(NULL);
	}
}

void GrassLayer::setColorMap(Texture *map, MapChannel channel)
{
	_clearColorMap();
	
	if (map){
		//Get the texture buffer
		HardwarePixelBufferSharedPtr buff = map->getBuffer();

		//Prepare a PixelBox (24-bit RGB) to receive the color values
		VertexElementType format = Root::getSingleton().getRenderSystem()->getColourVertexElementType();
		switch (format){
			case VET_COLOUR_ARGB:
				colorMap = new PixelBox(Box(0, 0, buff->getWidth(), buff->getHeight()), PF_A8R8G8B8);
				break;
			case VET_COLOUR_ABGR:
				colorMap = new PixelBox(Box(0, 0, buff->getWidth(), buff->getHeight()), PF_A8B8G8R8);
				break;
			default:
				OGRE_EXCEPT(0, "Unknown RenderSystem color format", "GrassLayer::setColorMap()");
				break;
		}

		colorMap->data = new uint8[colorMap->getConsecutiveSize()];

		if (channel == CHANNEL_COLOR){
			//Copy to the color map directly if no channel extraction is necessary
			buff->blitToMemory(*colorMap);
		} else {
			//If channel extraction is necessary, first convert to a PF_R8G8B8A8 format PixelBox
			//This is necessary for the code below to properly extract the desired channel
			PixelBox pixels(Box(0, 0, buff->getWidth(), buff->getHeight()), PF_R8G8B8A8);
			pixels.data = new uint8[pixels.getConsecutiveSize()];
			buff->blitToMemory(pixels);

			//Pick out a channel from the pixel buffer
			size_t channelOffset;
			switch (channel){
				case CHANNEL_RED: channelOffset = 3; break;
				case CHANNEL_GREEN: channelOffset = 2; break;
				case CHANNEL_BLUE: channelOffset = 1; break;
				case CHANNEL_ALPHA: channelOffset = 0; break;
				default: OGRE_EXCEPT(0, "Invalid channel", "GrassLayer::setColorMap()"); break;
			}
			
			//And copy that channel into the density map
			uint8 *inputPtr = (uint8*)pixels.data + channelOffset;
			uint8 *outputPtr = (uint8*)colorMap->data;
			uint8 *outputEndPtr = outputPtr + colorMap->getConsecutiveSize();
			while (outputPtr != outputEndPtr){
				*outputPtr++ = *inputPtr;
				*outputPtr++ = *inputPtr;
				*outputPtr++ = *inputPtr;
				*outputPtr++ = 0xFF;	//Full alpha
				inputPtr += 4;
			}
			
			//Finally, delete the temporary PF_R8G8B8A8 pixel buffer
			delete[] pixels.data;
		}
	}
}

void GrassLayer::_updateShaders()
{
	if (shaderNeedsUpdate){
		shaderNeedsUpdate = false;

		//Disable lighting and shadows
		material->setLightingEnabled(false);
		//material->setReceiveShadows(false);

		//Make sure the user's computer supports vertex shaders
		const RenderSystemCapabilities *caps = Root::getSingleton().getRenderSystem()->getCapabilities();
		if (caps->hasCapability(RSC_VERTEX_PROGRAM)){
			Pass *pass = material->getTechnique(0)->getPass(0);

			//Check if it's ok to set a shader (user's shaders shouldn't be overwritten)
			bool shaderOK;
			if (pass->hasVertexProgram()){
				const String &vName = pass->getVertexProgramName();
				if (vName.substr(0,11)  == "GrassLoader")
					shaderOK = true;
				else
					shaderOK = false;
			} else {
				shaderOK = true;
			}

			//Set up the vertex shader if appropriate
			if (shaderOK){
				//Compile the CG shader script based on the user's options
				StringUtil::StrStreamType tmpName;
				tmpName << "GrassLoader_";
				if (animate)
					tmpName << "anim_";
				if (blend)
					tmpName << "blend_";
				tmpName << fadeTechnique << "_";
				if (fadeTechnique == FADETECH_GROW || fadeTechnique == FADETECH_ALPHAGROW)
					tmpName << maxHeight << "_";
				tmpName << "vp";

				const String vertexProgName = tmpName.str();

				//Check if this shader already exists (if not, compile it)
				HighLevelGpuProgramPtr vertexShader;
				vertexShader = HighLevelGpuProgramManager::getSingleton().getByName(vertexProgName);
				if (vertexShader.isNull()){
					//Generate the grass shader
					String vertexProgSource;
					vertexProgSource = 
						"void main( \n"
						"	float4 iPosition : POSITION, \n"
						"	float4 iColor : COLOR, \n"
						"	float2 iUV       : TEXCOORD0,	\n"
						"	out float4 oPosition : POSITION, \n"
						"	out float4 oColor : COLOR, \n"
						"	out float2 oUV       : TEXCOORD0,	\n";

					if (animate) vertexProgSource +=
						"	uniform float time,	\n"
						"	uniform float frequency,	\n"
						"	uniform float4 direction,	\n";

					if (fadeTechnique == FADETECH_GROW || fadeTechnique == FADETECH_ALPHAGROW) vertexProgSource +=
						"	uniform float grassHeight,	\n";

					vertexProgSource +=
						"	uniform float4x4 worldViewProj,	\n"
						"	uniform float3 camPos, \n"
						"	uniform float fadeRange ) \n"
						"{	\n"
						"	oColor.rgb = iColor.rgb;   \n"
						"	float4 position = iPosition;	\n"
						"	float dist = distance(camPos.xz, position.xz);	\n";

					if (fadeTechnique == FADETECH_ALPHA || fadeTechnique == FADETECH_ALPHAGROW) vertexProgSource +=
							//Fade out in the distance
						"	oColor.a = 2.0f - (2.0f * dist / fadeRange);   \n";
					else vertexProgSource +=
						"	oColor.a = 1.0f;   \n";

					if (animate) vertexProgSource +=
						"	if (iUV.y == 0.0f){	\n"
								//Wave grass in breeze
						"		float offset = sin(time + position.x * frequency);	\n"
						"		position += direction * offset;	\n"
						"	}	\n";

					if (blend && animate) vertexProgSource +=
						"	else {	\n";
					else if (blend) vertexProgSource +=
						"	if (iUV.y != 0.0f){	\n";

					if (blend) vertexProgSource +=
								//Blend the base of nearby grass into the terrain
						"		if (oColor.a >= 1.0f) \n"
						"			oColor.a = 4.0f * ((dist / fadeRange) - 0.1f);	\n"
						"	}	\n";

					if (fadeTechnique == FADETECH_GROW || fadeTechnique == FADETECH_ALPHAGROW) vertexProgSource +=
						"	float offset = (2.0f * dist / fadeRange) - 1.0f; \n"
						"	position.y -= grassHeight * clamp(offset, 0, 1); ";

					vertexProgSource +=
						"	oPosition = mul(worldViewProj, position);  \n"

						"	oUV = iUV;\n"
						"}"; 
						
						vertexShader = HighLevelGpuProgramManager::getSingleton().createProgram(
							vertexProgName,
							ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
							"cg", GPT_VERTEX_PROGRAM);

						vertexShader->setSource(vertexProgSource);
						vertexShader->setParameter("profiles", "vs_1_1 arbvp1");
						vertexShader->setParameter("entry_point", "main");
						vertexShader->load();
				}

				//Apply the shader to the material
				pass->setVertexProgram(vertexProgName);
				pass->getVertexProgramParameters()->setNamedAutoConstant("worldViewProj", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
				pass->getVertexProgramParameters()->setNamedAutoConstant("camPos", GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
				pass->getVertexProgramParameters()->setNamedAutoConstant("fadeRange", GpuProgramParameters::ACT_CUSTOM, 1);

				if (animate){
					pass->getVertexProgramParameters()->setNamedAutoConstant("time", GpuProgramParameters::ACT_CUSTOM, 1);
					pass->getVertexProgramParameters()->setNamedAutoConstant("frequency", GpuProgramParameters::ACT_CUSTOM, 1);
					pass->getVertexProgramParameters()->setNamedAutoConstant("direction", GpuProgramParameters::ACT_CUSTOM, 4);
				}
				
				if (fadeTechnique == FADETECH_GROW || fadeTechnique == FADETECH_ALPHAGROW){
					pass->getVertexProgramParameters()->setNamedAutoConstant("grassHeight", GpuProgramParameters::ACT_CUSTOM, 1);
					pass->getVertexProgramParameters()->setNamedConstant("grassHeight", maxHeight * 1.05f);
				}

				float farViewDist = geom->getDetailLevels().front()->getFarRange();
				pass->getVertexProgramParameters()->setNamedConstant("fadeRange", farViewDist / 1.225f);
				//Note: 1.225 ~= sqrt(1.5), which is necessary since the far view distance is measured from the centers
				//of pages, while the vertex shader needs to fade grass completely out (including the closest corner)
				//before the page center is out of range.
			}
		}
	}
}



unsigned long GrassPage::GUID = 0;

void GrassPage::init(PagedGeometry *geom)
{
	sceneMgr = geom->getSceneManager();
}

void GrassPage::addEntity(Entity *entity, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
{
	SceneNode *node = sceneMgr->getRootSceneNode()->createChildSceneNode();
	node->setPosition(position);
	nodeList.push_back(node);

	Entity *ent = entity->clone(getUniqueID());
	ent->setCastShadows(false);
	ent->setRenderQueueGroup(entity->getRenderQueueGroup());
	node->attachObject(ent);
}

void GrassPage::removeEntities()
{
	std::list<SceneNode*>::iterator i;
	for (i = nodeList.begin(); i != nodeList.end(); ++i){
		SceneNode *node = *i;
		sceneMgr->destroyEntity(static_cast<Entity*>(node->getAttachedObject(0)));
		sceneMgr->destroySceneNode(node->getName());
	}
	nodeList.clear();
}

void GrassPage::setVisible(bool visible)
{
	std::list<SceneNode*>::iterator i;
	for (i = nodeList.begin(); i != nodeList.end(); ++i){
		SceneNode *node = *i;
		node->setVisible(visible);
	}
}
