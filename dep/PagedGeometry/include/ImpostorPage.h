/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

//ImpostorPage.h
//ImposterPage is an extension to PagedGeometry which displays entities as imposters.
//-------------------------------------------------------------------------------------

#ifndef __ImpostorPage_H__
#define __ImpostorPage_H__

#include "PagedGeometry.h"
#include "StaticBillboardSet.h"

#include "OgrePrerequisites.h"
#include "OgreTextureManager.h"
#include "OgreRenderTexture.h"


#define IMPOSTOR_YAW_ANGLES 8
#define IMPOSTOR_PITCH_ANGLES 4

class ImpostorBatch;
class ImpostorTexture;

//Blend modes used by ImpostorPage::setBlendMode()
typedef enum ImpostorBlendMode {
	ALPHA_REJECT_IMPOSTOR,
	ALPHA_BLEND_IMPOSTOR
};

//-------------------------------------------------------------------------------------
/**
\brief The ImpostorPage class renders entities as impostors (billboard images that look just like the real entity).

This is one of the geometry page types included in the StaticGeometry engine. These
page types should be added to a PagedGeometry object with PagedGeometry::addDetailLevel()
so the PagedGeometry will know how you want your geometry displayed.

To use this page type, use:
\code
PagedGeometry::addDetailLevel<ImpostorPage>(farRange);
\endcode

Of all the page types included in the PagedGeometry engine, this one is the fastest. It
uses impostors (billboards that look just like the real entity) to represent entities.
This way, your video card only has to render a bunch of 2D images, instead of a full 3D
mesh. Imposters are generally used off in the distance slightly, since they don't always
look exactly like the real thing, especially up close (since they are flat, and sometimes
slightly pixelated).

\note Impostors are generated only once for each entity. If you make any changes to your
entities, you'll have to force impostor regeneration by deleting the prerender files
located in your executable's working directory (named "Impostor.[ResourceGroup].[Entity].png")
*/
class ImpostorPage: public GeometryPage
{
	friend class ImpostorBatch;
	friend class ImpostorTexture;

public:
	void init(PagedGeometry *geom);
	~ImpostorPage();
	
	void setRegion(Ogre::Real left, Ogre::Real top, Ogre::Real right, Ogre::Real bottom);
	void addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale);
	void build();
	void removeEntities();
	
	void setVisible(bool visible);
	void setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist);

	void update();

	/**
	\brief Sets the resolution for single impostor images.
	\param pixels The width/height of one square impostor render
	
	The default impostor resolution is 128x128. Note that 32 impostor images
	will be stored in a single texture (8 x 4), so a impostor resolution of 128,
	for example, results in final texture size of 1024 x 512.
	
	\note Calling this function will have no effect unless it is done before
	any entities are added to any page. Also remember that you may have to
	delete the old impostor renders (located in your exe's directory) in
	order for the new ones to be generated.
	*/
	static void setImpostorResolution(int pixels) { impostorResolution = pixels; }

	/**
	\brief Sets the background color used when rendering impostor images.
	\param color The background color

	Choosing an impostor color that closely matches the main color of your objects
	is important to reduce mipmap artifacts. When distant objects are displayed as
	impostors, hardware mipmapping can cause the color surrounding your object (the
	background color) to "bleed" into the main image, which can result in incorrectly
	tinted distant objects.

	The default background color is ColourValue(0.0f, 0.3f, 0.0f, 0.0f), or dark green
	(this color was chosen because the main use of ImpostorPage is for trees, bushes, etc.)

	\note Calling this function will have no effect unless it is done before
	any entities are added to any page. Also remember that you may have to
	delete the old impostor renders (located in your exe's directory) in
	order for the new ones to be generated.
	*/
	static void setImpostorColor(const Ogre::ColourValue &color) { impostorBackgroundColor = color; }
	

	inline void setBlendMode(ImpostorBlendMode blendMode) { this->blendMode = blendMode; }
	inline ImpostorBlendMode getBlendMode() { return blendMode; }

