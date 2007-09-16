/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//ImpostorPage.cpp
//ImposterPage is an extension to PagedGeometry which displays entities as imposters.
//-------------------------------------------------------------------------------------

#include "ImpostorPage.h"
#include "StaticBillboardSet.h"

#include "OgreRoot.h"
#include "OgreTimer.h"
#include "OgreCamera.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreEntity.h"
#include "OgreHardwarePixelBuffer.h"
using namespace Ogre;


//-------------------------------------------------------------------------------------

unsigned int ImpostorPage::selfInstances = 0;

int ImpostorPage::impostorResolution = 128;
ColourValue ImpostorPage::impostorBackgroundColor = ColourValue(0.0f, 0.3f, 0.0f, 0.0f);


void ImpostorPage::init(PagedGeometry *geom)
{
	//Save pointers to PagedGeometry object
	sceneMgr = geom->getSceneManager();
	this->geom = geom;

	//Init. variables
	setBlendMode(ALPHA_REJECT_IMPOSTOR);
		
	if (++selfInstances == 1){
		//Set up a single instance of a scene node which will be used when rendering impostor textures
		sceneMgr->getRootSceneNode()->createChildSceneNode("ImpostorPage::renderNode");
	}
}

ImpostorPage::~ImpostorPage()
{
	//Delete all impostor batches
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		delete ibatch;
	}

	if (--selfInstances == 0){
		sceneMgr->destroySceneNode("ImpostorPage::renderNode");
	}
}


void ImpostorPage::setRegion(Ogre::Real left, Ogre::Real top, Ogre::Real right, Ogre::Real bottom)
{
	//Calculate center of region
	center.x = (left + right) * 0.5f;
	center.z = (top + bottom) * 0.5f;
	
	center.y = 0.0f;	//The center.y value is calculated when the entities are added
	aveCount = 0;
}

void ImpostorPage::addEntity(Entity *ent, const Vector3 &position, const Quaternion &rotation, const Vector3 &scale)
{
	//Get the impostor batch that this impostor will be added to
	ImpostorBatch *ibatch = ImpostorBatch::getBatch(this, ent);

	//Then add the impostor to the batch
	ibatch->addBillboard(position, rotation, scale);

	//Add the Y position to the center.y value (to be averaged later)
	center.y += position.y + ent->getBoundingBox().getCenter().y * scale.y;
	++aveCount;
}

void ImpostorPage::build()
{
	//Calculate the average Y value of all the added entities
	if (aveCount != 0)
		center.y /= aveCount;
	else
		center.y = 0.0f;

	//Build all batches
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		ibatch->build();
	}
}

void ImpostorPage::setVisible(bool visible)
{
	//Update visibility status of all batches
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		ibatch->setVisible(visible);
	}
}

void ImpostorPage::setFade(bool enabled, Real visibleDist, Real invisibleDist)
{
	//Update fade status of all batches
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		ibatch->setFade(enabled, visibleDist, invisibleDist);
	}
}

void ImpostorPage::removeEntities()
{
	//Clear all impostor batches
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		ibatch->clear();
	}

	//Reset y center
	center.y = 0.0f;
	aveCount = 0;
}

void ImpostorPage::update()
{
	//Calculate the direction the impostor batches should be facing
	Vector3 camPos = geom->getCamera()->getDerivedPosition();
		
	//Update all batches
	float distX = camPos.x - center.x;
	float distZ = camPos.z - center.z;
	float distY = camPos.y - center.y;
	float distRelZ = Math::Sqrt(distX * distX + distZ * distZ);
	Radian pitch = Math::ATan2(distY, distRelZ);

	Radian yaw;
	if (distRelZ > geom->getPageSize() * 3) {
		yaw = Math::ATan2(distX, distZ);
	} else {
		Vector3 dir = geom->getCamera()->getDerivedDirection();
		yaw = Math::ATan2(-dir.x, -dir.z);
	}

	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	for (iter = impostorBatches.begin(); iter != impostorBatches.end(); ++iter){
		ImpostorBatch *ibatch = iter->second;
		ibatch->setAngle(pitch.valueDegrees(), yaw.valueDegrees());
	}
}


//-------------------------------------------------------------------------------------

unsigned long ImpostorBatch::GUID = 0;

