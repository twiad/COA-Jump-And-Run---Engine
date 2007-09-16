/*-------------------------------------------------------------------------------------
Copyright (c) 2006 John Judnich

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------------*/

#ifndef __GrassLoader_H__
#define __GrassLoader_H__

#include "PagedGeometry.h"

#include "OgrePrerequisites.h"
#include "OgreMaterial.h"
#include "OgrePixelFormat.h"
#include "OgreStringConverter.h"


class GrassLayer;

/** \brief A PageLoader-derived object you can use with PagedGeometry to produce realistic grass. 

Using a GrassLoader is simple - simply create an instance, attach it to your PagedGeometry object
with PagedGeometry::setPageLoader(), and add your grass. Important: For best performance, it is
recommended that you use GrassPage (included in GrassLoader.h) to display geometry loaded by GrassLoader.
This page type is designed for best performance with this grass system. BatchPage and EntityPage
will work, although performance will be reduced slightly, and ImpostorPage will run extremely slow.

To add grass, just call addLayer(). addLayer() returns a GrassLayer object pointer, which you should
use to further configure your newly added grass. Properties like size, density, color, animation, etc.
can be controlled through the GrassLayer class.

If you enable animation on any of your grass layers, remember to call updateAnimation() every frame.
If you don't call this function, the grass will not be animated correctly.

\note By default, the GrassLoader doesn't know what shape your terrain so all grass will be placed at
0 height. To inform GrassLoader of the shape of your terrain, you must specify a height function
that returns the height (y coordinate) of your terrain at the given x and z coordinates. See
the TreeLoader2D::setHeightFunction() documentation for more information.

\warning If you attempt to use Ogre's scene queries to get the terrain height,
keep in mind that calculating the height of Ogre's built-in terrain this way can
be VERY slow if not done properly, and may cause stuttering due to long paging delays.
*/
class GrassLoader: public PageLoader
{
public:
	/** \brief Creates a new GrassLoader object. 
	\param geom The PagedGeometry object that this GrassLoader will be assigned to.*/
	GrassLoader(PagedGeometry *geom);
	~GrassLoader();

	/** \brief Adds a grass layer to the scene.
	\param material The initial grass texture to use (this can be changed later).
	
	Since all grass is potentially infinite, it is not added like normal entities which
	have a specific position. Instead you add a grass "layer" to the scene. A grass layer
	is a "carpet" of a single type of grass that gets applied everywhere in your world.
	If you want multiple types of grass with different appearances, you'll have to add
	a multiple grass layers for each style.
	
	Of course, a grass layer is not completely uniform. The GrassLayer class contains
	functions to vary grass size and density levels as desired.

	\see GrassLayer class for more information. */
	GrassLayer *addLayer(const Ogre::String &material);

	/** \brief Returns a list of added grass layers.
	
	This function returns a std::list<GrassLayer*> reference, which contains all grass
	layers which have been added to this GrassLoader. */
	inline std::list<GrassLayer*> &getLayerList() { return layerList; }

	/** \brief Updates the grass animation state.
	
	You should to call this function every frame in order for the grass animation to
	be updated. Otherwise, any breeze animation you enabled for the grass will freeze
	until you resume calling this function. */
	void updateAnimation();

	/** \brief Sets the global wind direction for this GrassLoader.

	GrassLayer animation properties are used to configure the most of the animation
	behavour (sway length, speed, etc.), but wind direction is not included in GrassLayer
	since this is really a global property. Using this function, you can set the "global"
	wind direction which affects all animated grass associated with this PageLoader.

	Default value is Vector3::UNIT_X.
		
	\note This only affects grass layers which have breeze animations enabled.
	*/
	inline void setWindDirection(Ogre::Vector3 &dir) { windDir = dir; }

	/** \brief Returns the global wind direction for this GrassLoader.

	\see setWindDirection() for more information about the wind direction. */
	inline Ogre::Vector3 &getWindDirection() { return windDir; }