protected:
	Ogre::SceneManager *sceneMgr;
	PagedGeometry *geom;

	ImpostorBlendMode blendMode;
	static int impostorResolution;
	static Ogre::ColourValue impostorBackgroundColor;
	
	static unsigned int selfInstances;
	static unsigned int updateInstanceID;
	unsigned int instanceID;

	Ogre::Timer updateTimer;

	Ogre::Vector3 center;
	int aveCount;
	
	std::map<Ogre::ResourceHandle, ImpostorBatch *> impostorBatches;
};


//-------------------------------------------------------------------------------------
//This is used internally by ImpostorPage to store a "batch" of impostors. Similar
//impostors are all batched into ImpostorBatch'es, which contain a BillboardSet (where
//the actual billboards are), and a pointer to an existing ImpostorTexture.
class ImpostorBatch
{
public:
	static ImpostorBatch *getBatch(ImpostorPage *group, Ogre::Entity *entity);
	~ImpostorBatch();

	inline void build()
	{
		bbset->build();
	}

	inline void clear()
	{
		bbset->clear();
	}

	inline void setVisible(bool visible)
	{
		bbset->setVisible(visible);
	}

	inline void setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist)
	{
		bbset->setFade(enabled, visibleDist, invisibleDist);
	}

	inline void addBillboard(const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale);

	void setAngle(float pitchDeg, float yawDeg);

protected:
	ImpostorBatch(ImpostorPage *group, Ogre::Entity *entity);

	ImpostorTexture *tex;
	StaticBillboardSet *bbset;

	ImpostorPage *igroup;

	Ogre::uint16 pitchIndex, yawIndex;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID(const Ogre::String &prefix)
	{
		return prefix + Ogre::StringConverter::toString(++GUID);
	}
};


//-------------------------------------------------------------------------------------
//This is used internally by ImpostorPage. An ImpostorTexture is actually multiple
//images of an entity from various rotations. ImpostorTextures are applied
//to billboards to create the effect of 3D shapes, when in reality they are simply
//flat billboards.
class ImpostorTexture
{
	friend class ImpostorBatch;

public:
	/** Returns a pointer to an ImpostorTexture for the specified entity. If one does not
	already exist, one will automatically be created.
	*/
	static ImpostorTexture *getTexture(ImpostorPage *group, Ogre::Entity *entity);
	
	/** remove created texture, note that all of the ImposterTextures
	must be deleted at once, because there is no track if a texture is still
	being used by something else
	*/
	static void removeTexture(ImpostorTexture* Texture);

	~ImpostorTexture();
	
protected:
	ImpostorTexture(ImpostorPage *group, Ogre::Entity *entity);

	static std::map<Ogre::ResourceHandle, ImpostorTexture *> selfList;
	
	void renderTextures(ImpostorPage *group, Ogre::Entity *entity);
	
	unsigned int textureSize;

	Ogre::MaterialPtr material[IMPOSTOR_PITCH_ANGLES][IMPOSTOR_YAW_ANGLES];
	Ogre::TexturePtr texture;

	Ogre::ResourceHandle sourceMesh;
	Ogre::AxisAlignedBox boundingBox;
	float entityDiameter, entityRadius;
	Ogre::Vector3 entityCenter;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID(const Ogre::String &prefix)
	{
		return prefix + Ogre::StringConverter::toString(++GUID);
	}
};



//-------------------------------------------------------------------------------------
//This is an inline function from ImposterBatch that had to be defined down below the
//ImpostorTexture class, because it uses it.
void ImpostorBatch::addBillboard(const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale)
{
	//float degrees = (Math::ACos(rotation.w)*2.0f).valueDegrees();
	const Ogre::Vector3 zVector = rotation * Ogre::Vector3::UNIT_Z;
	float degrees = Ogre::Math::ATan2(zVector.x, zVector.z).valueDegrees();
	if (degrees < 0) degrees += 360;

	int n = IMPOSTOR_YAW_ANGLES * (degrees / 360.0f) + 0.5f;
	Ogre::uint16 texCoordIndx = (IMPOSTOR_YAW_ANGLES - n) % IMPOSTOR_YAW_ANGLES;

	bbset->createBillboard(position + (rotation * tex->entityCenter) * scale,
							tex->entityDiameter * 0.5f * (scale.x + scale.z),
							tex->entityDiameter * scale.y,
							texCoordIndx);
}


#endif
