#ifndef SPRITES_H
#define SPRITES_H

namespace venk {
class Sprite;

class SpriteService : public Service<SpriteService> {
	friend class Sprite;

public:
	typedef SpriteService *ServicePtr_t;

	/**
	 * Initialse the image subsystem
	 * @return true if successful
	 */
	static bool initialise(SpriteService* self);

	/**
     * Create an individual sprite of w,h pixels at x,y in the texture
     */
	static std::shared_ptr<Sprite> createSprite(std::shared_ptr<Texture> texture, int sheetx, int sheety, int w, int h);

	/** 
     * render alloc active sprites
	 */
	void render(double alpha, SDL_Window* window, SDL_Renderer* renderer);

	/**
     * update all active sprites (deleting unreferenced ones)
     */
	void update(double t, double dt);
	
	/**
	 * Shutdown the image subsystem.
	 * @return returns true if successful
	 */
	static bool shutdown(SpriteService* self);

    
	typedef std::vector< std::weak_ptr<Sprite> > SpriteTable_t;
	
protected:
	static std::shared_ptr<Buffer>  vertexBuffer;
	static std::shared_ptr<Buffer>  uvBuffer;
	static std::shared_ptr<Buffer>  indexBuffer;
	static std::shared_ptr<Program> spriteShader;
	static SpriteTable_t            sprites;
	static Matrix44                 projection;
};

class Sprite
{
	friend class SpriteService;
private:
	float x,y;
	float scale;
	float u0, v0, u1, v1;
	std::shared_ptr<Texture> texture;
	
public:
	Sprite();	
	Sprite(std::shared_ptr<Texture> texture, int sheetx, int sheety, float w, float h);
	~Sprite();
	void render(double alpha, SDL_Window* window, SDL_Renderer* renderer);
	void update(double t, double dt);
	float getScale() const;
	void setScale(float newScale);
    float getX() const;
	float getY() const;
	void setXY(float x, float y);
};

}

#endif
