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
		
	};

	class Texture
	{
		friend class Program;

	private:
		GLuint            gl_texture;
		
	public:
		int				  width;
		int				  height;
		bool			  alpha;
		
		
		/**
		 * Create a texture from a loaded image 
         */
		Texture(const Image* image);

		/**
         * Select the texture as current for operation 
         */
		void select(GLuint i = 0);

		/**
         * Deselect their texture as current for operation 
         */
		void deselect(GLuint i = 0);

		/** 
         * appy the texture to the shader as a uniform 
         */
		// TODO
		
		~Texture();
	};


}

#endif