	/** \brief Sets the global density factor for this GrassLoader.

	This function can be used to up-scale or down-scale the density of all grass
	associated with this GrassLoader. This is typically used to provide the user
	the option to reduce grass density for better performance on slower machines.

	Final density values are calculated by multiplying the layer density by this
	density factor. For example, a layer with .4 density and a density factor of .5
	will result in a final density of .2 (.5 * .4)

	By default, the density factor is set to 1.0 so the layer density is not modified.
	*/
	inline void setDensityFactor(float density) { densityFactor = density; }

	/** \brief Returns the global density factor for this GrassLoader.

	\see setDensityFactor() for more information about the density factor. */
	inline float getDensityFactor() { return densityFactor; }

	/** \brief Sets the render queue group the grass will be rendered through
	\param queueID Enumerated value of the queue group to use
	
	Like Ogre's MovableObject::setRenderQueueGroup(), this allows you to customize
	the rendering order of your scene. Since grass is transparent, it's likely that
	you may encounter alpha-sorting issues between grass and your particle effects,
	for example. In this case you can use this function to adjust the rendering order
	of the grass to fix the problem.

	If you don't call this function, the RENDER_QUEUE_6 queue will be used.

	\note Once grass is loaded and being rendered, this won't have any effect on it.
	Be sure to call this function before the scene begins rendering, otherwise you will
	have to call PagedGeometry::reloadGeometry() to force a reload in order for the changes
	to take effect.	*/
	inline void setRenderQueueGroup(Ogre::uint8 queueID) { renderQueue = queueID; }

	/** \brief Sets the height function used to calculate grass Y coordinates
	\param heightFunction A pointer to a height function

	Unless you want all your grass placed at 0 height, you need to specify a height function
	so GrassLoader will be able to calculate the Y coordinate. The height function given
	to setHeightFunction() should use the following prototype (although you can name the
	function anything you want):

	\code
	Real getHeightAt(Real x, Real z);
	\endcode
	
	After you've defined a height function, using setHeightFunction is easy:

	\code
	pageLoader2D->setHeightFunction(&getHeightAt);
	\endcode
	*/
	void setHeightFunction(Ogre::Real (*heightFunction)(Ogre::Real x, Ogre::Real z)) { this->heightFunction = heightFunction; }


	/** INTERNAL FUNCTION - DO NOT USE */
	void loadPage(PageInfo &page);
	/** INTERNAL FUNCTION - DO NOT USE */
	void unloadPage(const PageInfo &page);

private:
	//List of grass types
	std::list<GrassLayer*> layerList;

	//Height data
	Ogre::Real (*heightFunction)(Ogre::Real x, Ogre::Real z);	//Pointer to height function

	//Misc.
	PagedGeometry *geom;
	Ogre::uint8 renderQueue;
	float densityFactor;

	//Animation
	Ogre::Timer windTimer;
	Ogre::Vector3 windDir;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID()
	{
		return "GrassLDR" + Ogre::StringConverter::toString(++GUID);
	}
};


/** \brief A technique used to render grass. Passed to GrassLayer::setRenderTechnique(). */
enum GrassTechnique
{
	/// Grass constructed of randomely placed and rotated quads
	GRASSTECH_QUAD,
	/// Grass constructed of two quads forming a "X" cross shape
	GRASSTECH_CROSSQUADS
};

/** \brief A technique used to fade grass into the distance. Passed to GrassLayer::setFadeTechnique(). */
enum FadeTechnique
{
	/// Grass that fades into the distance with transparency. Fairly effective in most cases.
	FADETECH_ALPHA,
	/// Grass that fades in by "growing" up out of the ground. Very effective when grass fades in against the sky, or with alpha-rejected grass.
	FADETECH_GROW,
	/// Grass that fades in by slowly becoming opaque while it "grows" up out of the ground. Effective with alpha grass fading in against the sky.
	FADETECH_ALPHAGROW
};



/** \brief Specifies which color channel(s) to extract from an image. Passed to GrassLayer::setDensityMap() and GrassLayer::setColorMap(). */
enum MapChannel {
	/// Use only the image's red color channel
	CHANNEL_RED,
	/// Use only the image's green color channel
	CHANNEL_GREEN,
	/// Use only the image's blue color channel
	CHANNEL_BLUE,
	/// Use only the image's alpha channel
	CHANNEL_ALPHA,
	/// Use the image's full RGB color information
	CHANNEL_COLOR
};

