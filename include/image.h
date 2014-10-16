#ifndef IMAGE_H
#define IMAGE_H


namespace venk
{
class Image;
class TileSheet;

class ImageService : public Service<ImageService>
{
public:
    typedef ImageService *ServicePtr_t;

    /**
     * Initialse the image subsystem
     * @return true if successful
     */
    static bool initialise(ImageService* self);

    /**
     * Shutdown the image subsystem.
     * @return returns true if successful
     */
    static bool shutdown(ImageService* self);

    /**
     * Actually create a image via the service
     */
    std::shared_ptr<Image> loadImage(const char *name);

    /**
     * Load a image by name..
     */
    std::shared_ptr<Image> getImage(const char *name);

private:
    typedef std::unordered_map< std::string, std::weak_ptr<Image> > ImageLookupTable_t;
    ImageLookupTable_t imageTable;
    const char *computeName(const char *name);
};

class Image
{
    friend class ImageService;
private:
    Uint8 *mPixels;
    int    mWidth;
    int    mHeight;
    int    mChannels;
  
    static const int maxImageNameLength = 63;

public:

    Image(const std::string& fileName);

    ~Image();

    // return an SDL_Surface that represents the image
    std::shared_ptr<SDL_Surface> surface() const;
    
    // return a pointer to the raw data
    const void *getData() const
    {
        return mPixels;
    }

    // access dimensions and things
    int width() const
    {
        return (mPixels != nullptr) ? mWidth : 0;
    }
    int height() const
    {
        return (mPixels != nullptr) ? mHeight : 0;
    }
    bool   hasAlpha() const
    {
        return (mPixels != nullptr) ? (mChannels <= 3) : false;
    }
	operator bool() const
	{
		return (mPixels != nullptr);
	}
};

}
#endif
