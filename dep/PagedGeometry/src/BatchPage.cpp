/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//BatchPage.cpp
//BatchPage is an extension to PagedGeometry which displays entities as static geometry.
//-------------------------------------------------------------------------------------

#include "BatchPage.h"
#include "BatchedGeometry.h"

#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreEntity.h"
#include "OgreRenderSystem.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"
using namespace Ogre;


//-------------------------------------------------------------------------------------


unsigned long BatchPage::refCount = 0;

void BatchPage::init(PagedGeometry *geom)
{
	sceneMgr = geom->getSceneManager();
	batch = new BatchedGeometry(sceneMgr);

	fadeEnabled = false;
	const RenderSystemCapabilities *caps = Root::getSingleton().getRenderSystem()->getCapabilities();
	if (caps->hasCapability(RSC_VERTEX_PROGRAM)){
		if (HighLevelGpuProgramManager::getSingleton().getByName("BatchPageVP").isNull()){
			String vertexProgSource =
				"void main( \n"
				"	float3 normal	 : NORMAL,	\n"
				"	float4 iPosition : POSITION, \n"
				"	float2 iUV       : TEXCOORD0,	\n"

				"	out float4 oPosition : POSITION, \n"
				"	out float4 oColor : COLOR, \n"
				"	out float2 oUV       : TEXCOORD0,	\n"
				"	out float4 oFog : FOG,	\n"

				"	uniform float4 ambient,	\n"
				"	uniform float4 objSpaceLight,	\n"
				"	uniform float4 lightColor,	\n"

				"	uniform float4x4 worldViewProj,	\n"
				"	uniform float3 camPos, \n"
				"	uniform float fadeGap, \n"
				"   uniform float invisibleDist )\n"

				"{	\n"
				//Perform lighting calculations (supports directional and point lights)
				"	float3 light = normalize(objSpaceLight.xyz - (iPosition.xyz * objSpaceLight.w)); \n"
				"	float diffuseFactor = max(dot(normal, light), 0); \n"
				"	oColor = ambient + diffuseFactor * lightColor; \n"

				//Fade out in the distance
				"	float dist = distance(camPos.xz, iPosition.xz);	\n"
				"	oColor.a *= (invisibleDist - dist) / fadeGap;   \n"

				"	oUV = iUV;	\n"
				"	oPosition = mul(worldViewProj, iPosition);  \n"
				"	oFog.x = oPosition.z; \n"
				"}"; 

			HighLevelGpuProgramPtr vertexShader = HighLevelGpuProgramManager::getSingleton().createProgram(
				"BatchPageVP",
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				"cg", GPT_VERTEX_PROGRAM);

			vertexShader->setSource(vertexProgSource);
			vertexShader->setParameter("profiles", "vs_1_1 arbvp1");
			vertexShader->setParameter("entry_point", "main");
			vertexShader->load();
		}
	}

	++refCount;
}

BatchPage::~BatchPage()
{
	delete batch;

	//Delete unfaded material references
	unfadedMaterials.clear();

	if (--refCount == 0){
		//Delete fade materials
		fadedMaterialMap.clear();
	}
}


void BatchPage::addEntity(Entity *ent, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
{
	batch->addEntity(ent, position, rotation, scale);
}

void BatchPage::build()
{
	batch->build();
}

void BatchPage::removeEntities()
{
	batch->clear();

	unfadedMaterials.clear();
	fadeEnabled = false;
}


void BatchPage::setVisible(bool visible)
{
	batch->setVisible(visible);
}

void BatchPage::setFade(bool enabled, Real visibleDist, Real invisibleDist)
{
	//If fade status has changed...
	if (fadeEnabled != enabled){
		BatchedGeometry::SubBatchIterator it = batch->getSubBatchIterator();
		fadeEnabled = enabled;

		if (enabled) {
			//Enable fading
			assert(unfadedMaterials.empty());
			while (it.hasMoreElements()){
				BatchedGeometry::SubBatch *subBatch = it.getNext();

				//Keep old unfaded material
				unfadedMaterials.push_back(subBatch->getMaterial());

				//And apply a new faded one
				MaterialPtr mat = getFadeMaterial(subBatch->getMaterial(), visibleDist, invisibleDist);
				subBatch->setMaterial(mat);
			}
			
			//Transparent batches should render after impostors
			batch->setRenderQueueGroup(RENDER_QUEUE_6);
		} else {
			//Disable fading
			unsigned int i = 0;
			while (it.hasMoreElements()){
				BatchedGeometry::SubBatch *subBatch = it.getNext();

				//Revert to the unfaded material
				subBatch->setMaterial(unfadedMaterials[i]);
				unfadedMaterials[i].setNull();

				++i;
			}
			unfadedMaterials.clear();

			//Opaque batches should render in the normal render queue
			batch->setRenderQueueGroup(RENDER_QUEUE_MAIN);
		}
	}
}


unsigned long BatchPage::GUID = 0;
BatchPage::FadedMaterialMap BatchPage::fadedMaterialMap;

MaterialPtr BatchPage::getFadeMaterial(MaterialPtr mat, Real visibleDist, Real invisibleDist)
{
	StringUtil::StrStreamType materialSignature;
	materialSignature << mat->getName() << "|";
	materialSignature << visibleDist << "|";
	materialSignature << invisibleDist << "|";

	FadedMaterialMap::iterator it = fadedMaterialMap.find(materialSignature.str());
	MaterialPtr fadeMaterial;

	//If a correctly faded version of this material exists...
	if (it != fadedMaterialMap.end()){
		//Use the existing fade material
		fadeMaterial = it->second;
	} else {
		//Otherwise clone the material
		fadeMaterial = mat->clone(getUniqueID("BatchFade"));

		//And apply the fade shader
		for (unsigned short t = 0; t < fadeMaterial->getNumTechniques(); ++t){
			Technique *tech = fadeMaterial->getTechnique(t);
			for (unsigned short p = 0; p < tech->getNumPasses(); ++p){
				Pass *pass = tech->getPass(p);

				//Setup vertex program
				pass->setVertexProgram("BatchPageVP");
				GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
				params->setNamedAutoConstant("ambient", GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
				params->setNamedAutoConstant("objSpaceLight", GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE);
				params->setNamedAutoConstant("lightColor", GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR);
				params->setNamedAutoConstant("worldViewProj", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
				params->setNamedAutoConstant("camPos", GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
				params->setNamedAutoConstant("fadeGap", GpuProgramParameters::ACT_CUSTOM);
				params->setNamedAutoConstant("invisibleDist", GpuProgramParameters::ACT_CUSTOM);

				//Set fade ranges
				params->setNamedConstant("invisibleDist", invisibleDist);
				params->setNamedConstant("fadeGap", invisibleDist - visibleDist);
				
				if (pass->getAlphaRejectFunction() == CMPF_ALWAYS_PASS)
					pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
			}
		}

		//Add it to the list so it can be reused later
		fadedMaterialMap.insert(std::pair<String, MaterialPtr>(materialSignature.str(), fadeMaterial));
	}

	return fadeMaterial;
}