/** \brief A data structure giving you full control over grass properties.

Grass is added to the scene through GrassLoader::addLayer(). This function returns a
pointer to a GrassLayer object; through this object you can configure the grass layer
any way you like - size, density, render technique, animation, etc. Simply call the
appropriate "set" member function to set the desired property.

Remember that you cannot create or delete layers directly. Layers can only be created
with GrassLoader::addLayer(), and may not be deleted manually (they will be deleted when
their GrassLoader is deleted).
*/
class GrassLayer
{
public:
	/** \brief Sets the material that is applied to all grass billboards/quads */
	void setMaterialName(const Ogre::String &matName);

	/** \brief Sets the minimum size that grass quads/billboards will be */
	void setMinimumSize(float width, float height);

	/** \brief Sets the maximum size that grass quads/billboards will be */
	void setMaximumSize(float width, float height);

	/** \brief Sets the maximum density (measured in grass quads/billboards per square unit) of grass */
	void setDensity(float density) { this->density = density; }

	/** \brief Sets the density map used for this grass layer
	\param mapFile The density map image
	\param channel The color channel(s) to from the image to interpret as density
	
	A density map is simply a greyscale image, similar to a heightmap, that specifies the grass
	density on your map. Full pixel intensity indicates that grass should be fully dense at that
	point (the maximum density is specified by GrassLayer::setDensity()), while no pixel intensity
	indicates that no grass should appear at that location.

	The channel parameter allows you to extract the density information from the image's
	red, green, blue, alpha, or color values. For example, you may store density values in the
	alpha channel, in which case you would use CHANNEL_ALPHA. By default, CHANNEL_COLOR is used,
	which means the image color is converted to greyscale internally and used as a density map.
	
	Note that since GrassLayer by default has no idea of your terrain/world boundaries, so you
	must specify a rectangular/square area of your world that is affected by density/color maps.
	To do this, use the setMapBounds() function. Normally this is set to your terrain's bounds
	so the density/color map is aligned to your heightmap, but you could apply it anywhere you
	want. */
	void setDensityMap(const Ogre::String &mapFile, MapChannel channel = CHANNEL_COLOR);

	/** \brief Sets the density map used for this grass layer

	Overloaded to accept a Texture object. See the original setDensityMap() documentation above
	for more detailed information on density maps.
	
	\note The texture data you provide is copied into the GrassLayer's own memory space, so you
	can delete the texture after calling this function without risk of crashing. */
	void setDensityMap(Ogre::Texture *map, MapChannel channel = CHANNEL_COLOR);

	/** \brief Sets the color map used for this grass layer
	\param mapFile The color map image
	\param channel The color channel(s) to from the image to use
	
	A color map is simply a texture that allows you to vary the color and shading of grass
	across your world for a more realistic look. For example, adding a dark spot to the center
	of your color map will make grass near the center of your terrain look darker, as long as
	you have the color map aligned to your terrain (see setMapBounds()).

	The channel parameter allows you to extract the color information from the image's
	red, green, blue, alpha, or color values. For example, you may store the desired shade of your
	grass in the red channel of an image, in which case you would use CHANNEL_RED (when you choose
	a single channel, it is converted to a greyscale color). By default, CHANNEL_COLOR is used,
	which uses the full color information availible in the image.
	
	Remember that since GrassLayer by default has no idea of your terrain/world boundaries, so you
	must specify a rectangular/square area of your world that is affected by density/color maps.
	To do this, use the setMapBounds() function. Normally this is set to your terrain's bounds
	so the density/color map is aligned to your heightmap, but you could apply it anywhere you
	want. */
	void setColorMap(const Ogre::String &mapFile, MapChannel channel = CHANNEL_COLOR);

	/** \brief Sets the color map used for this grass layer

	Overloaded to accept a Texture object. See the original setColorMap() documentation above
	for more detailed information on color maps.
	
	\note The texture data you provide is copied into the GrassLayer's own memory space, so you
	can delete the texture after calling this function without risk of crashing. */
	void setColorMap(Ogre::Texture *map, MapChannel channel = CHANNEL_COLOR);

