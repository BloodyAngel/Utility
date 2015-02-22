#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <boost/filesystem.hpp>

#include <type_traits>
#include <map>
#include <mutex>
#include <atomic>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


template<typename T, typename _ContentHandle>
class Content;



namespace do_not_use_directly{
    struct Load_Texture{
        static std::unique_ptr<sf::Texture> load(std::string str){
            std::unique_ptr<sf::Texture> texture = std::make_unique<sf::Texture>(sf::Texture());
            texture->loadFromFile(str);
            texture->setSmooth(true);
            texture->setRepeated(true);
            return texture;
        }
        static void release(sf::Texture*) { /* not required, delete will automatically be called by unique_ptr */ }
    };

    struct Load_SoundBuffer{
        static std::unique_ptr<sf::SoundBuffer> load(std::string str){
            std::unique_ptr<sf::SoundBuffer> sound = std::make_unique<sf::SoundBuffer>(sf::SoundBuffer());
            sound->loadFromFile(str);
            return sound;
        }
        static void release(sf::SoundBuffer*) { /* not required, delete will automatically be called by unique_ptr */ }
    };

    struct Load_Music{
        static std::unique_ptr<sf::Music> load(std::string str){
            std::unique_ptr<sf::Music> music(new sf::Music());
            music->openFromFile(str);
            return music;
        }
        static void release(sf::Music*) { /* not required, delete will automatically be called by unique_ptr */ }
    };
}


typedef Content<sf::SoundBuffer, do_not_use_directly::Load_SoundBuffer> SoundBufferContent;
typedef Content<sf::Texture    , do_not_use_directly::Load_Texture    >     TextureContent;
typedef Content<sf::Music      , do_not_use_directly::Load_Music      >       MusicContent;



template<typename T, typename _ContentHandle>
class Content final{
public:
    Content() = default;
    ~Content(){
         this->freeContent();
    }

    template<typename _StringType>
    explicit Content(_StringType&& path){
       this->loadContent(std::forward<_StringType>(path));
    }

    explicit Content(const Content& content) : m_Content(content.m_Content){
        ++Content::ContentStorageCounter[content.m_Content];
    }


    template<typename _PathType>
    static void setContentRoot(_PathType&& path){
        Content::ContentPath = std::forward<_PathType>(path);
    }


    template<typename _StringType>
    void loadContent(_StringType&& relative_path){
        namespace bf = boost::filesystem;

        std::lock_guard<std::recursive_mutex> m(Content::StorageMutex); // thread safty, do not change order of lock_guard and freeContent!
        this->freeContent();

        auto element = Content::ContentStorage.find(relative_path);

        if (element != Content::ContentStorage.end()){
            this->m_Content = element->second.get();
            ++Content::ContentStorageCounter[this->m_Content];
        }
        else{
            bf::path p = std::forward<_StringType>(relative_path);
            std::string absolutePath = bf::absolute(Content::ContentPath / p).string();

            std::unique_ptr<T> ptr = _ContentHandle::load(std::move(absolutePath));
            this->m_Content = ptr.get();
            Content::ContentStorageCounter[this->m_Content] = 1;

            Content::ContentStorage[relative_path] = std::move(ptr);
        }
    }

    Content& operator=(Content&& content) noexcept {
        this->freeContent();
        this->m_Content = std::move(content.m_Content);
        content.m_Content = nullptr;
        return *this;
    }
    Content& operator= (const Content& content){
        this->freeContent();
        this->m_Content = content.m_Content;
        ++Content::ContentStorageCounter[content.m_Content];
        return *this;
    }

    bool operator== (const Content& content){
        if (this->m_Content == content.m_Content)
            return true;
        return false;
    }
    bool operator!= (const Content& content){
        return !(*this == content);
    }

    operator T* () const {
        return this->m_Content;
    }
    T* get() const {
        return this->m_Content;
    }


private:
    T* m_Content = nullptr;
    void freeContent(){
        if (!this->m_Content)
            return;

        std::lock_guard<std::recursive_mutex> m(Content::StorageMutex); // lock content access

        // is the content still in use?
        auto e = Content::ContentStorageCounter.find(this->m_Content);
        --e->second;

        if (!e->second){
            // release content
            Content::ContentStorageCounter.erase(e);

            auto toDelete = Content::ContentStorage.end();
            for (auto iter = Content::ContentStorage.begin();
                 iter != Content::ContentStorage.end();
                 ++iter)
            {
                if (iter->second.get() == this->m_Content){
                    toDelete = iter;
                    break;
                }
            }

            if (toDelete != Content::ContentStorage.end()){
                _ContentHandle::release(toDelete->second.get());
                Content::ContentStorage.erase(toDelete);
            }
        }
        this->m_Content = nullptr;
    }

    // static class variables
    static boost::filesystem::path ContentPath;

    static std::recursive_mutex StorageMutex;
    static std::map<std::string, std::unique_ptr<T>> ContentStorage;

    static std::map<const T*, std::atomic_uint> ContentStorageCounter;
};



template<typename T, typename _ContentHandle>
std::recursive_mutex Content<T, _ContentHandle>::StorageMutex;

template<typename T, typename _ContentHandle>
boost::filesystem::path Content<T, _ContentHandle>::ContentPath = "";

template<typename T, typename _ContentHandle>
std::map<std::string, std::unique_ptr<T>> Content<T, _ContentHandle>::ContentStorage;

template<typename T, typename _ContentHandle>
std::map<const T*, std::atomic_uint> Content<T, _ContentHandle>::ContentStorageCounter;


#endif // CONTENT_HPP

