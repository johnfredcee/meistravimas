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
		std::shared_ptr<Texture> makeTexture(const char *name, const bool sdl = false);

		/**
		 * Actually create a texture via the service
		 */
		std::shared_ptr<Texture> makeTexture(const Image* img, const bool sdl = false);
		
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
		void 			  makeSDLTexture(const Image* image);
		GLuint            gl_texture;
		
	public:
		/**
		 * Create a texture from a loaded image 
         */
		Texture(const Image* image, const bool sdl = false);

		/**
         * Test for validity 
         */
		bool isValid() const { return texture_ptr != nullptr; }
		operator bool() const { return texture_ptr != nullptr; }

		/**
         * Return a pointer to an SDL texture for rendering
         */
	    SDL_Texture* getSDL_Texture();
		void selectSDL_Texture();
		void deselectSDL_Texture();
		void select(GLuint i = 0);
		void deselect(GLuint i = 0);
		~Texture();
	};


}

#endif