	/** \brief Sets the boundaries of the density/color maps
	\param bounds The map boundary

	By default, the GrassLayer has no knowledge of your terrain/world boundaries, so you must
	use this function to specify a rectangular/square area of your world, otherwise density/color maps
	won't work properly. The boundary given to this function defines the area where density/color
	maps take effect. Normally this is set to your terrain's bounds so the density/color map is aligned
	to your heightmap, but you could apply it anywhere you want.
	
	\note The grass system is infinite, so there's no need to worry about using too expansive
	boundaries. This setting simply configures the behavior of density and color maps. */
	void setMapBounds(const Ogre::TRect<Ogre::Real> &bounds) { mapBounds = bounds; }

	/** \brief Gets a pointer to the density map being used
	
	You can use this function to access the internal density map used by the GrassLoader. The
	PixelBox returned is an image in PF_BYTE_L (aka. PF_L8) byte format. You can modify this image
	any way you like in real-time, so long as you do not change the byte format.

	This function is useful in editors where the density map needs to be changed dynamically.
	Note that although you can change this map in real-time, the changes won't be uploaded to your
	video card until you call PagedGeometry::reloadGeometry(). If you don't, the grass you see
	will remain unchanged. */
	Ogre::PixelBox *getDensityMapData() { return densityMap; }

	/** \brief Gets a pointer to the color map being used
	
	You can use this function to access the internal color map used by the GrassLoader. The
	PixelBox returned is an image in PF_A8R8G8B8 format when running with DirectX, and
	PF_A8B8G8R8 when running with OpenGL. You can modify this image any way you like in
	real-time, so long as you do not change the byte format.

	This function is useful in editors where the color map needs to be changed dynamically.
	Note that although you can change this map in real-time, the changes won't be uploaded to your
	video card until you call PagedGeometry::reloadGeometry(). If you don't, the grass you see
	will remain unchanged. */
	Ogre::PixelBox *getColorMapData() { return densityMap; }

	/** \brief Sets the technique used to render this grass layer
	\param style The GrassTechnique style used to display grass.
	\param blendBase Whether or not grass base blending is enabled.
	
	The "style" setting allows you to choose from various construction methods, such as
	sprite-style grass quads, plain 3D quads, etc. See the GrassTechnique documentation
	for more information about this option. GRASSTECH_QUAD is used by default.

	Setting "blendBase" to true will enable grass base blending, a technique which helps
	reduce the unnatural flat appearance of grass quads near the camera. Since the flatness
	is most obvious where the grass intersects the terrain, this technique attempts to
	smoothly blend the base of near-by grass into the terrain.

	\note Base blending does not work well with alpha-rejected textures.
	*/
	void setRenderTechnique(GrassTechnique style, bool blendBase = false);

	/** \brief Sets the technique used when fading out distant grass
	\param style The FadeTechnique style used to fade grass.
	
	This "style" setting allows you to choose from various fade techniques. Depending on
	your scene, certain techniques may look better than others. The most compatible method
	is FADETECH_ALPHA (used by default), although better results can usually be achieved
	with other methods. See the FadeTechnique documentation for more information.
	*/
	void setFadeTechnique(FadeTechnique style);

	/** \brief Enables/disables animation on this layer
	
	Always use this function to disable animation, rather than setting SwayLength or SwaySpeed
	to 0. This function will use a different vertex shader which means improved performance
	when animation is disabled.
	*/
	void setAnimationEnabled(bool enabled);

	/** \brief Sets how far grass should sway back and forth

	\note Since this is measured in world units, you may have to adjust this depending on
	the size of your grass as set by setMinimumSize() and setMaximumSize().*/
	void setSwayLength(float mag) { animMag = mag; }

	/** \brief Sets the sway speed of the grass (measured in "sways-per-second") */
	void setSwaySpeed(float speed) { animSpeed = speed; }

	/** \brief Sets the smooth distribution (positional phase shift) of the grass swaying animation

	If you set this to 0, grass animation will look very unnatural, since all the grass sway motions
	will be in perfect synchronization (everything sways to the right, then everything sways to the
	left, etc.) This sets the "positional phase shift", which gives the grass a "wave" like phase
	distribution. The higher this value is, the more "chaotic" the wind will appear. Lower values give
	a smoother breeze appearance, but values too high can look unrealistic.
	*/
	void setSwayDistribution(float freq) { animFreq = freq; }

private:
	friend class GrassLoader;

