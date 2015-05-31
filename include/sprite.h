#ifndef SPRITES_H
#define SPRITES_H

namespace venk {

class Sprite;
class SpriteBatch;

class SpriteService : public Service<SpriteService> {
	friend class Sprite;
	friend class SpriteBatch;
public:	
	typedef SpriteService *ServicePtr_t;
	typedef std::vector< std::shared_ptr<SpriteBatch> > SpriteBatchArray_t;
	
	/**
	 * Initialse the image subsystem
	 * @return true if successful
	 */
	static bool initialise(SpriteService* self);

	/**
     * start walking over the batches
     */
	void beginBatchWalk();

	/**
     * get the next batch
	 */
	std::weak_ptr<SpriteBatch> nextBatch();
	
	std::weak_ptr<SpriteBatch> addBatch(std::shared_ptr<Texture> texture);

	bool removeBatch(Uint32 id);
		
	/**
	 * Shutdown the image subsystem.
	 * @return returns true if successful
	 */
	static bool shutdown(SpriteService* self);
    	
protected:
	static Uint32                        nextBatchId;	
	static std::shared_ptr<Program>      spriteShader;
	static SpriteBatchArray_t            batches;
	static SpriteBatchArray_t::iterator  batchWalker;
	static Matrix44                      projection;

};

class SpriteBatch
{
	friend class SpriteService;
private:
	std::shared_ptr<Texture> texture;
	std::vector< std::shared_ptr<Sprite> > sprites;
	std::shared_ptr<Buffer>  vertexBuffer;
	std::shared_ptr<Buffer>  uvBuffer;
	std::shared_ptr<Buffer>  indexBuffer;
	Uint32 id;
	
public:

	SpriteBatch(Uint32 batchId, std::shared_ptr<Texture> batchTexture);

	~SpriteBatch(); 

	/**
     * Number of sprites in this batch
     */
	size_t size() { return sprites.size(); };

	/**
     * Add a new sprite to the batch
     */
	std::weak_ptr<Sprite> addSprite(int sheetx, int sheety, int w, int h);
	
	/** 
     * render alloc active sprites
	 */
	void render(double alpha, SDL_Window* window, SDL_Renderer* renderer);

	/**
     * update all active sprites (deleting unreferenced ones)
     */
	void update(double t, double dt);
	
};

class Sprite
{
	friend class SpriteService;
	friend class SpriteBatch;
private:
	float x,y;
	float w,h;
	float scale;
	float u0, v0, u1, v1;
	
public:
	Sprite();	
	~Sprite();
	float getScale() const;
	void setScale(float newScale);
    float getX() const;
	float getY() const;
	void setXY(float x, float y);
};

}

#endif