ImpostorBatch::ImpostorBatch(ImpostorPage *group, Entity *entity)
{
	typedef std::pair<ResourceHandle, ImpostorBatch *> ListItem;
	
	//Render impostor texture for this entity
	tex = ImpostorTexture::getTexture(group, entity);
	
	//Create billboard set
	bbset = new StaticBillboardSet(group->sceneMgr);
	bbset->setTextureStacksAndSlices(IMPOSTOR_PITCH_ANGLES, IMPOSTOR_YAW_ANGLES);

	//Default the angle to 0 degrees
	pitchIndex = -1;
	yawIndex = -1;
	setAngle(0.0f, 0.0f);

	//Add self to impostorBatches list
	group->impostorBatches.insert(ListItem(tex->sourceMesh, this));

	//Init. variables
	igroup = group;
}

ImpostorBatch::~ImpostorBatch()
{
	//Delete billboard set
	delete bbset;
	//Delete texture
	ImpostorTexture::removeTexture(tex);
}

//Returns a pointer to an ImpostorBatch for the specified entity in the specified
//ImpostorPage. If one does not already exist, one will automatically be created.
ImpostorBatch *ImpostorBatch::getBatch(ImpostorPage *group, Entity *entity)
{
	//Search for an existing impostor batch for this entity
	std::map<ResourceHandle, ImpostorBatch *>::iterator iter;
	iter = group->impostorBatches.find(entity->getMesh()->getHandle());

	//If found..
	if (iter != group->impostorBatches.end()){
		//Return it
		return iter->second;
	} else {
		//Otherwise, return a new batch
		return (new ImpostorBatch(group, entity));
	}
}

//Rotates all the impostors to the specified angle (virtually - it actually changes
//their materials to produce this same effect)
void ImpostorBatch::setAngle(float pitchDeg, float yawDeg)
{
	//Calculate pitch material index
	Ogre::uint16 newPitchIndex;
	if (pitchDeg > 0){
		newPitchIndex = static_cast<Ogre::uint16>(IMPOSTOR_PITCH_ANGLES * (pitchDeg / 67.5f));
		if (newPitchIndex > IMPOSTOR_PITCH_ANGLES-1) newPitchIndex = IMPOSTOR_PITCH_ANGLES-1;
	} else {
		newPitchIndex = 0;
	}
	
	//Calculate yaw material index
	Ogre::uint16 newYawIndex;
	if (yawDeg > 0){
		newYawIndex = static_cast<Ogre::uint16>(IMPOSTOR_YAW_ANGLES * (yawDeg / 360.0f) + 0.5f) % IMPOSTOR_YAW_ANGLES;
	} else {
		newYawIndex = static_cast<Ogre::uint16>(IMPOSTOR_YAW_ANGLES + IMPOSTOR_YAW_ANGLES * (yawDeg / 360.0f) + 0.5f) % IMPOSTOR_YAW_ANGLES;
	}
	
	//Change materials if necessary
	if (newPitchIndex != pitchIndex || newYawIndex != yawIndex){
		pitchIndex = newPitchIndex;
		yawIndex = newYawIndex;
		bbset->setMaterial(tex->material[pitchIndex][yawIndex]->getName());
	}
}


//-------------------------------------------------------------------------------------

std::map<ResourceHandle, ImpostorTexture *> ImpostorTexture::selfList;
unsigned long ImpostorTexture::GUID = 0;

//Do not use this constructor yourself - instead, call getTexture()
//to get/create an ImpostorTexture for an Entity.
ImpostorTexture::ImpostorTexture(ImpostorPage *group, Entity *entity)
{
	typedef std::pair<ResourceHandle, ImpostorTexture *> ListItem;

	//Add self to list of ImpostorTexture's
	sourceMesh = entity->getMesh()->getHandle();
	selfList.insert(ListItem(sourceMesh, this));
	
	//Calculate the entity's bounding box and it's diameter
	boundingBox = entity->getBoundingBox();

	//Note - this radius calculation assumes the object is somewhat rounded (like trees/rocks/etc.)
	Real tmp;
	entityRadius = boundingBox.getMaximum().x - boundingBox.getCenter().x;
	tmp = boundingBox.getMaximum().y - boundingBox.getCenter().y;
	if (tmp > entityRadius) entityRadius = tmp;
	tmp = boundingBox.getMaximum().z - boundingBox.getCenter().z;
	if (tmp > entityRadius) entityRadius = tmp;

	entityDiameter = 2.0f * entityRadius;
	entityCenter = boundingBox.getCenter();
	
	//Render impostor textures
	renderTextures(group, entity);
	
	//Set up materials
	for (int o = 0; o < IMPOSTOR_YAW_ANGLES; ++o){
	for (int i = 0; i < IMPOSTOR_PITCH_ANGLES; ++i){
		material[i][o] = MaterialManager::getSingleton().create(getUniqueID("ImpostorMaterial"), "Impostors");

		Material *m = material[i][o].getPointer();
		Pass *p = m->getTechnique(0)->getPass(0);        
		
		TextureUnitState *t = p->createTextureUnitState(texture->getName());
		
		t->setTextureUScroll((float)o / IMPOSTOR_YAW_ANGLES);
		t->setTextureVScroll((float)i / IMPOSTOR_PITCH_ANGLES);

		p->setLightingEnabled(false);
		m->setReceiveShadows(false);
		
		if (group->getBlendMode() == ALPHA_REJECT_IMPOSTOR){
			p->setAlphaRejectSettings(CMPF_GREATER_EQUAL, 128);
			//p->setAlphaRejectSettings(CMPF_GREATER_EQUAL, 64);
		} else if (group->getBlendMode() == ALPHA_BLEND_IMPOSTOR){
			p->setSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
			p->setDepthWriteEnabled(false);  
		}
	}
	}
}

