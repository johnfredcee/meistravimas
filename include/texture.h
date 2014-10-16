#ifndef TEXTURE_H
#define TEXTURE_H


namespace venk
{
	class Image;
	class Program;
	class Texture;

	class TextureService : public Service<TextureService>
	{
		friend class Texture;
	public:
		typedef TextureService *ServicePtr_t;

		/**
		 * Initialse the texture subsystem
		 * @return true if successful
		 */
		static bool initialise(TextureService* self);

		/**
		 * Shutdown the texture subsystem.
		 * @return returns true if successful
		 */
		static bool shutdown(TextureService* self);

		/**
		 * Actually create a texture via the service
		 */
		std::shared_ptr<Texture> makeTexture(const char *name);

		/**
		 * Actually create a texture via the service
		 */
		std::shared_ptr<Texture> makeTexture(const Image* img);
		
		void setRenderer(std::weak_ptr<SDL_Renderer> renderer) { mCurrentRenderer = renderer; }
		void clearRenderer() { mCurrentRenderer = std::weak_ptr<SDL_Renderer>(); }

	private:
		/**
		 * Renderer we are currently using  for textures
		 */   
		static std::weak_ptr<SDL_Renderer> mCurrentRenderer;
	};

	class Texture
	{
		friend class Program;

	private:
		SDL_Texture*      texture_ptr;
	public:
		Texture(const Image* image);
		bool isValid() const { return texture_ptr != nullptr; }
		operator bool() const { return texture_ptr != nullptr; }
	    SDL_Texture* getSDL_Texture() { return texture_ptr; };
		void select();
		void deselect();
		~Texture();
	};


}

#endif