	/** \brief Do not create a GrassLayer directly - use GrassLoader->addLayer() */
	GrassLayer(PagedGeometry *geom);

	/** \brief Do not delete a GrassLayer yourself - the GrassLoader will do this automatically when it's deleted */
	~GrassLayer();

	//Updates the vertex shader used by this layer based on the animate enable status
	void _updateShaders();

	//Used by GrassLoader::loadPage() - populates an array of grass positions based on the density map
	//Returns the final number of grasses, which will always be <= grassCount
	unsigned int _populateGrassList(PageInfo page, float *posBuff, unsigned int grassCount);

	//Returns the density map value at the given location
	//Make sure a density map exists before calling this.
	inline float _getDensityAt(float x, float z)
	{
		assert(densityMap);

		unsigned int mapWidth = (unsigned int)densityMap->getWidth();
		unsigned int mapHeight = (unsigned int)densityMap->getHeight();
		float boundsWidth = mapBounds.width();
		float boundsHeight = mapBounds.height();

		unsigned int xindex = mapWidth * (x - mapBounds.left) / boundsWidth;
		unsigned int zindex = mapHeight * (z - mapBounds.top) / boundsHeight;
		if (xindex < 0 || zindex < 0 || xindex >= mapWidth || zindex >= mapHeight)
			return 0.0f;
		
		Ogre::uint8 *data = (Ogre::uint8*)densityMap->data;
		float val = data[mapWidth * zindex + xindex] / 255.0f;

		return val;
	}

	//Returns the color map value at the given location
	inline Ogre::uint32 _getColorAt(float x, float z)
	{
		if (!colorMap)
			return 0xFFFFFFFF;

		unsigned int mapWidth = (unsigned int)colorMap->getWidth();
		unsigned int mapHeight = (unsigned int)colorMap->getHeight();
		float boundsWidth = mapBounds.width();
		float boundsHeight = mapBounds.height();

		unsigned int xindex = mapWidth * (x - mapBounds.left) / boundsWidth;
		unsigned int zindex = mapHeight * (z - mapBounds.top) / boundsHeight;
		if (xindex < 0 || zindex < 0 || xindex >= mapWidth || zindex >= mapHeight)
			return 0xFFFFFFFF;
		
		Ogre::uint32 *data = (Ogre::uint32*)colorMap->data;
		return data[mapWidth * zindex + xindex];
	}


	//Grass material/shape properties
	Ogre::MaterialPtr material;
	float density;
	float minWidth, maxWidth;
	float minHeight, maxHeight;

	FadeTechnique fadeTechnique;
	GrassTechnique renderTechnique;

	//Property maps
	Ogre::PixelBox *densityMap;
	Ogre::PixelBox *colorMap;
	Ogre::TRect<Ogre::Real> mapBounds;
	void _clearDensityMap();
	void _clearColorMap();

	//Grass shader properties
	bool animate, blend, shaderNeedsUpdate;
	float animMag, animSpeed, animFreq;

	//Current frame of animation for this layer
	float waveCount;

	PagedGeometry *geom;
};


/** \brief A custom page type designed specifically for use with GrassLoader.

You can use this in your own project if you want, but remember that no optimizations
are performed. The given entity is simply cloned and attached to a new scene node as
quickly and simply as possible (this means there's no pooling overhead as in EntityPage,
but it also means potentially poor performance if you don't know what you're doing).
*/
class GrassPage: public GeometryPage
{
public:
	void init(PagedGeometry *geom);
	void addEntity(Ogre::Entity *ent, const Ogre::Vector3 &position, const Ogre::Quaternion &rotation, const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
	void removeEntities();
	void setFade(bool enabled, Ogre::Real visibleDist, Ogre::Real invisibleDist) {}
	void setVisible(bool visible);

private:
	Ogre::SceneManager *sceneMgr;

	std::list<Ogre::SceneNode*> nodeList;

	static unsigned long GUID;
	static inline Ogre::String getUniqueID()
	{
		return "GrassPage" + Ogre::StringConverter::toString(++GUID);
	}
};

#endif