ImpostorTexture::~ImpostorTexture()
{
	//Delete textures
	assert(!texture.isNull());
	String texName(texture->getName());
		
	texture.setNull();
	if (TextureManager::getSingletonPtr())
		TextureManager::getSingleton().remove(texName);
	
	//Delete materials
	for (int o = 0; o < IMPOSTOR_YAW_ANGLES; ++o){
	for (int i = 0; i < IMPOSTOR_PITCH_ANGLES; ++i){
		assert (!material[i][o].isNull());
		String matName(material[i][o]->getName());

		material[i][o].setNull();
		if (MaterialManager::getSingletonPtr())
			MaterialManager::getSingleton().remove(matName);
	}
	}
	
	//Remove self from list of ImpostorTexture's
	selfList.erase(sourceMesh);
}

void ImpostorTexture::renderTextures(ImpostorPage *group, Entity *entity)
{
	TexturePtr renderTexture;
	RenderTexture *renderTarget;
	Camera *renderCamera;
	Viewport *renderViewport;

	//Set up RTT texture
	textureSize = group->impostorResolution;
	renderTexture = TextureManager::getSingleton().createManual(getUniqueID("ImpostorTexture"), "Impostors",
				TEX_TYPE_2D, textureSize * IMPOSTOR_YAW_ANGLES, textureSize * IMPOSTOR_PITCH_ANGLES, 0, PF_A8R8G8B8, TU_RENDERTARGET);
	renderTexture->setNumMipmaps(MIP_UNLIMITED);
	
	//Set up render target
	renderTarget = renderTexture->getBuffer()->getRenderTarget(); 
	renderTarget->setAutoUpdated(false);
	
	//Set up camera
	renderCamera = group->sceneMgr->createCamera(getUniqueID("ImpostorCam"));
	renderCamera->setLodBias(1000.0f);
	renderViewport = renderTarget->addViewport(renderCamera);
	renderViewport->setOverlaysEnabled(false);
    renderViewport->setClearEveryFrame(true);
	renderViewport->setShadowsEnabled(false);
    renderViewport->setBackgroundColour(group->impostorBackgroundColor);
	
	//Set up scene node
	SceneNode* node = group->sceneMgr->getSceneNode("ImpostorPage::renderNode");
	
	node->attachObject(entity);
	node->setPosition(-entityCenter);
	
	//Set up camera FOV
	const Real objDist = entityRadius * 100;
	const Real nearDist = objDist - (entityRadius + 1); 
	const Real farDist = objDist + (entityRadius + 1);
	
	renderCamera->setAspectRatio(1.0f);
	renderCamera->setFOVy(Math::ATan(entityDiameter / objDist));
	renderCamera->setNearClipDistance(nearDist);
	renderCamera->setFarClipDistance(farDist);
	
	//Disable mipmapping (without this, masked textures look bad)
	MaterialManager *mm = MaterialManager::getSingletonPtr();
	FilterOptions oldMinFilter = mm->getDefaultTextureFiltering(FT_MIN);
	FilterOptions oldMagFilter = mm->getDefaultTextureFiltering(FT_MAG);
	FilterOptions oldMipFilter = mm->getDefaultTextureFiltering(FT_MIP);
	mm->setDefaultTextureFiltering(FO_POINT, FO_LINEAR, FO_NONE);

	//Disable fog
	FogMode oldFogMode = group->sceneMgr->getFogMode();
	ColourValue oldFogColor = group->sceneMgr->getFogColour();
	Real oldFogDensity = group->sceneMgr->getFogDensity();
	Real oldFogStart = group->sceneMgr->getFogStart();
	Real oldFogEnd = group->sceneMgr->getFogEnd();
	group->sceneMgr->setFog(FOG_NONE);
	
	//Only render the entity
	group->sceneMgr->setSpecialCaseRenderQueueMode(Ogre::SceneManager::SCRQM_INCLUDE); 
	group->sceneMgr->addSpecialCaseRenderQueue(RENDER_QUEUE_6);

	//uint8 oldRenderQueueGroup = entity->getRenderQueueGroup();
	entity->setRenderQueueGroup(RENDER_QUEUE_6);

	//Calculate the filename used to identity this render
	ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem", "BinFolder");
	String fileName = "Impostor." + entity->getMesh()->getGroup() + '.' + entity->getMesh()->getName() + ".png";

	//Attempt to load the pre-render file
	try{
		texture = TextureManager::getSingleton().load(fileName, "BinFolder", TEX_TYPE_2D, MIP_UNLIMITED);
	}
	catch (...){
		//If this has not been pre-rendered, do so now
		const float xDivFactor = 1.0f / IMPOSTOR_YAW_ANGLES;
		const float yDivFactor = 1.0f / IMPOSTOR_PITCH_ANGLES;
		for (int o = 0; o < IMPOSTOR_PITCH_ANGLES; ++o){ //4 pitch angle renders
			Radian pitch = Degree((90.0f * o) * yDivFactor); //0, 22.5, 45, 67.5

			for (int i = 0; i < IMPOSTOR_YAW_ANGLES; ++i){ //8 yaw angle renders
				Radian yaw = Degree((360.0f * i) * xDivFactor); //0, 45, 90, 135, 180, 225, 270, 315
					
				//Position camera
				renderCamera->setPosition(0, 0, 0);
				renderCamera->setOrientation(Quaternion::IDENTITY);
				renderCamera->pitch(-pitch);
				renderCamera->yaw(yaw);
				renderCamera->moveRelative(Vector3(0, 0, objDist));
						
				//Render the impostor
				renderViewport->setDimensions((float)(i) * xDivFactor, (float)(o) * yDivFactor, xDivFactor, yDivFactor);
				renderTarget->update();
			}
		}
		
		//Save RTT to file
		renderTarget->writeContentsToFile(fileName);

		//Load the render into the appropriate texture view
		texture = TextureManager::getSingleton().load(fileName, "BinFolder", TEX_TYPE_2D, MIP_UNLIMITED);
	}
	

	//entity->setRenderQueueGroup(oldRenderQueueGroup);
	group->sceneMgr->removeSpecialCaseRenderQueue(RENDER_QUEUE_6);
	group->sceneMgr->setSpecialCaseRenderQueueMode(Ogre::SceneManager::SCRQM_EXCLUDE); 

	//Re-enable mipmapping
	mm->setDefaultTextureFiltering(oldMinFilter, oldMagFilter, oldMipFilter);

	//Re-enable fog
	group->sceneMgr->setFog(oldFogMode, oldFogColor, oldFogDensity, oldFogStart, oldFogEnd);

	//Delete camera
	renderTarget->removeViewport(0);
	renderCamera->getSceneManager()->destroyCamera(renderCamera);
	
	//Delete scene node
	node->detachAllObjects();

	//Delete RTT texture
	assert(!renderTexture.isNull());
	String texName2(renderTexture->getName());

	renderTexture.setNull();
	if (TextureManager::getSingletonPtr())
		TextureManager::getSingleton().remove(texName2);
}

void ImpostorTexture::removeTexture(ImpostorTexture* Texture)
{
	//Search for an existing impostor texture, in case it was already deleted
	for(std::map<ResourceHandle, ImpostorTexture *>::iterator iter=selfList.begin();
		iter!=selfList.end(); ++iter)
	{
		if(iter->second==Texture)
		{
			delete Texture;
			return;
		}
	}
	// no need to anything if it was not found, chances are that it was already deleted
}

ImpostorTexture *ImpostorTexture::getTexture(ImpostorPage *group, Entity *entity)
{
	//Search for an existing impostor texture for the given entity
	std::map<ResourceHandle, ImpostorTexture *>::iterator iter;
	iter = selfList.find(entity->getMesh()->getHandle());
	
	//If found..
	if (iter != selfList.end()){
		//Return it
		return iter->second;		
	} else {
		//Otherwise, return a new texture
		return (new ImpostorTexture(group, entity));
	}
}
